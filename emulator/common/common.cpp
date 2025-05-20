#include "common.h"

namespace cpp6502
{


IDevice::~IDevice() = default;

Error::~Error()
{
    if (thrown_)
    {
        return;
    }
    Print();
    fmt::println("Missing exception throw !");
    abort();
}

Error::Error(const char *errorName, std::string errorMsg, source_location l)
    : std::exception()
    , thrown_(false)
{
    errorString_ += fmt::format("error: {}\n", errorName);
    errorString_ += fmt::format("msg: {}\n", errorMsg);
    errorString_ += fmt::format("file: {}\n", l.file_name());
    errorString_ += fmt::format("function: {}\n", l.function_name());
    errorString_ += fmt::format("line: {}\n", l.line());
}

const char *Error::what() const noexcept
{
    return errorString_.c_str();
}

inline const Error &Error::Print() const
{
    fmt::println("ERROR: {}", what());
    return *this;
}

void Error::Throw() const
{
    thrown_ = true;
    throw *this;
}

}

