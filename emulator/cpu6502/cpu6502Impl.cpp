#include "cpu6502Impl.h"

#define BYTES_TO_WORD(bytes) (Word(bytes[0]) | (Word(bytes[1]) << 8))
#define HI_BYTE(word) ((word) >> 8)
#define LO_BYTE(word) ((word) & 0xff)

#define CLR_LO_BYTE(word) ((word) & (Word(0xff) << 8))
#define CLR_HI_BYTE(word) ((word) & Word(0xff))

#define BIT(v, b) ((Byte(v) & (1 << (b))) >> (b))
#define BIT_SET(v, b, newV) ((newV) ? ((v) | (1 << (b))) : ((v) & ~(1 << (b))))

#define SIGN(v) (BIT(v, 7))

#define FLAG_Carry()        BIT(registers.S, FlagPos_Carry    )
#define FLAG_Zero()         BIT(registers.S, FlagPos_Zero     )
#define FLAG_Interrupt()    BIT(registers.S, FlagPos_Interrupt)
#define FLAG_Decimal()      BIT(registers.S, FlagPos_Decimal  )
#define FLAG_Break()        BIT(registers.S, FlagPos_Break    )
#define FLAG_Overflow()     BIT(registers.S, FlagPos_Overflow )
#define FLAG_Negative()     BIT(registers.S, FlagPos_Negative )

#define FLAG_SET_Carry(v)       BIT_SET(registers.S, FlagPos_Carry    , v)
#define FLAG_SET_Zero(v)        BIT_SET(registers.S, FlagPos_Zero     , v)
#define FLAG_SET_Interrupt(v)   BIT_SET(registers.S, FlagPos_Interrupt, v)
#define FLAG_SET_Decimal(v)     BIT_SET(registers.S, FlagPos_Decimal  , v)
#define FLAG_SET_Break(v)       BIT_SET(registers.S, FlagPos_Break    , v)
#define FLAG_SET_Overflow(v)    BIT_SET(registers.S, FlagPos_Overflow , v)
#define FLAG_SET_Negative(v)    BIT_SET(registers.S, FlagPos_Negative , v)

#define FLAGVALUE_ADD_C(a, b, abWord) (((abWord) > 0xff) ? 1 : 0)
#define FLAGVALUE_ADD_Z(a, b, abWord) ((Byte(abWord) == 0) ? 1 : 0)
#define FLAGVALUE_ADD_N(a, b, abWord) (SIGN(abWord))
#define FLAGVALUE_ADD_V(a, b, abWord) (( (SIGN(a) == SIGN(b)) && (SIGN(abWord) != SIGN(a)) ) ? 1 : 0)

#define FLAGVALUE_SUB_C(a, b, diffWord) (((a) >= (b)) ? 1 : 0)
#define FLAGVALUE_SUB_Z(a, b, diffWord) ((Byte(diffWord) == 0) ? 1 : 0)
#define FLAGVALUE_SUB_N(a, b, diffWord) (SIGN(diffWord))
#define FLAGVALUE_SUB_V(a, b, diffWord) (((SIGN(a) != SIGN(b)) && (SIGN(diffWord) != SIGN(a))) ? 1 : 0)

#define FLAGVALUE_CMP_C(a, b, diffWord) (((a) >= (b)) ? 1 : 0)
#define FLAGVALUE_CMP_Z(a, b, diffWord) ((Byte(diffWord) == 0) ? 1 : 0)
#define FLAGVALUE_CMP_N(a, b, diffWord) (SIGN(diffWord))

// ++
#define FLAGVALUE_INC_Z(result) ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_INC_N(result) (SIGN(result))

// --
#define FLAGVALUE_DEC_Z(result) ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_DEC_N(result) (SIGN(result))

// AND, ORA, EOR
#define FLAGVALUE_LOGIC_Z(result) ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_LOGIC_N(result) (SIGN(result))

// ASL (Arithmetic Shift Left)
#define FLAGVALUE_ASL_C(original) BIT(original, 7)
#define FLAGVALUE_ASL_Z(result)   ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_ASL_N(result)   (SIGN(result))

// LSR (Logical Shift Right)
#define FLAGVALUE_LSR_C(original) BIT(original, 0)
#define FLAGVALUE_LSR_Z(result)   ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_LSR_N(result)   (SIGN(result))  // Always 0, because bit 7 is zero

// ROL (Rotate Left)
#define FLAGVALUE_ROL_C(original) BIT(original, 7)
#define FLAGVALUE_ROL_Z(result)   ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_ROL_N(result)   (SIGN(result))

// ROR (Rotate Right)
#define FLAGVALUE_ROR_C(original) BIT(original, 0)
#define FLAGVALUE_ROR_Z(result)   ((Byte(result) == 0) ? 1 : 0)
#define FLAGVALUE_ROR_N(result)   (SIGN(result))

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

Routine Cpu6502::Impl::ResetInstruction()
{
    state.operand[0] = memory_->Read(0xFFFC);
    WaitClock();

    state.operand[1] = memory_->Read(0xFFFD);
    registers.PC = BYTES_TO_WORD(state.operand);
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
    Execute( Op_ADC(memAcc) );

    co_return;
}

Routine Cpu6502::Impl::ReadAddress(uint16_t memAcc)
{
    if (memAcc == MemAcc_Immediate)
    {
        co_return;
    }
    if (memAcc & MemAcc_Need_1_OrMore_Operand)
    {
        // Cycle 1
        state.operand[0] = memory_->Read( registers.PC );
        registers.PC++;
        WaitClock();
    }
    if (memAcc & MemAcc_Need_2_Operand)
    {
        // Cycle 2
        state.operand[1] = memory_->Read( registers.PC );
        registers.PC++;
        WaitClock();
    }

    switch (memAcc)
    {
    case MemAcc_ZeroPage:
    {
        state.address = BYTES_TO_WORD(state.operand);
    }break;
    case MemAcc_ZeroPage_X:
    {
        state.address = (BYTES_TO_WORD(state.operand) + registers.X) % 0xff;
        WaitClock();
    }break;
    case MemAcc_ZeroPage_Y:
    {
        state.address = (BYTES_TO_WORD(state.operand) + registers.Y) % 0xff;
        WaitClock();
    }break;
    case MemAcc_Relative:
    {
        state.address = registers.PC + SByte( state.operand[0] );
        state.nextPage = CLR_LO_BYTE(registers.PC) != CLR_LO_BYTE(state.address);
        // TODO:
    }break;
    case MemAcc_Absolute:
    {
        state.address = BYTES_TO_WORD(state.operand);
        WaitClock();
    }break;
    case MemAcc_Absolute_X:
    {
        Word a = BYTES_TO_WORD(state.operand);
        state.address = a + registers.X;
        state.nextPage = CLR_LO_BYTE(a) != CLR_LO_BYTE(state.address);
        WaitClock();
    }break;
    case MemAcc_Absolute_Y:
    {
        Word a = BYTES_TO_WORD(state.operand);
        state.address = a + registers.Y;
        state.nextPage = CLR_LO_BYTE(a) != CLR_LO_BYTE(state.address);
        WaitClock();
    }break;
    case MemAcc_Indirect:
    {
        // Cycle 3
        Word a = BYTES_TO_WORD(state.operand);
        state.indirect[0] = memory_->Read( a );
        WaitClock();

        // Cycle 4
        state.indirect[1] = memory_->Read( a + 1);
        state.address = BYTES_TO_WORD(state.indirect);
        WaitClock();
        // TODO:
    }break;
    case MemAcc_Indexed_X:
    {
        // Cycle 3
        Word a = (BYTES_TO_WORD(state.operand) + registers.X) % 0xff;
        state.indirect[0] = memory_->Read( a );
        WaitClock();

        // Cycle 4
        state.indirect[1] = memory_->Read( (a + 1) % 0xff );
        WaitClock();

        // Cycle 5
        state.address = BYTES_TO_WORD(state.indirect);
        WaitClock();
    }break;
    case MemAcc_Indexed_Y:
    {
        // Cycle 3
        Word zeroPage = BYTES_TO_WORD(state.operand);
        state.indirect[0] = memory_->Read( zeroPage );
        WaitClock();

        // Cycle 4
        state.indirect[1] = memory_->Read( (zeroPage + 1) % 0xff );
        Word a = BYTES_TO_WORD(state.indirect);
        state.address = a + registers.Y;
        state.nextPage = CLR_LO_BYTE(a) != CLR_LO_BYTE(state.address);
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
    if (memAcc == MemAcc_Immediate)
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

Routine Cpu6502::Impl::Op_ADC(uint16_t memAcc)
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

}

