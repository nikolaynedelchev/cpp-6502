#include "cpu6502.h"
#include "cpu6502Impl.h"

namespace cpp6502
{

Cpu6502::Cpu6502() = default;

std::shared_ptr<Cpu6502> Cpu6502::CreateInstance(IMemory *memory)
{
    auto cpu = std::shared_ptr<Cpu6502>(new Cpu6502);
    impl_ = std::make_shared<Cpu6502::Impl>(memory);
    return cpu;
}

void Cpu6502::Clock()
{
    impl_->Clock();
}


}

