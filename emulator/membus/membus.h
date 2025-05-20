#pragma once
#include <common/common.h>

namespace cpp6502
{

// Assemble
class IMemory
{
public:
    virtual ~IMemory();
    virtual Byte Read(Address) = 0;
    virtual void Write(Address, Byte) = 0;
    virtual std::string ToString() const = 0;
};

class Membus : public IMemory, public IDevice
{
public:

    // IDevice
    void Clock() final;

    // IMemory
    Byte Read(Address) final;
    void Write(Address, Byte) final;
    std::string ToString() const final;

    void Connect(IMemory* mem, Address startAddress, Address endAddress);
    void Disconnect(IMemory* mem);

private:

    struct Slot{IMemory* memory; Address start; Address end;};
    std::vector<Slot> slots_;
    Byte lastValildData_ = 0;
    bool isReady_ = false;
};

}

