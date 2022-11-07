/**
* Copyright 2015-2022 Daniel Weiner
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#pragma once

#include <Fusion/Types.h>

#include <fmt/format.h>

#include <iosfwd>
#include <string>

namespace Fusion
{
struct Error;
class Failure;

using ErrorCode = int32_t;

//
//
//
ErrorCode GetLastError();

//
//
//
std::string_view ToString(
    ErrorCode platformCode,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    ErrorCode platformCode,
    char(&buffer)[LENGTH])
{
    return ToString(platformCode, buffer, LENGTH);
}

//
//
//
std::string ToString(ErrorCode platformCode);

//
//
//
std::ostream& operator<<(std::ostream& o, ErrorCode platformCode);

//
//
//
constexpr ErrorCode ErrorCodeInvalid = (ErrorCode)(INT32_MIN);

//
//
//
constexpr ErrorCode ErrorCodeGeneric = (ErrorCode)(-1);

//
//
//
constexpr ErrorCode ErrorCodeSuccess = (ErrorCode)(0);

//
//
//
class Error final
{
public:
    //
    //
    //
    ErrorCode platformCode = ErrorCodeInvalid;

    //
    //
    //
    ErrorCode error = ErrorCodeGeneric;

    //
    //
    //
    std::string_view errstr;

public:
    //
    //
    //
    static Error Errno();

    //
    //
    //
    static Error Errno(ErrorCode platformCode);

    //
    //
    //
    constexpr Error(ErrorCode error, std::string_view errstr)
        : error(error)
        , errstr(errstr)
    { }

    explicit operator bool() const;

    //
    //
    //
    Error operator()(ErrorCode platformCode) const;

    bool operator==(const Error& err) const;
    bool operator!=(const Error& err) const;

private:
    //
    //
    //
    Error(const Error& err, ErrorCode platformCode);
};

//
//
//
std::string_view ToString(
    const Error& error,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    const Error& error,
    char(&buffer)[LENGTH])
{
    return ToString(error, buffer, LENGTH);
}

//
//
//
std::string ToString(const Error& error);

//
//
//
std::ostream& operator<<(std::ostream& o, const Error& error);

//
//
//
constexpr static const Error E_FAILURE{ ErrorCodeGeneric, "E_FAILURE" };

//
//
//
constexpr static const Error E_SUCCESS{ ErrorCodeSuccess, "E_SUCCESS" };

//
//
//
constexpr static const Error E_ACCESS_DENIED{ 2, "E_ACCESS_DENIED" };

//
//
//
constexpr static const Error E_DISK_FULL{ 3, "E_DISK_FULL" };

//
//
//
constexpr static const Error E_EXISTS{ 4, "E_EXISTS" };

//
//
//
constexpr static const Error E_INVALID_ARGUMENT{ 5, "E_INVALID_ARGUMENT" };

//
//
//
constexpr static const Error E_NOT_FOUND{ 6, "E_NOT_FOUND" };

//
//
//
constexpr static const Error E_INSUFFICIENT_RESOURCES{ 7, "E_INSUFFICIENT_RESOURCES" };

//
//
//
constexpr static const Error E_INTERRUPTED{ 8, "E_INTERRUPTED" };

//
//
//
constexpr static const Error E_RESOURCE_NOT_AVAILABLE{ 9, "E_RESOURCE_NOT_AVAILABLE" };
//
//
//
class Failure
{
    using ErrorType = Fusion::Error;

public:

    //
    //
    //
    static Failure Errno();

    //
    //
    //
    Failure();

    //
    //
    //
    explicit Failure(ErrorCode platformCode);

    //
    //
    //
    Failure(ErrorType error);

    //
    //
    //
    Failure(std::string_view message);

    //
    //
    //
    template<typename ...Args>
    Failure(fmt::format_string<Args...> format, Args&& ...args);

    //
    //
    //
    ~Failure();

    //
    //
    //
    operator bool() const;

    //
    //
    //
    ErrorCode Code() const;

    //
    //
    //
    ErrorType& Error();

    //
    //
    //
    const ErrorType& Error() const;

    //
    //
    //
    std::string_view Message() const;

    //
    //
    //
    std::string Summary() const;

    //
    //
    //
    Failure& WithCode(ErrorCode platformCode);

    //
    //
    //
    Failure& WithContext(std::string message);

    //
    //
    //
    template<typename ...Args>
    Failure& WithContext(fmt::format_string<Args...> format, Args&& ...args);

private:
    std::string m_message;
    ErrorType m_error;
};

//
//
//
std::string_view ToString(
    const Failure& error,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    const Failure& error,
    char(&buffer)[LENGTH])
{
    return ToString(error, buffer, LENGTH);
}

//
//
//
std::string ToString(const Failure& fail);

//
//
//
std::ostream& operator<<(std::ostream& o, const Failure& fail);

}  // namespace Error

#define FUSION_IMPL_ERROR 1
#include <fusion/impl/Error.h>
