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

#include <Fusion/Environment.h>

TEST(EnvironmentTests, SetAndGet)
{
    ASSERT_TRUE(Environment::Get(
        "FUSION_TESTS_SETANDGET").empty());
    ASSERT_TRUE(Environment::Set(
        "FUSION_TESTS_SETANDGET", "value 123"));
    ASSERT_EQ(Environment::Get("FUSION_TESTS_SETANDGET"), "value 123");
}

TEST(EnvironmentTests, HasVariable)
{
    ASSERT_FALSE(Environment::Has("FUSION_TESTS_HASVARIABLE"));
    ASSERT_TRUE(Environment::Set(
        "FUSION_TESTS_HASVARIABLE", "value 456"));
    ASSERT_TRUE(Environment::Has("FUSION_TESTS_HASVARIABLE"));
}

TEST(EnvironmentTests, UnsetVariable)
{
    ASSERT_FALSE(Environment::Has("FUSION_TESTS_UNSETVARIABLE"));

    ASSERT_TRUE(Environment::Set(
        "FUSION_TESTS_UNSETVARIABLE", "value 789"));
    ASSERT_EQ(
        Environment::Get("FUSION_TESTS_UNSETVARIABLE"),
        "value 789");

    ASSERT_TRUE(Environment::Unset("FUSION_TESTS_UNSETVARIABLE"));
    ASSERT_FALSE(Environment::Has("FUSION_TESTS_UNSETVARIABLE"));
}

TEST(EnvironmentTests, TempEnvironment)
{
    EXPECT_FALSE(Environment::Has("FUSION_TESTS_TEMPENVIRONMENT"));
    {
        TemporaryEnvironment env;
        ASSERT_FALSE(env.Unset("FUSION_TESTS_TEMPENVIRONMENT"));
        ASSERT_TRUE(env.Set("FUSION_TESTS_TEMPENVIRONMENT", "123"));
        ASSERT_EQ(Environment::Get("FUSION_TESTS_TEMPENVIRONMENT"), "123");
    }
    EXPECT_FALSE(Environment::Has("FUSION_TESTS_TEMPENVIRONMENT"));
}
