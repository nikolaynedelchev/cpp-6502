#include "cpu6502Impl.h"

namespace cpp6502
{

Cpu6502::Impl::Impl(IMemory *memory)
    : memory_(memory)
{

}

void Cpu6502::Impl::Reset()
{
    registers = {};
    state = {};
    activeInstruction_.Destroy();

    activeInstruction_ = ResetInstruction();
}

void Cpu6502::Impl::Clock()
{
    if (activeInstruction_.Done())
    {
        activeInstruction_.Destroy();
        activeInstruction_ = StartNewInstruction();
    }
    else
    {
        activeInstruction_.Resume();
    }
}

std::string Cpu6502::Impl::ToString() const noexcept
{
    return "TODO: to stirng not implemented";
}

Routine Cpu6502::Impl::ResetInstruction()
{
    state.operand[0] = memory_->Read(0xFFFC);
    WaitClock();

    state.operand[1] = memory_->Read(0xFFFD);
    registers.PC = Bitwise::BytesToWord(state.operand);
    WaitClock();

    co_return;
}

Routine Cpu6502::Impl::StartNewInstruction()
{
    state = {};
    state.opcode = memory_->Read( registers.PC );
    registers.PC++;
    WaitClock();

    // TODO: decode opcode mem access mode
    state.addressMode = 1;

    // TODO: decode opcode instruction handler
    Execute( Instr_ADC() );

    co_return;
}

Routine Cpu6502::Impl::ReadAddress()
{
    //  Name          Bytes     Routine Cycles  Total Cycles
    //-------------------------------------------------------
    // Immediate   .... 0 ......... 0 ............. 1
    // Accumulator .... 0 ......... 0 ............. 1
    // ZeroPage    .... 1 ......... 1 ............. 2
    // ZeroPage_X  .... 1 ......... 2 ............. 3
    // ZeroPage_Y  .... 1 ......... 2 ............. 3
    // Relative    .... 1 ......... 1 TODO ........ 2 TODO
    // Absolute    .... 2 ......... 2 ............. 3
    // Absolute_X  .... 2 ......... 2 ............. 3
    // Absolute_Y  .... 2 ......... 2 ............. 3
    // Indirect    .... 2 ......... 4 TODO ........ 5 TODO
    // Indexed_X   .... 1 ......... 4 ............. 5
    // Indexed_Y   .... 1 ......... 3 TODO ........ 4 TODO

    // 0 operands
    if (state.addressMode == Meta::MemAcc_Immediate ||
        state.addressMode == Meta::MemAcc_Accumulator)
    {
        co_return;
    }

    // 1 operand
    if (state.addressMode & (
            Meta::MemAcc_ZeroPage   |
            Meta::MemAcc_ZeroPage_X |
            Meta::MemAcc_ZeroPage_Y |
            Meta::MemAcc_Relative   |
            Meta::MemAcc_Indexed_X  |
            Meta::MemAcc_Indexed_Y)
        )
    {
        // Cycle 1
        // read first operand
        state.operand[0] = memory_->Read( registers.PC );
        registers.PC++;
        WaitClock();
    }

    // 2 operand
    if (state.addressMode & (
            Meta::MemAcc_Absolute   |
            Meta::MemAcc_Absolute_X |
            Meta::MemAcc_Absolute_Y |
            Meta::MemAcc_Indirect)
        )
    {
        // Cycle 2
        // read second operand
        state.operand[1] = memory_->Read( registers.PC );
        registers.PC++;
        WaitClock();
    }

    switch (state.addressMode)
    {
    case Meta::MemAcc_ZeroPage:
    {
        state.address = Bitwise::BytesToWord(state.operand);
    }break;
    case Meta::MemAcc_ZeroPage_X:
    {
        state.address = (Bitwise::BytesToWord(state.operand) + registers.X) % 0xff;
        WaitClock();
    }break;
    case Meta::MemAcc_ZeroPage_Y:
    {
        state.address = (Bitwise::BytesToWord(state.operand) + registers.Y) % 0xff;
        WaitClock();
    }break;
    case Meta::MemAcc_Relative:
    {
        state.address = registers.PC + SByte( state.operand[0] );
        state.nextPage = Bitwise::ClrLoByte(registers.PC) != Bitwise::ClrLoByte(state.address);
        // TODO:
    }break;
    case Meta::MemAcc_Absolute:
    {
        state.address = Bitwise::BytesToWord(state.operand);
    }break;
    case Meta::MemAcc_Absolute_X:
    {
        Word a = Bitwise::BytesToWord(state.operand);
        state.address = a + registers.X;
        state.nextPage = Bitwise::ClrLoByte(a) != Bitwise::ClrLoByte(state.address);
    }break;
    case Meta::MemAcc_Absolute_Y:
    {
        Word a = Bitwise::BytesToWord(state.operand);
        state.address = a + registers.Y;
        state.nextPage = Bitwise::ClrLoByte(a) != Bitwise::ClrLoByte(state.address);
    }break;
    case Meta::MemAcc_Indirect:
    {
        // Cycle 3
        Word a = Bitwise::BytesToWord(state.operand);
        state.indirect[0] = memory_->Read( a );
        WaitClock();

        // Cycle 4
        state.indirect[1] = memory_->Read( a + 1);
        state.address = Bitwise::BytesToWord(state.indirect);
        WaitClock();
        // TODO:
    }break;
    case Meta::MemAcc_Indexed_X:
    {
        // Cycle 3
        Word a = (Bitwise::BytesToWord(state.operand) + registers.X) % 0xff;
        state.indirect[0] = memory_->Read( a );
        WaitClock();

        // Cycle 4
        state.indirect[1] = memory_->Read( (a + 1) % 0xff );
        WaitClock();

        // Cycle 5
        state.address = Bitwise::BytesToWord(state.indirect);
        WaitClock();
    }break;
    case Meta::MemAcc_Indexed_Y:
    {
        // Cycle 3
        Word zeroPage = Bitwise::BytesToWord(state.operand);
        state.indirect[0] = memory_->Read( zeroPage );
        WaitClock();

        // Cycle 4
        state.indirect[1] = memory_->Read( (zeroPage + 1) % 0xff );
        Word a = Bitwise::BytesToWord(state.indirect);
        state.address = a + registers.Y;
        state.nextPage = Bitwise::ClrLoByte(a) != Bitwise::ClrLoByte(state.address);
        WaitClock();
        // TODO: STA exception in cycles
    }break;
    default:
        break;
    }

    co_return;
}

Routine Cpu6502::Impl::ReadData()
{
    if (state.addressMode == Meta::MemAcc_Immediate)
    {
        state.operand[0] = memory_->Read( registers.PC );
        registers.PC++;
        state.data = state.operand[0];
    }
    else if (state.addressMode == Meta::MemAcc_Accumulator)
    {
        state.data = registers.A;
        // no extra cycle
        co_return;
    }
    else
    {
        state.data = memory_->Read( state.address );
    }

    // Cycle 1
    WaitClock();

    co_return;
}

Routine Cpu6502::Impl::WriteData()
{
    if (state.addressMode == Meta::MemAcc_Immediate)
    {
        Error("Cpu6502::Impl", "Unexpected write")
                .Msg("CPU: {}", ToString())
                .Throw();
    }
    else if (state.addressMode == Meta::MemAcc_Accumulator)
    {
        registers.A = state.data;
        // no extra cycle
        co_return;
    }
    else
    {
        memory_->Write( state.address, state.data );
    }

    // Cycle 1
    WaitClock();

    if (state.addressMode == Meta::MemAcc_Absolute_X)
    {
        // Cycle 2
        WaitClock();
    }

    co_return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// ADC: Add with Carry
Routine Cpu6502::Impl::Instr_ADC()
{
    Execute( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        WaitClock();
    }

    Execute( ReadData() );

    state.helper16 = registers.A + state.data + registers.Carry();
    state.helper8 = Byte(state.helper16);

    registers.SetCarry( state.helper16 > 255 ? 1 : 0 );
    registers.SetZero( state.helper8 == 0 ? 1 : 0 );

    registers.SetOverflow(
                (Bitwise::Sign(registers.A) == Bitwise::Sign(state.data)) &&
                (Bitwise::Sign(registers.A) != Bitwise::Sign(state.helper8))
                ? 1 : 0);

    registers.SetNegative( Bitwise::Sign(state.helper8) );

    registers.A = state.helper8;

    co_return;
}

// AND: Logical AND
Routine Cpu6502::Impl::Instr_AND()
{
    Execute( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        WaitClock();
    }

    Execute( ReadData() );

    registers.A &= state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign(registers.A) );

    co_return;
}

// ASL: Arithmetic Shift Left
Routine Cpu6502::Impl::Instr_ASL()
{
    Execute( ReadAddress() );
    Execute( ReadData() );

    registers.SetCarry( Bitwise::Bit(state.data, 7) );

    state.data <<= 1;

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign(state.data) );

    WaitClock();

    Execute( WriteData() );

    co_return;
}

// BCC: Branch if Carry Clear
Routine Cpu6502::Impl::Instr_BCC()
{
    Execute( ReadAddress() );

    if (0 == registers.Carry())
    {
        co_return;
    }

    if (state.nextPage)
    {
        WaitClock();
    }

    registers.PC = state.address;
    WaitClock();

    co_return;
}

// BCS: Branch if Carry Set
Routine Cpu6502::Impl::Instr_BCS()
{
    Execute( ReadAddress() );

    if (0 != registers.Carry())
    {
        co_return;
    }

    if (state.nextPage)
    {
        WaitClock();
    }

    registers.PC = state.address;
    WaitClock();

    co_return;
}


// BEQ: Branch if Equal
Routine Cpu6502::Impl::Instr_BEQ()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BEQ")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// BIT: Bit Test
Routine Cpu6502::Impl::Instr_BIT()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BIT")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// BMI: Branch if Minus
Routine Cpu6502::Impl::Instr_BMI()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BMI")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// BNE: Branch if Not Equal
Routine Cpu6502::Impl::Instr_BNE()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BNE")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// BPL: Branch if Positive
Routine Cpu6502::Impl::Instr_BPL()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BPL")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// BRK: Force Interrupt
Routine Cpu6502::Impl::Instr_BRK()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BRK")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// BVC: Branch if Overflow Clear
Routine Cpu6502::Impl::Instr_BVC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BVC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// BVS: Branch if Overflow Set
Routine Cpu6502::Impl::Instr_BVS()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BVS")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// CLC: Clear Carry Flag
Routine Cpu6502::Impl::Instr_CLC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// CLD: Clear Decimal Mode
Routine Cpu6502::Impl::Instr_CLD()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLD")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// CLI: Clear Interrupt Disable
Routine Cpu6502::Impl::Instr_CLI()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLI")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// CLV: Clear Overflow Flag
Routine Cpu6502::Impl::Instr_CLV()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLV")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// CMP: Compare
Routine Cpu6502::Impl::Instr_CMP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CMP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// CPX: Compare X Register
Routine Cpu6502::Impl::Instr_CPX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CPX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// CPY: Compare Y Register
Routine Cpu6502::Impl::Instr_CPY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CPY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// DEC: Decrement Memory
Routine Cpu6502::Impl::Instr_DEC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// DEX: Decrement X Register
Routine Cpu6502::Impl::Instr_DEX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// DEY: Decrement Y Register
Routine Cpu6502::Impl::Instr_DEY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// EOR: Exclusive OR
Routine Cpu6502::Impl::Instr_EOR()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "EOR")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// INC: Increment Memory
Routine Cpu6502::Impl::Instr_INC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// INX: Increment X Register
Routine Cpu6502::Impl::Instr_INX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// INY: Increment Y Register
Routine Cpu6502::Impl::Instr_INY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// JMP: Jump
Routine Cpu6502::Impl::Instr_JMP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "JMP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// JSR: Jump to Subroutine
Routine Cpu6502::Impl::Instr_JSR()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "JSR")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// LDA: Load Accumulator
Routine Cpu6502::Impl::Instr_LDA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// LDX: Load X Register
Routine Cpu6502::Impl::Instr_LDX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// LDY: Load Y Register
Routine Cpu6502::Impl::Instr_LDY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// LSR: Logical Shift Right
Routine Cpu6502::Impl::Instr_LSR()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LSR")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// NOP: No Operation
Routine Cpu6502::Impl::Instr_NOP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "NOP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// ORA: Logical Inclusive OR
Routine Cpu6502::Impl::Instr_ORA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ORA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// PHA: Push Accumulator
Routine Cpu6502::Impl::Instr_PHA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PHA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// PHP: Push Processor Status
Routine Cpu6502::Impl::Instr_PHP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PHP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// PLA: Pull Accumulator
Routine Cpu6502::Impl::Instr_PLA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PLA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// PLP: Pull Processor Status
Routine Cpu6502::Impl::Instr_PLP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PLP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// ROL: Rotate Left
Routine Cpu6502::Impl::Instr_ROL()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ROL")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// ROR: Rotate Right
Routine Cpu6502::Impl::Instr_ROR()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ROR")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// RTI: Return from Interrupt
Routine Cpu6502::Impl::Instr_RTI()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "RTI")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// RTS: Return from Subroutine
Routine Cpu6502::Impl::Instr_RTS()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "RTS")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// SBC: Subtract with Carry
Routine Cpu6502::Impl::Instr_SBC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SBC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// SEC: Set Carry Flag
Routine Cpu6502::Impl::Instr_SEC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SEC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// SED: Set Decimal Flag
Routine Cpu6502::Impl::Instr_SED()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SED")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// SEI: Set Interrupt Disable
Routine Cpu6502::Impl::Instr_SEI()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SEI")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// STA: Store Accumulator
Routine Cpu6502::Impl::Instr_STA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// STX: Store X Register
Routine Cpu6502::Impl::Instr_STX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// STY: Store Y Register
Routine Cpu6502::Impl::Instr_STY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// TAX: Transfer Accumulator to X
Routine Cpu6502::Impl::Instr_TAX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TAX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// TAY: Transfer Accumulator to Y
Routine Cpu6502::Impl::Instr_TAY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TAY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// TSX: Transfer Stack Pointer to X
Routine Cpu6502::Impl::Instr_TSX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TSX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// TXA: Transfer X to Accumulator
Routine Cpu6502::Impl::Instr_TXA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TXA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// TXS: Transfer X to Stack Pointer
Routine Cpu6502::Impl::Instr_TXS()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TXS")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

// TYA: Transfer Y to Accumulator
Routine Cpu6502::Impl::Instr_TYA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TYA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

}


