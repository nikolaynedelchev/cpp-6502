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
    uint16_t memAcc = 1; // TODO: implement it

    // TODO: decode opcode instruction handler
    Execute( Instr_ADC(memAcc) );

    co_return;
}

Routine Cpu6502::Impl::ReadAddress(uint16_t memAcc)
{
    if (memAcc == Meta::MemAcc_Immediate)
    {
        co_return;
    }
    if (memAcc & Meta::MemAcc_Need_1_OrMore_Operand)
    {
        // Cycle 1
        state.operand[0] = memory_->Read( registers.PC );
        registers.PC++;
        WaitClock();
    }
    if (memAcc & Meta::MemAcc_Need_2_Operand)
    {
        // Cycle 2
        state.operand[1] = memory_->Read( registers.PC );
        registers.PC++;
        WaitClock();
    }

    switch (memAcc)
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
        WaitClock();
    }break;
    case Meta::MemAcc_Absolute_X:
    {
        Word a = Bitwise::BytesToWord(state.operand);
        state.address = a + registers.X;
        state.nextPage = Bitwise::ClrLoByte(a) != Bitwise::ClrLoByte(state.address);
        WaitClock();
    }break;
    case Meta::MemAcc_Absolute_Y:
    {
        Word a = Bitwise::BytesToWord(state.operand);
        state.address = a + registers.Y;
        state.nextPage = Bitwise::ClrLoByte(a) != Bitwise::ClrLoByte(state.address);
        WaitClock();
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

Routine Cpu6502::Impl::ReadData(uint16_t memAcc)
{
    if (memAcc == Meta::MemAcc_Immediate)
    {
        state.operand[0] = memory_->Read( registers.PC );
        registers.PC++;
        state.data = state.operand[0];
    }
    else
    {
        state.data = memory_->Read( state.address );
    }

    // Cycle 1
    WaitClock();

    co_return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Routine Cpu6502::Impl::Instr_ADC(uint16_t memAcc)
{
    Execute( ReadAddress(memAcc) );
    Execute( ReadData(memAcc) );

    if (state.nextPage)
    {
        // Cycle 1
        WaitClock();
    }

    Word sum = registers.A + state.data + FLAG_Carry();

    FLAG_SET_Carry( FLAGVALUE_ADD_C(registers.A, state.data, sum) );
    FLAG_SET_Zero( FLAGVALUE_ADD_Z(registers.A, state.data, sum) );
    FLAG_SET_Overflow( FLAGVALUE_ADD_V(registers.A, state.data, sum) );
    FLAG_SET_Negative( FLAGVALUE_ADD_N(registers.A, state.data, sum) );

    co_return;
}

Routine Cpu6502::Impl::Instr_AND(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "AND")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_ASL(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ASL")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BCC(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BCC")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BCS(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BCS")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BEQ(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BEQ")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BIT(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BIT")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BMI(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BMI")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BNE(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BNE")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BPL(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BPL")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BRK(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BRK")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BVC(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BVC")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_BVS(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "BVS")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CLC(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLC")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CLD(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLD")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CLI(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLI")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CLV(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CLV")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CMP(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CMP")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CPX(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CPX")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_CPY(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "CPY")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_DEC(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEC")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_DEX(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEX")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_DEY(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "DEY")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_EOR(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "EOR")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_INC(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INC")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_INX(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INX")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_INY(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "INY")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_JMP(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "JMP")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_JSR(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "JSR")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_LDA(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDA")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_LDX(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDX")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_LDY(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LDY")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_LSR(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "LSR")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_NOP(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "NOP")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_ORA(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ORA")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_PHA(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PHA")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_PHP(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PHP")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_PLA(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PLA")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_PLP(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "PLP")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_ROL(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ROL")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_ROR(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "ROR")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_RTI(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "RTI")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_RTS(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "RTS")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_SBC(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SBC")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_SEC(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SEC")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_SED(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SED")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_SEI(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "SEI")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_STA(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STA")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_STX(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STX")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_STY(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "STY")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TAX(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TAX")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TAY(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TAY")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TSX(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TSX")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TXA(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TXA")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TXS(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TXS")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

Routine Cpu6502::Impl::Instr_TYA(uint16_t memAcc)
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "TYA")
        .Msg("MemMode: {}", memAcc)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}

}


