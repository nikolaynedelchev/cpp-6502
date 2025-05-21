#include <fmt/format.h>
#include <common/common.h>
#include <coroutine>
#include <iostream>
#include <common/routine.h>

#define WaitClock() co_await Routine::Suspend{}
#define WaitRoutine(r) while(r.Resume()){co_await Routine::Suspend{};}
namespace cpp6502::opcodes_codegen
{
const std::string& GetOpcodesJsonString();

Routine CoroutineZ() {
    std::cout << "CoroutineZ step 1\n";
    WaitClock();
    std::cout << "CoroutineZ step 2\n";
    WaitClock();
    std::cout << "CoroutineZ step 3\n";
}

Routine CoroutineA() {
    std::cout << "CoroutineA step 1\n";
    WaitClock();
    std::cout << "CoroutineA step 2\n";
    WaitClock();
    std::cout << "CoroutineA step 3 (calling Z)\n";
    auto z = CoroutineZ();
    WaitRoutine(z);
    std::cout << "CoroutineA step 4 (exit Z)\n";
}

Routine CoroutineB() {
    std::cout << "CoroutineB step 1\n";
    WaitClock();
    std::cout << "CoroutineB step 2 (calling A)\n";
    auto r = CoroutineA();
    WaitRoutine(r);
    std::cout << "CoroutineB step 3 (exit A)\n";
    WaitClock();
    std::cout << "CoroutineB step 3\n";
}

void CoroutineTestB()
{
    fmt::println("Creating coroutine B");
    Routine b;
    b = CoroutineB();
    fmt::println("Executing coroutine B");

    fmt::println("CLOCK...");
    while(b.Resume())
    {
        fmt::println("CLOCK...");
    }

    fmt::println("Destroing first coroutine B");
    b.Destroy();
    fmt::println("Creating second coroutine B");
    b = CoroutineB();
    fmt::println("Executing second coroutine B");
    while(b.Resume());
}

void CoroutineTest()
{
    {
        fmt::println("Creating coroutine A");
        Routine a = CoroutineA();
        fmt::println("Creating coroutine B");
        auto b = CoroutineB();

        fmt::println("Stepping into the while");
        while (a.Resume() || b.Resume())
        {
            fmt::println("While step");
        }

        a.Destroy();
        a = CoroutineB();
        while(a.Resume());
    }

    fmt::println("While done");

}

int Main(int /*argc*/, char* /*argv*/[])
{
    fmt::println("Hello world");
    CoroutineTestB();
    Json opcodes = Json::parse( GetOpcodesJsonString() );

    fmt::println("Opcodes dump:\n{}", opcodes.dump(3));

    fmt::println("\n--------------------\nGoodbye\n");

    return 0;
}

}

int main(int argc, char* argv[]){return cpp6502::opcodes_codegen::Main(argc, argv);}
