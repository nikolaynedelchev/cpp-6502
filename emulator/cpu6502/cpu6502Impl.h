#pragma once
#include "cpu6502.h"
#include <common/routine.h>
#include <common/bitwise.h>

#define WaitClock() co_yield Routine::Empty{}
#define WaitRoutine(r) while(r.Resume()){co_yield Routine::Empty{};}
#define Execute(func) {auto r = func; WaitRoutine(r);}
#define CallMember(member) ((*this).*(member))()

namespace cpp6502
{

struct Cpu6502::Impl
{
    struct Meta;
    struct Instruction;
    using InstructionRoutine = Routine (Cpu6502::Impl::*)();


    Impl(IMemory* memory);

    void PowerOn();
    void Reset();
    void Clock();

    struct
    {
        Address PC = 0;
        Byte SP = 0xFF;
        Byte A = 0;
        Byte X = 0;
        Byte Y = 0;
        Byte S = 1 << 5; // UnusedFlag should always read as 1

        inline Byte Carry()         const   noexcept { return Bitwise::Bit(S, 0 ) ;}
        inline Byte Zero()          const   noexcept { return Bitwise::Bit(S, 1 ) ;}
        inline Byte Interrupt()     const   noexcept { return Bitwise::Bit(S, 2 ) ;}
        inline Byte Decimal()       const   noexcept { return Bitwise::Bit(S, 3 ) ;}
        inline Byte Break()         const   noexcept { return 0; }          //4
        inline Byte UnusedFlag()    const   noexcept { return 1; }          //5
        inline Byte Overflow()      const   noexcept { return Bitwise::Bit(S, 6 ) ;}
        inline Byte Negative()      const   noexcept { return Bitwise::Bit(S, 7 ) ;}

        inline void SetCarry(Byte f)        noexcept { Bitwise::SetBit(S, 0, f ) ;}
        inline void SetZero(Byte f)         noexcept { Bitwise::SetBit(S, 1, f ) ;}
        inline void SetInterrupt(Byte f)    noexcept { Bitwise::SetBit(S, 2, f ) ;}
        inline void SetDecimal(Byte f)      noexcept { Bitwise::SetBit(S, 3, f ) ;}
        inline void SetBreak()              noexcept { Bitwise::SetBit(S, 4, 0 ) ;}
        inline void SetUnusedFlag()         noexcept { Bitwise::SetBit(S, 5, 1 ) ;}
        inline void SetOverflow(Byte f)     noexcept { Bitwise::SetBit(S, 6, f ) ;}
        inline void SetNegative(Byte f)     noexcept { Bitwise::SetBit(S, 7, f ) ;}

        inline void ApplyPush()             noexcept { SP--; }
        inline void ApplyPool()             noexcept { SP++; }
        inline Word StackAddress()          noexcept { return Word(0x0100) + SP; }

    } registers;

    struct
    {
        Byte opcode = 0;
        InstructionRoutine instruction;

        Byte operand[2] = {0, 0};
        Byte indirect[2] = {0, 0};
        bool nextPage = false;

        Word address = 0;
        uint16_t addressMode = 0;

        Byte data = 0;

        Byte helper8;
        Byte helper16;
    } state;

    struct
    {
        // An original 6502 has does not correctly fetch the target
        // address if the indirect vector falls on a page boundary
        // (e.g. $xxFF where xx is any value from $00 to $FF).
        // In this case fetches the LSB from $xxFF as expected
        // but takes the MSB from $xx00.
        // This is fixed in some later chips like the 65SC02
        bool originalIndirectFetch = true;
    }cfg;

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
    static constexpr Byte FlagPos_Carry     = 0;
    static constexpr Byte FlagPos_Zero      = 1;
    static constexpr Byte FlagPos_Interrupt = 2;
    static constexpr Byte FlagPos_Decimal   = 3;
    static constexpr Byte FlagPos_Break     = 4;
    static constexpr Byte FlagPos_Unused    = 5;
    static constexpr Byte FlagPos_Overflow  = 6;
    static constexpr Byte FlagPos_Negative  = 7;

    static constexpr Byte FlagMask_Carry     = (1 << FlagPos_Carry);
    static constexpr Byte FlagMask_Zero      = (1 << FlagPos_Zero);
    static constexpr Byte FlagMask_Interrupt = (1 << FlagPos_Interrupt);
    static constexpr Byte FlagMask_Decimal   = (1 << FlagPos_Decimal);
    static constexpr Byte FlagMask_Break     = (1 << FlagPos_Break);
    static constexpr Byte FlagMask_Unused    = (1 << FlagPos_Unused);
    static constexpr Byte FlagMask_Overflow  = (1 << FlagPos_Overflow);
    static constexpr Byte FlagMask_Negative  = (1 << FlagPos_Negative);

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
    static constexpr uint16_t MemAcc_Aux_ResetVec   = ( 1 << 12 );
    static constexpr uint16_t MemAcc_All            = ( 1 << 13 ) - 1;

    static constexpr uint16_t MemAcc_INVALID        = ( 0xffff  );

    using InstructionsLookup = Instruction[256];
    static const InstructionsLookup& Instructions();
};


}
