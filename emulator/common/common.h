#pragma once
#include <nlohmann/json.hpp>
#include <stdint.h>
#include <fmt/format.h>
#include "source_location.h"
#include "spinlock.h"
#include "stopwatch.h"

namespace cpp6502
{

using Json = nlohmann::json;
using Byte = uint8_t;
using SByte = int8_t;
using Word = uint16_t;
using SWord = int16_t;
using Address = uint16_t;

class IDevice
{
public:
    virtual ~IDevice();

    virtual void Clock() = 0;
};

struct Error : public std::exception
{
    ~Error();
    Error(const char* errorName, std::string errorMsg = "", source_location l = source_location::current());
    virtual const char* what() const noexcept;

    template<typename... Args>
    Error& Msg(fmt::format_string<Args...> fmtStr, Args&&... args);

    const Error& Print() const;
    void Throw() const;

protected:
    std::string errorString_;
    mutable bool thrown_ = false;
};

template<typename... Args>
Error& Error::Msg(fmt::format_string<Args...> fmtStr, Args &&...args)
{
    errorString_ += fmt::format(fmtStr, std::forward<Args>(args)...) + "\n";
    return *this;
}


}
