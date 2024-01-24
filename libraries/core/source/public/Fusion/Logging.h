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

#pragma once

#include <Fusion/Fwd/Logging.h>

#include <Fusion/Compiler.h>
#include <Fusion/Enum.h>
#include <Fusion/Macros.h>
#include <Fusion/Platform.h>
#include <Fusion/Thread.h>

#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include <fmt/format.h>

namespace Fusion
{
//
//
//
enum class LogLevel : uint8_t
{
    Ignored = 0,
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,

    _Count
};

//
//
//
std::string_view ToString(LogLevel level);

//
//
//
std::string_view ToString(LogLevel level, char* buffer, size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(LogLevel level, char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(std::ostream& o, LogLevel level);

//
//
//
struct LogLocation
{
    std::string_view filename;
    std::string_view shortFilename;
    std::string_view function;
    uint32_t lineno{ 0 };

    constexpr LogLocation() = default;
    constexpr LogLocation(
        const char* filename,
        const char* function,
        uint32_t lineno)
        : filename(filename)
        , shortFilename(Basename(filename))
        , function(function)
        , lineno(lineno)
    {}

    constexpr bool Empty() const
    {
        return filename.empty();
    }

    constexpr operator bool() const
    {
        return !Empty();
    }

    static constexpr std::string_view Basename(const char* path)
    {
        const char* filename = path;

        while (*path)
        {
#if FUSION_PLATFORM_WINDOWS
            if (*path == '\\' || *path == '/')
#else
            if (*path == '/')
#endif
            {
                filename = path + 1;
            }
            ++path;
        }

        return std::string_view(filename, size_t(path - filename));
    }
};

//
//
//
struct LogRecord
{
    std::chrono::system_clock::time_point time{
        std::chrono::system_clock::now() };

    std::string_view logger;
    Thread::Id threadId{ Thread::INVALID_THREAD_ID };
    LogLevel level{ LogLevel::Ignored };
    LogLocation location;
    std::string message;
};

//
//
//
class LogFormatter
    : public std::enable_shared_from_this<LogFormatter>
{
public:
    LogFormatter(const LogFormatter&) = delete;
    LogFormatter& operator=(const LogFormatter&) = delete;

public:
    //
    //
    //
    struct Params
    {
        //
        //
        //
        bool json{ false };

        //
        //
        //
        bool colors{ false };

        //
        //
        //
        bool location{ true };

        //
        //
        //
        bool timestamps{ true };
    };

public:
    //
    //
    //
    static std::shared_ptr<LogFormatter> Create(Params params);

public:
    LogFormatter() = default;
    virtual ~LogFormatter() = default;

public:
    //
    //
    //
    virtual void FormatTo(
        std::string& buffer,
        const LogRecord& record) const = 0;

    //
    //
    //
    std::string Format(const LogRecord& record) const;
};

//
//
//
class Logger final
{
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

public:
    friend class Logging;

public:
    //
    //
    //
    using LogSink = std::function<void(const LogRecord&)>;

public:
    Logger(Logger&& other) noexcept;
    Logger& operator=(Logger&& other) noexcept;

public:
    ~Logger();

public:
    //
    //
    //
    template<typename... Args>
    void Log(
        LogLevel level,
        LogLocation location,
        fmt::format_string<Args...> format,
        Args&& ...args) const noexcept;

public:
    //
    //
    //
    void AddSink(LogSink sink);

    //
    //
    //
    Logger Duplicate(std::string name) const;

    //
    //
    //
    LogLevel GetLogLevel() const;

    //
    //
    //
    std::string_view GetName() const;

    //
    //
    //
    void Log(
        LogLevel level,
        LogLocation location,
        std::string message) const;

    //
    //
    //
    void Log(const LogRecord& record) const;

    //
    //
    //
    void SetLogLevel(LogLevel level);

    //
    //
    //
    void SetPropagate(bool value);

private:
    //
    //
    //
    Logger(
        std::string name,
        LogSink sink);

private:
    std::string m_name;
    std::atomic<LogLevel> m_logLevel{ LogLevel::Error };
    std::atomic<bool> m_propagate{ true };
    LogSink m_sink;

private:
    std::vector<LogSink> m_sinks;
};

//
//
//
class SyslogSink final
{
public:
    SyslogSink(const SyslogSink&) = delete;
    SyslogSink& operator=(const SyslogSink&) = delete;

public:
    //
    //
    //
    enum class Facility : uint8_t
    {
        User = 0,
        System,
        Local
    };

    //
    //
    //
    enum class Option : uint8_t
    {
        None = 0,
        Pid = 1 << 1,      // Include PID in message
        Console = 1 << 2,  // Errors logged to console
        NoDelay = 1 << 3,  // Connect when sending the first message
        Lazy = 1 << 4,     // Connect when the first message is sent.
        NoWait = 1 << 5,   // Do not wait for child processes

        Default = (Option::Pid | Option::Lazy | NoWait)
    };

public:
    struct Params
    {
        //
        //
        //
        std::string identity;

        //
        //
        //
        Option options{ Option::Default };

        //
        //
        //
        Facility facility{ Facility::User };

        //
        //
        //
        int32_t local{ 0 };

        constexpr Params(std::string_view identity)
            : identity(identity)
        { }
    };

public:
    //
    //
    //
    SyslogSink(Params params);

    //
    //
    //
    SyslogSink(
        std::shared_ptr<LogFormatter> formatter,
        Params params);

    ~SyslogSink();

    //
    //
    //
    void Log(const LogRecord& record) const;

private:
    Params m_params;
    std::shared_ptr<LogFormatter> m_formatter;
};

FUSION_ENUM_OPS(SyslogSink::Option);

//
//
//
class QueuedLogSink
{
public:
    QueuedLogSink(const QueuedLogSink&) = delete;
    QueuedLogSink& operator=(const QueuedLogSink&) = delete;

public:
    //
    //
    //
    QueuedLogSink() = default;

    //
    //
    //
    QueuedLogSink(uint64_t count);

    //
    //
    //
    QueuedLogSink(
        std::shared_ptr<LogFormatter> formatter,
        uint64_t count = UINT64_MAX);

    virtual ~QueuedLogSink();

    //
    //
    //
    size_t Capacity() const FUSION_EXCLUDES(m_mutex);

    //
    //
    //
    bool Empty() const FUSION_EXCLUDES(m_mutex);

    //
    //
    //
    void Flush(std::function<void(std::string_view)> fn) FUSION_EXCLUDES(m_mutex);;

    //
    //
    //
    void Log(const LogRecord& record);

    //
    //
    //
    size_t Size() const FUSION_EXCLUDES(m_mutex);

protected:
    //
    //
    //
    void Log(std::string message) FUSION_EXCLUDES(m_mutex);

protected:
    mutable std::mutex m_mutex;

private:
    // TODO: eventually replace this with a lock-free queue
    std::deque<std::string> m_messages FUSION_GUARDED_BY(m_mutex);

private:
    size_t m_count{ UINT64_MAX };
    std::shared_ptr<LogFormatter> m_formatter;
};

//
//
//
class Logging final
{
public:
    using LogSink = Logger::LogSink;

public:
    //
    //
    //
    static void AddSink(LogSink sink);

    //
    //
    //
    static Logger Get(std::string name);

    //
    //
    //
    static Logger Get(
        std::string name,
        std::initializer_list<LogSink> sinks);

    //
    //
    //
    static LogLevel GetLogLevel();

    //
    //
    //
    static void SetLogLevel(LogLevel level);

public:
    //
    //
    //
    static void Start();

    //
    //
    //
    static void Stop();
};
}  // namespace Fusion

#ifdef FUSION_DISABLE_LOGGING
#define FUSION_LOG(...) (void)
#else
#define FUSION_LOG(logger,...) logger.Log(__VA_ARGS__)
#endif  // FUSION_DISABLE_LOGGING

#ifndef _NDEBUG
// For the debug builds we specifically include the function, filename,
// and line number that the log line occurs on.
#define FUSION_LOG_EXTRA(logger,level,...) \
    FUSION_LOG(logger, \
        level, \
        Fusion::LogLocation{ __FILE__, FUSION_CURRENT_FUNCTION, __LINE__ }, \
        __VA_ARGS__)
#else
// For non-Debug builds don't include the extra context to the
// logger because it may contain sensitive information.
#define FUSION_LOG_EXTRA(logger,level,...) \
    FUSION_LOG(logger, \
        level, \
        Fusion::LogLocation{}, \
        __VA_ARGS__)
#endif

#define FUSION_LOG_DEBUG(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Debug, \
        __VA_ARGS__)

#define FUSION_LOG_ERROR(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Error, \
        __VA_ARGS__)

#define FUSION_LOG_FATAL(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Fatal, \
        __VA_ARGS__)

#define FUSION_LOG_INFO(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Info, \
        __VA_ARGS__)

#define FUSION_LOG_WARNING(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Warning, \
        __VA_ARGS__)

#define FUSION_IMPL_LOGGING 1
#include <Fusion/Impl/Logging.h>
