#pragma once
#include <cstdint>
#include <stdint.h>

namespace cpp6502
{

struct source_location
{
public:
    static constexpr source_location current(
        const char* file = __builtin_FILE(),
        const char* function = __builtin_FUNCTION(),
        const uint_least32_t line = __builtin_LINE(),
#if defined(__GNUC__)
        const uint_least32_t column = 0
#else
        const uint_least32_t column = __builtin_COLUMN()
#endif
        ) noexcept
    {
        source_location result;
        result.file_ = file;
        result.function_ = function;
        result.line_ = line;
        result.column_ = column;
        return result;
    }

    constexpr const char* file_name() const noexcept
    {
        return file_;
    }

    constexpr const char* function_name() const noexcept
    {
        return function_;
    }

    constexpr uint_least32_t line() const noexcept
    {
        return line_;
    }

    constexpr std::uint_least32_t column() const noexcept
    {
        return column_;
    }

private:
    const char* file_ = "";
    const char* function_ = "";
    uint_least32_t line_ = 0;
    uint_least32_t column_ = 0;
};

}
