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

std::string Ram::ToString() const
{
    return fmt::format("[Ram] start: {}, end: {}, buffsize: {}",
                       start_, end_, memory_.size());
}

}

