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

bool Cpu6502::IsInstructionDone() const noexcept
{
    return impl_->IsInstructionDone();
}

void Cpu6502::Clock()
{
    impl_->Clock();
}

void Cpu6502::ForceState(const State& initial) noexcept
{
    impl_->ForceState(std::move(initial));
}

bool Cpu6502::Compate(const State& state) const noexcept
{
    return impl_->Compate(state);
}

Cpu6502::Lifetime Cpu6502::GetLifetime() const noexcept
{
    return impl_->GetLifetime();
}


}

