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

#include <ctime>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

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
    m_logBuffer.reserve(m_params.bufferSize);
    m_jsonBuffer.reserve(m_params.bufferSize);
}

ConsoleLogger::~ConsoleLogger()
{
    using namespace std::string_view_literals;

    if (!m_logBuffer.empty())
    {
        Console::Write(m_params.target, m_logBuffer);
        m_logBuffer.clear();
    }
    if (m_params.colors)
    {
        Console::Write(m_params.target, "\x1B[0m"sv);
    }
}

void ConsoleLogger::FlushBuffer(
    LogLevel level,
    std::string& buffer)
{
    bool capacity = (buffer.size() >= m_params.bufferSize);

    if (level >= LogLevel::Warning || capacity)
    {
        Console::Write(m_params.target, buffer);
        buffer.clear();
    }

    if (capacity)
    {
        if (buffer.capacity() > m_params.bufferSize)
        {
            buffer.resize(m_params.bufferSize);
            buffer.shrink_to_fit();
        }
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
    using namespace std::chrono;
    using namespace rapidjson;

    Document doc;
    Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    if (record.location)
    {
        Value location(kObjectType);

        location.AddMember(
            "filename",
            Value(
                record.location.shortFilename.data(),
                record.location.shortFilename.size(),
                allocator),
            allocator);

        location.AddMember(
            "lineno",
            record.location.lineno,
            allocator);

        doc.AddMember("location", std::move(location), allocator);
    }

    doc.AddMember(
        "logger",
        Value(
            record.logger.data(),
            record.logger.size(),
            allocator),
        allocator);

    {
        Value level(kObjectType);
        std::string_view str = ToString(record.level);

        level.AddMember(
            "desc",
            Value(
                str.data(),
                str.size(),
                allocator),
            allocator);

        level.AddMember("level", uint8_t(record.level), allocator);
        doc.AddMember("level", std::move(level), allocator);
    }

    doc.AddMember("threadId", record.threadId, allocator);

    char buffer[32] = { 0 };
    std::string_view time;
    {
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

        time = std::string_view(buffer, count);
    }
    
    if (!time.empty())
    {
        doc.AddMember(
            "timestamp",
            Value(
                time.data(),
                time.size(),
                allocator),
            allocator);
    }

    doc.AddMember(
        "message",
        Value(
            record.message.c_str(),
            record.message.size(),
            allocator),
        allocator);

    StringBuffer buf;
    PrettyWriter<rapidjson::StringBuffer> writer(buf);
    doc.Accept(writer);

    size_t length = buf.GetLength();
    m_jsonBuffer.reserve(length);
    m_jsonBuffer.append(buf.GetString(), length);
    m_jsonBuffer += "\n";

    FlushBuffer(record.level, m_jsonBuffer);
}

void ConsoleLogger::LogText(const LogRecord& record)
{
    using namespace std::chrono;
    using namespace std::string_view_literals;
    using Color = Console::Color;

    m_logBuffer.reserve(64 + record.message.size());

    if (m_params.colors)
    {
        m_logBuffer.append(ToString(Color::Reset));

        switch (record.level)
        {
        case LogLevel::Fatal:
            m_logBuffer.append(ToString(Color::Magenta));
            break;
        case LogLevel::Error:
            m_logBuffer.append(ToString(Color::Red));
            break;
        case LogLevel::Warning:
            m_logBuffer.append(ToString(Color::Yellow));
            break;
        case LogLevel::Trace:
            m_logBuffer.append(ToString(Color::Cyan));
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

        m_logBuffer.append("["sv);
        m_logBuffer.append(std::string_view(buffer, count));
        m_logBuffer.append("]"sv);
    }

    if (!record.logger.empty())
    {
        m_logBuffer.append("["sv);
        m_logBuffer.append(record.logger);
        m_logBuffer.append("]"sv);
    }

    m_logBuffer.append("["sv);
    m_logBuffer.append(ToString(record.level));
    m_logBuffer.append("]"sv);

#ifndef _NDEBUG
    if (record.location)
    {
        m_logBuffer.append("["sv);
        m_logBuffer.append(record.location.shortFilename);
        m_logBuffer.append(":"sv);
        m_logBuffer.append(std::to_string(record.location.lineno));
        m_logBuffer.append("]"sv);
    }
#endif

    m_logBuffer.append(" "sv);
    m_logBuffer.append(record.message);
    m_logBuffer.append("\n"sv);

    if (m_params.colors)
    {
        m_logBuffer.append(ToString(Color::Reset));
    }

    FlushBuffer(record.level, m_logBuffer);
}
// ConsoleLogger                                             END
// -------------------------------------------------------------
}  // namespace Fusion
