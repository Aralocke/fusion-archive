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

#include <Fusion/Internal/Logging.h>
#include <Fusion/Assert.h>
#include <Fusion/StringUtil.h>

#include <iostream>

constexpr static std::string_view ROOT_LOGGER{ "fusion" };

namespace Fusion
{
// -------------------------------------------------------------
// LogHandler                                              START
// LogHandler                                                END
// -------------------------------------------------------------
// Logger                                                  START
Logger::Logger(std::string_view name)
    : m_name(name)
    , m_propagate(false)
{ }

Logger::Logger(
    Logger* parent,
    std::string_view name)
    : m_name(name)
    , m_logLevel(parent->GetLogLevel())
    , m_flushLevel(parent->GetFlushLevel())
    , m_parent(parent->shared_from_this())
{ }

Logger::~Logger()
{ }

void Logger::AddHandler(std::shared_ptr<LogHandler> handler)
{
    m_handlers.push_back(std::move(handler));
}

void Logger::ErrorHandler(std::string message) const
{
    FUSION_UNUSED(message);
}

LogLevel Logger::GetFlushLevel() const
{
    return m_flushLevel;
}

LogLevel Logger::GetLogLevel() const
{
    return m_logLevel;
}

std::string_view Logger::GetName() const
{
    return m_name;
}

bool Logger::IsLevelHandled(LogLevel level) const
{
    return level >= m_logLevel.load(std::memory_order_relaxed);
}

void Logger::Log(
    LogLevel level,
    LogLocation location,
    std::string_view view) const
{
    if (level == LogLevel::Ignored) return;
    if (view.empty()) return;
    if (!IsLevelHandled(level)) return;

    std::string message(view);

    Log(LogRecord{
        .level = level,
        .location = location,
        .message = std::move(message),
        .threadId = ThreadUtil::GetCurrentThreadId(),
        .time = Clock::now(),
    });
}

void Logger::Log(LogRecord record) const
{
    FUSION_ASSERT(!record.message.empty());
    FUSION_ASSERT(IsLevelHandled(record.level));

    LogLevel flushLevel = m_flushLevel.load(std::memory_order_relaxed);

    for (auto& handler : m_handlers)
    {
        if (handler->IsLevelHandled(record.level))
        {
            handler->Log(record);
        }
        if (record.level >= flushLevel)
        {
            handler->Flush();
        }
    }
    if (m_propagate)
    {
        FUSION_ASSERT(m_parent);
        m_parent->Log(record);
    }
}

void Logger::SetFlushLevel(LogLevel level)
{
    m_flushLevel = level;
}

void Logger::SetLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::SetPropagate(bool flag)
{
    m_propagate = flag;
}

// Logger                                                    END
// -------------------------------------------------------------
// Logging                                                 START
void Logging::Drop(std::string_view name)
{
    Internal::LogRegistry::Instance()
        .Drop(name);
}

void Logging::DropAll()
{
    Internal::LogRegistry::Instance()
        .DropAll();
}

std::shared_ptr<Logger> Logging::Get()
{
    return Internal::LogRegistry::Instance()
        .Get("");
}

std::shared_ptr<Logger> Logging::Get(std::string name)
{
    return Internal::LogRegistry::Instance()
        .Get(std::move(name));
}
// Logging                                                   END
// -------------------------------------------------------------
// LogLevel                                                START
std::string_view ToString(LogLevel level)
{
    using namespace std::string_view_literals;

    switch (level)
    {
    case LogLevel::Ignored:
        return "Ignored"sv;
    case LogLevel::Trace:
        return "Trace"sv;
    case LogLevel::Debug:
        return "Debug"sv;
    case LogLevel::Info:
        return "Info"sv;
    case LogLevel::Warning:
        return "Warning"sv;
    case LogLevel::Error:
        return "Error"sv;
    case LogLevel::Fatal:
        return "Fatal"sv;
    }

    return "Ignored"sv;
}

std::string_view ToString(
    LogLevel level,
    char* buffer,
    size_t length)
{
    auto value = ToString(level);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(
    std::ostream& o,
    LogLevel level)
{
    return o << ToString(level);
}
// LogLevel                                                  END
// -------------------------------------------------------------
// LogRegistry                                             START
Internal::LogRegistry& Internal::LogRegistry::Instance()
{
    static LogRegistry s_instance{};
    return s_instance;
}

Internal::LogRegistry::LogRegistry()
{
}

Internal::LogRegistry::~LogRegistry()
{
    DropAll();
}

std::shared_ptr<Logger> Internal::LogRegistry::Get(std::string name)
{
    using namespace std::string_view_literals;

    std::unique_lock lock(m_mutex);

    if (!m_root)
    {
        auto root = std::make_shared<Logger>(ROOT_LOGGER);
        {
            root->SetLogLevel(LogLevel::Warning);
            root->SetFlushLevel(LogLevel::Warning);
            root->SetPropagate(false);
        }

        auto [it, inserted] = m_loggers.emplace(
            ROOT_LOGGER,
            std::move(root));

        FUSION_ASSERT(inserted);
        m_root = it->second;
    }

    if (name.empty())
    {
        return m_root;
    }

    // We append the root logger name as a prefix to force the map to be structured
    // on a specific root key.

    name = fmt::format(FMT_STRING("{}.{}"), ROOT_LOGGER, name);

    if (auto iter = m_loggers.find(name); iter != m_loggers.end())
    {
        return iter->second;
    }

    // A little TL;DR for what we're doing here. The Loggers automatically propagate
    // log messages that they cannot handle back to their parent logger. This means
    // we need a parent when we create them. We also want the user to be able to quiet
    // an entire tree of loggers if they so choose.
    //
    // Side bonus, any settings or added features down the line can automatically
    // transfer to children loggers as we add them.

    std::vector<std::string_view> relatives = StringUtil::SplitViews(name, "."sv);
    std::vector<std::string_view> segments;
    Logger* next{ nullptr };

    for (std::string_view logger : relatives)
    {
        segments.push_back(logger);
        std::string key = StringUtil::Join(segments, "."sv);

        if (auto iter = m_loggers.find(key); iter != m_loggers.end())
        {
            next = iter->second.get();
        }
        else
        {
            std::string_view loggerName{ key };
            loggerName = loggerName.substr(ROOT_LOGGER.size() + 1);
            auto logger = std::make_shared<Logger>(next, loggerName);
            FUSION_ASSERT(next);

            auto [it, inserted] = m_loggers.try_emplace(
                std::move(key),
                std::move(logger));

            key.clear();
            FUSION_ASSERT(inserted);
            next = it->second.get();
        }
    }

    return next->shared_from_this();
}

void Internal::LogRegistry::Drop(std::string_view name)
{
    if (name.empty())
    {
        return;
    }

    FUSION_ASSERT(name != ROOT_LOGGER);

    std::unique_lock lock(m_mutex);

    // TODO: This should drop every logger that descends from
    //       the given logger if it is a full match.

    std::string n(name);
    m_loggers.erase(n);
}

void Internal::LogRegistry::DropAll()
{
    Drop(ROOT_LOGGER);
}
// LogRegistry                                               END
// -------------------------------------------------------------
}  // namespace Fusion
