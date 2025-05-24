#include "cpu6502Impl.h"
#include "quirks.hpp"

#define WaitRoutine(routine) {auto r = routine; while(r.Resume()){ co_yield FlushInstruction(); }}

namespace cpp6502
{

inline static constexpr Routine::Empty FlushWrite() noexcept
{
    return Routine::Empty{};
}

inline static constexpr Routine::Empty WaitRead() noexcept
{
    return Routine::Empty{};
}

inline static constexpr Routine::Empty FlushInstruction() noexcept
{
    return Routine::Empty{};
}

Cpu6502::Impl::Impl(IMemory *memory)
    : memory_(memory)
{

}

void Cpu6502::Impl::PowerOn()
{
    Reset();
}

void Cpu6502::Impl::Reset()
{
    activeInstruction_.Destroy();
    activeInstruction_ = ResetInstruction();
}

void Cpu6502::Impl::Clock()
{
    lifetime.cycleCounter++;
    instructionCounter++;
    if (IsInstructionDone())
    {
        instructionCounter = 1;
        activeInstruction_.Destroy();
        activeInstruction_ = StartNewInstruction();
    }
    activeInstruction_.Resume();
}

bool Cpu6502::Impl::IsInstructionDone() const noexcept
{
    return activeInstruction_.Done();
}

void Cpu6502::Impl::ForceState(const State& initial) noexcept
{
    activeInstruction_.Destroy();

    registers.PC =  initial.PC;
    registers.SP =  initial.SP;
    registers.A  =  initial.A;
    registers.X  =  initial.X;
    registers.Y  =  initial.Y;
    registers.S  =  initial.S;
    registers.SetBreak();
    registers.SetUnusedFlag();

    for(const auto& [address, data] : initial.mem)
    {
        memory_->Unsafe_Write(address, data);
    }
}

bool Cpu6502::Impl::Compate(const State& state) const noexcept
{
    if( registers.PC !=  state.PC ||
        registers.SP !=  state.SP ||
        registers.A  !=  state.A  ||
        registers.X  !=  state.X  ||
        registers.Y  !=  state.Y  ||
        registers.S  !=  state.S)
    {
        return false;
    }

    for(const auto& [address, data] : state.mem)
    {
        if (data != memory_->Unsafe_Read(address))
        {
            return false;
        }
    }
    return true;
}

Cpu6502::Lifetime Cpu6502::Impl::GetLifetime() const noexcept
{
    return lifetime;
}

std::string Cpu6502::Impl::ToString() const noexcept
{
    return fmt::format(
        "PC:x{:04X} SP:x{:02X} A:x{:02X} X:x{:02X} Y:x{:02X} S:x{:02X} Flags[(N:{})(V:{})(U:{})(B:{})(D:{})(I:{})(Z:{})(C:{})], OP:x{:02X}/{}",
        registers.PC,
        registers.SP,
        registers.A,
        registers.X,
        registers.Y,
        registers.S,
        registers.Negative()   ? '1' : '0',
        registers.Overflow()   ? '1' : '0',
        registers.UnusedFlag() ? '1' : '0',
        registers.Break()      ? '1' : '0',
        registers.Decimal()    ? '1' : '0',
        registers.Interrupt()  ? '1' : '0',
        registers.Zero()       ? '1' : '0',
        registers.Carry()      ? '1' : '0',
        state.opcode,
        Meta::Instructions()[state.opcode].name
    );
}

inline constexpr Routine::Empty Cpu6502::Impl::RepeatLastMemoryOperation()
{
    memory_->RepeatLastOperation();
    return Routine::Empty{};
}

inline constexpr Routine::Empty Cpu6502::Impl::DummyRead()
{
    state.helper8 = memory_->ReadFromLastAddress();
    return Routine::Empty{};
}

inline constexpr Routine::Empty Cpu6502::Impl::DummyRead(Address address)
{
    state.helper8 = memory_->Read(address);
    return Routine::Empty{};
}

inline constexpr Routine::Empty Cpu6502::Impl::DummyRead_PC()
{
    state.helper8 = memory_->Read(registers.PC);
    return Routine::Empty{};
}

inline constexpr Routine::Empty Cpu6502::Impl::DummyRead_Addr()
{
    state.helper8 = memory_->Read(state.address);
    return Routine::Empty{};
}

inline constexpr Routine::Empty Cpu6502::Impl::DummyWrite()
{
    memory_->WriteToLastAddressLastData();
    return Routine::Empty{};
}

inline constexpr Routine::Empty Cpu6502::Impl::DummyWrite(Byte data)
{
    memory_->WriteToLastAddress(data);
    return Routine::Empty{};
}

inline constexpr Routine::Empty Cpu6502::Impl::PrepareNextPage()
{
    state.helper8 = memory_->Read(state.pageCrossedAddr.value_or(state.address));
    return Routine::Empty{};
}

////////////////////////////////////////////////////////////////

Routine Cpu6502::Impl::ResetInstruction()
{
    registers = {};
    state = {};

    // Cycle 1,2
    // dummy opcode fetch
    for(int step = 0; step < 2; step++)
    {
        {
            memory_->Read(registers.PC);
            registers.PC++;
        } co_yield WaitRead();
    }

    // Cycle 3,4,5
    for(int step = 0; step < 3; step++)
    {
        {
            memory_->Read(registers.StackAddress());    // 6502 quirk: RESET issues three dummy stack “pushes”
            registers.ApplyPush();                      // with R/W held HIGH (read-only) to drop SP to $FD
        }  co_yield WaitRead();                        // no data is written; not a bug.
    }

    {
        // Cycle 6
        state.operand[0] = memory_->Read(0xFFFC);
    } co_yield WaitRead();

    {
        // Cycle 7
        state.operand[1] = memory_->Read(0xFFFD);
        registers.PC = Bitwise::BytesToWord(state.operand);
    } co_yield WaitRead();

    co_return;
}

Routine Cpu6502::Impl::StartNewInstruction()
{
    lifetime.instructionCounter++;
    state = {};

    {
        // Cycle 1
        // fetch opcode
        state.opcode = memory_->Read( registers.PC );
        registers.PC++;
        state.addressMode = Meta::Instructions()[state.opcode].memoryMode;
        state.instruction = Meta::Instructions()[state.opcode].instruction;
    } co_yield WaitRead();

    // Call member function
    WaitRoutine( ((*this).*(state.instruction))() );

    co_return;
}

Routine Cpu6502::Impl::ReadAddress()
{
//  Name       Bytes  Cycles  Total +Read +Write
//-------------------------------------------------------
// Immediate   .. 0 ... 0 ..... 1 ... 2 .... 0
// Accumulator .. 0 ... 0 ..... 1 ... 1 .... 1
// ZeroPage    .. 1 ... 1 ..... 2 ... 3 .... 4
// ZeroPage_X  .. 1 ... 2 ..... 3 ... 4 .... 5
// ZeroPage_Y  .. 1 ... 2 ..... 3 ... 4 .... 5
// Relative    .. 1 ... 1 ..... 2*... 3*.... 4
// Absolute    .. 2 ... 2 ..... 3 ... 4 .... 5
// Absolute_X  .. 2 ... 2*..... 3*... 4*.... 6
// Absolute_Y  .. 2 ... 2*..... 3*... 4*.... 6
// Indirect    .. 2 ... 4 ..... 5 ... 6 .... 7
// Indexed_X   .. 1 ... 4 ..... 5 ... 6 .... 7
// Indexed_Y   .. 1 ... 3*..... 4*... 5*.... 6
// Aux_ResetVec.. 0 ... 2 ..... 3 ... 4 .... 5

    // 0 operands
    if (state.addressMode == Meta::MemAcc_Accumulator)
    {
        co_return;
    }
    if (state.addressMode == Meta::MemAcc_Immediate)
    {
        // 0 operands, PC is used like memory addres
        // ReadData will read it later
        // my interpretation, not sure it is a good idea, but it will work
        state.address = registers.PC;
        registers.PC++;
        co_return;
    }

    // 1 operand
    if (state.addressMode & (
            Meta::MemAcc_ZeroPage   |
            Meta::MemAcc_ZeroPage_X |
            Meta::MemAcc_ZeroPage_Y |
            Meta::MemAcc_Relative   |
            Meta::MemAcc_Indexed_X  |
            Meta::MemAcc_Indexed_Y  |
            Meta::MemAcc_Absolute   |
            Meta::MemAcc_Absolute_X |
            Meta::MemAcc_Absolute_Y |
            Meta::MemAcc_Indirect)
        )
    {
        // Cycle 1
        {
            // fetch operand 1
            state.operand[0] = memory_->Read( registers.PC );
            registers.PC++;
        } co_yield WaitRead();
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
        {
            // fetch operand 2
            state.operand[1] = memory_->Read( registers.PC );
            registers.PC++;
        } co_yield WaitRead();
    }

    switch (state.addressMode)
    {
    case Meta::MemAcc_ZeroPage:
    {
        state.address = Bitwise::BytesToWord(state.operand);
    }break;
    case Meta::MemAcc_ZeroPage_X:
    {
        // Cycle 2
        {
            state.address = Bitwise::BytesToWord(state.operand);
            state.helper8 = memory_->Read(state.address);
            state.address = (state.address + registers.X) & 0xff;
        } co_yield WaitRead();
    }break;
    case Meta::MemAcc_ZeroPage_Y:
    {
        // Cycle 2
        {
            state.address = Bitwise::BytesToWord(state.operand);
            state.helper8 = memory_->Read(state.address);
            state.address = (state.address + registers.Y) & 0xff;
        } co_yield WaitRead();
    }break;
    case Meta::MemAcc_Relative:
    {
        Bitwise::SetHiByte16(state.pageCrossedAddr.emplace(0),
                             Bitwise::HiByte16(registers.PC));

        state.address = registers.PC + SByte( state.operand[0] );

        Bitwise::SetLoByte16(state.pageCrossedAddr.value(),
                             Bitwise::LoByte16(state.address));
    }break;
    case Meta::MemAcc_Absolute:
    {
        state.address = Bitwise::BytesToWord(state.operand);
    }break;
    case Meta::MemAcc_Absolute_X:
    {
        state.helper16  = Bitwise::BytesToWord(state.operand);

        Bitwise::SetHiByte16(state.pageCrossedAddr.emplace(0),
                             Bitwise::HiByte16(state.helper16));

        state.address = state.helper16  + registers.X;

        Bitwise::SetLoByte16(state.pageCrossedAddr.value(),
                             Bitwise::LoByte16(state.address));
    }break;
    case Meta::MemAcc_Absolute_Y:
    {
        state.helper16  = Bitwise::BytesToWord(state.operand);

        Bitwise::SetHiByte16(state.pageCrossedAddr.emplace(0),
                             Bitwise::HiByte16(state.helper16));

        state.address = state.helper16  + registers.Y;

        Bitwise::SetLoByte16(state.pageCrossedAddr.value(),
                             Bitwise::LoByte16(state.address));
    }break;
    case Meta::MemAcc_Indirect:
    {
        // Cycle 3
        {
            state.helper16 = Bitwise::BytesToWord(state.operand);
            state.indirect[0] = memory_->Read( state.helper16  );
        } co_yield WaitRead();

        // Cycle 4
        {
            quirks::AddressWrappingOnIndirectFetch_byte_2_PRE(this);
            state.indirect[1] = memory_->Read( state.helper16 + 1 );
            state.address = Bitwise::BytesToWord(state.indirect);
        } co_yield WaitRead();
        // TODO:
    }break;
    case Meta::MemAcc_Indexed_X:
    {
        // Cycle 2
        {
            state.helper16 = Bitwise::BytesToWord(state.operand);
            memory_->Read( state.helper16 );
        } co_yield WaitRead();

        // Cycle 3
        {
            state.helper16 = (state.helper16 + registers.X) & 0xff;
            state.indirect[0] = memory_->Read( state.helper16 );
        } co_yield WaitRead();

        // Cycle 4
        {
            state.indirect[1] = memory_->Read( (state.helper16 + 1) & 0xff );
            state.address = Bitwise::BytesToWord(state.indirect);
        } co_yield WaitRead();

    }break;
    case Meta::MemAcc_Indexed_Y:
    {
        // Cycle 3
        {
            state.helper16 = Bitwise::BytesToWord(state.operand);
            state.indirect[0] = memory_->Read( state.helper16 );
        } co_yield WaitRead();

        // Cycle 4
        {
            state.indirect[1] = memory_->Read( (state.helper16 + 1) & 0xff );
            state.helper16 = Bitwise::BytesToWord(state.indirect);

            Bitwise::SetHiByte16(state.pageCrossedAddr.emplace(0),
                                 Bitwise::HiByte16(state.helper16));

            state.address = state.helper16  + registers.Y;

            Bitwise::SetLoByte16(state.pageCrossedAddr.value(),
                                 Bitwise::LoByte16(state.address));
        } co_yield WaitRead();
        // TODO: STA exception in cycles
    }break;
    case Meta::MemAcc_Aux_ResetVec:
    {
        // Cycle 1
        {
            state.indirect[0] = memory_->Read( 0xfffe );
        } co_yield WaitRead();

        // Cycle 2
        {
            state.indirect[1] = memory_->Read( 0xffff );
            state.address = Bitwise::BytesToWord(state.indirect);
            registers.PC = state.address;
        } co_yield WaitRead();
    }break;
    default:
        break;
    }

    co_return;
}

Routine Cpu6502::Impl::ReadData()
{
    if (state.addressMode == Meta::MemAcc_Accumulator)
    {
        state.data = registers.A;
        // no extra cycle
        co_return;
    }

    // Cycle 1
    {
        state.data = memory_->Read( state.address );
    } co_yield WaitRead();

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
        // no extra cycle
        registers.A = state.data;
        co_return;
    }

    // // Cycle 1
    // // fix ASL 0x06
    // co_yield DummyWrite(state.data); {}

    // 0x99 STA
    // if (state.addressMode & (
    //     Meta::MemAcc_Absolute_X |
    //     Meta::MemAcc_Absolute_Y)
    //     )
    // {
    //     // Cycle 2
    //     // TODO: Not sure
    //     co_yield DummyWrite(); {}
    // }

    {
        memory_->Write(state.address, state.data);
    } co_yield FlushWrite();

    co_return;
}

Routine Cpu6502::Impl::PrepareWriteData()
{
    if (state.addressMode == Meta::MemAcc_Immediate)
    {
        Error("Cpu6502::Impl", "Unexpected write")
            .Msg("CPU: {}", ToString())
            .Throw();
    }

    if (state.addressMode == Meta::MemAcc_Accumulator)
    {
        // Cycle 1
        // fix x06, ASL
        co_yield DummyRead(registers.PC); {}
    }
    else if(state.addressMode == Meta::MemAcc_Absolute_X ||
            state.addressMode == Meta::MemAcc_Absolute_Y)
    {
        // Cycle 1
        co_yield DummyRead(); {}
    }
    else
    {
        // Cycle 1
        co_yield DummyWrite(); {}
    }

    co_return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// ADC: Add with Carry
Routine Cpu6502::Impl::Instr_ADC()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    state.helper16 = registers.A + state.data + registers.Carry();
    state.helper8 = Byte(state.helper16);

    registers.SetCarry( state.helper16 > 0xff ? 1 : 0 );
    registers.SetZero( state.helper8 == 0 ? 1 : 0 );

    registers.SetOverflow(
                (Bitwise::Sign8(registers.A) == Bitwise::Sign8(state.data)) &&
                (Bitwise::Sign8(registers.A) != Bitwise::Sign8(state.helper8))
                ? 1 : 0);

    registers.SetNegative( Bitwise::Sign8(state.helper8) );

    registers.A = state.helper8;

    co_return;
}

// AND: Logical AND
Routine Cpu6502::Impl::Instr_AND()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    registers.A &= state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );

    co_return;
}

// ASL: Arithmetic Shift Left
Routine Cpu6502::Impl::Instr_ASL()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
        WaitRoutine( ReadData() );
    }
    else
    {
        WaitRoutine( ReadData() );
        WaitRoutine( PrepareWriteData() );
    }

    registers.SetCarry( Bitwise::Bit8(state.data, 7) );

    state.data <<= 1;

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    WaitRoutine( WriteData() );

    co_return;
}

// BCC: Branch if Carry Clear
Routine Cpu6502::Impl::Instr_BCC()
{
    WaitRoutine( ReadAddress() );

    if (registers.Carry())
    {
        co_return;
    }

    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC = state.address;
    }

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 2
        co_yield PrepareNextPage(); {}
    }

    co_return;
}

// BCS: Branch if Carry Set
Routine Cpu6502::Impl::Instr_BCS()
{
    WaitRoutine( ReadAddress() );

    if ( ! registers.Carry())
    {
        co_return;
    }

    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC = state.address;
    }

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 2
        co_yield PrepareNextPage(); {}
    }

    co_return;
}


// BEQ: Branch if Equal
Routine Cpu6502::Impl::Instr_BEQ()
{
    WaitRoutine( ReadAddress() );

    if ( ! registers.Equal())
    {
        co_return;
    }

    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC = state.address;
    }

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 2
        co_yield PrepareNextPage(); {}
    }

    co_return;
}

// BIT: Bit Test
Routine Cpu6502::Impl::Instr_BIT()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    registers.SetZero( (registers.A & state.data) == 0 ? 1 : 0 );
    registers.SetOverflow( Bitwise::Bit8(state.data, 6) );
    registers.SetNegative( Bitwise::Bit8(state.data, 7) );

    co_return;
}

// BMI: Branch if Minus
Routine Cpu6502::Impl::Instr_BMI()
{
    WaitRoutine( ReadAddress() );

    if (registers.Positive())
    {
        co_return;
    }

    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC = state.address;
    }

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 2
        co_yield PrepareNextPage(); {}
    }
    co_return;
}

// BNE: Branch if Not Equal
Routine Cpu6502::Impl::Instr_BNE()
{
    WaitRoutine( ReadAddress() );

    if (registers.Equal())
    {
        co_return;
    }

    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC = state.address;
    }

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 2
        co_yield PrepareNextPage(); {}
    }

    co_return;
}

// BPL: Branch if Positive
Routine Cpu6502::Impl::Instr_BPL()
{
    WaitRoutine( ReadAddress() );

    if (registers.Negative())
    {
        co_return;
    }

    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC = state.address;
    }

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 2
        co_yield PrepareNextPage(); {}
    }

    co_return;
}

// BRK: Force Interrupt
Routine Cpu6502::Impl::Instr_BRK()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC++;
    }

    // Cycle 2
    {
        memory_->Write( registers.StackAddress(), Bitwise::HiByte16( registers.PC ) );
        registers.ApplyPush();
    } co_yield FlushWrite();

    // Cycle 3

    {
        memory_->Write( registers.StackAddress(), Bitwise::LoByte16( registers.PC ) );
        registers.ApplyPush();
    } co_yield FlushWrite();

    // Cycle 4
    {
        memory_->Write( registers.StackAddress(), registers.S | Meta::FlagMask_Break );
        registers.ApplyPush();
        quirks::StatusRegOnBreak_POST(this);
    } co_yield FlushWrite();

    state.addressMode = Meta::MemAcc_Aux_ResetVec;
    WaitRoutine( ReadAddress() );
    co_return;
}

// BVC: Branch if Overflow Clear
Routine Cpu6502::Impl::Instr_BVC()
{
    WaitRoutine( ReadAddress() );

    if (registers.Overflow())
    {
        co_return;
    }

    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC = state.address;
    }

    // Cycle 2
    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        co_yield PrepareNextPage(); {}
    }

    co_return;
}

// BVS: Branch if Overflow Set
Routine Cpu6502::Impl::Instr_BVS()
{
    WaitRoutine( ReadAddress() );

    if ( ! registers.Overflow())
    {
        co_return;
    }

    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.PC = state.address;
    }

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 2
        co_yield PrepareNextPage(); {}
    }

    co_return;
}

// CLC: Clear Carry Flag
Routine Cpu6502::Impl::Instr_CLC()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.SetCarry( 0 );
    }

    co_return;
}

// CLD: Clear Decimal Mode
Routine Cpu6502::Impl::Instr_CLD()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.SetDecimal( 0 );
    }

    co_return;
}

// CLI: Clear Interrupt Disable
Routine Cpu6502::Impl::Instr_CLI()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.SetInterrupt( 0 );
    }

    co_return;
}

// CLV: Clear Overflow Flag
Routine Cpu6502::Impl::Instr_CLV()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.SetOverflow( 0 );
    }

    co_return;
}

// CMP: Compare
Routine Cpu6502::Impl::Instr_CMP()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    registers.SetCarry( registers.A >= state.data ? 1 : 0);
    registers.SetZero( registers.A == state.data ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A - state.data) );

    co_return;
}

// CPX: Compare X Register
Routine Cpu6502::Impl::Instr_CPX()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    registers.SetCarry( registers.X >= state.data ? 1 : 0);
    registers.SetZero( registers.X == state.data ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.X - state.data) );

    co_return;
}

// CPY: Compare Y Register
Routine Cpu6502::Impl::Instr_CPY()
{
    WaitRoutine( ReadAddress() );
    WaitRoutine( ReadData() );

    registers.SetCarry( registers.Y >= state.data ? 1 : 0);
    registers.SetZero( registers.Y == state.data ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.Y - state.data) );

    co_return;
}

// DEC: Decrement Memory
Routine Cpu6502::Impl::Instr_DEC()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
        WaitRoutine( ReadData() );
    }
    else
    {
        WaitRoutine( ReadData() );
        WaitRoutine( PrepareWriteData() );
    }

    if (state.addressMode == Meta::MemAcc_Absolute_X)
    {
        WaitRoutine( WriteData() );
    }

    // Cycle 1,2
    state.data--;
    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    WaitRoutine( WriteData() );

    co_return;
}

// DEX: Decrement X Register
Routine Cpu6502::Impl::Instr_DEX()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.X--;
        registers.SetZero( registers.X == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.X ));
    }

    co_return;
}

// DEY: Decrement Y Register
Routine Cpu6502::Impl::Instr_DEY()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.Y--;
        registers.SetZero( registers.Y == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.Y ));
    }

    co_return;
}

// EOR: Exclusive OR
Routine Cpu6502::Impl::Instr_EOR()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    registers.A ^= state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );

    co_return;
}

// INC: Increment Memory
Routine Cpu6502::Impl::Instr_INC()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
        WaitRoutine( ReadData() );
    }
    else
    {
        WaitRoutine( ReadData() );
        WaitRoutine( PrepareWriteData() );
    }

    if (state.addressMode == Meta::MemAcc_Absolute_X)
    {
        WaitRoutine( WriteData() );
    }

    // Cycle 1,2
    state.data++;
    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    WaitRoutine( WriteData() );

    co_return;
}

// INX: Increment X Register
Routine Cpu6502::Impl::Instr_INX()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.X++;
        registers.SetZero( registers.X == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.X ));
    }

    co_return;
}

// INY: Increment Y Register
Routine Cpu6502::Impl::Instr_INY()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.Y++;
        registers.SetZero( registers.Y == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.Y ));
    }

    co_return;
}

// JMP: Jump
Routine Cpu6502::Impl::Instr_JMP()
{
    WaitRoutine( ReadAddress() );
    registers.PC = state.address;

    co_return;
}

// JSR: Jump to Subroutine
Routine Cpu6502::Impl::Instr_JSR()
{
    // Cycle 1
    {
        state.operand[0] = memory_->Read( registers.PC );
        registers.PC++;
    } co_yield WaitRead();

    // Cycle 2
    co_yield DummyRead(registers.StackAddress()); {}

    // Cycle 3
    {
        memory_->Write( registers.StackAddress(), Bitwise::HiByte16( registers.PC ) );
        registers.ApplyPush();
    } co_yield FlushWrite();

    // Cycle 4
    {
        memory_->Write( registers.StackAddress(), Bitwise::LoByte16( registers.PC ) );
        registers.ApplyPush();
    } co_yield FlushWrite();

    // Cycle 5
    {
        state.operand[1] = memory_->Read( registers.PC );
        registers.PC = Bitwise::BytesToWord(state.operand);
    } co_yield WaitRead();

    co_return;
}

// LDA: Load Accumulator
Routine Cpu6502::Impl::Instr_LDA()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    registers.A = state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );

    co_return;
}

// LDX: Load X Register
Routine Cpu6502::Impl::Instr_LDX()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    registers.X = state.data;

    registers.SetZero( registers.X == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.X) );

    co_return;
}

// LDY: Load Y Register
Routine Cpu6502::Impl::Instr_LDY()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    registers.Y = state.data;

    registers.SetZero( registers.Y == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.Y) );

    co_return;
}

// LSR: Logical Shift Right
Routine Cpu6502::Impl::Instr_LSR()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
        WaitRoutine( ReadData() );
    }
    else
    {
        WaitRoutine( ReadData() );
        WaitRoutine( PrepareWriteData() );
    }

    registers.SetCarry( Bitwise::Bit8(state.data, 0) );

    state.data >>= 1;

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    WaitRoutine( WriteData() );

    co_return;
}

// NOP: No Operation
Routine Cpu6502::Impl::Instr_NOP()
{
    // Cycle 1
    co_yield DummyRead_PC(); {}

    co_return;
}

// ORA: Logical Inclusive OR
Routine Cpu6502::Impl::Instr_ORA()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    registers.A |= state.data;

    registers.SetZero( registers.A == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(registers.A) );

    co_return;
}

// PHA: Push Accumulator
Routine Cpu6502::Impl::Instr_PHA()
{
    // Cycle 1
    co_yield DummyRead_PC(); {}

    // Cycle 2
    {
        memory_->Write( registers.StackAddress(), registers.A );
        registers.ApplyPush();
    } co_yield FlushWrite();

    co_return;
}

// PHP: Push Processor Status
Routine Cpu6502::Impl::Instr_PHP()
{
    // Cycle 1
    co_yield DummyRead_PC(); {}

    {
        state.helper8 = registers.S;
        quirks::StatusRegOgPHP_PRE(this);
        memory_->Write( registers.StackAddress(), state.helper8 | Meta::FlagMask_Break );
        registers.ApplyPush();
    } co_yield FlushWrite();

    co_return;
}

// PLA: Pull Accumulator
Routine Cpu6502::Impl::Instr_PLA()
{
    // Cycle 1
    co_yield DummyRead_PC();

    // Cycle 2
    co_yield DummyRead(registers.StackAddress()); {}

    // Cycle 3
    {
        registers.ApplyPool();
        registers.A = memory_->Read( registers.StackAddress() );
        registers.SetZero( registers.A == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8(registers.A) );
    } co_yield WaitRead();

    co_return;
}

// PLP: Pull Processor Status
Routine Cpu6502::Impl::Instr_PLP()
{
    // Cycle 1
    co_yield DummyRead_PC();{}

    // Cycle 2
    co_yield DummyRead(registers.StackAddress()); {}

    // Cycle 3
    {
        registers.ApplyPool();
        registers.S = memory_->Read( registers.StackAddress() );
        registers.SetUnusedFlag();
        registers.SetBreak();
    } co_yield WaitRead();

    co_return;
}

// ROL: Rotate Left
Routine Cpu6502::Impl::Instr_ROL()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
        WaitRoutine( ReadData() );
    }
    else
    {
        WaitRoutine( ReadData() );
        WaitRoutine( PrepareWriteData() );
    }

    state.helper8 = registers.Carry();
    registers.SetCarry( Bitwise::Bit8(state.data, 7) );

    state.data <<= 1;
    Bitwise::SetBit8(state.data, 0, state.helper8);

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    WaitRoutine( WriteData() );

    co_return;
}

// ROR: Rotate Right
Routine Cpu6502::Impl::Instr_ROR()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
        WaitRoutine( ReadData() );
    }
    else
    {
        WaitRoutine( ReadData() );
        WaitRoutine( PrepareWriteData() );
    }

    state.helper8 = registers.Carry();
    registers.SetCarry( Bitwise::Bit8(state.data, 0) );

    state.data >>= 1;
    Bitwise::SetBit8(state.data, 7, state.helper8);

    registers.SetZero( state.data == 0 ? 1 : 0 );
    registers.SetNegative( Bitwise::Sign8(state.data) );

    WaitRoutine( WriteData() );

    co_return;
}

// RTI: Return from Interrupt
Routine Cpu6502::Impl::Instr_RTI()
{
    // Cycle 1
    co_yield DummyRead_PC();

    // Cycle 2
    co_yield DummyRead(registers.StackAddress()); {}
    {
        registers.ApplyPool();
    }

    // Cycle 3
    {
        state.helper8 = memory_->Read( registers.StackAddress() );
        registers.S = state.helper8;
        registers.SetUnusedFlag();
        registers.SetBreak();
    } co_yield WaitRead();

    // Cycle 4
    {
        registers.ApplyPool();
        state.helper8 = memory_->Read(registers.StackAddress());
        Bitwise::SetLoByte16(registers.PC, state.helper8);
    } co_yield WaitRead();

    // Cycle 5
    {
        registers.ApplyPool();
        state.helper8 = memory_->Read(registers.StackAddress());
        Bitwise::SetHiByte16(registers.PC, state.helper8);
    } co_yield WaitRead();

    co_return;
}

// RTS: Return from Subroutine
Routine Cpu6502::Impl::Instr_RTS()
{
    // Cycle 1
    co_yield DummyRead_PC();

    // Cycle 2
    co_yield DummyRead(registers.StackAddress()); {}

    // Cycle 3
    {
        registers.ApplyPool();
        state.helper8 = memory_->Read(registers.StackAddress());
        Bitwise::SetLoByte16(registers.PC, state.helper8);
    } co_yield WaitRead();

    // Cycle 4
    {
        registers.ApplyPool();
        state.helper8 = memory_->Read(registers.StackAddress());
        Bitwise::SetHiByte16(registers.PC, state.helper8);
    } co_yield WaitRead();

    // Cycle 5
    co_yield DummyRead_PC();
    {
        registers.PC++;
    }

    co_return;
}

// SBC: Subtract with Carry
Routine Cpu6502::Impl::Instr_SBC()
{
    WaitRoutine( ReadAddress() );

    if (state.pageCrossedAddr.value_or(state.address) != state.address)
    {
        // Cycle 1
        co_yield PrepareNextPage(); {}
    }

    WaitRoutine( ReadData() );

    // not carry
    state.helper8 = registers.A - state.data - (registers.Carry() ? 0 : 1);

    registers.SetCarry(
        (registers.A >= (state.data + (registers.Carry() ? 0 : 1)))
        ? 1 : 0);

    registers.SetZero( state.helper8 == 0 ? 1 : 0 );

    registers.SetOverflow(
        (Bitwise::Sign8(registers.A) != Bitwise::Sign8(state.data)) &&
        (Bitwise::Sign8(registers.A) != Bitwise::Sign8(state.helper8))
        ? 1:0);

    registers.SetNegative( Bitwise::Sign8( state.helper8 ) );

    registers.A = state.helper8;

    co_return;
}

// SEC: Set Carry Flag
Routine Cpu6502::Impl::Instr_SEC()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.SetCarry( 1 );
    }

    co_return;
}

// SED: Set Decimal Flag
Routine Cpu6502::Impl::Instr_SED()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.SetDecimal( 1 );
    }

    co_return;
}

// SEI: Set Interrupt Disable
Routine Cpu6502::Impl::Instr_SEI()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.SetInterrupt( 1 );
    }

    co_return;
}

// STA: Store Accumulator
Routine Cpu6502::Impl::Instr_STA()
{
    WaitRoutine( ReadAddress() );

    if ( state.addressMode == Meta::MemAcc_Indexed_Y ||
         state.addressMode == Meta::MemAcc_Absolute_X ||
         state.addressMode == Meta::MemAcc_Absolute_Y)
    {
        if (state.pageCrossedAddr.value_or(state.address) != state.address)
        {
            // Cycle 1
            co_yield PrepareNextPage(); {}
        }
        else
        {
            co_yield DummyRead(state.address); {}
        }
    }

    state.data = registers.A;
    WaitRoutine( WriteData() );

    co_return;
}

// STX: Store X Register
Routine Cpu6502::Impl::Instr_STX()
{
    WaitRoutine( ReadAddress() );
    state.data = registers.X;
    WaitRoutine( WriteData() );

    co_return;
}

// STY: Store Y Register
Routine Cpu6502::Impl::Instr_STY()
{
    WaitRoutine( ReadAddress() );
    state.data = registers.Y;
    WaitRoutine( WriteData() );

    co_return;
}

// TAX: Transfer Accumulator to X
Routine Cpu6502::Impl::Instr_TAX()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.X = registers.A;
        registers.SetZero( registers.X == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.X ) );
    }

    co_return;
}

// TAY: Transfer Accumulator to Y
Routine Cpu6502::Impl::Instr_TAY()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.Y = registers.A;
        registers.SetZero( registers.Y == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.Y ) );
    }

    co_return;
}

// TSX: Transfer Stack Pointer to X
Routine Cpu6502::Impl::Instr_TSX()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.X = registers.SP;
        registers.SetZero( registers.X == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.X ) );
    }

    co_return;
}

// TXA: Transfer X to Accumulator
Routine Cpu6502::Impl::Instr_TXA()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.A = registers.X;
        registers.SetZero( registers.A == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.A ) );
    }

    co_return;
}

// TXS: Transfer X to Stack Pointer
Routine Cpu6502::Impl::Instr_TXS()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.SP = registers.X;
    }

    co_return;
}

// TYA: Transfer Y to Accumulator
Routine Cpu6502::Impl::Instr_TYA()
{
    // Cycle 1
    co_yield DummyRead_PC();
    {
        registers.A = registers.Y;
        registers.SetZero( registers.A == 0 ? 1 : 0 );
        registers.SetNegative( Bitwise::Sign8( registers.A ) );
    }

    co_return;
}

}


