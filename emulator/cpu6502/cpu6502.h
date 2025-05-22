#pragma once
#include <common/common.h>
#include <membus/membus.h>
#include <memory>

namespace cpp6502
{

class Cpu6502 : public IDevice
{
private:
    Cpu6502();

public:
    std::shared_ptr<Cpu6502> CreateInstance(IMemory* memory);

    // IDevice interface
    void Clock() final;

    struct Impl;
private:
    std::shared_ptr<Impl> impl_;
};

}
