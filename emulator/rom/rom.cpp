#include "rom.h"

namespace cpp6502
{

Rom::Rom(Address start, Word end)
    : IMemory()
    , start_(start)
    , end_(end)
{
    if(start > end)
    {
        Error("Rom")
            .Msg("Rom not valid")
            .Msg("Start: {}", start)
            .Msg("End: {}", end)
            .Throw();
    }
    memory_.resize(end - start + 1);
    for(auto& b : memory_) b = 0;
}

std::vector<Byte> &Rom::Memory()
{
    return memory_;
}

Address Rom::Start() const noexcept
{
    return start_;
}

Address Rom::End() const noexcept
{
    return end_;
}

Byte& Rom::operator[](Address address)
{
    return memory_[address - start_];
}

const Byte& Rom::operator[](Address address) const
{
    return memory_[address - start_];
}

void Rom::Overrite(Address address, const std::vector<Byte> &mem)
{
    for(auto b : mem)
    {
        memory_[address - start_] = b;
        address++;
    }
}

const std::vector<Byte> &Rom::Memory() const
{
    return memory_;
}

Byte Rom::Read(Address address)
{
    if (address < start_ || address > end_)
    {
        Error("Rom")
            .Msg("Read address not valid")
            .Msg("Address: {}", address)
            .Msg("Start: {}", start_)
            .Msg("End: {}", end_)
            .Throw();
    }

    return memory_[address - start_];
}

void Rom::Write(Address address, Byte data)
{
    if (address < start_ || address > end_)
    {
        Error("Rom")
            .Msg("Write address not valid")
            .Msg("Address: {}", address)
            .Msg("Start: {}", start_)
            .Msg("End: {}", end_)
            .Msg("Data: {}", int(data))
            .Throw();
    }
}

std::string Rom::ToString() const
{
    return fmt::format("[Rom] start: {}, end: {}, buffsize: {}",
                       start_, end_, memory_.size());
}

}

