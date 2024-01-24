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

#include <Fusion/Macros.h>
#include <Fusion/Types.h>

#include <iosfwd>
#include <memory>
#include <mutex>
#include <string>

#include <fmt/format.h>

namespace Fusion
{
//
//
//
class Console final
{
public:
    //
    //
    //
    enum class Color : uint8_t
    {
        None = 0,

        // Formatting Codes
        Reset,
        Bold,
        Dark,
        Underline,
        Blink,
        Reverse,
        Concealed,
        ClearLine,

        // Foreground Colors
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,

        // Background Colors
        BackgroundBlack,
        BackgroundRed,
        BackgroundGreen,
        BackgroundYellow,
        BackgroundBlue,
        BackgroundMagenta,
        BackgroundCyan,
        BackgroundWhite,

        // Bold Colors
        BoldYellow,
        BoldRed,
        BoldOnRed,

        _Count
    };

public:
    //
    //
    //
    enum class Target : uint8_t
    {
        None = 0,
        Stdin,
        Stdout,
        Stderr,
    };

public:
    //
    //
    //
    struct Features
    {
        //
        //
        //
        bool colors{ false };

        //
        //
        //
        bool bold{ false };

        //
        //
        //
        bool dark{ false };

        //
        //
        //
        bool echo{ false };

        //
        //
        //
        bool interactive{ false };

        //
        //
        //
        int32_t width{ INT32_MAX };
    };

    //
    //
    //
    static Features GetFeatures(Target target);

public:
    //
    //
    //
    static bool IsInteractive(Target target);

    //
    //
    //
    static void SetConsoleEcho(bool value);

    //
    //
    //
    static void Write(
        Target target,
        std::string_view str);

    //
    //
    //
    template<typename ...Args>
    static void Write(
        Target target,
        fmt::format_string<Args...> format,
        Args&& ...args) noexcept;
};

//
//
//
std::string_view ToString(Console::Color color);

//
//
//
std::ostream& operator<<(std::ostream& o, Console::Color color);

//
//
//
class ConsoleLogger final
{
public:
    ConsoleLogger(const ConsoleLogger&) = delete;
    ConsoleLogger& operator=(const ConsoleLogger&) = delete;

public:
    struct Params
    {
        //
        //
        //
        Console::Target target{ Console::Target::Stdout };

        //
        //
        //
        size_t bufferSize{ 4 * 1024 };

        //
        //
        //
        bool colors{ false };

        //
        //
        //
        bool dark{ false };

        //
        //
        //
        bool timestamps{ false };

        //
        //
        //
        bool json{ false };
    };

public:
    ConsoleLogger(Params params);
    ConsoleLogger(
        std::shared_ptr<LogFormatter> formatter,
        Params params);
    ~ConsoleLogger();

    //
    //
    //
    void Log(const LogRecord& record);

private:
    //
    //
    //
    void LogJson(const LogRecord& record) FUSION_REQUIRES_LOCK(m_mutex);

    //
    //
    //
    void LogText(const LogRecord& record) FUSION_REQUIRES_LOCK(m_mutex);

    //
    //
    //
    void FlushBuffer(LogLevel level, std::string& buffer) FUSION_REQUIRES_LOCK(m_mutex);

private:
    Params m_params;
    std::string m_buffer FUSION_GUARDED_BY(m_mutex);
    std::shared_ptr<LogFormatter> m_formatter FUSION_GUARDED_BY(m_mutex);
    mutable std::mutex m_mutex;
};
}  // namespace Fusion

#define FUSION_IMPL_CONSOLE 1
#include <Fusion/Impl/Console.h>
