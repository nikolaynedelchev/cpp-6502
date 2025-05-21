#pragma once
#include "cpu6502.h"

namespace cpp6502
{

class Cpu6502::Impl
{
public:
    Impl(IMemory* memory);

    // IDevice interface
public:
    void Clock();

    struct
    {
        Address PC;
        Byte SP;
        Byte A;
        Byte X;
        Byte Y;
        Byte S;
    } registers;


    struct
    {
        Byte opcode = 0;
        Byte operand[2] = {0, 0};
        Byte pointer[2] = {0, 0};
        bool nextPage = false;

        bool isAccumulatorMode = false;
        Word address = 0;
        Byte data = 0;
    }state;

    using TastFunc = void (Cpu6502::Impl::*)();
    struct Task
    {
        TastFunc execute;
        bool requireNewCycle;
        bool consumeCycel;
    };
    using Tasks = std::vector<Task>;

    void StartOpcodeExecution();

    static void AddTasks(Tasks& t1, Tasks t2);

    void Read_ResetAddress_1();
    void Read_ResetAddress_2();

    void Read_Opcode();
    void Read_Operand_1();
    void Read_Operand_2();
    void Read_Pointer_1();
    void Read_Pointer_2();

    void Address_ZeroPage();
    void Address_ZeroPage_X();
    void Address_ZeroPage_Y();

    void Address_Relative();

    void Address_Absolute();
    void Address_Absolute_X();
    void Address_Absolute_Y();

    void Address_Indirect();
    void Address_Indexed_X();
    void Address_Indexed_Y();

    void ReadData();
    void WriteData();

    void Opcode_JMP();


    struct AddressingModes
    {
        static Tasks ZeroPage();
        static Tasks ZeroPage_X();
        static Tasks ZeroPage_Y();

        static Tasks Relative();

        static Tasks Absolute();
        static Tasks Absolute_X();
        static Tasks Absolute_Y();

        static Tasks Indirect();
        static Tasks Indexed_X();
        Tasks Indexed_Y();
    };

    IMemory* memory_;
};

}
