#include <fmt/format.h>
#include <common/common.h>

namespace cpp6502::opcodes_codegen
{
const std::string& GetOpcodesJsonString();

int Main(int /*argc*/, char* /*argv*/[])
{
    fmt::println("Hello world");
    Json opcodes = Json::parse( GetOpcodesJsonString() );

    fmt::println("Opcodes dump:\n{}", opcodes.dump(3));

    fmt::println("\n--------------------\nGoodbye\n");

    return 0;
}

}

int main(int argc, char* argv[]){return cpp6502::opcodes_codegen::Main(argc, argv);}
