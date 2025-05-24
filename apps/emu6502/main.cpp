#include <fstream>
#include <fmt/format.h>
#include <cpu6502/cpu6502.h>
#include <cpu6502/cpu6502Impl.h>

#include <ram/ram.h>
#include <rom/rom.h>
#include <common/common.h>

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

std::string ReadFileToString(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
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

// "pc": 35714,
//     "s": 81,
//     "a": 203,
//     "x": 117,
//     "y": 162,
//     "p": 106,

std::string ParseSingleTest(const Json& test, Cpu6502::State& initial, Cpu6502::State& final, std::vector<Membus::TestSequence>& testSequence)
{
    try
    {
        auto parse = [&test](Cpu6502::State& state, const std::string& name)
        {
            state.PC = test.at(name).at("pc");
            state.SP = test.at(name).at("s");
            state.A = test.at(name).at("a");
            state.X = test.at(name).at("x");
            state.Y = test.at(name).at("y");
            state.S = test.at(name).at("p");
            for (const auto& j : test.at(name).at("ram"))
            {
                state.mem.push_back( {j[0], j[1]} );
            }
        };

        parse(initial, "initial");
        parse(final, "final");

        for (const auto& j : test.at("cycles"))
        {
            testSequence.push_back( { .address = j[0], .data = j[1], .isRead = (j[2] == "read")?true:false   } );
        }
    }
    catch(const std::exception& e)
    {
        fmt::println("Error in json: {}", test.dump(3));
        fmt::println("exception: {}", e.what());
        abort();
    }

    return test.at("name");
}

void JsonTestTest(size_t opcode, std::string instr, size_t startFrom = 0)
{
    std::string numStr = fmt::format("{:02x}", opcode);
    std::string fileName = "/home/nikolay/ndn/cpp-6502/tests/SingleStepTests/6502/v1/" + numStr + ".json";
    auto tests = Json::parse( ReadFileToString( fileName ) );
    fmt::println("Opcode: x{:02x}, {} tests loaded", opcode, tests.size());
    int fails = 0;
    int testNum = 0;
    for (auto& test : tests)
    {
        if(testNum < startFrom)
        {
            testNum++;
            continue;
        }
        if (fails > 0)
        {
            break;
        }

        Cpu6502::State initial;
        Cpu6502::State final;
        std::vector<Membus::TestSequence> testSequence;
        std::string name = ParseSingleTest(test, initial, final, testSequence);

        Ram ram(0x0000, 0xFFFF);

        Membus memBus;
        memBus.Connect(&ram, ram.Start(), ram.End());

        auto cpu = Cpu6502::CreateInstance(&memBus);
        cpu->PowerOn();

        if (6 == testNum)
        {
            int rtrt = 34;
            (void)rtrt;
        }
        cpu->ForceState(initial);
        memBus.LookForSequence(testSequence);

        while (true)
        {
            memBus.Clock();
            cpu->Clock();

            if (cpu->IsInstructionDone() &&
                cpu->Compate(final) &&
                memBus.IsSequenceOk() &&
                memBus.SequenceStepsLeft() == 0)
            {
                //fmt::println( "OK: {}", name );
                break;
            }

            if (!memBus.IsSequenceOk())
            {

                fmt::println( "Fail name: {}", name );
                fmt::println( "MemSeqStep: {}, left: {}", memBus.SequenceStep(), memBus.SequenceStepsLeft());
                fmt::println( "cycles: {}, instructions: {}", cpu->GetLifetime().cycleCounter, cpu->GetLifetime().instructionCounter);
                fmt::println( "cpu: {}", cpu->Dump());
                fmt::println( "\n\nInitital part: {}", test.at("initial").dump() );
                fmt::println( "Final    part: {}", test.at("final").dump() );
                fmt::println( "Cycles   part: {}", test.at("cycles").dump() );
                fmt::println( "\nTest {} fail, json:\n{}\n-----\n", testNum, test.dump());
                fmt::println("\n\n[[[  instr: {}, opcode: x{:02X}, test: {}  ]]]\n", instr, opcode, testNum);
                fails++;
                break;
            }
            if (memBus.SequenceStepsLeft() == 0 && cpu->IsInstructionDone())
            {
                fmt::println( "Fail: {}", name );
                fmt::println( "Memory looks OK, but cpu compare fails (maybe)" );
                fmt::println( "MemSeqStep: {}, left: {}", memBus.SequenceStep(), memBus.SequenceStepsLeft());
                fmt::println( "cycles: {}, instructions: {}", cpu->GetLifetime().cycleCounter, cpu->GetLifetime().cycleCounter);
                fmt::println( "cpu: {}", cpu->Dump());
                fmt::println( "\n\nInitital part: {}", test.at("initial").dump() );
                fmt::println( "Final    part: {}", test.at("final").dump() );
                fmt::println( "Cycles   part: {}", test.at("cycles").dump() );
                fmt::println( "\nTest {} fail, json:\n{}\n-----\n", testNum, test.dump());
                fmt::println("\n\n[[[  instr: {}, opcode: x{:02X}, test: {}  ]]]\n", instr, opcode, testNum);
                fails++;
                break;
            }
        }
        testNum++;
    }
    if (fails)
    {
        fmt::println("test failed, opcode: {}", opcode);
        fmt::println("fails: {}", fails);
    }
    else
    {
        fmt::println("Opcode: {} [ OK ]", opcode);
    }
}

int Main(int /*argc*/, char* /*argv*/[])
{
    fmt::println("Hello world");

    //BootTest();
    //BinImageTest();
    const auto& instructions = Cpu6502::Impl::Meta::Instructions();

    size_t opcode = 0x1E; // ASL
    size_t startFromTest = 11; // 0;
    for(size_t i = opcode; i < 256; i++)
    {
        if (instructions[i].name == "INVALID")
        {
            fmt::println("test: x{:02X}, invalid command, SKIPEPD", i);
        }
        else
        {
            fmt::println("Start test: x{:02X}, {}", i, instructions[i].name);
            JsonTestTest(i, instructions[i].name, startFromTest);
        }
        fmt::println("\n-------------------------------\n");
    }
    fmt::println("\n\n\nGoodbye !!!");

    return 0;
}

}


int main(int argc, char* argv[]){return cpp6502::emu::Main(argc, argv);}
