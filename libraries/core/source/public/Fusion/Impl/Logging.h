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

#if !defined(FUSION_IMPL_LOGGING)
#error "Logging impl included before main header"
#endif

namespace Fusion
{
template<size_t LENGTH>
std::string_view ToString(LogLevel level, char(&buffer)[LENGTH])
{
    return ToString(level, buffer, LENGTH);
}

template<typename... Args>
void Logger::Log(
    LogLevel level,
    LogLocation location,
    fmt::format_string<Args...> format,
    Args&& ...args) const noexcept
{
    LogRecord record{
        .logger = m_name,
        .threadId = Thread::CurrentThreadId(),
        .level = level,
        .location = location,
    };

    try
    {
        std::string message = fmt::format(
            format,
            std::forward<Args>(args)...);

        record.message = std::move(message);
        Log(std::move(record));
    }
    catch (const std::exception& ex)
    {
        std::string error = fmt::format(
            "Failed to format log message: '{}'",
            format.get());

        record.message = std::move(error);
        Log(std::move(record));
    }
}
}  // namespace Fusion

template <>
struct fmt::formatter<Fusion::LogLevel> : fmt::formatter<std::string_view>
{
    auto format(Fusion::LogLevel level, format_context& ctx) const
    {
        return formatter<std::string_view>::format(
            Fusion::ToString(level),
            ctx);
    }
};
