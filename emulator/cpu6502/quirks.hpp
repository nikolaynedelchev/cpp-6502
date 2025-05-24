#pragma once

#include "cpu6502Impl.h"

namespace cpp6502::quirks
{

static inline void StatusRegOnBreak_POST(Cpu6502::Impl* cpu) noexcept
{
    cpu->registers.SetInterrupt(1);
}

}
