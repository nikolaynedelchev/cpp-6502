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

std::suspend_always Routine::promise_type::yield_value(const char *value) noexcept
{
    current_yield = value;
    return {};
}

std::suspend_always Routine::promise_type::yield_value(Empty) noexcept
{
    return {};
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

Routine::Routine() = default;

Routine::Routine(std::coroutine_handle<promise_type> h) : handle_(h)
{

}

Routine::Routine(Routine &&other) noexcept : handle_(std::move(other.handle_))
{
    other.handle_ = {};
}

Routine &Routine::operator=(Routine &&other) noexcept
{
    if (this != &other)
    {
        Destroy();
        handle_ = std::move(other.handle_);
        other.handle_ = {};
    }
    return *this;
}

Routine::operator bool() const noexcept
{
    return handle_ && (!handle_.done());
}

Routine::~Routine()
{
    Destroy();
}

bool Routine::Done() const
{
    if (handle_ == nullptr)
    {
        return true;
    }
    return handle_.done();
}

bool Routine::Resume()
{
    if (!handle_ || handle_.done())
    {
        return false;
    }
    handle_.resume();
    return !handle_.done();
}

void Routine::Destroy()
{
    if (handle_)
    {
        handle_.destroy();
    }
    handle_ = {};
}

const char *Routine::LastYielded() const noexcept
{
    if (handle_)
    {
        return handle_.promise().current_yield;
    }
    return nullptr;
}


}
