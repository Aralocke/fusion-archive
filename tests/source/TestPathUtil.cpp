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
#include <Fusion/Path.h>
#include <Fusion/PathUtil.h>

#include <algorithm>

TEST(PathUtilTests, Basename)
{
    ASSERT_EQ(PathUtil::Basename(""), "");
    ASSERT_EQ(PathUtil::Basename("/"), "");

#if FUSION_PLATFORM_WINDOWS
    ASSERT_EQ(PathUtil::Basename("c:///////"), "");
    ASSERT_EQ(PathUtil::Basename("c://../..///../"), "");
    ASSERT_EQ(PathUtil::Basename("\\foo\\bar\\"), "bar");
    ASSERT_EQ(PathUtil::Basename("c://foor/bar/base.txt"), "base.txt");
#endif

    ASSERT_EQ(PathUtil::Basename("../../../../a.txt"), "a.txt");
    ASSERT_EQ(PathUtil::Basename("a/b/c"), "c");
    ASSERT_EQ(PathUtil::Basename("a/../b/./..//////c/d.txt"), "d.txt");
}

TEST(PathUtilTests, Dirname)
{
    ASSERT_EQ(PathUtil::Dirname(""), "");
    ASSERT_EQ(PathUtil::Dirname("foo"), "");

#if FUSION_PLATFORM_WINDOWS
    ASSERT_EQ(PathUtil::Dirname("C:\\foo\\bar"), "c:/foo");
    ASSERT_EQ(PathUtil::Dirname("C:\\foo\\bar\\test.txt"), "c:/foo/bar");
    ASSERT_EQ(PathUtil::Dirname("C:\\foo"), "c:/");
    ASSERT_EQ(PathUtil::Dirname("C://"), "c:/");
    ASSERT_EQ(PathUtil::Dirname("\\foo\\bar\\.\\baz"), "/foo/bar");
#endif

    EXPECT_FALSE(Environment::Has("FUSION_CWD"));
    {
        TemporaryEnvironment env;
        EXPECT_TRUE(env.Set("FUSION_CWD", "/foo/bar"));

        ASSERT_EQ(PathUtil::Dirname("."), "/foo");
        ASSERT_EQ(PathUtil::Dirname("./bar"), "/foo/bar");
    }

    ASSERT_EQ(PathUtil::Dirname("/"), "/");
    ASSERT_EQ(PathUtil::Dirname("/a/b/c"), "/a/b");
    ASSERT_EQ(PathUtil::Dirname("/a/b"), "/a");
    ASSERT_EQ(PathUtil::Dirname("/a"), "/");
}

TEST(PathUtilTests, Extension)
{
    ASSERT_EQ(PathUtil::Extension("/"), "");

#if FUSION_PLATFORM_WINDOWS
    ASSERT_EQ(PathUtil::Extension("C:\\foo\\bar"), "");
    ASSERT_EQ(PathUtil::Extension("C:\\foo\\bar\\test.txt"), "txt");
    ASSERT_EQ(PathUtil::Extension("C://"), "");
    ASSERT_EQ(PathUtil::Extension("\\foo\\bar\\.\\baz"), "");
#endif

    ASSERT_EQ(PathUtil::Extension("/foo.bar/baz.txt"), "txt");
    ASSERT_EQ(PathUtil::Extension("text.txt"), "txt");

    ASSERT_EQ(PathUtil::Extension(".."), "");
    ASSERT_EQ(PathUtil::Extension("."), "");
    ASSERT_EQ(PathUtil::Extension("~"), "");

    ASSERT_EQ(PathUtil::Extension(".abc"), "abc");
}

TEST(PathUtilTests, IsAbsolute)
{
#if FUSION_PLATFORM_WINDOWS
    ASSERT_TRUE(PathUtil::IsAbsolute("C:/"));
    ASSERT_TRUE(PathUtil::IsAbsolute("C:\\"));
    ASSERT_TRUE(PathUtil::IsAbsolute("C:/a/b"));
    ASSERT_TRUE(PathUtil::IsAbsolute("//a/b/c"));
    ASSERT_TRUE(PathUtil::IsAbsolute("\\a\\b\\c"));
#else
    ASSERT_FALSE(PathUtil::IsAbsolute("C:/"));
    ASSERT_FALSE(PathUtil::IsAbsolute("C:\\"));
    ASSERT_FALSE(PathUtil::IsAbsolute("C:/a/b"));
    ASSERT_FALSE(PathUtil::IsAbsolute("//a/b/c"));
    ASSERT_FALSE(PathUtil::IsAbsolute("\\a\\b\\c"));
#endif

    ASSERT_TRUE(PathUtil::IsAbsolute("/"));
    ASSERT_TRUE(PathUtil::IsAbsolute("/a/b"));
    ASSERT_TRUE(PathUtil::IsAbsolute("/a/./b/../c"));

    ASSERT_FALSE(PathUtil::IsAbsolute("a/b"));
    ASSERT_FALSE(PathUtil::IsAbsolute("b"));
    ASSERT_FALSE(PathUtil::IsAbsolute("."));
    ASSERT_FALSE(PathUtil::IsAbsolute("~"));
    ASSERT_FALSE(PathUtil::IsAbsolute("../.."));
}

TEST(PathUtilTests, IsNormalizedRoot)
{
    auto Normalize = [](std::string_view p) -> std::string
    {
        return PathUtil::Normalize(p);
    };
    auto IsNormalizedRoot = [&](std::string_view p) -> bool
    {
        return PathUtil::IsNormalizedRoot(Normalize(p));
    };

#if FUSION_PLATFORM_WINDOWS
    ASSERT_TRUE(IsNormalizedRoot("C:/"));
    ASSERT_TRUE(IsNormalizedRoot("C:\\"));
    ASSERT_TRUE(IsNormalizedRoot("////a/b/c"));
    ASSERT_TRUE(IsNormalizedRoot("\\\\a\\b\\c"));
    ASSERT_TRUE(IsNormalizedRoot("\\"));
    ASSERT_TRUE(IsNormalizedRoot("\\\\"));
    ASSERT_FALSE(IsNormalizedRoot("C:/a/b"));
#else
    ASSERT_FALSE(IsNormalizedRoot("C:/"));
    ASSERT_FALSE(IsNormalizedRoot("C:\\"));
    ASSERT_FALSE(IsNormalizedRoot("C:/a/b"));
    ASSERT_FALSE(IsNormalizedRoot("//a/b/c"));
    ASSERT_FALSE(IsNormalizedRoot("\\a\\b\\c"));
    ASSERT_FALSE(IsNormalizedRoot("\\"));
    ASSERT_FALSE(IsNormalizedRoot("\\\\"));
#endif

    ASSERT_TRUE(IsNormalizedRoot("/.."));
    ASSERT_TRUE(IsNormalizedRoot("/"));

    ASSERT_TRUE(IsNormalizedRoot("/../../../../../"));
    ASSERT_FALSE(IsNormalizedRoot("/a/b/./../.././c"));
    ASSERT_FALSE(IsNormalizedRoot("/a/b/c"));
}

TEST(PathUtilTests, IsRelative)
{
    ASSERT_TRUE(PathUtil::IsRelative("/abc", "/"));
    ASSERT_TRUE(PathUtil::IsRelative("/a/b/c", "/a/b"));
    ASSERT_TRUE(PathUtil::IsRelative("/a/b/./c/d/..", "/a/b/.."));

    ASSERT_FALSE(PathUtil::IsRelative("/a", "/a"));
    ASSERT_FALSE(PathUtil::IsRelative("/a", "/b"));
    ASSERT_FALSE(PathUtil::IsRelative("/a/b", "/a/c"));

#if FUSION_PLATFORM_WINDOWS
    ASSERT_TRUE(PathUtil::IsRelative("c:/a/b", "c:/a"));
    ASSERT_TRUE(PathUtil::IsRelative("c:\\a\\b", "c:\\a"));
    ASSERT_TRUE(PathUtil::IsRelative("\\\\a\\b\\c", "\\\\a\\b"));
#endif

    ASSERT_FALSE(PathUtil::IsRelative("/b", "/a"));
    ASSERT_FALSE(PathUtil::IsRelative("/", "/"));

    ASSERT_FALSE(PathUtil::IsRelative("", ""));
}

TEST(PathUtilTests, Join)
{
    ASSERT_EQ(PathUtil::Join("a", "b"), "a/b");
    ASSERT_EQ(PathUtil::Join("a", "b", "c"), "a/b/c");
    ASSERT_EQ(PathUtil::Join("a", "b", "c", "d"), "a/b/c/d");
    ASSERT_EQ(PathUtil::Join("a", "b", "c", "d", "e", "f", "g"), "a/b/c/d/e/f/g");

    ASSERT_EQ(PathUtil::Join("a", "/b"), "/b");
    ASSERT_EQ(PathUtil::Join("", "b"), "b");
    ASSERT_EQ(PathUtil::Join("a", "..", "b"), "b");

#if FUSION_PLATFORM_WINDOWS
    ASSERT_EQ(PathUtil::Join("C:", "a", "..", "b"), "c:/b");
    ASSERT_EQ(PathUtil::Join("A:/b/c", "B:/c/d"), "b:/c/d");
#endif

    ASSERT_EQ(PathUtil::Join("", "abc"), "abc");
    ASSERT_EQ(PathUtil::Join("abc", ""), "abc");
    ASSERT_EQ(PathUtil::Join("", ""), "");
}

TEST(PathUtilTests, LongPath)
{
#if FUSION_PLATFORM_WINDOWS
    static const constexpr std::string_view longPath =
        "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/" "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/"
        "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/" "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/"
        "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/" "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/"
        "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/" "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/"
        "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z" "/" "abc/def/ghi/jkl/mno/pqr/stu/v/wxy/z";

    ASSERT_EQ(PathUtil::LongPath(""), "");
    ASSERT_EQ(PathUtil::LongPath("/"), "/");

    EXPECT_FALSE(Environment::Has("FUSION_CWD"));
    {
        TemporaryEnvironment env;
        ASSERT_TRUE(env.Set("FUSION_CWD", "/foo/bar"));

        ASSERT_EQ(PathUtil::LongPath("../.."), "/");
    }
    
    ASSERT_EQ(PathUtil::LongPath("C:/a/b"), "c:/a/b");

    ASSERT_EQ(PathUtil::LongPath(longPath), longPath);
    {
        std::string lp(longPath);
        {
            lp.insert(0, "c:/"sv);
        }

        std::string fp(lp);
        {
            fp.insert(0, "\\\\?\\"sv);

            std::replace(begin(fp), end(fp),
                Path::SeparatorUnix,
                Path::SeparatorWindows);
        }

        ASSERT_EQ(PathUtil::LongPath(lp), fp);
    }
    ASSERT_EQ(PathUtil::LongPath("C:/ a / b "), "\\\\?\\c:\\ a \\ b ");
    ASSERT_EQ(PathUtil::LongPath("C:/a/b "), "\\\\?\\c:\\a\\b ");
    ASSERT_EQ(PathUtil::LongPath("C:/a/b."), "\\\\?\\c:\\a\\b.");
#endif

    ASSERT_EQ(PathUtil::LongPath("a/../b"), "b");
    ASSERT_EQ(PathUtil::LongPath("a/b/c"), "a/b/c");
}

TEST(PathUtilTests, NormalizePaths)
{
    ASSERT_EQ(PathUtil::Normalize("a/./b"), "a/b");
    ASSERT_EQ(PathUtil::Normalize("a//////////b"), "a/b");
    ASSERT_EQ(PathUtil::Normalize("a/../b"), "b");
    ASSERT_EQ(PathUtil::Normalize("a/../b\\//\\//"), "b");

#if FUSION_PLATFORM_WINDOWS
    ASSERT_EQ(PathUtil::Normalize("c://a/../b"), "c:/b");
    ASSERT_EQ(PathUtil::Normalize("c://a/../../b"), "c:/b");
    ASSERT_EQ(PathUtil::Normalize("c://a/../../////../../../b"), "c:/b");

    ASSERT_EQ(PathUtil::Normalize("c:"), "c:/");
    ASSERT_EQ(PathUtil::Normalize("c://"), "c:/");
    ASSERT_EQ(PathUtil::Normalize("c:\\\\\\\\\\"), "c:/");
#endif

    EXPECT_FALSE(Environment::Has("FUSION_CWD"));
    {
        TemporaryEnvironment env;
        EXPECT_TRUE(env.Set("FUSION_CWD", "/foo/bar"));

        ASSERT_EQ(PathUtil::Normalize("."), "/foo/bar");
        ASSERT_EQ(PathUtil::Normalize("./baz"), "/foo/bar/baz");
        ASSERT_EQ(PathUtil::Normalize("./baz///////bar"), "/foo/bar/baz/bar");

        ASSERT_EQ(PathUtil::Normalize(".."), "/foo");
        ASSERT_EQ(PathUtil::Normalize("../bar"), "/foo/bar");
    }

    ASSERT_EQ(PathUtil::Normalize("/"), "/");
    ASSERT_EQ(PathUtil::Normalize("////////////"), "/");

    EXPECT_FALSE(Environment::Has("FUSION_HOME"));
    {
        TemporaryEnvironment env;
#if FUSION_PLATFORM_WINDOWS
        EXPECT_TRUE(env.Set("FUSION_HOME", "c:\\foo\\bar"));

        ASSERT_EQ(PathUtil::Normalize("~"), "c:/foo/bar");
        ASSERT_EQ(PathUtil::Normalize("~/baz"), "c:/foo/bar/baz");
        ASSERT_EQ(PathUtil::Normalize("~/../../../../../baz"), "c:/baz");
#else
        EXPECT_TRUE(env.Set("FUSION_HOME", "\\foo\\bar"));

        ASSERT_EQ(PathUtil::Normalize("~"), "/foo/bar");
        ASSERT_EQ(PathUtil::Normalize("~/baz"), "/foo/bar/baz");
        ASSERT_EQ(PathUtil::Normalize("~/../../../../../baz"), "/baz");
#endif
    }

    ASSERT_EQ(PathUtil::Normalize("a/.."), "");
    ASSERT_EQ(PathUtil::Normalize("a/b/../../../../../"), "../../..");

    ASSERT_EQ(PathUtil::Normalize("//foo/bar"), "//foo/bar/");
    ASSERT_EQ(PathUtil::Normalize("//foo/bar//////"), "//foo/bar/");

#if FUSION_PLATFORM_WINDOWS
    ASSERT_EQ(PathUtil::Normalize("C:\\foo\\bar\\..\\..\\..\\..\\..\\..\\"), "c:/");
    ASSERT_EQ(PathUtil::Normalize("C:/foo/bar/../../.././.././../.."), "c:/");
#endif
}

TEST(PathUtilTests, Normcase)
{
#if FUSION_PLATFORM_WINDOWS
    ASSERT_EQ(PathUtil::Normcase("C:/"), "c:/");
    ASSERT_EQ(PathUtil::Normcase("AbCdEfG"), "abcdefg");
    ASSERT_EQ(PathUtil::Normcase("/foO/BaR"), "/foo/bar");
    ASSERT_EQ(PathUtil::Normcase("C:/foO/BaR"), "c:/foo/bar");

    ASSERT_EQ(PathUtil::Normcase("\\\\\\\\"), "////");
#else
    ASSERT_EQ(PathUtil::Normcase("/foO/BaR"), "/foO/BaR");
    ASSERT_EQ(PathUtil::Normcase("\\foO\\BaR"), "/foO/BaR");
#endif

    ASSERT_EQ(PathUtil::Normcase("/"), "/");
    ASSERT_EQ(PathUtil::Normcase(""), "");
}

TEST(PathUtilTests, RelPath)
{
    ASSERT_EQ(PathUtil::RelPath("", ""), "");

    ASSERT_EQ(PathUtil::RelPath("b/c", "/a"), "/a/b/c");
    ASSERT_EQ(PathUtil::RelPath("/b/./c", "/a"), "/b/c");
    ASSERT_EQ(PathUtil::RelPath("b/../c", "/a"), "/a/c");

    ASSERT_EQ(PathUtil::RelPath("c/..", "/a/b"), "/a/b");
    ASSERT_EQ(PathUtil::RelPath("c/../../../d", "/a/b"), "/d");
    ASSERT_EQ(PathUtil::RelPath("b/c", "/a"), "/a/b/c");

    ASSERT_EQ(PathUtil::RelPath("..", "/"), "/");
    ASSERT_EQ(PathUtil::RelPath("/", ".."), "/");

#if FUSION_PLATFORM_WINDOWS
    ASSERT_EQ(PathUtil::RelPath("a", "C:/"), "c:/a");
    ASSERT_EQ(PathUtil::RelPath("a/../b", "C:/"), "c:/b");
    ASSERT_EQ(PathUtil::RelPath("foo/bar", "\\foo\\bar"), "/foo/bar/foo/bar");

    ASSERT_EQ(PathUtil::RelPath("..", "C:/"), "c:/");
    ASSERT_EQ(PathUtil::RelPath("../../../..", "c:/../.."), "c:/");
#endif

    EXPECT_FALSE(Environment::Has("FUSION_CWD"));
    {
        TemporaryEnvironment env;
        ASSERT_TRUE(env.Set("FUSION_CWD", "/foo/bar"));

        ASSERT_EQ(PathUtil::RelPath("a"), "/foo/bar/a");
        ASSERT_EQ(PathUtil::RelPath(".."), "/foo");
        ASSERT_EQ(PathUtil::RelPath("/a/b"), "/a/b");

        ASSERT_EQ(PathUtil::RelPath("../../.."), "/");
    }
}
