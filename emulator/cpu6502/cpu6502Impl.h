#pragma once
#include "cpu6502.h"
#include <common/routine.h>
#include <common/bitwise.h>

#define WaitClock() co_yield Routine::Empty{}
#define WaitRoutine(r) while(r.Resume()){co_yield Routine::Empty{};}
#define Execute(func) {auto r = func; WaitRoutine(r);}

namespace cpp6502
{

struct Cpu6502::Impl
{
    struct Meta;
    struct Instruction;
    using InstructionRoutine = Routine (Cpu6502::Impl::*)();


    Impl(IMemory* memory);

    void Reset();
    void Clock();

    struct
    {
        Address PC = 0;
        Byte SP = 0;
        Byte A = 0;
        Byte X = 0;
        Byte Y = 0;
        Byte S = 0;

        inline Byte Carry()                 noexcept { return Bitwise::Bit(S, 0 ) ;}
        inline Byte Zero()                  noexcept { return Bitwise::Bit(S, 1 ) ;}
        inline Byte Interrupt()             noexcept { return Bitwise::Bit(S, 2 ) ;}
        inline Byte Decimal()               noexcept { return Bitwise::Bit(S, 3 ) ;}
        inline Byte Break()                 noexcept { return Bitwise::Bit(S, 4 ) ;}
        inline Byte Overflow()              noexcept { return Bitwise::Bit(S, 5 ) ;}
        inline Byte Negative()              noexcept { return Bitwise::Bit(S, 6 ) ;}

        inline void SetCarry(Byte f)        noexcept { Bitwise::SetBit(S, 0, f ) ;}
        inline void SetZero(Byte f)         noexcept { Bitwise::SetBit(S, 1, f ) ;}
        inline void SetInterrupt(Byte f)    noexcept { Bitwise::SetBit(S, 2, f ) ;}
        inline void SetDecimal(Byte f)      noexcept { Bitwise::SetBit(S, 3, f ) ;}
        inline void SetBreak(Byte f)        noexcept { Bitwise::SetBit(S, 4, f ) ;}
        inline void SetOverflow(Byte f)     noexcept { Bitwise::SetBit(S, 5, f ) ;}
        inline void SetNegative(Byte f)     noexcept { Bitwise::SetBit(S, 6, f ) ;}
    } registers;

    struct
    {
        Byte opcode = 0;

        Byte operand[2] = {0, 0};
        Byte indirect[2] = {0, 0};
        bool nextPage = false;

        Word address = 0;
        uint16_t addressMode = 0;

        Byte data = 0;

        Byte helper8;
        Byte helper16;
    } state;

    std::string ToString() const noexcept;

    Routine ResetInstruction();
    Routine StartNewInstruction();
    Routine ReadAddress();
    Routine ReadData();
    Routine WriteData();

    Routine Instr_ADC();
    Routine Instr_AND();
    Routine Instr_ASL();
    Routine Instr_BCC();
    Routine Instr_BCS();
    Routine Instr_BEQ();
    Routine Instr_BIT();
    Routine Instr_BMI();
    Routine Instr_BNE();
    Routine Instr_BPL();
    Routine Instr_BRK();
    Routine Instr_BVC();
    Routine Instr_BVS();
    Routine Instr_CLC();
    Routine Instr_CLD();
    Routine Instr_CLI();
    Routine Instr_CLV();
    Routine Instr_CMP();
    Routine Instr_CPX();
    Routine Instr_CPY();
    Routine Instr_DEC();
    Routine Instr_DEX();
    Routine Instr_DEY();
    Routine Instr_EOR();
    Routine Instr_INC();
    Routine Instr_INX();
    Routine Instr_INY();
    Routine Instr_JMP();
    Routine Instr_JSR();
    Routine Instr_LDA();
    Routine Instr_LDX();
    Routine Instr_LDY();
    Routine Instr_LSR();
    Routine Instr_NOP();
    Routine Instr_ORA();
    Routine Instr_PHA();
    Routine Instr_PHP();
    Routine Instr_PLA();
    Routine Instr_PLP();
    Routine Instr_ROL();
    Routine Instr_ROR();
    Routine Instr_RTI();
    Routine Instr_RTS();
    Routine Instr_SBC();
    Routine Instr_SEC();
    Routine Instr_SED();
    Routine Instr_SEI();
    Routine Instr_STA();
    Routine Instr_STX();
    Routine Instr_STY();
    Routine Instr_TAX();
    Routine Instr_TAY();
    Routine Instr_TSX();
    Routine Instr_TXA();
    Routine Instr_TXS();
    Routine Instr_TYA();

    Routine activeInstruction_;
    IMemory* memory_;
};

struct Cpu6502::Impl::Instruction
{
    Byte opcode = 0;
    std::string name;
    uint16_t memoryMode;
    std::string description;
    Byte size;
    InstructionRoutine instruction;
};

struct Cpu6502::Impl::Meta
{
    static constexpr uint16_t FlagPos_Carry     = 0;
    static constexpr uint16_t FlagPos_Zero      = 1;
    static constexpr uint16_t FlagPos_Interrupt = 2;
    static constexpr uint16_t FlagPos_Decimal   = 3;
    static constexpr uint16_t FlagPos_Break     = 4;
    static constexpr uint16_t FlagPos_Overflow  = 5;
    static constexpr uint16_t FlagPos_Negative  = 6;

    static constexpr uint16_t FlagMask_Carry     = (1 << FlagPos_Carry);
    static constexpr uint16_t FlagMask_Zero      = (1 << FlagPos_Zero);
    static constexpr uint16_t FlagMask_Interrupt = (1 << FlagPos_Interrupt);
    static constexpr uint16_t FlagMask_Decimal   = (1 << FlagPos_Decimal);
    static constexpr uint16_t FlagMask_Break     = (1 << FlagPos_Break);
    static constexpr uint16_t FlagMask_Overflow  = (1 << FlagPos_Overflow);
    static constexpr uint16_t FlagMask_Negative  = (1 << FlagPos_Negative);

    static constexpr uint16_t MemAcc_Implied        = ( 0 << 0  );
    static constexpr uint16_t MemAcc_Immediate      = ( 1 << 0  );
    static constexpr uint16_t MemAcc_Accumulator    = ( 1 << 1  );
    static constexpr uint16_t MemAcc_ZeroPage       = ( 1 << 2  );
    static constexpr uint16_t MemAcc_ZeroPage_X     = ( 1 << 3  );
    static constexpr uint16_t MemAcc_ZeroPage_Y     = ( 1 << 4  );
    static constexpr uint16_t MemAcc_Relative       = ( 1 << 5  );
    static constexpr uint16_t MemAcc_Absolute       = ( 1 << 6  );
    static constexpr uint16_t MemAcc_Absolute_X     = ( 1 << 7  );
    static constexpr uint16_t MemAcc_Absolute_Y     = ( 1 << 8  );
    static constexpr uint16_t MemAcc_Indirect       = ( 1 << 9  );
    static constexpr uint16_t MemAcc_Indexed_X      = ( 1 << 10 );
    static constexpr uint16_t MemAcc_Indexed_Y      = ( 1 << 11 );
    static constexpr uint16_t MemAcc_All            = ( 1 << 12 ) - 1;

    static constexpr uint16_t MemAcc_INVALID        = ( 0xffff  );

    using InstructionsLookup = Instruction[256];
    static const InstructionsLookup& Instructions();
};


}
