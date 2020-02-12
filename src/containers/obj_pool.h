#pragma once

#include "common/chunk_allocator.h"
#include <new>

template<typename T, typename Args>
struct ObjPool
{
    ChunkAllocator m_chunks;

    void Init()
    {
        m_chunks.Init(Alloc_Tlsf, sizeof(T));
    }
    void Reset()
    {
        m_chunks.Reset();
    }

    T* New(Args args) { return new (m_chunks.Allocate()) T(args); }
    void Delete(T* ptr) { if (ptr) { ptr->~T(); m_chunks.Free(ptr); } }
};