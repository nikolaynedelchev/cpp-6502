#include "membus.h"
#include <algorithm>
#include <fmt/format.h>

namespace cpp6502
{

IMemory::~IMemory() = default;

void Membus::Clock()
{
    isReady_ = true;
}

Byte Membus::Read(Address address)
{
    lastValildAddress_ = address;
    return ReadFromLastAddress();
}

Byte Membus::ReadFromLastAddress()
{
    if (!isReady_)
    {
        Error("Membus")
            .Msg("Memory not ready for read")
            .Msg("Address: {}", lastValildAddress_)
            .Throw();
    }
    isReady_ = false;
    isLastRead_ = true;
    lastValildData_ = Unsafe_Read(lastValildAddress_);
    if (testSequence_.size())
    {
        if (testSequence_[sequenceIdx_].address != lastValildAddress_ ||
            testSequence_[sequenceIdx_].data != lastValildData_ ||
            testSequence_[sequenceIdx_].isRead == false)
        {
            isSequenceOk = false;
            fmt::println("[actual] Cmd: R, Address: x{:04X}/{}, data: x{:02X}/{}", lastValildAddress_, lastValildAddress_, lastValildData_, lastValildData_);
            fmt::println("[expect] Cmd: {}, Address: x{:04X}/{}, data: x{:02X}/{}", (testSequence_[sequenceIdx_].isRead)?"R":"W",
                         testSequence_[sequenceIdx_].address, testSequence_[sequenceIdx_].address,
                         testSequence_[sequenceIdx_].data, testSequence_[sequenceIdx_].data);
            int dummyStop = 1;
            (void)dummyStop;
        }
        sequenceIdx_++;

        if (sequenceIdx_ >= testSequence_.size())
        {
            testSequence_.clear();
            sequenceIdx_ = 0;
        }
    }
    return lastValildData_;
}

void Membus::Write(Address address, Byte data)
{
    lastValildAddress_ = address;
    lastValildData_ = data;
    WriteToLastAddressLastData();
}

void Membus::WriteLastData(Address address)
{
    lastValildAddress_ = address;
    WriteToLastAddressLastData();
}

void Membus::WriteToLastAddress(Byte data)
{
    lastValildData_ = data;
    WriteToLastAddressLastData();
}

void Membus::WriteToLastAddressLastData()
{
    if (!isReady_)
    {
        Error("Membus")
            .Msg("Memory not ready for write")
            .Msg("Address: {}", lastValildAddress_)
            .Throw();
    }
    if (testSequence_.size())
    {
        if (testSequence_[sequenceIdx_].address != lastValildAddress_ ||
            testSequence_[sequenceIdx_].data != lastValildData_ ||
            testSequence_[sequenceIdx_].isRead == true)
        {
            isSequenceOk = false;
            fmt::println("[actual] Cmd: W, Address: x{:04X}/{}, data: x{:02X}/{} ",
                         lastValildAddress_, lastValildAddress_, lastValildData_, lastValildData_);
            fmt::println("[expect] Cmd: {}, Address: x{:04X}/{}, data: x{:02X}/{}", (testSequence_[sequenceIdx_].isRead)?"R":"W",
                         testSequence_[sequenceIdx_].address, testSequence_[sequenceIdx_].address,
                         testSequence_[sequenceIdx_].data, testSequence_[sequenceIdx_].data);
            int dummyStop = 1;
            (void)dummyStop;
        }
        sequenceIdx_++;

        if (sequenceIdx_ >= testSequence_.size())
        {
            testSequence_.clear();
            sequenceIdx_ = 0;
        }
    }
    isReady_ = false;
    isLastRead_ = false;
    Unsafe_Write(lastValildAddress_, lastValildData_);
}

void Membus::RepeatLastOperation()
{
    if (isLastRead_)
    {
        ReadFromLastAddress();
    }
    else
    {
        WriteToLastAddressLastData();
    }
}

std::string Membus::ToString() const
{
    std::string str;
    str += fmt::format("Is readt: {}, ", isReady_);
    str += fmt::format("Last valid data: {}\n", lastValildData_);
    for(const auto& slot : slots_)
    {
        str += fmt::format("Slot start: {}, Slot end: {}, Device: {}\n",
                           slot.start, slot.end, slot.memory->ToString());
    }
    return str;
}

Byte Membus::Unsafe_Read(Address address)
{
    for(auto& slot : slots_)
    {
        if (slot.start <= address && slot.end >= address)
        {
            return slot.memory->Read(address);
        }
    }

    fmt::println("[membus warning] memory device not found, read failed, address: {}", address);
    return lastValildData_;
}

void Membus::Unsafe_Write(Address address, Byte data)
{
    for(auto& slot : slots_)
    {
        if (slot.start <= address && slot.end >= address)
        {
            slot.memory->Write(address, data);
            return;
        }
    }

    fmt::println("[membus warning] memory device not found, write failed, address: {}", address);
}

void Membus::Connect(IMemory *mem, Address startAddress, Address endAddress)
{
    slots_.push_back({mem, startAddress, endAddress});
    std::sort(slots_.begin(), slots_.end(),
    [](const Slot& a, const Slot& b) {
        return a.start < b.start;
    });

    for (size_t i = 0; i < slots_.size(); ++i)
    {
        bool isInvalid = (slots_[i].start > slots_[i].end);
        bool isOverlap = (i > 0 && slots_[i].start <= slots_[i - 1].end);

        if (isInvalid || isOverlap)
        {
            Error("Membus")
                .Msg("Slot Error")
                .Msg("Index: {}", i)
                .Msg("Start: {}", slots_[i].start)
                .Msg("End: {}", slots_[i].end)
                .Msg("Is Invalid: {}", isInvalid)
                .Msg("Is Overlap: {}", isOverlap)
                .Throw();
        }
    }
}

void Membus::Disconnect(IMemory *mem)
{
    auto sz = slots_.size();
    slots_.erase(
        std::remove_if(slots_.begin(), slots_.end(),
        [mem](const Slot& s) {
           return s.memory == mem;
        }),
        slots_.end());

    if (sz == slots_.size())
    {
        fmt::println("[membus warning] memory device not found and not removed");
    }
}

void Membus::LookForSequence(std::vector<TestSequence> testSequence) noexcept
{
    testSequence_ = std::move(testSequence);
    isSequenceOk = true;
    sequenceIdx_ = 0;
}


bool Membus::IsSequenceOk() const noexcept
{
    return isSequenceOk;
}

size_t Membus::SequenceStep() const noexcept
{
    return sequenceIdx_;
}

size_t Membus::SequenceStepsLeft() const noexcept
{
    return testSequence_.size() - sequenceIdx_;
}

}

