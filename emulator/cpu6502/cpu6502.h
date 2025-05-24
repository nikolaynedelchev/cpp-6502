#pragma once
#include <common/common.h>
#include <membus/membus.h>
#include <memory>
#include <vector>

namespace cpp6502
{

class Cpu6502 : public IDevice
{
private:
    Cpu6502();

public:
    static std::shared_ptr<Cpu6502> CreateInstance(IMemory* memory);
    void PowerOn();
    void Reset();
    std::string Dump() const noexcept;
    bool IsInstructionDone() const noexcept;

    // IDevice interface
    void Clock() final;

    struct Lifetime
    {
        uint64_t instructionCounter = 0;
        uint64_t cycleCounter = 0;
    };

    struct State
    {
        Address PC = 0;
        Byte SP = 0;
        Byte A = 0;
        Byte X = 0;
        Byte Y = 0;
        Byte S = 0;
        std::vector<std::pair<Address, Byte>> mem;
    };

    // Return how many steps takes from initial to target {cycles, instructions}
    void ForceState(const State& initial) noexcept;
    bool Compate(const State& state) const noexcept;
    Lifetime GetLifetime() const noexcept;


    struct Impl;
private:
    std::shared_ptr<Impl> impl_;
};

}
