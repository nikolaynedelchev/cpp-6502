#include "cpu6502.h"
#include "cpu6502Impl.h"

namespace cpp6502
{

Cpu6502::Cpu6502() = default;

std::shared_ptr<Cpu6502> Cpu6502::CreateInstance(IMemory *memory)
{
    auto cpu = std::shared_ptr<Cpu6502>(new Cpu6502);
    cpu->impl_ = std::make_shared<Cpu6502::Impl>(memory);
    return cpu;
}

void Cpu6502::PowerOn()
{
    impl_->PowerOn();
}

void Cpu6502::Reset()
{
    impl_->Reset();
}

std::string Cpu6502::Dump() const noexcept
{
    return impl_->ToString();
}

void Cpu6502::Clock()
{
    impl_->Clock();
}


}

