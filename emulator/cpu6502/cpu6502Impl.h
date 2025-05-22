#pragma once
#include "cpu6502.h"
#include <common/routine.h>

#define WaitClock() co_await Routine::Suspend{}
#define WaitRoutine(r) while(r.Resume()){co_await Routine::Suspend{};}
#define Execute(func) {auto r = func; WaitRoutine(r);}

namespace cpp6502
{

struct Cpu6502::Impl
{
    struct Meta;
    struct Instruction;
    using InstructionRoutine = Routine (Cpu6502::Impl::*)(uint16_t);


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
    } registers;

    struct
    {
        Byte opcode = 0;
        Byte operand[2] = {0, 0};
        Byte indirect[2] = {0, 0};
        bool nextPage = false;

        bool isAccumulatorMode = false;
        Word address = 0;
        Byte data = 0;
    } state;


    std::string ToString() const noexcept;

    Routine ResetInstruction();
    Routine StartNewInstruction();
    Routine ReadAddress(uint16_t memAcc);
    Routine ReadData(uint16_t memAcc);

    Routine Instr_ADC(uint16_t memAcc);
    Routine Instr_AND(uint16_t memAcc);
    Routine Instr_ASL(uint16_t memAcc);
    Routine Instr_BCC(uint16_t memAcc);
    Routine Instr_BCS(uint16_t memAcc);
    Routine Instr_BEQ(uint16_t memAcc);
    Routine Instr_BIT(uint16_t memAcc);
    Routine Instr_BMI(uint16_t memAcc);
    Routine Instr_BNE(uint16_t memAcc);
    Routine Instr_BPL(uint16_t memAcc);
    Routine Instr_BRK(uint16_t memAcc);
    Routine Instr_BVC(uint16_t memAcc);
    Routine Instr_BVS(uint16_t memAcc);
    Routine Instr_CLC(uint16_t memAcc);
    Routine Instr_CLD(uint16_t memAcc);
    Routine Instr_CLI(uint16_t memAcc);
    Routine Instr_CLV(uint16_t memAcc);
    Routine Instr_CMP(uint16_t memAcc);
    Routine Instr_CPX(uint16_t memAcc);
    Routine Instr_CPY(uint16_t memAcc);
    Routine Instr_DEC(uint16_t memAcc);
    Routine Instr_DEX(uint16_t memAcc);
    Routine Instr_DEY(uint16_t memAcc);
    Routine Instr_EOR(uint16_t memAcc);
    Routine Instr_INC(uint16_t memAcc);
    Routine Instr_INX(uint16_t memAcc);
    Routine Instr_INY(uint16_t memAcc);
    Routine Instr_JMP(uint16_t memAcc);
    Routine Instr_JSR(uint16_t memAcc);
    Routine Instr_LDA(uint16_t memAcc);
    Routine Instr_LDX(uint16_t memAcc);
    Routine Instr_LDY(uint16_t memAcc);
    Routine Instr_LSR(uint16_t memAcc);
    Routine Instr_NOP(uint16_t memAcc);
    Routine Instr_ORA(uint16_t memAcc);
    Routine Instr_PHA(uint16_t memAcc);
    Routine Instr_PHP(uint16_t memAcc);
    Routine Instr_PLA(uint16_t memAcc);
    Routine Instr_PLP(uint16_t memAcc);
    Routine Instr_ROL(uint16_t memAcc);
    Routine Instr_ROR(uint16_t memAcc);
    Routine Instr_RTI(uint16_t memAcc);
    Routine Instr_RTS(uint16_t memAcc);
    Routine Instr_SBC(uint16_t memAcc);
    Routine Instr_SEC(uint16_t memAcc);
    Routine Instr_SED(uint16_t memAcc);
    Routine Instr_SEI(uint16_t memAcc);
    Routine Instr_STA(uint16_t memAcc);
    Routine Instr_STX(uint16_t memAcc);
    Routine Instr_STY(uint16_t memAcc);
    Routine Instr_TAX(uint16_t memAcc);
    Routine Instr_TAY(uint16_t memAcc);
    Routine Instr_TSX(uint16_t memAcc);
    Routine Instr_TXA(uint16_t memAcc);
    Routine Instr_TXS(uint16_t memAcc);
    Routine Instr_TYA(uint16_t memAcc);

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

    static constexpr uint16_t MemAcc_All_Absolute = MemAcc_Absolute + MemAcc_Absolute_X + MemAcc_Absolute_Y;
    static constexpr uint16_t MemAcc_All_ZeroPage = MemAcc_ZeroPage + MemAcc_ZeroPage_X + MemAcc_ZeroPage_Y;
    static constexpr uint16_t MemAcc_All_Indexed = MemAcc_Indexed_X + MemAcc_Indexed_Y;

    static constexpr uint16_t MemAcc_Need_0_Operand = MemAcc_Immediate + MemAcc_Accumulator;

    static constexpr uint16_t MemAcc_Need_1_Operand = MemAcc_All_ZeroPage +
                                                      MemAcc_Relative +
                                                      MemAcc_All_Indexed;

    static constexpr uint16_t MemAcc_Need_2_Operand = MemAcc_All_Absolute + MemAcc_Indirect;

    static constexpr uint16_t MemAcc_Need_1_OrMore_Operand = MemAcc_Need_1_Operand + MemAcc_Need_2_Operand;

    using InstructionsLookup = Instruction[256];
    static const InstructionsLookup& Instructions();
};


}
