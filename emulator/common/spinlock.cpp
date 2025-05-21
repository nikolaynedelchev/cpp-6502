#include "spinlock.h"

#include <thread>
#include <chrono>

namespace cpp6502
{

SpinLock::SpinLock() = default;

void SpinLock::Lock()
{
    int attempts = 0;
    while (m_flag.test_and_set(std::memory_order_acquire))
    {
        attempts++;
        if (attempts < 2048)
        {
            // Be stubborn, busy spin
        }
        else if (attempts < 2050)
        {
            // Be nice, yield the thread
            std::this_thread::yield();
        }
        else
        {
            // Give up for now
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            attempts = 0;
        }
    }
}

void SpinLock::Unlock()
{
    m_flag.clear(std::memory_order_release);
}

SpinLockGuard::SpinLockGuard(SpinLock &lock) : m_lock(lock)
{
    m_lock.Lock();
}

SpinLockGuard::~SpinLockGuard()
{
    m_lock.Unlock();
}

}
