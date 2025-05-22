#include <fmt/format.h>
#include <common/common.h>
#include <common/routine.h>

#define WaitClock() co_await Routine::Suspend{}
#define WaitRoutine(r) while(r.Resume()){co_await Routine::Suspend{};}
namespace cpp6502::tests
{

Routine CoroutineZ() {
    fmt::println("CoroutineZ step 1");
    WaitClock();
    fmt::println("CoroutineZ step 2");
    WaitClock();
    fmt::println("CoroutineZ step 3");
}

Routine CoroutineA() {
    fmt::println("CoroutineA step 1");
    WaitClock();
    fmt::println("CoroutineA step 2");
    WaitClock();
    fmt::println("CoroutineA step 3 (calling Z)");
    auto z = CoroutineZ();
    WaitRoutine(z);
    fmt::println("CoroutineA step 4 (exit Z)");
}

Routine CoroutineB() {
    fmt::println("CoroutineB step 1");
    WaitClock();
    fmt::println("CoroutineB step 2 (calling A)");
    auto r = CoroutineA();
    WaitRoutine(r);
    fmt::println("CoroutineB step 3 (exit A)");
    WaitClock();
    fmt::println("CoroutineB step 3");
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
    fmt::println("Tests Hello world");

    CoroutineTestB();

    fmt::println("\n--------------------\nGoodbye\n");
    return 0;
}

}

int main(int argc, char* argv[]){return cpp6502::tests::Main(argc, argv);}
