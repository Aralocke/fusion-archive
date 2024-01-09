/**
 * Copyright 2015-2024 Daniel Weiner
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

#include <Fusion/Console.h>

#include <Fusion/Assert.h>
#include <Fusion/Logging.h>
#include <Fusion/Platform.h>

#include <iostream>

#if FUSION_PLATFORM_POSIX
#include <ctime>
#endif

namespace Fusion
{
// -------------------------------------------------------------
// Console::Color                                          START
std::string_view ToString(Console::Color color)
{
    using namespace std::string_view_literals;
    using Color = Console::Color;

    static constexpr std::string_view s_colors[size_t(Color::_Count)] = {
        ""sv,                 // None
        "\033[m"sv,           // Reset
        "\033[1m"sv,          // Bold
        "\033[2m"sv,          // Dark
        "\033[4m"sv,          // Underline
        "\033[5m"sv,          // Blink
        "\033[7m"sv,          // Reverse
        "\033[8m"sv,          // Concealed
        "\033[K"sv,           // ClearLine

        "\033[30m"sv,         // Black
        "\033[31m"sv,         // Red
        "\033[32m"sv,         // Green
        "\033[33m"sv,         // Yellow
        "\033[34m"sv,         // Blue
        "\033[35m"sv,         // Magenta
        "\033[36m"sv,         // Cyan
        "\033[37m"sv,         // White

        "\033[40m"sv,         // BgBlack
        "\033[41m"sv,         // BgRed
        "\033[42m"sv,         // BgGreen
        "\033[43m"sv,         // BgYellow
        "\033[44m"sv,         // BgBlue
        "\033[45m"sv,         // BgMagenta
        "\033[46m"sv,         // BgCyan
        "\033[47m"sv,         // BgWhite

        "\033[33m\033[1m"sv,  // BoldYellow
        "\033[31m\033[1m"sv,  // BoldRed
        "\033[1m\033[41m"sv   // BoldOnRed
    };

    static_assert(
        sizeof(s_colors) / sizeof(std::string_view)
            == size_t(Color::_Count),
        "Colors is missing an entry");

    return s_colors[size_t(color)];
}

std::ostream& operator<<(
    std::ostream& o,
    Console::Color color)
{
    return o << ToString(color);
}
// Console::Color                                            END
// -------------------------------------------------------------
// Console                                                 START
void Console::Write(
    Target target,
    std::string_view str)
{
    FILE* fd = nullptr;

    switch (target)
    {
    case Target::Stderr:
        fd = stderr;
        break;
    case Target::Stdout:
        fd = stdout;
        break;
    default:
        return;
    }

    FUSION_ASSERT(fd);
    std::ignore = std::fwrite(str.data(), 1, str.size(), fd);
}
// Console                                                   END
// -------------------------------------------------------------
// ConsoleLogger                                           START
ConsoleLogger::ConsoleLogger(Params params)
    : m_params(std::move(params))
{
    m_buffer.reserve(m_params.bufferSize);
}

ConsoleLogger::~ConsoleLogger()
{
    using namespace std::string_view_literals;

    if (!m_buffer.empty())
    {
        Console::Write(m_params.target, m_buffer);
        m_buffer.clear();
    }
    if (m_params.colors)
    {
        Console::Write(m_params.target, "\x1B[0m"sv);
    }
}

void ConsoleLogger::Log(const LogRecord& record)
{
    std::lock_guard lock(m_mutex);

    if (m_params.json)
    {
        LogJson(record);
    }
    else
    {
        LogText(record);
    }
}

void ConsoleLogger::LogJson(const LogRecord& record)
{
    // NO-OP
}

void ConsoleLogger::LogText(const LogRecord& record)
{
    using namespace std::chrono;
    using namespace std::string_view_literals;
    using Color = Console::Color;

    m_buffer.reserve(64 + record.message.size());

    if (m_params.colors)
    {
        m_buffer.append(ToString(Color::Reset));

        switch (record.level)
        {
        case LogLevel::Fatal:
            m_buffer.append(ToString(Color::Magenta));
            break;
        case LogLevel::Error:
            m_buffer.append(ToString(Color::Red));
            break;
        case LogLevel::Warning:
            m_buffer.append(ToString(Color::Yellow));
            break;
        case LogLevel::Trace:
            m_buffer.append(ToString(Color::Cyan));
            break;
        default:
            break;
        }
    }

    if (m_params.timestamps)
    {
        char buffer[32] = { 0 };

        time_t input = system_clock::to_time_t(record.time);
        struct tm now = { 0 };

#if FUSION_PLATFORM_WINDOWS
        localtime_s(&now, &input);
#else
        localtime_r(&input, &now);
#endif

        size_t count = strftime(
            buffer,
            sizeof(buffer) - 1,
            "%Y-%m-%dT%H:%M:%S",
            &now);
        FUSION_ASSERT(count != 0);

        m_buffer.append("["sv);
        m_buffer.append(std::string_view(buffer, count));
        m_buffer.append("]"sv);
    }

    if (!record.logger.empty())
    {
        m_buffer.append("["sv);
        m_buffer.append(record.logger);
        m_buffer.append("]"sv);
    }

    m_buffer.append("["sv);
    m_buffer.append(ToString(record.level));
    m_buffer.append("]"sv);

#ifndef _NDEBUG
    if (record.location)
    {
        m_buffer.append("["sv);
        m_buffer.append(record.location.shortFilename);
        m_buffer.append(":"sv);
        m_buffer.append(std::to_string(record.location.lineno));
        m_buffer.append("]"sv);
    }
#endif

    m_buffer.append(" "sv);
    m_buffer.append(record.message);
    m_buffer.append("\n"sv);

    if (m_params.colors)
    {
        m_buffer.append(ToString(Color::Reset));
    }

    bool capacity = (m_buffer.size() >= m_params.bufferSize);

    if (record.level >= LogLevel::Warning || capacity)
    {
        Console::Write(m_params.target, m_buffer);
        m_buffer.clear();
    }

    if (capacity)
    {
        if (m_buffer.capacity() > m_params.bufferSize)
        {
            m_buffer.resize(m_params.bufferSize);
            m_buffer.shrink_to_fit();
        }
    }
}
// ConsoleLogger                                             END
// -------------------------------------------------------------
}  // namespace Fusion
