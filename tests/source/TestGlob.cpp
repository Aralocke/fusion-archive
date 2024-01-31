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

#include <Fusion/Glob.h>

TEST(GlobTests, ZeroOrMoreWildcardCaseMatching)
{
    ASSERT_TRUE(Glob::Match("a*z", "abz", true));
    ASSERT_TRUE(Glob::Match("a*/d*/g*", "abc/def/ghi", true));
    ASSERT_TRUE(Glob::Match("*", "abc", true));
    ASSERT_TRUE(Glob::Match("abc", "abc", true));
    ASSERT_TRUE(Glob::Match("abc*", "abc", true));

    ASSERT_FALSE(Glob::Match("A*C", "abc", true));
    ASSERT_FALSE(Glob::Match("*/def/*", "ABC/DEF/GHI", true));
    ASSERT_FALSE(Glob::Match("ABC*", "abc", true));

    ASSERT_TRUE(Glob::Match(
        "abc*ghijkl*pqrst*xyz",
        "abcdefghijklmnopqrstuvwxyz",
        true));
}

TEST(GlobTests, ZeroOrMoreWildcardInsensitiveMatching)
{
    ASSERT_TRUE(Glob::Match("a*z", "ABZ", false));
    ASSERT_TRUE(Glob::Match("a*/D*/g*", "aBc/dEf/gHi", false));
    ASSERT_TRUE(Glob::Match("*", "aBc", false));
    ASSERT_TRUE(Glob::Match("ABc", "abc", false));
    ASSERT_TRUE(Glob::Match("abc*", "AbC", false));

    ASSERT_TRUE(Glob::Match("A*C", "abc", false));
    ASSERT_TRUE(Glob::Match("*/def/*", "ABC/DEF/GHI", false));
    ASSERT_TRUE(Glob::Match("ABC*", "abc", false));

    ASSERT_TRUE(Glob::Match(
        "abc*ghijkl*pqrst*xyz",
        "abcLALALAghijklLALALApqrstDEDEDExyz", false));
}

TEST(GlobTests, SingleCharacterMatching)
{
    ASSERT_TRUE(Glob::Match("a?c", "abc"));
    ASSERT_TRUE(Glob::Match("a?/c", "ab/c"));
    ASSERT_TRUE(Glob::Match("a?c", "aBc"));

    ASSERT_TRUE(Glob::Match(
        "?bcd?fgh?jklmn?pqrst?vwxyz",
        "abcdefghijklmnopqrstuvwxyz"));

    ASSERT_FALSE(Glob::Match("a?c", "a/c"));
}
