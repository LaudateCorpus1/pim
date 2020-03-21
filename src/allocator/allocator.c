#include "allocator/allocator.h"

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "tlsf/tlsf.h"
#include "common/atomics.h"

static const int32_t kKilobyte = 1 << 10;
static const int32_t kMegabyte = 1 << 20;
static const int32_t kGigabyte = 1 << 30;

static const int32_t kInitCapacity = 0;
static const int32_t kPermCapacity = 128 << 20;
static const int32_t kTempCapacity = 4 << 20;
static const int32_t kTlsCapacity = 1 << 20;

typedef struct linear_allocator_s
{
    int64_t base;
    int64_t head;
    int64_t capacity;
} linear_allocator_t;

static int32_t ms_tempIndex;
static pthread_mutex_t ms_perm_mtx;
static tlsf_t ms_perm;
static linear_allocator_t ms_temp[2];
static PIM_TLS tlsf_t ms_local;

static tlsf_t create_tlsf(int32_t capacity)
{
    ASSERT(capacity > 0);

    void* memory = malloc(capacity);
    ASSERT(memory);

    tlsf_t tlsf = tlsf_create_with_pool(memory, capacity);
    ASSERT(tlsf);

    return tlsf;
}

static void create_linear(linear_allocator_t* alloc, int32_t capacity)
{
    ASSERT(alloc);
    ASSERT(capacity > 0);

    void* memory = malloc(capacity);
    ASSERT(memory);

    alloc->base = (int64_t)memory;
    alloc->capacity = capacity;
    alloc->head = 0;
}

static void destroy_linear(linear_allocator_t* alloc)
{
    ASSERT(alloc);
    free((void*)(alloc->base));
    memset(alloc, 0, sizeof(linear_allocator_t));
}

static void* linear_alloc(linear_allocator_t* alloc, int32_t bytes)
{
    int64_t head = fetch_add_i64(&(alloc->head), bytes, MO_Acquire);
    int64_t tail = head + bytes;
    int64_t addr = alloc->base + head;
    return (tail < alloc->capacity) ? (void*)addr : 0;
}

static void linear_free(linear_allocator_t* alloc, void* ptr, int32_t bytes)
{
    int64_t addr = (int64_t)ptr;
    int64_t base = alloc->base;
    int64_t head = addr - base;
    int64_t tail = head + bytes;
    cmpex_i64(&(alloc->head), &tail, head, MO_Release);
}

static void linear_clear(linear_allocator_t* alloc)
{
    store_i64(&(alloc->head), 0, MO_Release);
}

static void PIM_CDECL Init(void)
{
    int32_t rv = pthread_mutex_init(&ms_perm_mtx, NULL);
    ASSERT(rv == 0);

    ms_tempIndex = 0;
    ms_perm = create_tlsf(kPermCapacity);
    create_linear(ms_temp + 0, kTempCapacity);
    create_linear(ms_temp + 1, kTempCapacity);
}

static void PIM_CDECL Update(void)
{
    ms_tempIndex = (ms_tempIndex + 1) & 1;
    linear_clear(ms_temp + ms_tempIndex);
}

static void PIM_CDECL Shutdown(void)
{
    int32_t rv = pthread_mutex_destroy(&ms_perm_mtx);
    ASSERT(rv == 0);

    free(ms_perm);
    ms_perm = 0;
    free(ms_local);
    ms_local = 0;

    destroy_linear(ms_temp + 0);
    destroy_linear(ms_temp + 1);
}

static void* PIM_CDECL Alloc(EAlloc type, int32_t bytes)
{
    void* ptr = 0;

    if (bytes > 0)
    {
        bytes = ((bytes + 16) + 15) & ~15;

        switch (type)
        {
        default:
            ASSERT(0);
            break;
        case EAlloc_Init:
            ptr = malloc(bytes);
            break;
        case EAlloc_Perm:
            pthread_mutex_lock(&ms_perm_mtx);
            ptr = tlsf_memalign(ms_perm, 16, bytes);
            pthread_mutex_unlock(&ms_perm_mtx);
            break;
        case EAlloc_Temp:
            ptr = linear_alloc(ms_temp + ms_tempIndex, bytes);
            break;
        case EAlloc_TLS:
            if (!ms_local)
            {
                ms_local = create_tlsf(kTlsCapacity);
            }
            ptr = tlsf_memalign(ms_local, 16, bytes);
            break;
        }

        if (ptr)
        {
            int32_t* header = (int32_t*)ptr;
            header[0] = type;
            header[1] = bytes - 16;
            ptr = header + 4;

            ASSERT(((int64_t)ptr & 15) == 0);
        }
    }

    return ptr;
}

static void PIM_CDECL Free(void* ptr)
{
    if (ptr)
    {
        ASSERT(((int64_t)ptr & 15) == 0);
        int32_t* header = (int32_t*)ptr - 4;
        EAlloc type = header[0];
        int32_t bytes = header[1];
        ASSERT(bytes > 0);
        ASSERT((bytes & 15) == 0);

        switch (type)
        {
        default:
            ASSERT(0);
            break;
        case EAlloc_Init:
            free(header);
            break;
        case EAlloc_Perm:
            tlsf_free(ms_perm, header);
            break;
        case EAlloc_Temp:
            linear_free(ms_temp + ms_tempIndex, header, bytes + 16);
            break;
        case EAlloc_TLS:
            ASSERT(ms_local);
            tlsf_free(ms_local, header);
            break;
        }
    }
}

static void* PIM_CDECL Realloc(EAlloc type, void* prev, int32_t bytes)
{
    if (!prev)
    {
        return Alloc(type, bytes);
    }
    if (bytes <= 0)
    {
        ASSERT(bytes == 0);
        Free(prev);
        return 0;
    }

    int32_t* prevHdr = (int32_t*)prev - 4;
    const int32_t prevBytes = prevHdr[1];
    ASSERT(prevBytes > 0);
    if (bytes <= prevBytes)
    {
        return prev;
    }

    bytes = (bytes > (prevBytes * 2)) ? bytes : (prevBytes * 2);

    void* next = Alloc(type, bytes);
    if (next)
    {
        memcpy(next, prev, prevBytes);
    }

    Free(prev);

    return next;
}

static void* PIM_CDECL Calloc(EAlloc type, int32_t bytes)
{
    void* ptr = Alloc(type, bytes);
    if (ptr)
    {
        memset(ptr, 0, bytes);
    }
    return ptr;
}

const allocator_t CAllocator =
{
    .Init = Init,
    .Update = Update,
    .Shutdown = Shutdown,
    .Alloc = Alloc,
    .Free = Free,
    .Realloc = Realloc,
    .Calloc = Calloc,
};