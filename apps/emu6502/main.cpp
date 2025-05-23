#include <fstream>
#include <fmt/format.h>
#include <cpu6502/cpu6502.h>
#include <cpu6502/cpu6502Impl.h>

#include <ram/ram.h>
#include <rom/rom.h>

namespace cpp6502::emu
{

std::vector<uint8_t> ReadBinaryFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer((size_t(fileSize)));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
    {
        throw std::runtime_error("Failed to read file: " + filePath);
    }

    return buffer;
}

void BootTest()
{
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
}

void BinImageTest()
{
    Ram ram(0x0000, 0xFFFF);

    Membus memBus;
    memBus.Connect(&ram, ram.Start(), ram.End());

    auto testImage = ReadBinaryFile("/home/nikolay/ndn/cpp-6502/tests/klaus2m5/bin_files/6502_functional_test.bin");
    ram.Overrite(0, testImage);
    ram[0xFFFC] = 0x00;
    ram[0xFFFD] = 0x04;

    auto cpu = Cpu6502::CreateInstance(&memBus);
    cpu->PowerOn();

    for(auto cycle = 0; cycle < 223100; cycle++)
    {
        auto before = cpu->Dump();
        memBus.Clock();
        cpu->Clock();
        fmt::println("Cycle: {} ended,\nInitial CPU: {}\nFinal   CPU: {}\n----------", cycle + 1, before, cpu->Dump());
        if (cycle % 20 == 0)
        {
            int a = 34;
            (void)a;
        }
    }
}

int Main(int /*argc*/, char* /*argv*/[])
{
    fmt::println("Hello world");

    //BootTest();
    BinImageTest();
    return 0;
}

}


int main(int argc, char* argv[]){return cpp6502::emu::Main(argc, argv);}
