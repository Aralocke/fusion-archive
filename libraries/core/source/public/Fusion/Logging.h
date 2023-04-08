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

#include <Fusion/Fwd/Logging.h>
#include <Fusion/Compiler.h>
#include <Fusion/DateTime.h>

#include <atomic>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include <fmt/format.h>

namespace Fusion::Internal { class LogRegistry; }

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
    Fatal = 0xFF,
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
    const char* filename{ nullptr };
    const char* func{ nullptr };
    uint32_t lineno{ 0 };

    constexpr LogLocation() = default;

    constexpr LogLocation(
        const char* filename,
        const char* func,
        uint32_t lineno)
        : filename(filename)
        , func(func)
        , lineno(lineno)
    { }
};

//
//
//
struct LogRecord
{
    LogLevel level{ LogLevel::Ignored };
    LogLocation location;
    std::string message;
    size_t threadId{ size_t(-1) };
    Clock::time_point time{ Clock::now() };
};

//
//
//
class LogHandler
    : public std::enable_shared_from_this<LogHandler>
{
public:
    struct NoopMutex
    {
        void lock() { }
        void unlock() { }
    };

public:
    virtual ~LogHandler() = default;

    virtual void Flush() = 0;
    virtual void Log(const LogRecord& record) = 0;
    virtual bool IsLevelHandled(LogLevel level) const = 0;

protected:
    LogHandler() = default;
};

//
//
//
template<typename Mutex>
class LogHandlerImpl : public LogHandler
{
public:
    //
    //
    //
    ~LogHandlerImpl() = default;

    //
    //
    //
    void Flush() final override;

    //
    //
    //
    void Log(const LogRecord& record) final override;

protected:
    //
    //
    //
    LogHandlerImpl() = default;

    //
    //
    //
    virtual void DoFlush() = 0;

    //
    //
    //
    virtual void DoLog(const LogRecord& record) = 0;

private:
    Mutex m_mutex;
};

namespace Internal
{
//
//
//
struct NoopMutex
{
    void lock() { }
    void unlock() { }
};
}

//
//
//
using MultiThreadedLogHandler = LogHandlerImpl<std::mutex>;

//
//
//
using NonThreadedLogHandler = LogHandlerImpl<Internal::NoopMutex>;

//
//
//
class Logger final
    : public std::enable_shared_from_this<Logger>
{
public:
    //
    //
    //
    Logger(std::string_view name);

    //
    //
    //
    Logger(Logger* parent, std::string_view name);

    //
    //
    //
    ~Logger();

    //
    //
    //
    void AddHandler(std::shared_ptr<LogHandler> handler);

    //
    //
    //
    LogLevel GetFlushLevel() const;

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
        std::string_view message) const;

    //
    //
    //
    template<typename ...Args>
    void Log(
        LogLevel level,
        LogLocation location,
        fmt::format_string<Args...> message,
        Args&& ...args) const noexcept;

    //
    //
    //
    void SetFlushLevel(LogLevel level);

    //
    //
    //
    void SetLogLevel(LogLevel level);

    //
    //
    //
    void SetPropagate(bool flag);

private:
    //
    //
    //
    void ErrorHandler(std::string message) const;

    //
    //
    //
    bool IsLevelHandled(LogLevel level) const;

    //
    //
    //
    void Log(LogRecord record) const;

private:
    std::string m_name;
    std::atomic<LogLevel> m_logLevel{ LogLevel::Error };
    std::atomic<LogLevel> m_flushLevel{ LogLevel::Error };
    std::atomic<bool> m_propagate{ true };

private:
    std::shared_ptr<Logger> m_parent;
    std::vector<std::shared_ptr<LogHandler>> m_handlers;
};

class Logging final
{
public:
    //
    //
    //
    static void Drop(std::string_view name);

    //
    //
    //
    static void DropAll();

    //
    //
    //
    static std::shared_ptr<Logger> Get();

    //
    //
    //
    static std::shared_ptr<Logger> Get(std::string name);
};

}  // namespace Fusion

#ifdef FUSION_DISABLE_LOGGING
#define FUSION_LOG(...) (void)
#else
#define FUSION_LOG(logger,...) logger->Log(__VA_ARGS__)
#endif  // FUSION_DISABLE_LOGGING

#ifndef _NDEBUG
// For the debug builds we specifically include the function, filename,
// and line number that the log line occurs on.
#define FUSION_LOG_EXTRA(logger,level,...) \
    FUSION_LOG(logger, level, \
        Fusion::LogLocation{__FILE__, FUSION_CURRENT_FUNCTION, __LINE__}, \
        __VA_ARGS__)
#else
// For non-Debug builds don't include the extra context to the
// logger because it may contain sensitive information.
#define FUSION_LOG_EXTRA(logger,level,...) \
    FUSION_LOG(logger, \
        level, \
        fusion::Location("", "", 0), \
        __VA_ARGS__)
#endif

#define FUSION_LOG_DEBUG(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Debug, \
        __VA_ARGS__)

#define FUSION_LOG_CRITICAL(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Fatal, \
        __VA_ARGS__)

#define FUSION_LOG_ERROR(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Error, \
        __VA_ARGS__)

#define FUSION_LOG_INFO(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Info, \
        __VA_ARGS__)

#define FUSION_LOG_TRACE(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Trace, \
        __VA_ARGS__)

#define FUSION_LOG_WARNING(logger,...) \
    FUSION_LOG_EXTRA(logger, \
        Fusion::LogLevel::Warning, \
        __VA_ARGS__)

#define FUSION_IMPL_LOGGING 1
#include <Fusion/Impl/Logging.h>
