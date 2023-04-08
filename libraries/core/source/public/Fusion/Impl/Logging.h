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

#if !defined(FUSION_IMPL_LOGGING)
#error "Logging impl included before main header"
#endif

#include <Fusion/ThreadUtil.h>

namespace Fusion
{
template<size_t LENGTH>
std::string_view ToString(
    LogLevel level,
    char(&buffer)[LENGTH])
{
    return ToString(level, buffer, LENGTH);
}

template<typename Mutex>
void LogHandlerImpl<Mutex>::Flush()
{
    std::lock_guard lock(m_mutex);
    DoFlush();
}

template<typename Mutex>
void LogHandlerImpl<Mutex>::Log(const LogRecord& record)
{
    std::lock_guard lock(m_mutex);
    DoLog(record);
}

template<typename ...Args>
void Fusion::Logger::Log(
    LogLevel level,
    LogLocation loc,
    fmt::format_string<Args...> message,
    Args&& ...args) const noexcept
{
    using namespace std::string_view_literals;

    if (level == LogLevel::Ignored) return;
    if (!IsLevelHandled(level)) return;

    LogRecord record{
        .level = level,
        .location = loc,
        .threadId = ThreadUtil::GetCurrentThreadId(),
        .time = Clock::now(),
    };

    try
    {
        record.message = fmt::format(
            message,
            std::forward<Args>(args)...);
    }
    catch (const std::exception& e)
    {
        if (loc.filename)
        {
            // We know where the error occurred.
            ErrorHandler(
                fmt::format(FMT_STRING("{} [{}:{}]"),
                    e.what(),
                    loc.filename,
                    loc.lineno));
        }
        else
        {
            ErrorHandler(e.what());
        }
        return;
    }
    catch (...)
    {
        ErrorHandler("Unknown exception in logger");
        return;
    }

    Log(std::move(record));
}
}  // namespace Fusion

template<>
struct fmt::formatter<Fusion::LogLevel>
    : fmt::formatter<fmt::string_view>
{
    template<typename Context>
    auto format(
        const Fusion::LogLevel& level,
        Context& ctx)
    {
        return formatter<fmt::string_view>::format(
            Fusion::ToString(level),
            ctx);
    }
};
