#pragma once

#include <coroutine>
#include <fmt/format.h>

namespace cpp6502
{

struct Routine
{
    struct Suspend
    {
        bool await_ready() const noexcept;
        void await_suspend(std::coroutine_handle<>) const noexcept;
        void await_resume() const noexcept;
    };

    struct promise_type
    {
        Routine get_return_object() noexcept;
        std::suspend_always initial_suspend() noexcept;
        std::suspend_always final_suspend() noexcept;
        void return_void() noexcept;
        void unhandled_exception() noexcept;

        // use custom memory allocation
        static void* operator new(size_t size);
        static void operator delete(void* ptr, size_t size) noexcept;
    };

    ///////////////////

    Routine();
    explicit Routine(std::coroutine_handle<promise_type> h);
    Routine(const Routine&) = delete;
    Routine& operator=(const Routine&) = delete;

    Routine(Routine&& other) noexcept;
    Routine& operator=(Routine&& other) noexcept;

    ~Routine();

    bool Done() const;
    bool Resume();
    void Destroy();

    std::coroutine_handle<promise_type> handle_;
};

}
