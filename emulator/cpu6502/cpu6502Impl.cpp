#include "cpu6502Impl.h"
#include <common/bitwise.h>

#define FLAG_Carry()        Bitwise::Bit(registers.S, Meta::FlagPos_Carry    )
#define FLAG_Zero()         Bitwise::Bit(registers.S, Meta::FlagPos_Zero     )
#define FLAG_Interrupt()    Bitwise::Bit(registers.S, Meta::FlagPos_Interrupt)
#define FLAG_Decimal()      Bitwise::Bit(registers.S, Meta::FlagPos_Decimal  )
#define FLAG_Break()        Bitwise::Bit(registers.S, Meta::FlagPos_Break    )
#define FLAG_Overflow()     Bitwise::Bit(registers.S, Meta::FlagPos_Overflow )
#define FLAG_Negative()     Bitwise::Bit(registers.S, Meta::FlagPos_Negative )

#define FLAG_SET_Carry(v)       Bitwise::SetBit(registers.S, Meta::FlagPos_Carry    , v)
#define FLAG_SET_Zero(v)        Bitwise::SetBit(registers.S, Meta::FlagPos_Zero     , v)
#define FLAG_SET_Interrupt(v)   Bitwise::SetBit(registers.S, Meta::FlagPos_Interrupt, v)
#define FLAG_SET_Decimal(v)     Bitwise::SetBit(registers.S, Meta::FlagPos_Decimal  , v)
#define FLAG_SET_Break(v)       Bitwise::SetBit(registers.S, Meta::FlagPos_Break    , v)
#define FLAG_SET_Overflow(v)    Bitwise::SetBit(registers.S, Meta::FlagPos_Overflow , v)
#define FLAG_SET_Negative(v)    Bitwise::SetBit(registers.S, Meta::FlagPos_Negative , v)

#define FLAGVALUE_ADD_C(a, b, abWord) (((abWord) > 0xff) ? 1 : 0)
#define FLAGVALUE_ADD_Z(a, b, abWord) ((Byte(abWord) == 0) ? 1 : 0)
#define FLAGVALUE_ADD_N(a, b, abWord) (Bitwise::Sign(abWord))
#define FLAGVALUE_ADD_V(a, b, abWord) (( (Bitwise::Sign(a) == Bitwise::Sign(b)) && (Bitwise::Sign(abWord) != Bitwise::Sign(a)) ) ? 1 : 0)

#define FLAGVALUE_SUB_C(a, b, diffWord) (((a) >= (b)) ? 1 : 0)
#define FLAGVALUE_SUB_Z(a, b, diffWord) ((Byte(diffWord) == 0) ? 1 : 0)
#define FLAGVALUE_SUB_N(a, b, diffWord) (Bitwise::Sign(diffWord))
#define FLAGVALUE_SUB_V(a, b, diffWord) (((Bitwise::Sign(a) != Bitwise::Sign(b)) && (Bitwise::Sign(diffWord) != Bitwise::Sign(a))) ? 1 : 0)

#define FLAGVALUE_CMP_C(a, b, diffWord) (((a) >= (b)) ? 1 : 0)
#define FLAGVALUE_CMP_Z(a, b, diffWord) ((Byte(diffWord) == 0) ? 1 : 0)
#define FLAGVALUE_CMP_N(a, b, diffWord) (Bitwise::Sign(diffWord))

// ++
#define FLAGVALUE_INC_Z(result) ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_INC_N(result) (Bitwise::Sign(result))

// --
#define FLAGVALUE_DEC_Z(result) ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_DEC_N(result) (Bitwise::Sign(result))

// AND, ORA, EOR
#define FLAGVALUE_LOGIC_Z(result) ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_LOGIC_N(result) (Bitwise::Sign(result))

// ASL (Arithmetic Shift Left)
#define FLAGVALUE_ASL_C(original) Bitwise::Bit(original, 7)
#define FLAGVALUE_ASL_Z(result)   ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_ASL_N(result)   (Bitwise::Sign(result))

// LSR (Logical Shift Right)
#define FLAGVALUE_LSR_C(original) Bitwise::Bit(original, 0)
#define FLAGVALUE_LSR_Z(result)   ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_LSR_N(result)   (Bitwise::Sign(result))  // Always 0, because bit 7 is zero

// ROL (Rotate Left)
#define FLAGVALUE_ROL_C(original) Bitwise::Bit(original, 7)
#define FLAGVALUE_ROL_Z(result)   ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_ROL_N(result)   (Bitwise::Sign(result))

// ROR (Rotate Right)
#define FLAGVALUE_ROR_C(original) Bitwise::Bit(original, 0)
#define FLAGVALUE_ROR_Z(result)   ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_ROR_N(result)   (Bitwise::Sign(result))

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

Routine Cpu6502::Impl::Instr_ADC()
{
    Execute( ReadAddress() );

    if (state.nextPage)
    {
        // Cycle 1
        WaitClock();
    }

    Execute( ReadData() );

    state.helper16 = registers.A + state.data + FLAG_Carry();
    state.helper8 = Byte(state.helper16);

    FLAG_SET_Carry( state.helper16 > 255 ? 1 : 0 );
    FLAG_SET_Zero( state.helper8 == 0 ? 1 : 0 );

    FLAG_SET_Overflow(
                (Bitwise::Sign(registers.A) == Bitwise::Sign(state.data)) &&
                (Bitwise::Sign(registers.A) != Bitwise::Sign(state.helper8))
                ? 1 : 0);

    FLAG_SET_Negative( Bitwise::Sign(state.helper8) );

    registers.A = state.helper8;

    co_return;
}

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

    FLAG_SET_Zero( registers.A == 0 ? 1 : 0 );
    FLAG_SET_Negative( Bitwise::Sign(registers.A) );

    co_return;
}

Routine Cpu6502::Impl::Instr_ASL()
{
    Execute( ReadAddress() );
    Execute( ReadData() );

    FLAG_SET_Carry( Bitwise::Bit(state.data, 7) );

    state.data <<= 1;

    FLAG_SET_Zero( state.data == 0 ? 1 : 0 );
    FLAG_SET_Negative( Bitwise::Sign(state.data) );

    WaitClock();

    Execute( WriteData() );

    co_return;
}

Routine Cpu6502::Impl::Instr_BCC()
{
    Execute( ReadAddress() );

    if (FLAG_Carry())
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

Routine Cpu6502::Impl::Instr_BCS()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BCS")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BEQ()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BEQ")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BIT()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BIT")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BMI()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BMI")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BNE()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BNE")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BPL()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BPL")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BRK()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BRK")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BVC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BVC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BVS()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BVS")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CLC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CLD()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLD")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CLI()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLI")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CLV()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLV")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CMP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CMP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CPX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CPX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CPY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CPY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_DEC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_DEX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_DEY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_EOR()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "EOR")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_INC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_INX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_INY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_JMP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "JMP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_JSR()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "JSR")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_LDA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_LDX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_LDY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_LSR()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LSR")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_NOP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "NOP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_ORA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ORA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_PHA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PHA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_PHP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PHP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_PLA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PLA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_PLP()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PLP")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_ROL()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ROL")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_ROR()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ROR")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_RTI()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "RTI")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_RTS()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "RTS")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_SBC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SBC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_SEC()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SEC")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_SED()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SED")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_SEI()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SEI")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_STA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_STX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_STY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TAX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TAX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TAY()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TAY")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TSX()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TSX")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TXA()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TXA")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TXS()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TXS")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

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


