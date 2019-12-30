#pragma once

#include "common/minmax.h"
#include "common/int_types.h"
#include <string.h>

namespace Allocator
{
    void Init();
    void Update();
    void Shutdown();

    void* Alloc(AllocType type, i32 bytes);
    void Free(void* prev);
    void* Realloc(AllocType type, void* prev, i32 bytes);

    inline void* Calloc(AllocType type, i32 bytes)
    {
        void* ptr = Alloc(type, bytes);
        memset(ptr, 0, bytes);
        return ptr;
    }

    template<typename T>
    inline T* AllocT(AllocType type, i32 count)
    {
        return (T*)Alloc(type, sizeof(T) * count);
    }

    template<typename T>
    inline T* ReallocT(AllocType type, T* prev, i32 count)
    {
        return (T*)Realloc(type, prev, sizeof(T) * count);
    }

    template<typename T>
    inline T* CallocT(AllocType type, i32 count)
    {
        return (T*)Calloc(type, sizeof(T) * count);
    }

    template<typename T>
    inline T* Reserve(
        AllocType type,
        T* prev,
        i32& capacity,
        i32 count)
    {
        if (count > capacity)
        {
            capacity = Max(count, Max(capacity * 2, 16));
            return ReallocT<T>(type, prev, capacity);
        }
        return prev;
    }

    inline void* ImGuiAllocFn(size_t sz, void*) { return Alloc(Alloc_Pool, (i32)sz); }
    inline void ImGuiFreeFn(void* ptr, void*) { Free(ptr); }
};