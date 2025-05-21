#include "routine.h"
#include "mempool.h"

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

void *Routine::promise_type::operator new(size_t size)
{
    return MemPool::Malloc(size);
}

void Routine::promise_type::operator delete(void *ptr, size_t size) noexcept
{
    try
    {
        MemPool::Free(ptr, size);
    }
    catch (const std::exception& e)
    {
        fmt::println("[Routine] Exception in delete operator:\n{}", e.what());
        abort();
    }
    catch (...)
    {
        fmt::println("[Routine] Unknown Exception in delete operator");
        abort();
    }
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
