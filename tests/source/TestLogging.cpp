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

#include <Fusion/Tests/Tests.h>

#include <Fusion/Internal/Logging.h>

#include <vector>

namespace
{
class LoggingTestsHandler final
    : public NonThreadedLogHandler
{
protected:
    void DoFlush() override
    {
        ASSERT_FALSE(records.empty());

        count += records.size();
        records.clear();
    }

    void DoLog(const LogRecord& record) override
    {
        ASSERT_TRUE(IsLevelHandled(record.level));
        records.emplace_back(record);
    }

public:
    bool IsLevelHandled(LogLevel level) const override
    {
        return level >= logLevel;
    }

public:
    LogLevel logLevel{ LogLevel::Info };
    std::vector<LogRecord> records;
    uint32_t count{ 0 };
};

class LoggingTests : public ::testing::Test
{
public:
    void SetUp() override
    {
        logger.reset();
        logger = std::make_shared<Logger>("test");
        logger->SetLogLevel(LogLevel::Info);
        logger->SetFlushLevel(LogLevel::Warning);

        handler.reset();
        handler = std::make_shared<LoggingTestsHandler>();
        logger->AddHandler(handler);
    }

    void TearDown() override
    {
        if (logger)
        {
            logger.reset();
        }
        if (handler)
        {
            handler.reset();
        }
    }

public:
    std::shared_ptr<Logger> logger;
    std::shared_ptr<LoggingTestsHandler> handler;
};

class LoggingRegistryTests : public ::testing::Test
{};
}

TEST_F(LoggingTests, LoggerUse)
{
    FUSION_LOG_DEBUG(logger, "This should be dropped");
    ASSERT_TRUE(handler->records.empty());

    FUSION_LOG_INFO(logger, "Test '{}'", 123);
    ASSERT_EQ(handler->records.size(), 1);

    FUSION_LOG_ERROR(logger, "This triggers a flush");
    ASSERT_TRUE(handler->records.empty());
    ASSERT_EQ(handler->count, 2);
}

TEST_F(LoggingRegistryTests, MultipleLoggers)
{
    auto& registry = Internal::LogRegistry::Instance();

    auto root = Logging::Get();
    auto nest1 = Logging::Get("nest1");
    auto nest2 = Logging::Get("nest1.nest2");
    auto nest3 = Logging::Get("nest1.nest2.nest3");

    ASSERT_TRUE(true);
}

