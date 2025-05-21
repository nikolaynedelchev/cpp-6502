#pragma once
#include <cstddef>

namespace cpp6502
{

class MemPool
{
public:
    static void* Malloc(size_t);
    static void Free(void*, size_t);
};

}
