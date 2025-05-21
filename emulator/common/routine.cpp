#include "routine.h"

namespace cpp6502
{

bool Routine::Suspend::await_ready() const noexcept{return false;}
void Routine::Suspend::await_resume() const noexcept {}
void Routine::Suspend::await_suspend(std::coroutine_handle<>) const noexcept {}

Routine Routine::promise_type::get_return_object() noexcept
{
    return Routine{std::coroutine_handle<promise_type>::from_promise(*this)};
}

std::suspend_always Routine::promise_type::initial_suspend() noexcept
{
    return {};
}

std::suspend_always Routine::promise_type::final_suspend() noexcept
{
    return {};
}

void Routine::promise_type::return_void() noexcept
{

}

void Routine::promise_type::unhandled_exception() noexcept
{
    std::terminate();
}

Routine::Routine(std::coroutine_handle<promise_type> h) : handle_(h)
{

}

Routine::~Routine()
{
    if (handle_) handle_.destroy();
}

bool Routine::Resume()
{
    if (!handle_.done())
    {
        handle_.resume();
    }
    return !handle_.done();
}


}
