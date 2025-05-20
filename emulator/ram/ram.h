#pragma once
#include <common/common.h>
#include <membus/membus.h>

namespace cpp6502
{

class Ram : IMemory
{
public:
    Ram(Address start, Word end);

    std::vector<Byte>& Memory();
    const std::vector<Byte>& Memory() const;

    // IMemory interface
    Byte Read(Address);
    void Write(Address, Byte);
    std::string ToString() const;

private:
    Address start_;
    Address end_;
    std::vector<Byte> memory_;
};

}

