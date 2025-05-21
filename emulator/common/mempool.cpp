#include "mempool.h"
#include <type_traits>
#include <array>
#include "common.h"

namespace cpp6502
{

namespace MemPoolInternal
{
    static constexpr size_t s_maxBufferSize = 1024;
    static constexpr size_t s_poolSize = 32;

    using Buffer = std::aligned_storage_t<s_maxBufferSize, alignof(std::max_align_t)>;

    static std::array<Buffer, s_poolSize> s_buffers;
    static std::array<void*, s_poolSize> s_freeBuffers;

    static size_t s_freeBuffersIdx = ([](){
        // Init MemPool
        for(size_t i = 0; i < s_poolSize; i++)
        {
            s_freeBuffers[i] = &s_buffers[i];
        }
    }(),
    s_poolSize); //initialize s_freeBuffersIdx
}

void *MemPool::Malloc(size_t size)
{
    if (size > MemPoolInternal::s_maxBufferSize ||
        MemPoolInternal::s_freeBuffersIdx == 0)
    {
        Error("MemPool", "Memory Malloc Error")
            .Msg("size: {}, maxsize: {}", size, MemPoolInternal::s_maxBufferSize)
            .Msg("s_freeBuffersIdx: {}", MemPoolInternal::s_freeBuffersIdx)
            .Throw();
    }

    MemPoolInternal::s_freeBuffersIdx--;
    return MemPoolInternal::s_freeBuffers[ MemPoolInternal::s_freeBuffersIdx ];
}

void MemPool::Free(void *ptr, size_t size)
{
    if (size > MemPoolInternal::s_maxBufferSize ||
        MemPoolInternal::s_freeBuffersIdx == MemPoolInternal::s_poolSize)
    {
        Error("MemPool", "Memory Free Error")
            .Msg("size: {}, maxsize: {}", size, MemPoolInternal::s_maxBufferSize)
            .Msg("s_freeBuffersIdx: {}", MemPoolInternal::s_freeBuffersIdx)
            .Throw();
    }

    MemPoolInternal::s_freeBuffers[ MemPoolInternal::s_freeBuffersIdx ] = ptr;
    MemPoolInternal::s_freeBuffersIdx++;
}

}
