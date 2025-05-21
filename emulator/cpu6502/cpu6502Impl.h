#pragma once
#include "cpu6502.h"
#include <common/routine.h>

#define WaitClock() co_await Routine::Suspend{}
#define WaitRoutine(r) while(r.Resume()){co_await Routine::Suspend{};}
#define Execute(func) {auto r = func; WaitRoutine(r);}
namespace cpp6502
{

class Cpu6502::Impl
{
public:
    Impl(IMemory* memory);

    // IDevice interface
public:

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
    }state;

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

    static constexpr uint16_t MemAcc_All_Absolute = MemAcc_Absolute + MemAcc_Absolute_X + MemAcc_Absolute_Y;
    static constexpr uint16_t MemAcc_All_ZeroPage = MemAcc_ZeroPage + MemAcc_ZeroPage_X + MemAcc_ZeroPage_Y;
    static constexpr uint16_t MemAcc_All_Indexed = MemAcc_Indexed_X + MemAcc_Indexed_Y;

    static constexpr uint16_t MemAcc_Need_0_Operand = MemAcc_Immediate + MemAcc_Accumulator;

    static constexpr uint16_t MemAcc_Need_1_Operand = MemAcc_All_ZeroPage +
                                                      MemAcc_Relative +
                                                      MemAcc_All_Indexed;

    static constexpr uint16_t MemAcc_Need_2_Operand = MemAcc_All_Absolute + MemAcc_Indirect;

    static constexpr uint16_t MemAcc_Need_1_OrMore_Operand = MemAcc_Need_1_Operand + MemAcc_Need_2_Operand;

    Routine ResetInstruction();
    Routine StartNewInstruction();
    Routine ReadAddress(uint16_t memAcc);
    Routine ReadData(uint16_t memAcc);

    Routine Op_ADC(uint16_t memAcc);

    Routine activeInstruction_;
    IMemory* memory_;
};

}
