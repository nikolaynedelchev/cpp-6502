#include "stopwatch.h"

namespace cpp6502
{

int64_t Stopwatch::Measure() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>( _measureDuration() ).count();
}

int64_t Stopwatch::MeasureMicros() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>( _measureDuration() ).count();
}

void Stopwatch::Pause()
{
    if (m_isPaused)
    {
        return;
    }
    m_isPaused = true;
    m_pauseStartTime = std::chrono::steady_clock::now();
}

void Stopwatch::Resume()
{
    if (!m_isPaused)
    {
        return;
    }
    m_isPaused = false;
    m_pauseDuration += (std::chrono::steady_clock::now() - m_pauseStartTime);
}

int64_t Stopwatch::Reset()
{
    auto result = Measure();
    *this = Stopwatch();
    return result;
}

const Stopwatch &Stopwatch::GetGlobalTimer()
{
    static Stopwatch s_sw;
    return s_sw;
}

std::chrono::steady_clock::duration Stopwatch::_measureDuration() const
{
    auto now = std::chrono::steady_clock::now();
    auto pauseTime = m_pauseDuration;
    if (m_isPaused)
    {
        pauseTime += now - m_pauseStartTime;
    }

    return (now - m_startTime) - pauseTime;
}

}
