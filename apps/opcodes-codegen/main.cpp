#include <fmt/format.h>
#include <common/common.h>
#include <fstream>
#include <set>

namespace cpp6502::opcodes_codegen
{

const std::string& GetOpcodesJsonString();


static std::string s_start = R"(
// AUTO GENERATED FILE
// CHECK 'opcodes-codegen'

#include "cpu6502Impl.h"

namespace cpp6502
{

const Cpu6502::Impl::Meta::InstructionsLookup &Cpu6502::Impl::Meta::Instructions()
{
    static InstructionsLookup s_lookup = {)";

static std::string s_mid = R"(
        {
            .opcode = [[OPCODE]],
            .name = "[[NAME]]",
            .memoryMode = [[MEMMODE]],
            .description = "[[DESC]]",
            .size = [[LENGTH]],
            .instruction = [[INSTRUCTION]]
        })";

static std::string s_end = R"(
    };
    return s_lookup;
}

}
)";


static std::string s_instrDecl = "    Routine Instr_[[NAME]](uint16_t memAcc);\n";

static std::string s_instrImpl = R"(
// [[NAME]]: [[DESC]]
Routine Cpu6502::Impl::Instr_[[NAME]]()
{
    Error("Cpu6502::Impl", "Not implemented")
        .Msg("Instr: {}", "[[NAME]]")
        .Msg("MemMode: {}", state.addressMode)
        .Msg("CPU: {}", ToString())
        .Throw();

    co_return;
}
)";

void ReplaceAll(std::string& source, const std::string& from, const std::string& to)
{
    size_t startPos = 0;
    while ((startPos = source.find(from, startPos)) != std::string::npos)
    {
        source.replace(startPos, from.length(), to);
        startPos += to.length();
    }
}

static std::string NumToHex(int i)
{
    std::ostringstream oss;
    oss << "0x"
        << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
        << (i & 0xFF);
    return oss.str();
}

int Main(int /*argc*/, char* /*argv*/[])
{
    (void)NumToHex;

    fmt::println("Hello world");

    std::string codes[256];
    for(size_t i = 0; i < 256; i++)
    {
        codes[i] = s_mid;
    }

    std::vector<Json> allcodesJson = Json::parse( GetOpcodesJsonString() ).at("opcodes");
    std::set< std::pair<std::string, std::string> > uniqueInstr;

    for (const Json& j : allcodesJson)
    {
        std::string opcodeStr = j.at("opcode");
        std::string desc = j.at("description");
        std::string sizeStr = j.at("bytes");
        std::string name = j.at("name");
        std::string mode = j.at("mode");

        size_t opcode = std::stoul(opcodeStr, nullptr, 0);
        auto& inst = codes[opcode];

        ReplaceAll(inst, "[[OPCODE]]", opcodeStr);
        ReplaceAll(inst, "[[NAME]]", name);
        ReplaceAll(inst, "[[MEMMODE]]", mode);
        ReplaceAll(inst, "[[DESC]]", desc);
        ReplaceAll(inst, "[[LENGTH]]", sizeStr);
        ReplaceAll(inst, "[[INSTRUCTION]]", "&Cpu6502::Impl::Instr_" + name);

        uniqueInstr.insert({name, desc}); // sort by opcode
    }

    for(size_t i = 0; i < 256; i++)
    {
        ReplaceAll(codes[i], "[[OPCODE]]", NumToHex(i));
        ReplaceAll(codes[i], "[[NAME]]", "INVALID");
        ReplaceAll(codes[i], "[[MEMMODE]]", "MemAcc_INVALID");
        ReplaceAll(codes[i], "[[DESC]]", "Opcode not in use");
        ReplaceAll(codes[i], "[[LENGTH]]", "0");
        ReplaceAll(codes[i], "[[INSTRUCTION]]", "nullptr");
    }

    std::string sourceFile = s_start + "\n";
    for(size_t i = 0; i < 256; i++)
    {
        sourceFile += codes[i];
        if (i != 255)
        {
            sourceFile += ",";
        }
        sourceFile += "\n";
    }
    sourceFile += s_end;

    fmt::println("{}", sourceFile);

    std::string fileName = "cpu6502_instr.gen.cpp";
    std::ofstream outFile(fileName);
    if (outFile)
    {
        outFile << sourceFile << std::flush;
        fmt::println("Generated source saved in : {}", fileName);
    }
    else
    {
        fmt::println("File error, file not saved");
    }
    outFile.close();

    std::string declarations;
    std::string implementations;

    for(const auto& p : uniqueInstr)
    {
        const auto& name = p.first;
        const auto& desc = p.second;

        auto decl = s_instrDecl;
        ReplaceAll(decl, "[[NAME]]", name);
        declarations += decl;

        auto impl = s_instrImpl;
        ReplaceAll(impl, "[[NAME]]", name);
        ReplaceAll(impl, "[[DESC]]", desc);
        implementations += impl;
    }

    fmt::println("----------------------------");
    fmt::println("Declarations: ");
    fmt::println("");
    fmt::println("{}", declarations);
    fmt::println("----------");

    fmt::println("----------------------------");
    fmt::println("Implementation: ");
    fmt::println("");
    fmt::println("{}", implementations);
    fmt::println("----------");

    fileName = "helper.gen.cpp";
    std::ofstream helperOutFile(fileName);
    if (helperOutFile)
    {
        helperOutFile << declarations << "\n" << "/////////////////////"
                      << implementations << "\n" << std::flush;
        fmt::println("Generated helper saved in : {}", fileName);
    }
    else
    {
        fmt::println("File error, helper file not saved");
    }
    helperOutFile.close();

    fmt::println("\n--------------------\nGoodbye\n");
    return 0;
}

}

int main(int argc, char* argv[]){return cpp6502::opcodes_codegen::Main(argc, argv);}
