#pragma once
#include <cstddef>

namespace cpp6502
{

class MemPool
{
    static void* Malloc(size_t);
    static void Free(void*, size_t);
};

}
