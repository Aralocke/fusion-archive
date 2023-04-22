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

#include <Fusion/Error.h>
#include <Fusion/StringUtil.h>

#include <array>
#include <iostream>

#include <fmt/format.h>

namespace Fusion
{
std::string ToString(ErrorCode platformCode)
{
    std::array<char, 256> buffer = { 0 };
    std::string_view result = ToString(platformCode, buffer.data(), buffer.size());

    return std::string{ result };
}

std::ostream& operator<<(std::ostream& o, ErrorCode platformCode)
{
    std::array<char, 256> buffer = { 0 };
    std::string_view result = ToString(platformCode, buffer.data(), buffer.size());

    return o << result;
}

Error Error::Errno()
{
    return Errno(GetLastError());
}

Error::Error(const Error& err, ErrorCode platformCode)
    : platformCode(platformCode)
    , error(err.error)
    , errstr(err.errstr)
{ }

Error::operator bool() const
{
    return this->error != ErrorCodeSuccess;
}

Error Error::operator()(ErrorCode platformCode) const
{
    return Error{ *this, platformCode };
}

bool Error::operator==(const Error& err) const
{
    return this->error == err.error;
}

bool Error::operator!=(const Error& err) const
{
    return !operator==(err);
}

std::string_view ToString(
    const Error& error,
    char* buffer,
    size_t length)
{
    if (buffer
        && length > 0
        && error.platformCode != ErrorCodeInvalid)
    {
        FixedStringBuilder builder(buffer, length);
        {
            auto res = fmt::format_to_n(
                builder.Head(),
                builder.Remaining(),
                "[{}] ",
                error.errstr);

            builder.Offset(res.size);
        }
        {
            std::string_view message = ToString(
                error.platformCode,
                builder.Head(),
                builder.Remaining());

            builder.Offset(message.size());
        }
        {
            auto res = fmt::format_to_n(
                builder.Head(),
                builder.Remaining(),
                " ({})",
                error.platformCode);

            builder.Offset(res.size);
        }
        return builder.View();
    }
    return error.errstr;
}

std::string ToString(const Error& error)
{
    std::array<char, 256> buffer = { 0 };
    std::string_view result = ToString(
        error,
        buffer.data(),
        buffer.size());

    return std::string{ result };
}

std::ostream& operator<<(std::ostream& o, const Error& error)
{
    std::array<char, 256> buffer = { 0 };
    std::string_view result = ToString(
        error,
        buffer.data(),
        buffer.size());

    return o << result;
}

Failure Failure::Errno()
{
    return Error::Errno();
}

Failure::Failure()
    : m_error(E_FAILURE)
{ }

Failure::Failure(ErrorCode platformCode)
    : m_error{ Error::Errno(platformCode) }
{ }

Failure::Failure(ErrorType error)
    : m_error{ error }
{ }

Failure::Failure(std::string_view message)
    : m_message(message)
    , m_error(E_FAILURE)
{ }

Failure::~Failure() = default;

Failure::operator bool() const
{
    return bool(m_error.error);
}

ErrorCode Failure::Code() const
{
    return m_error.error;
}

Failure::ErrorType& Failure::Error()
{
    return m_error;
}

const Failure::ErrorType& Failure::Error() const
{
    return m_error;
}

std::string_view Failure::Message() const
{
    return m_message;
}

std::string Failure::Summary() const
{
    using namespace std::string_view_literals;

    std::string summary;
    summary.reserve(256);

    if (m_error)
    {
        FixedStringBuilder builder(
            summary.data(),
            summary.capacity());

        builder += '[';
        builder.Offset(
            ToString(
                m_error,
                builder.Head(),
                builder.Remaining()).size());
        builder += "] "sv;
        summary.resize(builder.Size());
    }
    if (!m_message.empty())
    {
        summary += m_message;
    }

    return summary;
}

Failure& Failure::WithCode(ErrorCode platformCode)
{
    m_error = Error::Errno(platformCode);
    return *this;
}

Failure& Failure::WithContext(std::string message)
{
    using namespace std::string_view_literals;

    if (!m_message.empty())
    {
        message += ": "sv;
        message += m_message;
        m_message = std::move(message);
    }
    else
    {
        m_message = std::move(message);
    }
    return *this;
}
}  // namespace Fusion
