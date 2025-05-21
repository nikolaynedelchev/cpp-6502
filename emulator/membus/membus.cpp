#include "membus.h"
#include <algorithm>
#include <fmt/format.h>

namespace cpp6502
{

void Membus::Clock()
{
    isReady_ = true;
}

Byte Membus::Read(Address address)
{
    if (!isReady_)
    {
        Error("Membus")
            .Msg("Memory not ready for read")
            .Msg("Address: {}", address)
            .Throw();
    }
    isReady_ = false;

    for(auto& slot : slots_)
    {
        if (slot.start <= address && slot.end >= address)
        {
            lastValildData_ = slot.memory->Read(address);
            return lastValildData_;
        }
    }

    fmt::println("[membus warning] memory device not found, read failed, address: {}", address);
    return lastValildData_;
}

void Membus::Write(Address address, Byte data)
{
    if (!isReady_)
    {
        Error("Membus")
            .Msg("Memory not ready for write")
            .Msg("Address: {}", address)
            .Throw();
    }

    isReady_ = false;
    lastValildData_ = data;

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

}

