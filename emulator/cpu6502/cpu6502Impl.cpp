#include "cpu6502Impl.h"

#define WaitRoutine(routine) {auto r = routine; while(r.Resume()){ WaitClock(); }}

namespace cpp6502
{

inline static constexpr Routine::Empty WaitClock() noexcept
{
    return Routine::Empty{};
}

Cpu6502::Impl::Impl(IMemory *memory)
    : memory_(memory)
{

}

void Cpu6502::Impl::PowerOn()
{
    Reset();
}

void Cpu6502::Impl::Reset()
{
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
    return fmt::format(
        "PC:{:04X} SP:{:02X} A:{:02X} X:{:02X} Y:{:02X} S:{:02X} Flags[(N:{})(V:{})(U:{})(B:{})(D:{})(I:{})(Z:{})(C:{})]",
        registers.PC,
        registers.SP,
        registers.A,
        registers.X,
        registers.Y,
        registers.S,
        registers.Negative()   ? '1' : '0',
        registers.Overflow()   ? '1' : '0',
        registers.UnusedFlag() ? '1' : '0',
        registers.Break()      ? '1' : '0',
        registers.Decimal()    ? '1' : '0',
        registers.Interrupt()  ? '1' : '0',
        registers.Zero()       ? '1' : '0',
        registers.Carry()      ? '1' : '0'
    );
}

Routine Cpu6502::Impl::ResetInstruction()
{
    registers = {};
    state = {};

    // Cycle 1,2
    // dummy opcode fetch
    for(int step = 0; step < 2; step++)
    {
        memory_->Read(registers.PC);
        registers.PC++;
        co_yield WaitClock();
    }

    // Cycle 3,4,5
    for(int step = 0; step < 3; step++)
    {
        memory_->Read(registers.StackAddress());    // 6502 quirk: RESET issues three dummy stack “pushes”
        registers.ApplyPush();                      // with R/W held HIGH (read-only) to drop SP to $FD
        co_yield WaitClock();                       // no data is written; not a bug.
    }

    // Cycle 6
    state.operand[0] = memory_->Read(0xFFFC);
    co_yield WaitClock();

    // Cycle 7
    state.operand[1] = memory_->Read(0xFFFD);
    registers.PC = Bitwise::BytesToWord(state.operand);
    co_yield WaitClock();

    co_return;
}

Routine Cpu6502::Impl::StartNewInstruction()
{
    state = {};
    state.opcode = memory_->Read( registers.PC );
    registers.PC++;
    co_yield WaitClock();

    state.addressMode = Meta::Instructions()[state.opcode].memoryMode;
    state.instruction = Meta::Instructions()[state.opcode].instruction;

    // Call member funktion
    WaitRoutine( ((*this).*(state.instruction))() );

    co_return;
}

Routine Cpu6502::Impl::ReadAddress()
{
//  Name       Bytes  Cycles  Total +Read +Write
//-------------------------------------------------------
// Immediate   .. 0 ... 0 ..... 1 ... 2 .... 0
// Accumulator .. 0 ... 0 ..... 1 ... 1 .... 1
// ZeroPage    .. 1 ... 1 ..... 2 ... 3 .... 4
// ZeroPage_X  .. 1 ... 2 ..... 3 ... 4 .... 5
// ZeroPage_Y  .. 1 ... 2 ..... 3 ... 4 .... 5
// Relative    .. 1 ... 1 ..... 2*... 3*.... 4
// Absolute    .. 2 ... 2 ..... 3 ... 4 .... 5
// Absolute_X  .. 2 ... 2*..... 3*... 4*.... 6
// Absolute_Y  .. 2 ... 2*..... 3*... 4*.... 6
// Indirect    .. 2 ... 4 ..... 5 ... 6 .... 7
// Indexed_X   .. 1 ... 4 ..... 5 ... 6 .... 7
// Indexed_Y   .. 1 ... 3*..... 4*... 5*.... 6
// Aux_ResetVec.. 0 ... 2 ..... 3 ... 4 .... 5

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
            Meta::MemAcc_Indexed_Y  |
            Meta::MemAcc_Absolute   |
            Meta::MemAcc_Absolute_X |
            Meta::MemAcc_Absolute_Y |
            Meta::MemAcc_Indirect)
        )
    {
        // Cycle 1
        // read first operand
        state.operand[0] = memory_->Read( registers.PC );
        registers.PC++;
        co_yield WaitClock();
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
        co_yield WaitClock();
    }

    switch (state.addressMode)
    {
    case Meta::MemAcc_ZeroPage:
    {
        state.address = Bitwise::BytesToWord(state.operand);
    }break;
    case Meta::MemAcc_ZeroPage_X:
    {
        state.address = (Bitwise::BytesToWord(state.operand) + registers.X) & 0xff;
        co_yield WaitClock();
    }break;
    case Meta::MemAcc_ZeroPage_Y:
    {
        state.address = (Bitwise::BytesToWord(state.operand) + registers.Y) & 0xff;
        co_yield WaitClock();
    }break;
    case Meta::MemAcc_Relative:
    {
        state.address = registers.PC + SByte( state.operand[0] );
        state.nextPage = Bitwise::ClrLoByte16(registers.PC) != Bitwise::ClrLoByte16(state.address);
        // TODO:
    }break;
    case Meta::MemAcc_Absolute:
    {
        state.address = Bitwise::BytesToWord(state.operand);
    }break;
    case Meta::MemAcc_Absolute_X:
    {
        state.helper16  = Bitwise::BytesToWord(state.operand);
        state.address = state.helper16  + registers.X;
        state.nextPage = Bitwise::ClrLoByte16(state.helper16 ) != Bitwise::ClrLoByte16(state.address);
    }break;
    case Meta::MemAcc_Absolute_Y:
    {
        state.helper16  = Bitwise::BytesToWord(state.operand);
        state.address = state.helper16  + registers.Y;
        state.nextPage = Bitwise::ClrLoByte16(state.helper16 ) != Bitwise::ClrLoByte16(state.address);
    }break;
    case Meta::MemAcc_Indirect:
    {
        // Cycle 3
        state.helper16  = Bitwise::BytesToWord(state.operand);
        state.indirect[0] = memory_->Read( state.helper16  );
        co_yield WaitClock();

        // Cycle 4
        if ( Bitwise::LoByte16(state.helper16) == 0xff &&
             cfg.originalIndirectFetch)
        {
            // An original 6502 has does not correctly fetch the target
            // address if the indirect vector falls on a page boundary
            state.indirect[1] = memory_->Read( Bitwise::ClrLoByte16(state.helper16) );
        }
        else
        {
            state.indirect[1] = memory_->Read( state.helper16 + 1 );
        }
        state.address = Bitwise::BytesToWord(state.indirect);
        co_yield WaitClock();
        // TODO:
    }break;
    case Meta::MemAcc_Indexed_X:
    {
        // Cycle 3
        state.helper16  = (Bitwise::BytesToWord(state.operand) + registers.X) & 0xff;
        state.indirect[0] = memory_->Read( state.helper16 );
        co_yield WaitClock();

        // Cycle 4
        state.indirect[1] = memory_->Read( (state.helper16 + 1) & 0xff );
        co_yield WaitClock();

        // Cycle 5
        state.address = Bitwise::BytesToWord(state.indirect);
        co_yield WaitClock();
    }break;
    case Meta::MemAcc_Indexed_Y:
    {
        // Cycle 3
        state.helper16 = Bitwise::BytesToWord(state.operand);
        state.indirect[0] = memory_->Read( state.helper16 );
        co_yield WaitClock();

        // Cycle 4
        state.indirect[1] = memory_->Read( (state.helper16 + 1) & 0xff );
        state.helper16 = Bitwise::BytesToWord(state.indirect);
        state.address = state.helper16  + registers.Y;
        state.nextPage = Bitwise::ClrLoByte16(state.helper16) != Bitwise::ClrLoByte16(state.address);
        co_yield WaitClock();
        // TODO: STA exception in cycles
    }break;
    case Meta::MemAcc_Aux_ResetVec:
    {
        // Cycle 1
        state.indirect[0] = memory_->Read( 0xfffe );
        co_yield WaitClock();

        // Cycle 2
        state.indirect[1] = memory_->Read( 0xffff );
        co_yield WaitClock();

        state.address = Bitwise::BytesToWord(state.indirect);
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
    co_yield WaitClock();

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
    co_yield WaitClock();

    if (state.addressMode & (
            Meta::MemAcc_Absolute_X |
            Meta::MemAcc_Absolute_Y)
        )
    {
        // Cycle 2
        co_yield WaitClock();
    }

    co_return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// ADC: Add with Carry
Routine Cpu6502::Impl::Instr_ADC()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    state.helper16 = registers.A + state.data + registers.Carry();
    state.helper8 = Byte(state.helper16);

    registers.SetCarry( state.helper16 > 0xff ? 1 : 0 );
    registers.SetZero( state.helper8 == 0 ? 1 : 0 );

    registers.SetOverflow(
                (Bitwise::Sign8(registers.A) == Bitwise::Sign8(state.data)) &&
                (Bitwise::Sign8(registers.A) != Bitwise::Sign8(state.helper8))
                ? 1 : 0);

    registers.SetNegative( Bitwise::Sign8(state.helper8) );

    registers.A = state.helper8;

    co_return;
}

// AND: Logical AND
Routine Cpu6502::Impl::Instr_AND()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    registers.A &= state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );

    co_return;
}

// ASL: Arithmetic Shift Left
Routine Cpu6502::Impl::Instr_ASL()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    registers.SetCarry( Bitwise::Bit8(state.data, 7) );

    state.data <<= 1;

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    // Cycle 1
    co_yield WaitClock();

    WaitRoutine( WriteData() );

    co_return;
}

// BCC: Branch if Carry Clear
Routine Cpu6502::Impl::Instr_BCC()
{
    WaitRoutine( ReadAddress() );

    if (registers.Carry())
    {
        co_return;
    }

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    // Cycle 1/2
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// BCS: Branch if Carry Set
Routine Cpu6502::Impl::Instr_BCS()
{
    WaitRoutine( ReadAddress() );

    if ( ! registers.Carry())
    {
        co_return;
    }

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    // Cycle 1/2
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}


// BEQ: Branch if Equal
Routine Cpu6502::Impl::Instr_BEQ()
{
    WaitRoutine( ReadAddress() );

    if ( ! registers.Equal())
    {
        co_return;
    }

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    // Cycle 1/2
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// BIT: Bit Test
Routine Cpu6502::Impl::Instr_BIT()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    registers.SetZero( (registers.A & state.data) == 0 ? 1 : 0 );
    registers.SetOverflow( Bitwise::Bit8(state.data, 6) );
    registers.SetNegative( Bitwise::Bit8(state.data, 7) );

    co_return;
}

// BMI: Branch if Minus
Routine Cpu6502::Impl::Instr_BMI()
{
    WaitRoutine( ReadAddress() );

    if (registers.Positive())
    {
        co_return;
    }

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    // Cycle 1/2
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// BNE: Branch if Not Equal
Routine Cpu6502::Impl::Instr_BNE()
{
    WaitRoutine( ReadAddress() );

    if (registers.Equal())
    {
        co_return;
    }

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    // Cycle 1/2
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// BPL: Branch if Positive
Routine Cpu6502::Impl::Instr_BPL()
{
    WaitRoutine( ReadAddress() );

    if (registers.Negative())
    {
        co_return;
    }

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    // Cycle 1/2
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// BRK: Force Interrupt
Routine Cpu6502::Impl::Instr_BRK()
{
    // Cycle 1
    memory_->Write( registers.StackAddress(), Bitwise::HiByte16( registers.PC + 2 ) );
    registers.ApplyPush();
    co_yield WaitClock();

    // Cycle 2
    memory_->Write( registers.StackAddress(), Bitwise::LoByte16( registers.PC + 2 ) );
    registers.ApplyPush();
    co_yield WaitClock();

    // Cycle 3
    memory_->Write( registers.StackAddress(), registers.S | Meta::FlagPos_Break );
    co_yield WaitClock();

    state.addressMode = Meta::MemAcc_Aux_ResetVec;
    WaitRoutine( ReadAddress() );

    // Cycle 4
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// BVC: Branch if Overflow Clear
Routine Cpu6502::Impl::Instr_BVC()
{
    WaitRoutine( ReadAddress() );

    if (registers.Overflow())
    {
        co_return;
    }

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    // Cycle 1/2
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// BVS: Branch if Overflow Set
Routine Cpu6502::Impl::Instr_BVS()
{
    WaitRoutine( ReadAddress() );

    if ( ! registers.Overflow())
    {
        co_return;
    }

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    // Cycle 1/2
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// CLC: Clear Carry Flag
Routine Cpu6502::Impl::Instr_CLC()
{
    // Cycle 1
    registers.SetCarry( 0 );
    co_yield WaitClock();

    co_return;
}

// CLD: Clear Decimal Mode
Routine Cpu6502::Impl::Instr_CLD()
{
    // Cycle 1
    registers.SetDecimal( 0 );
    co_yield WaitClock();

    co_return;
}

// CLI: Clear Interrupt Disable
Routine Cpu6502::Impl::Instr_CLI()
{
    // Cycle 1
    registers.SetInterrupt( 0 );
    co_yield WaitClock();

    co_return;
}

// CLV: Clear Overflow Flag
Routine Cpu6502::Impl::Instr_CLV()
{
    // Cycle 1
    registers.SetOverflow( 0 );
    co_yield WaitClock();

    co_return;
}

// CMP: Compare
Routine Cpu6502::Impl::Instr_CMP()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    registers.SetCarry( registers.A >= state.data ? 1 : 0);
    registers.SetZero( registers.A == state.data ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A - state.data) );

    co_return;
}

// CPX: Compare X Register
Routine Cpu6502::Impl::Instr_CPX()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    registers.SetCarry( registers.X >= state.data ? 1 : 0);
    registers.SetZero( registers.X == state.data ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.X - state.data) );

    co_return;
}

// CPY: Compare Y Register
Routine Cpu6502::Impl::Instr_CPY()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    registers.SetCarry( registers.Y >= state.data ? 1 : 0);
    registers.SetZero( registers.Y == state.data ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.Y - state.data) );

    co_return;
}

// DEC: Decrement Memory
Routine Cpu6502::Impl::Instr_DEC()
{
    WaitRoutine( ReadAddress() );

    // Do not apply state.nextPage, just skip it

    WaitRoutine( ReadData() );

    // Cycle 1
    state.data--;
    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );
    co_yield WaitClock();

    WaitRoutine( WriteData() );

    co_return;
}

// DEX: Decrement X Register
Routine Cpu6502::Impl::Instr_DEX()
{
    // Cycle 1
    registers.X--;
    registers.SetZero( registers.X == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.X ));
    co_yield WaitClock();

    co_return;
}

// DEY: Decrement Y Register
Routine Cpu6502::Impl::Instr_DEY()
{
    // Cycle 1
    registers.Y--;
    registers.SetZero( registers.Y == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.Y ));
    co_yield WaitClock();

    co_return;
}

// EOR: Exclusive OR
Routine Cpu6502::Impl::Instr_EOR()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    registers.A ^= state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );

    co_return;
}

// INC: Increment Memory
Routine Cpu6502::Impl::Instr_INC()
{
    WaitRoutine( ReadAddress() );

    // Do not apply state.nextPage, just skip it

    WaitRoutine( ReadData() );

    // Cycle 1
    state.data++;
    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );
    co_yield WaitClock();

    WaitRoutine( WriteData() );

    co_return;
}

// INX: Increment X Register
Routine Cpu6502::Impl::Instr_INX()
{
    // Cycle 1
    registers.X++;
    registers.SetZero( registers.X == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.X ));
    co_yield WaitClock();

    co_return;
}

// INY: Increment Y Register
Routine Cpu6502::Impl::Instr_INY()
{
    // Cycle 1
    registers.Y++;
    registers.SetZero( registers.Y == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.Y ));
    co_yield WaitClock();

    co_return;
}

// JMP: Jump
Routine Cpu6502::Impl::Instr_JMP()
{
    WaitRoutine( ReadAddress() );
    registers.PC = state.address;

    co_return;
}

// JSR: Jump to Subroutine
Routine Cpu6502::Impl::Instr_JSR()
{
    WaitRoutine( ReadAddress() );

    // Cycle 1
    memory_->Write( registers.StackAddress(), Bitwise::HiByte16( registers.PC - 1 ) );
    registers.ApplyPush();
    co_yield WaitClock();

    // Cycle 2
    memory_->Write( registers.StackAddress(), Bitwise::LoByte16( registers.PC - 1 ) );
    registers.ApplyPush();
    co_yield WaitClock();

    // Cycle 3
    registers.PC = state.address;
    co_yield WaitClock();

    co_return;
}

// LDA: Load Accumulator
Routine Cpu6502::Impl::Instr_LDA()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    registers.A = state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );

    co_return;
}

// LDX: Load X Register
Routine Cpu6502::Impl::Instr_LDX()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    registers.X = state.data;

    registers.SetZero( registers.X == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.X) );

    co_return;
}

// LDY: Load Y Register
Routine Cpu6502::Impl::Instr_LDY()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    registers.Y = state.data;

    registers.SetZero( registers.Y == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.Y) );

    co_return;
}

// LSR: Logical Shift Right
Routine Cpu6502::Impl::Instr_LSR()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    registers.SetCarry( Bitwise::Bit8(state.data, 0) );

    state.data >>= 1;

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    // Cycle 1
    co_yield WaitClock();

    WaitRoutine( WriteData() );

    co_return;
}

// NOP: No Operation
Routine Cpu6502::Impl::Instr_NOP()
{
    // Cycle 1
    co_yield WaitClock();

    co_return;
}

// ORA: Logical Inclusive OR
Routine Cpu6502::Impl::Instr_ORA()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    registers.A |= state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );

    co_return;
}

// PHA: Push Accumulator
Routine Cpu6502::Impl::Instr_PHA()
{
    // Cycle 1
    memory_->Write( registers.StackAddress(), registers.A );
    co_yield WaitClock();

    // Cycle 2
    registers.ApplyPush();
    co_yield WaitClock();

    co_return;
}

// PHP: Push Processor Status
Routine Cpu6502::Impl::Instr_PHP()
{
    // Cycle 1
    memory_->Write( registers.StackAddress(), registers.S );
    co_yield WaitClock();

    // Cycle 2
    registers.ApplyPush();
    co_yield WaitClock();

    co_return;
}

// PLA: Pull Accumulator
Routine Cpu6502::Impl::Instr_PLA()
{
    // Cycle 1
    registers.ApplyPool();
    co_yield WaitClock();

    // Cycle 2
    registers.A = memory_->Read( registers.StackAddress() );
    co_yield WaitClock();

    // Cycle 3
    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );
    co_yield WaitClock();

    co_return;
}

// PLP: Pull Processor Status
Routine Cpu6502::Impl::Instr_PLP()
{
    // Cycle 1
    registers.ApplyPool();
    co_yield WaitClock();

    // Cycle 2
    registers.S = memory_->Read( registers.StackAddress() );
    co_yield WaitClock();

    // Cycle 3
    registers.SetUnusedFlag();
    co_yield WaitClock();

    co_return;
}

// ROL: Rotate Left
Routine Cpu6502::Impl::Instr_ROL()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    state.helper8 = registers.Carry();
    registers.SetCarry( Bitwise::Bit8(state.data, 7) );

    state.data <<= 1;
    Bitwise::SetBit8(state.data, 0, state.helper8);

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    // Cycle 1
    co_yield WaitClock();

    WaitRoutine( WriteData() );

    co_return;
}

// ROR: Rotate Right
Routine Cpu6502::Impl::Instr_ROR()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    state.helper8 = registers.Carry();
    registers.SetCarry( Bitwise::Bit8(state.data, 0) );

    state.data >>= 1;
    Bitwise::SetBit8(state.data, 7, state.helper8);

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    // Cycle 1
    co_yield WaitClock();

    WaitRoutine( WriteData() );

    co_return;
}

// RTI: Return from Interrupt
Routine Cpu6502::Impl::Instr_RTI()
{
    // Cycle 1
    registers.ApplyPool();
    state.helper8 = memory_->Read( registers.StackAddress() );
    co_yield WaitClock();

    // Cycle 2
    registers.S = state.helper8;
    registers.SetUnusedFlag();
    registers.SetBreak();
    co_yield WaitClock();

    // Cycle 3
    registers.ApplyPool();
    Bitwise::SetLoByte16(registers.PC, memory_->Read(registers.StackAddress()) );
    co_yield WaitClock();

    // Cycle 4
    registers.ApplyPool();
    Bitwise::SetHiByte16(registers.PC, memory_->Read(registers.StackAddress()) );
    co_yield WaitClock();

    // Cycle 5
    co_yield WaitClock();

    co_return;
}

// RTS: Return from Subroutine
Routine Cpu6502::Impl::Instr_RTS()
{
    // Cycle 1
    registers.ApplyPool();
    state.helper8 = memory_->Read( registers.StackAddress() );
    co_yield WaitClock();

    // Cycle 2
    registers.S = state.helper8;
    registers.SetUnusedFlag();
    registers.SetBreak();
    co_yield WaitClock();

    // Cycle 3
    registers.ApplyPool();
    Bitwise::SetLoByte16(registers.PC, memory_->Read(registers.StackAddress()) );
    co_yield WaitClock();

    // Cycle 4
    registers.ApplyPool();
    Bitwise::SetHiByte16(registers.PC, memory_->Read(registers.StackAddress()) );
    registers.PC++;
    co_yield WaitClock();

    // Cycle 5
    co_yield WaitClock();

    co_return;
}

// SBC: Subtract with Carry
Routine Cpu6502::Impl::Instr_SBC()
{
    WaitRoutine( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        co_yield WaitClock();
    }

    WaitRoutine( ReadData() );

    // not carry
    state.helper8 = registers.A - state.data - (registers.Carry() ? 0 : 1);

    registers.SetCarry(
        (registers.A >= (state.data + (registers.Carry() ? 0 : 1)))
        ? 1 : 0);

    registers.SetZero( state.helper8 == 0 ? 1 : 0 );

    registers.SetOverflow(
        (Bitwise::Sign8(registers.A) != Bitwise::Sign8(state.data)) &&
        (Bitwise::Sign8(registers.A) != Bitwise::Sign8(state.helper8))
        ? 1:0);

    registers.SetNegative( Bitwise::Sign8( state.helper8 ) );

    registers.A = state.helper8;

    co_return;
}

// SEC: Set Carry Flag
Routine Cpu6502::Impl::Instr_SEC()
{
    // Cycle 1
    registers.SetCarry( 1 );
    co_yield WaitClock();

    co_return;
}

// SED: Set Decimal Flag
Routine Cpu6502::Impl::Instr_SED()
{
    // Cycle 1
    registers.SetDecimal( 1 );
    co_yield WaitClock();

    co_return;
}

// SEI: Set Interrupt Disable
Routine Cpu6502::Impl::Instr_SEI()
{
    // Cycle 1
    registers.SetInterrupt( 1 );
    co_yield WaitClock();

    co_return;
}

// STA: Store Accumulator
Routine Cpu6502::Impl::Instr_STA()
{
    WaitRoutine( ReadAddress() );
    if ( state.addressMode == Meta::MemAcc_Indexed_Y )
    {
        // Cycle 1
        // TODO: Verify it!
        co_yield WaitClock();
    }
    state.data = registers.A;
    WaitRoutine( WriteData() );

    co_return;
}

// STX: Store X Register
Routine Cpu6502::Impl::Instr_STX()
{
    WaitRoutine( ReadAddress() );
    state.data = registers.X;
    WaitRoutine( WriteData() );

    co_return;
}

// STY: Store Y Register
Routine Cpu6502::Impl::Instr_STY()
{
    WaitRoutine( ReadAddress() );
    state.data = registers.Y;
    WaitRoutine( WriteData() );

    co_return;
}

// TAX: Transfer Accumulator to X
Routine Cpu6502::Impl::Instr_TAX()
{
    // Cycle 1
    registers.X = registers.A;
    registers.SetZero( registers.X == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.X ) );
    co_yield WaitClock();

    co_return;
}

// TAY: Transfer Accumulator to Y
Routine Cpu6502::Impl::Instr_TAY()
{
    // Cycle 1
    registers.Y = registers.A;
    registers.SetZero( registers.Y == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.Y ) );
    co_yield WaitClock();

    co_return;
}

// TSX: Transfer Stack Pointer to X
Routine Cpu6502::Impl::Instr_TSX()
{
    // Cycle 1
    registers.X = registers.SP;
    registers.SetZero( registers.X == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.X ) );
    co_yield WaitClock();

    co_return;
}

// TXA: Transfer X to Accumulator
Routine Cpu6502::Impl::Instr_TXA()
{
    // Cycle 1
    registers.A = registers.X;
    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.A ) );
    co_yield WaitClock();

    co_return;
}

// TXS: Transfer X to Stack Pointer
Routine Cpu6502::Impl::Instr_TXS()
{
    // Cycle 1
    registers.SP = registers.X;
    co_yield WaitClock();

    co_return;
}

// TYA: Transfer Y to Accumulator
Routine Cpu6502::Impl::Instr_TYA()
{
    // Cycle 1
    registers.A = registers.Y;
    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8( registers.A ) );
    co_yield WaitClock();

    co_return;
}

}


