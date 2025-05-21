#include "cpu6502.h"

#define BYTES_TO_WORD(bytes) (Word(bytes[0]) | (Word(bytes[1]) << 8))
#define HI_BYTE(word) ((word) >> 8)
#define LO_BYTE(word) ((word) & 0xff)

struct NewCycle
{
    static constexpr bool True = true;
    static constexpr bool False = false;
};

struct ConsumeCycle
{
    static constexpr bool True = true;
    static constexpr bool False = false;
};

static constexpr bool CONSUME_CYCLE = true;
static constexpr bool CONTINUE_CYCLE = false;

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

Cpu6502::Impl::Impl(IMemory *memory)
{
    state.opcode = 0x4C;    // JMP
    registers.PC = 0xFFFC;  // Reset vector address
    StartOpcodeExecution();
}

void Cpu6502::Impl::StartOpcodeExecution()
{

}

void Cpu6502::Impl::AddTasks(Tasks &t1, Tasks t2)
{
    for(auto& t : t2)
    {
        t1.push_back(std::move(t));
    }
}

void Cpu6502::Impl::Read_ResetAddress_1()
{
    state.operand[0] = memory_->Read(0xFFFC);
}

void Cpu6502::Impl::Read_ResetAddress_2()
{
    state.operand[1] = memory_->Read(0xFFFD);
}

void Cpu6502::Impl::Read_Opcode()
{
    state.opcode = memory_->Read( registers.PC );
    registers.PC++;
}

void Cpu6502::Impl::Read_Operand_1()
{
    state.operand[0] = memory_->Read( registers.PC );
    registers.PC++;
}

void Cpu6502::Impl::Read_Operand_2()
{
    state.operand[1] = memory_->Read( registers.PC );
    registers.PC++;
}

void Cpu6502::Impl::Read_Pointer_1()
{
    state.pointer[0] = memory_->Read( state.address );
}

void Cpu6502::Impl::Read_Pointer_2()
{
    state.pointer[1] = memory_->Read( state.address + 1 );
}

void Cpu6502::Impl::Address_ZeroPage()
{
    state.address = Word(state.operand[0]);
}

void Cpu6502::Impl::Address_ZeroPage_X()
{
    state.address = Word(state.operand[0] + registers.X);
}

void Cpu6502::Impl::Address_ZeroPage_Y()
{
    state.address = Word(state.operand[0] + registers.Y);
}

void Cpu6502::Impl::Address_Relative()
{
    state.address = registers.PC + state.operand[0];
    state.nextPage = (HI_BYTE(state.address) != HI_BYTE(registers.PC));
}

void Cpu6502::Impl::Address_Absolute()
{
    state.address = BYTES_TO_WORD(state.operand);
}

void Cpu6502::Impl::Address_Absolute_X()
{
    auto baseAddress = BYTES_TO_WORD(state.operand);
    state.address = baseAddress + registers.X;
    state.nextPage = (HI_BYTE(state.address) != HI_BYTE(baseAddress));
}

void Cpu6502::Impl::Address_Absolute_Y()
{
    auto baseAddress = BYTES_TO_WORD(state.operand);
    state.address = baseAddress + registers.Y;
    state.nextPage = (HI_BYTE(state.address) != HI_BYTE(baseAddress));
}

void Cpu6502::Impl::Address_Indirect()
{
    state.address = BYTES_TO_WORD(state.pointer);
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::ZeroPage()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_ZeroPage, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::ZeroPage_X()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_ZeroPage_X, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::ZeroPage_Y()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_ZeroPage_Y, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::Relative()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_Relative, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::Absolute()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Read_Operand_2, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_Absolute, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::Absolute_X()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Read_Operand_2, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_Absolute_X, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::Absolute_Y()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Read_Operand_2, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_Absolute_Y, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::Indirect()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Read_Operand_2, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_Absolute, NewCycle::False, ConsumeCycle::False},
        {&Cpu6502::Impl::Read_Pointer_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Read_Pointer_2, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_Indirect, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::Indexed_X()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_ZeroPage_X, NewCycle::False, ConsumeCycle::False},
        {&Cpu6502::Impl::Read_Pointer_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Read_Pointer_2, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_Indirect, NewCycle::False, ConsumeCycle::False}
    };
}

Cpu6502::Impl::Tasks Cpu6502::Impl::AddressingModes::Indexed_Y()
{
    return {
        {&Cpu6502::Impl::Read_Operand_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_ZeroPage_Y, NewCycle::False, ConsumeCycle::False},
        {&Cpu6502::Impl::Read_Pointer_1, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Read_Pointer_2, NewCycle::True, ConsumeCycle::True},
        {&Cpu6502::Impl::Address_Indirect, NewCycle::False, ConsumeCycle::False}
    };
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Cpu6502::Cpu6502() = default;

std::shared_ptr<Cpu6502> Cpu6502::CreateInstance(IMemory *memory)
{
    auto cpu = std::shared_ptr<Cpu6502>(new Cpu6502);
    impl_ = std::make_shared<Cpu6502::Impl>(memory);
    return cpu;
}

void Cpu6502::Clock()
{
    impl_->Clock();
}


}

