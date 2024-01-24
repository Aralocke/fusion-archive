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

#include <Fusion/Internal/Logging.h>

#include <Fusion/Assert.h>
#include <Fusion/Platform.h>

#include <ctime>
#include <iostream>

#if FUSION_PLATFORM_POSIX
#include <syslog.h>
#endif

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace Fusion
{
// -------------------------------------------------------------
// Logging Root                                            START
std::atomic<bool> Internal::Logging::s_started{ false };
std::shared_ptr< Internal::Logging>
    Internal::Logging::s_instance{};
std::mutex Internal::Logging::s_mutex;

std::shared_ptr<Internal::Logging>
Internal::Logging::Instance()
{
    if (s_started)
    {
        return s_instance;
    }

    return nullptr;
}

void Internal::Logging::Start()
{
    if (!s_started)
    {
        std::lock_guard lock(s_mutex);

        if (s_started)
        {
            return;
        }

        s_instance = std::make_shared<Logging>();
        s_started.store(true);
    }
}

void Internal::Logging::Stop()
{
    if (s_started)
    {
        std::lock_guard lock(s_mutex);

        if (!s_started)
        {
            return;
        }

        s_instance.reset();
        s_started = false;
    }
}

Internal::Logging::~Logging()
{
    m_sinks.clear();
}

void Internal::Logging::AddSink(LogSink sink)
{
    m_sinks.emplace_back(std::move(sink));
}

LogLevel Internal::Logging::GetLogLevel() const
{
    return m_logLevel.load();
}

void Internal::Logging::Log(const LogRecord& record) const
{
    if (record.level >= m_logLevel.load())
    {
        for (const LogSink& sink : m_sinks)
        {
            sink(record);
        }
    }
}

void Internal::Logging::SetLogLevel(LogLevel level)
{
    m_logLevel.store(level);
}
// Logging Root                                              END
// -------------------------------------------------------------
// LogLevel                                                START
std::string_view ToString(LogLevel level)
{
    using namespace std::string_view_literals;

    static std::string_view s_logLevels[size_t(LogLevel::_Count)] = {
        "Ignored"sv,
        "Trace"sv,
        "Debug"sv,
        "Info"sv,
        "Warning"sv,
        "Error"sv,
        "Fatal"sv
    };

    return s_logLevels[size_t(level)];
}

std::string_view ToString(
    LogLevel level,
    char* buffer,
    size_t length)
{
    std::string_view str = ToString(level);
    buffer[str.copy(buffer, length)] = 0;

    return buffer;
}

std::ostream& operator<<(
    std::ostream& o,
    LogLevel level)
{
    return o << ToString(level);
}
// LogLevel                                                  END
// -------------------------------------------------------------
// LogFormatter                                            START
std::shared_ptr<LogFormatter> LogFormatter::Create(Params params)
{
    if (params.json)
    {
        return std::make_shared<Internal::JsonLogFormatter>(
            std::move(params));
    }
    else
    {
        return std::make_shared<Internal::StandardLogFormatter>(
            std::move(params));
    }
}

std::string LogFormatter::Format(const LogRecord& record) const
{
    std::string message;
    FormatTo(message, record);

    return message;
}
// LogFormatter                                              END
// -------------------------------------------------------------
// StandardLogFormatter                                    START
Internal::StandardLogFormatter::StandardLogFormatter(
    Params params)
    : m_params(std::move(params))
{ }

Internal::StandardLogFormatter::~StandardLogFormatter() = default;

void Internal::StandardLogFormatter::FormatTo(
    std::string& message,
    const LogRecord& record) const
{
    using namespace std::chrono;
    using namespace std::string_view_literals;

    message.reserve(64 + record.message.size());

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

        message.append("["sv);
        message.append(std::string_view(buffer, count));
        message.append("]"sv);
    }

    if (!record.logger.empty())
    {
        message.append("["sv);
        message.append(record.logger);
        message.append("]"sv);
    }

    message.append("["sv);
    message.append(ToString(record.level));
    message.append("]"sv);

    if (record.location && m_params.location)
    {
        message.append("["sv);
        message.append(record.location.shortFilename);
        message.append(":"sv);
        message.append(std::to_string(record.location.lineno));
        message.append("]"sv);
    }

    message.append(" "sv);
    message.append(record.message);
    message.append("\n"sv);
}
// StandardLogFormatter                                      END
// -------------------------------------------------------------
// JsonLogFormatter                                        START
Internal::JsonLogFormatter::JsonLogFormatter(Params params)
    : m_params(std::move(params))
{ }

Internal::JsonLogFormatter::~JsonLogFormatter() = default;

void Internal::JsonLogFormatter::FormatTo(
    std::string& message,
    const LogRecord& record) const
{
    using namespace std::chrono;
    using namespace rapidjson;

    Document doc;
    Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    if (record.location && m_params.location)
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
    message.reserve(length + 1);
    message.append(buf.GetString(), length);
    message += "\n";
}
// JsonLogFormatter                                          END
// -------------------------------------------------------------
// Logger                                                  START
Logger::Logger(std::string name, LogSink sink)
    : m_name(std::move(name))
    , m_sink(std::move(sink))
{
    FUSION_ASSERT(!m_name.empty());
}

Logger::Logger(Logger&& other) noexcept
    : m_name(std::move(other.m_name))
    , m_logLevel(other.m_logLevel.load())
    , m_propagate(other.m_propagate.load())
    , m_sink(std::move(other.m_sink))
    , m_sinks(std::move(other.m_sinks))
{ }

Logger& Logger::operator=(Logger&& other) noexcept
{
    if (m_name != other.m_name)
    {
        m_name = std::move(other.m_name);
        m_logLevel = other.m_logLevel.load();
        m_propagate = other.m_propagate.load();
        m_sink = std::move(other.m_sink);
        m_sinks = std::move(other.m_sinks);
    }
    return *this;
}

Logger::~Logger()
{
    m_sink = nullptr;
    m_sinks.clear();
}

void Logger::AddSink(LogSink sink)
{
    m_sinks.emplace_back(std::move(sink));
    m_propagate = false;
}

Logger Logger::Duplicate(std::string name) const
{
    Logger dup(std::move(name), m_sink);
    dup.m_logLevel = m_logLevel.load();
    dup.m_propagate = m_propagate.load();
    dup.m_sinks = m_sinks;
    return dup;
}

LogLevel Logger::GetLogLevel() const
{
    return m_logLevel;
}

std::string_view Logger::GetName() const
{
    return m_name;
}

void Logger::Log(
    LogLevel level,
    LogLocation location,
    std::string message) const
{
    Log(LogRecord{
        .logger = m_name,
        .threadId = Thread::CurrentThreadId(),
        .level = level,
        .location = location,
        .message = std::move(message),
    });
}

void Logger::Log(const LogRecord& record) const
{
    LogLevel level = m_logLevel.load();

    if (m_propagate)
    {
        if (record.level >= level)
        {
            m_sink(record);
        }
    }

    for (const auto& sink : m_sinks)
    {
        if (record.level >= level)
        {
            sink(record);
        }
    }
}

void Logger::SetLogLevel(LogLevel level)
{
    m_logLevel.store(level);
}

void Logger::SetPropagate(bool value)
{
    m_propagate.store(value);
}
// Logger                                                    END
// -------------------------------------------------------------
// Logging                                                 START
void Logging::AddSink(Logger::LogSink sink)
{
    auto root = Internal::Logging::Instance();
    FUSION_ASSERT(root, "Logging system has not been started");

    root->AddSink(std::move(sink));
}

Logger Logging::Get(std::string name)
{
    auto root = Internal::Logging::Instance();
    FUSION_ASSERT(root, "Logging system has not been started");

    Logger logger(
        std::move(name),
        [root{ std::move(root) }](const LogRecord& record) {
            root->Log(record);
        });

    logger.SetLogLevel(GetLogLevel());
    return logger;
}

Logger Logging::Get(
    std::string name,
    std::initializer_list<LogSink> sinks)
{
    auto root = Internal::Logging::Instance();
    FUSION_ASSERT(root, "Logging system has not been started");

    Logger logger(
        std::move(name),
        [root{ std::move(root) }](const LogRecord& record) {
            root->Log(record);
        });

    for (LogSink sink : sinks)
    {
        logger.AddSink(std::move(sink));
    }

    logger.SetLogLevel(GetLogLevel());
    return logger;
}

LogLevel Logging::GetLogLevel()
{
    auto root = Internal::Logging::Instance();
    FUSION_ASSERT(root, "Logging system has not been started");

    return root->GetLogLevel();
}

void Logging::SetLogLevel(LogLevel level)
{
    auto root = Internal::Logging::Instance();
    FUSION_ASSERT(root, "Logging system has not been started");

    root->SetLogLevel(level);
}

void Logging::Start()
{
    Internal::Logging::Start();
}

void Logging::Stop()
{
    Internal::Logging::Stop();
}
// Logging                                                   END
// -------------------------------------------------------------
// SyslogSink                                              START
SyslogSink::SyslogSink(Params params)
    : m_params(std::move(params))
{
#if FUSION_PLATFORM_POSIX
    static constexpr int32_t s_locals[8] = {
        LOG_LOCAL0, LOG_LOCAL1, LOG_LOCAL2, LOG_LOCAL3,
        LOG_LOCAL4, LOG_LOCAL5, LOG_LOCAL6, LOG_LOCAL7
    };

    int32_t facility = 0;
    switch (m_params.facility)
    {
    case Facility::User:
        facility = LOG_USER;
        break;
    case Facility::System:
        facility = LOG_DAEMON;
        break;
    case Facility::Local:
    {
        FUSION_ASSERT(m_params.local >= 0);
        FUSION_ASSERT(m_params.local <= 7);

        facility = s_locals[m_params.local];
        break;
    }
    default:
        FUSION_ASSERT(false, "Invalid facility to syslog");
    }

    int32_t options = 0;
    if (+(m_params.options & Option::Pid))
    {
        options |= LOG_PID;
    }
    if (+(m_params.options & Option::Console))
    {
        options |= LOG_CONS;
    }
    if (+(m_params.options & Option::NoDelay))
    {
        options |= LOG_NDELAY;
    }
    if (+(m_params.options & Option::Lazy))
    {
        options |= LOG_ODELAY;
    }
    if (+(m_params.options & Option::NoWait))
    {
        options |= LOG_NOWAIT;
    }

    FUSION_ASSERT(!m_params.identity.empty());
    ::openlog(m_params.identity.c_str(), options, facility);
#endif  // FUSION_PLATFORM_POSIX
}

SyslogSink::~SyslogSink()
{
#if FUSION_PLATFORM_POSIX
    ::closelog();
#endif  // FUSION_PLATFORM_POSIX
}

void SyslogSink::Log(const LogRecord& record) const
{
#if FUSION_PLATFORM_POSIX
    int32_t priority = 0;
    switch (record.level)
    {
    case LogLevel::Fatal:
        priority = LOG_CRIT;
        break;
    case LogLevel::Error:
        priority = LOG_ERR;
        break;
    case LogLevel::Warning:
        priority = LOG_WARNING;
        break;
    case LogLevel::Info:
        priority = LOG_INFO;
        break;
    case LogLevel::Debug:
        priority = LOG_DEBUG;
        break;
    default:
        priority = LOG_DEBUG;
        break;
    }

    std::string message;

    if (record.location)
    {
        message = fmt::format("[{}][{}][{}:{}] {}",
            record.level,
            record.threadId,
            record.location.shortFilename,
            record.location.lineno,
            record.message);
    }
    else
    {
        message = fmt::format("[{}][{}] {}",
            record.level,
            record.threadId,
            record.message);
    }

    ::syslog(
        priority,
        "%.*s",
        std::min<int32_t>(message.size(), INT32_MAX),
        message.c_str());
#endif  // FUSION_PLATFORM_POSIX

    FUSION_UNUSED(record);
}
// SyslogSink                                                END
// -------------------------------------------------------------
}  // namespace Fusion
