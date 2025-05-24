#pragma once

#include "cpu6502Impl.h"

namespace cpp6502::quirks
{

static inline void AddressWrappingOnIndirectFetch_byte_2_PRE(Cpu6502::Impl* cpu) noexcept
{
    // An original 6502 has does not correctly fetch the target
    // address if the indirect vector falls on a page boundary
    // (e.g. $xxFF where xx is any value from $00 to $FF).
    // In this case fetches the LSB from $xxFF as expected
    // but takes the MSB from $xx00.
    // This is fixed in some later chips like the 65SC02

    if ( Bitwise::LoByte16(cpu->state.helper16) == 0xff )
    {
        cpu->state.helper16 = Bitwise::ClrLoByte16(cpu->state.helper16);
        cpu->state.helper16--;
    }
}

static inline void StatusRegOgPHP_PRE(Cpu6502::Impl* cpu) noexcept
{
    // Cycle 2
    // 0672 : c930            >            cmp #(0      |fao)&m8    ;expected flags + always on bits
    // 6502_functional_test.lst
    // Klaus2m5
    cpu->state.helper8 |= Cpu6502::Impl::Meta::FlagMask_Break;
}

static inline void StatusRegOnBreak_POST(Cpu6502::Impl* cpu) noexcept
{
    cpu->registers.SetInterrupt(1);
}

}
