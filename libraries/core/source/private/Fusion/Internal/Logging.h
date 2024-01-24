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

#include <Fusion/Logging.h>

#include <Fusion/Macros.h>

#include <memory>
#include <mutex>

namespace Fusion::Internal
{
//
//
// 
class Logging final
    : public std::enable_shared_from_this<Logging>
{
public:
    Logging(const Logging&) = delete;
    Logging& operator=(const Logging&) = delete;

public:
    using LogSink = Logger::LogSink;

public:
    //
    //
    //
    static std::shared_ptr<Logging> Instance();

    //
    //
    //
    static void Start();

    //
    //
    //
    static void Stop();

public:
    Logging() = default;
    ~Logging();

    //
    //
    //
    void AddSink(LogSink sink);

    //
    //
    //
    LogLevel GetLogLevel() const;

    //
    //
    //
    void Log(const LogRecord& record) const;

    //
    //
    //
    void SetLogLevel(LogLevel level);

private:
    std::atomic<LogLevel> m_logLevel{ LogLevel::Error };
    std::vector<LogSink> m_sinks;

private:
    static std::atomic<bool> s_started;
    static std::shared_ptr<Logging> s_instance FUSION_GUARDED_BY(s_mutex);
    static std::mutex s_mutex;
};

//
//
//
class StandardLogFormatter : public LogFormatter
{
public:
    //
    //
    //
    StandardLogFormatter(Params params);

    ~StandardLogFormatter() override;

public:
    //
    //
    //
    void FormatTo(
        std::string& buffer,
        const LogRecord& record) const override;

private:
    Params m_params;
};

//
//
//
class JsonLogFormatter : public LogFormatter
{
public:
    //
    //
    //
    JsonLogFormatter(Params params);

    ~JsonLogFormatter() override;

public:
    //
    //
    //
    void FormatTo(
        std::string& buffer,
        const LogRecord& record) const override;

private:
    Params m_params;
};
}  // namespace Fusion::Internal
