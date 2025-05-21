#include "cpu6502Impl.h"

#define BYTES_TO_WORD(bytes) (Word(bytes[0]) | (Word(bytes[1]) << 8))
#define HI_BYTE(word) ((word) >> 8)
#define LO_BYTE(word) ((word) & 0xff)

namespace cpp6502
{

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

}

