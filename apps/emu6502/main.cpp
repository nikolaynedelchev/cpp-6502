#include <fmt/format.h>
#include <cpu6502/cpu6502.h>
#include <cpu6502/cpu6502Impl.h>

#include <ram/ram.h>
#include <rom/rom.h>

namespace cpp6502::emu
{

int Main(int /*argc*/, char* /*argv*/[])
{
    fmt::println("Hello world");
    Ram ram(0x0000, 0x7FFF);
    Rom rom(0xC000, 0xFFFF);

    Membus memBus;
    memBus.Connect(&ram, ram.Start(), ram.End());
    memBus.Connect(&rom, rom.Start(), rom.End());

    rom[0xFFFC] = 0x34;
    rom[0xFFFD] = 0x12;

    auto cpu = Cpu6502::CreateInstance(&memBus);
    cpu->PowerOn();

    for(auto cycle = 0; cycle < 8; cycle++)
    {
        auto before = cpu->Dump();
        memBus.Clock();
        fmt::println("Cycle: {} ended,\nInitial CPU: {}\nFinal   CPU: {}\n----------", cycle + 1, before, cpu->Dump());
        cpu->Clock();
    }

    return 0;
}

}


int main(int argc, char* argv[]){return cpp6502::emu::Main(argc, argv);}
