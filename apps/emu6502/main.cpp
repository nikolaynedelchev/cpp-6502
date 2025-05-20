#include <fmt/format.h>

namespace cpp6502::emu
{

int Main(int /*argc*/, char* /*argv*/[])
{
    fmt::println("Hello world");
    return 0;
}

}

int main(int argc, char* argv[]){return cpp6502::emu::Main(argc, argv);}
