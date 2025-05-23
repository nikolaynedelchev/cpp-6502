#pragma once
#include <common/common.h>
#include <vector>

namespace cpp6502
{

// Assemble
class IMemory
{
public:
    virtual ~IMemory();
    virtual Byte Read(Address) = 0;
    virtual Byte ReadFromLastAddress() = 0;

    virtual void Write(Address, Byte) = 0;
    virtual void WriteLastData(Address) = 0;
    virtual void WriteToLastAddress(Byte) = 0;
    virtual void WriteToLastAddressLastData() = 0;

    virtual void RepeatLastOperation() = 0;

    virtual std::string ToString() const = 0;
    virtual Byte Unsafe_Read(Address) = 0;
    virtual void Unsafe_Write(Address, Byte) = 0;
};

class Membus : public IMemory, public IDevice
{
public:

    // IDevice
    void Clock() final;

    // IMemory
    Byte Read(Address) final;
    Byte ReadFromLastAddress() final;

    void Write(Address, Byte) final;
    void WriteLastData(Address) final;
    void WriteToLastAddress(Byte) final;
    void WriteToLastAddressLastData() final;

    void RepeatLastOperation() final;

    std::string ToString() const final;
    Byte Unsafe_Read(Address) final;
    void Unsafe_Write(Address, Byte) final;

    void Connect(IMemory* mem, Address startAddress, Address endAddress);
    void Disconnect(IMemory* mem);

    struct TestSequence
    {
        Address address = 0;
        Byte data = 0;
        bool isRead = false;
    };

    void LookForSequence(std::vector<TestSequence> testSequence) noexcept;
    bool IsSequenceOk() const noexcept;
    size_t SequenceStep() const noexcept;
    size_t SequenceStepsLeft() const noexcept;

private:

    struct Slot{IMemory* memory; Address start; Address end;};
    std::vector<Slot> slots_;
    Byte lastValildData_ = 0;
    Word lastValildAddress_ = 0;
    bool isLastRead_ = true;

    bool isReady_ = false;
    std::vector<TestSequence> testSequence_;
    bool isSequenceOk = true;
    size_t sequenceIdx_ = 0;
};

}

