#include "ram.h"

namespace cpp6502
{

Ram::Ram(Address start, Word end)
    : IMemory()
    , start_(start)
    , end_(end)
{
    if(start > end)
    {
        Error("Ram")
            .Msg("Ram not valid")
            .Msg("Start: {}", start)
            .Msg("End: {}", end)
            .Throw();
    }
    memory_.resize(end - start + 1);
    for(auto& b : memory_) b = 0;
}

std::vector<Byte> &Ram::Memory()
{
    return memory_;
}

const std::vector<Byte> &Ram::Memory() const
{
    return memory_;
}

Address Ram::Start() const noexcept
{
    return start_;
}

Address Ram::End() const noexcept
{
    return end_;
}

Byte& Ram::operator[](Address address)
{
    return memory_[address - start_];
}

const Byte& Ram::operator[](Address address) const
{
    return memory_[address - start_];
}

void Ram::Overrite(Address address, const std::vector<Byte> &mem)
{
    for(auto b : mem)
    {
        memory_[address - start_] = b;
        address++;
    }
}

Byte Ram::Read(Address address)
{
    if (address < start_ || address > end_)
    {
        Error("Ram")
            .Msg("Read address not valid")
            .Msg("Address: {}", address)
            .Msg("Start: {}", start_)
            .Msg("End: {}", end_)
            .Throw();
    }
    return memory_[address - start_];
}

Byte Ram::ReadFromLastAddress()
{
    Error("Ram::ReadFromLastAddress", "Not Implamented")
        .Throw();
    return 0;
}

void Ram::Write(Address address, Byte data)
{
    if (address < start_ || address > end_)
    {
        Error("Ram")
            .Msg("Write address not valid")
            .Msg("Address: {}", address)
            .Msg("Start: {}", start_)
            .Msg("End: {}", end_)
            .Msg("Data: {}", int(data))
            .Throw();
    }
    memory_[address - start_] = data;
}

void Ram::WriteLastData(Address)
{
    Error("Ram::WriteLastData", "Not Implamented")
        .Throw();
}

void Ram::WriteToLastAddress(Byte)
{
    Error("Ram::WriteToLastAddress", "Not Implamented")
        .Throw();
}

void Ram::WriteToLastAddressLastData()
{
    Error("Ram::WriteToLastAddressLastData", "Not Implamented")
        .Throw();
}

void Ram::RepeatLastOperation()
{
    Error("Ram::RepeatLastOperation", "Not Implamented")
        .Throw();
}

std::string Ram::ToString() const
{
    return fmt::format("[Ram] start: {}, end: {}, buffsize: {}",
                       start_, end_, memory_.size());
}

Byte Ram::Unsafe_Read(Address address)
{
    return memory_[address - start_];
}

void Ram::Unsafe_Write(Address address, Byte data)
{
    memory_[address - start_] = data;
}

}

