#pragma once

#include <atomic>

namespace cpp6502
{

struct SpinLock
{
public:
    void Lock();
    void Unlock();

    SpinLock();
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(SpinLock&&) = delete;
private:
    std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

class SpinLockGuard
{
public:
    explicit SpinLockGuard(SpinLock& lock);
    ~SpinLockGuard();
    SpinLockGuard(const SpinLockGuard&) = delete;
    SpinLockGuard& operator=(const SpinLockGuard&) = delete;
    SpinLockGuard(SpinLockGuard&&) = delete;
    SpinLockGuard& operator=(SpinLockGuard&&) = delete;
private:
    SpinLock& m_lock;
};

}

