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

#include <Fusion/Tests/Tests.h>

#include <Fusion/Console.h>
#include <Fusion/Logging.h>
#include <Fusion/Memory.h>
#include <Fusion/Platform.h>

#include <array>
#include <latch>

TEST(LoggingTests, LogMessageBasicSink)
{
    Logging::Start();
    FUSION_SCOPE_GUARD([] { Logging::Stop(); });

    bool sunk = false;
    std::latch barrier(1);

    Logging::AddSink([&](const LogRecord& record) {
        ASSERT_EQ(record.message, "test 123");
        sunk = true;
        barrier.count_down();
    });

    Logger testLogger = Logging::Get("Test");
    FUSION_LOG_ERROR(testLogger, "test {}", 123);

    barrier.wait();
    ASSERT_TRUE(sunk);
}

TEST(LoggingTests, WithConsoleLoggerSingleMessage)
{
    Logging::Start();
    FUSION_SCOPE_GUARD([] { Logging::Stop(); });

    ConsoleLogger console(ConsoleLogger::Params{
        .colors = true,
        .dark = true,
        .timestamps = true,
    });

    bool sunk = false;
    std::latch barrier(1);

    Logging::AddSink([&](const LogRecord& record) {
        ASSERT_EQ(record.message, "test 123");
        console.Log(record);

        sunk = true;
        barrier.count_down();
    });

    Logger testLogger = Logging::Get("Test");
    FUSION_LOG_ERROR(testLogger, "test {}", 123);

    barrier.wait();
    ASSERT_TRUE(sunk);
}

TEST(LoggingTests, WithConsoleLoggerMultiMessage)
{
    Logging::Start();
    FUSION_SCOPE_GUARD([] { Logging::Stop(); });

    Logging::SetLogLevel(LogLevel::Debug);

    ConsoleLogger console(ConsoleLogger::Params{
        .colors = true,
        .dark = true,
        .timestamps = true,
    });

    int32_t hits = 0;
    std::latch barrier(5);

    Logging::AddSink([&](const LogRecord& record) {
        ASSERT_EQ(record.message, "test 123");
        console.Log(record);

        ++hits;
        barrier.count_down();
    });

    Logger testLogger = Logging::Get("Test");

    FUSION_LOG_FATAL(testLogger, "test {}", 123);
    FUSION_LOG_ERROR(testLogger, "test {}", 123);
    FUSION_LOG_WARNING(testLogger, "test {}", 123);
    FUSION_LOG_INFO(testLogger, "test {}", 123);
    FUSION_LOG_DEBUG(testLogger, "test {}", 123);

    barrier.wait();
    ASSERT_TRUE(hits == 5);
}

TEST(LoggingTests, WithJsonLoggerMultiMessage)
{
    Logging::Start();
    FUSION_SCOPE_GUARD([] { Logging::Stop(); });

    Logging::SetLogLevel(LogLevel::Debug);

    ConsoleLogger console(ConsoleLogger::Params{
        .colors = true,
        .dark = true,
        .timestamps = true,
        .json = true,
    });

    int32_t hits = 0;
    std::latch barrier(5);

    Logging::AddSink([&](const LogRecord& record) {
        ASSERT_EQ(record.message, "test 123");
        console.Log(record);

        ++hits;
        barrier.count_down();
    });

    Logger testLogger = Logging::Get("Test");

    FUSION_LOG_FATAL(testLogger, "test {}", 123);
    FUSION_LOG_ERROR(testLogger, "test {}", 123);
    FUSION_LOG_WARNING(testLogger, "test {}", 123);
    FUSION_LOG_INFO(testLogger, "test {}", 123);
    FUSION_LOG_DEBUG(testLogger, "test {}", 123);

    barrier.wait();
    ASSERT_TRUE(hits == 5);
}

TEST(LoggingTests, SyslogTest)
{
#if !FUSION_PLATFORM_POSIX
    GTEST_SKIP() << "Skipping syslog tests on non-posix systems";
#endif

    Logging::Start();
    FUSION_SCOPE_GUARD([] { Logging::Stop(); });

    Logging::SetLogLevel(LogLevel::Debug);

    SyslogSink syslog(SyslogSink::Params{ "TestLogging" });

    bool sunk = false;
    std::latch barrier(1);

    Logging::AddSink([&](const LogRecord& record) {
        ASSERT_EQ(record.message, "test 123");
        syslog.Log(record);

        sunk = true;
        barrier.count_down();
    });

    Logger testLogger = Logging::Get("Test");
    FUSION_LOG_ERROR(testLogger, "test {}", 123);

    barrier.wait();
    ASSERT_TRUE(sunk);
}

TEST(LoggingTests, QueuedLogTest)
{
    Logging::Start();
    FUSION_SCOPE_GUARD([] { Logging::Stop(); });

    QueuedLogSink queue(2);
    ASSERT_EQ(queue.Capacity(), 2);
    ASSERT_TRUE(queue.Empty());

    uint32_t count = 0;
    std::latch barrier(3);

    Logging::AddSink([&](const LogRecord& record) {
        ASSERT_FALSE(record.message.empty());
        queue.Log(record);

        ++count;
        barrier.count_down();
    });

    Logger logger = Logging::Get("Test");

    FUSION_LOG_ERROR(logger, "test {}", 1);
    {
        ASSERT_EQ(queue.Size(), 1);
        ASSERT_FALSE(queue.Empty());
    }

    FUSION_LOG_ERROR(logger, "test {}", 2);
    {
        ASSERT_EQ(queue.Size(), 2);
        ASSERT_FALSE(queue.Empty());
    }

    FUSION_LOG_ERROR(logger, "test {}", 3);
    {
        ASSERT_EQ(queue.Size(), 2);
        ASSERT_FALSE(queue.Empty());
    }

    barrier.wait();
    ASSERT_EQ(count, 3);

    std::vector<std::string> messages;
    queue.Flush([&](std::string_view msg) {
        messages.emplace_back(msg);
    });

    constexpr const auto results = std::to_array<std::string_view>({
        "test 2",
        "test 3",
    });

    ASSERT_EQ(messages.size(), results.size());
    for (size_t i = 0; i < messages.size(); ++i)
    {
        ASSERT_EQ(messages[i], results[i]);
    }
}
