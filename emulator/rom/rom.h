#pragma once
#include <common/common.h>
#include <membus/membus.h>

namespace cpp6502
{

class Rom : public IMemory
{
public:
    Rom(Address start, Word end);

    std::vector<Byte>& Memory();
    const std::vector<Byte>& Memory() const;

    Address Start() const noexcept;
    Address End() const noexcept;
    Byte& operator[](Address address);
    const Byte& operator[](Address address) const;
    void Overrite(Address, const std::vector<Byte>& mem);

    // IMemory interface
    Byte Read(Address);
    void Write(Address, Byte);
    std::string ToString() const;
    Byte Unsafe_Read(Address) final;
    void Unsafe_Write(Address, Byte) final;

private:
    Address start_;
    Address end_;
    std::vector<Byte> memory_;
};

}

