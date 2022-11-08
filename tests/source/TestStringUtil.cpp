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

#include <Fusion/StringUtil.h>

#include <array>

TEST(StringUtilTests, Compare_CaseSensitive)
{
    // const char*
    ASSERT_TRUE(StringUtil::Compare("abc", "def") < 0);
    ASSERT_TRUE(StringUtil::Compare("abc", "abc") == 0);
    ASSERT_TRUE(StringUtil::Compare("def", "abc") > 0);

    // string_view
    ASSERT_TRUE(StringUtil::Compare("abc"sv, "def"sv) < 0);
    ASSERT_TRUE(StringUtil::Compare("abc"sv, "abc"sv) == 0);
    ASSERT_TRUE(StringUtil::Compare("def"sv, "abc"sv) > 0);

    // sizing
    ASSERT_TRUE(StringUtil::Compare("aab"sv, "aacb"sv) < 0);
    ASSERT_TRUE(StringUtil::Compare("defg"sv, "hij"sv) < 0);
    ASSERT_TRUE(StringUtil::Compare("hij"sv, "hijk"sv) == 0);
    ASSERT_TRUE(StringUtil::Compare("hijk"sv, "hij"sv) == 0);
    ASSERT_TRUE(StringUtil::Compare("lmno"sv, "hijk"sv) > 0);
    ASSERT_TRUE(StringUtil::Compare("pqrs"sv, "jk"sv) > 0);
}

TEST(StringUtilTests, Compare_CaseInsensitive)
{
    // const char*
    ASSERT_TRUE(StringUtil::CompareI("abcd", "ABCD") == 0);
    ASSERT_TRUE(StringUtil::CompareI("aBcD", "AbCd") == 0);
    ASSERT_TRUE(StringUtil::CompareI("aabb", "BBCC") < 0);
    ASSERT_TRUE(StringUtil::CompareI("AaBb", "CcBB") < 0);
    ASSERT_TRUE(StringUtil::CompareI("dd", "CC") > 0);
    ASSERT_TRUE(StringUtil::CompareI("ddEE", "ccBB") > 0);

    // string_view
    ASSERT_TRUE(StringUtil::CompareI("abcd"sv, "ABCD"sv) == 0);
    ASSERT_TRUE(StringUtil::CompareI("aBcD"sv, "AbCd"sv) == 0);
    ASSERT_TRUE(StringUtil::CompareI("aabb"sv, "BBCC"sv) < 0);
    ASSERT_TRUE(StringUtil::CompareI("AaBb"sv, "CcBB"sv) < 0);
    ASSERT_TRUE(StringUtil::CompareI("dd"sv, "CC"sv) > 0);
    ASSERT_TRUE(StringUtil::CompareI("ddEE"sv, "ccBB"sv) > 0);

    // sizing
    ASSERT_TRUE(StringUtil::CompareI("abc"sv, "ABCD"sv) == 0);
    ASSERT_TRUE(StringUtil::CompareI("aBc"sv, "AbC"sv) == 0);
    ASSERT_TRUE(StringUtil::CompareI("aab"sv, "BBCC"sv) < 0);
    ASSERT_TRUE(StringUtil::CompareI("AaBb"sv, "CcB"sv) < 0);
    ASSERT_TRUE(StringUtil::CompareI("ddee"sv, "CCbb"sv) > 0);
    ASSERT_TRUE(StringUtil::CompareI("ddEE"sv, "ccBB"sv) > 0);
}

TEST(StringUtilTests, Copy_CString)
{
    constexpr auto str = "abcdefghijklmnopqrstuvwxyz"sv;

    std::array<char, 64> buffer = { 0 };
    std::string_view empty{};
    {
        auto res = StringUtil::Copy(
            buffer.data(),
            buffer.size(),
            str.data(),
            str.size());
        ASSERT_EQ(res, str);
        ASSERT_EQ(
            res.size(),
            StringUtil::Length(buffer.data(), buffer.size()));
    }

    ASSERT_EQ(StringUtil::Copy(
        nullptr,
        buffer.size(),
        str.data(),
        str.size()), empty);

    ASSERT_EQ(StringUtil::Copy(
        buffer.data(),
        0,
        str.data(),
        str.size()), empty);

    ASSERT_EQ(StringUtil::Copy(
        buffer.data(),
        buffer.size(),
        nullptr,
        str.size()), empty);

    ASSERT_EQ(StringUtil::Copy(
        buffer.data(),
        buffer.size(),
        str.data(),
        0), empty);
}

TEST(StringUtilTests, Copy_Span)
{
    constexpr auto str = "abcdefghijklmnopqrstuvwxyz"sv;

    std::array<char, 64> buffer = { 0 };
    std::string_view empty{};
    {
        auto res = StringUtil::Copy(buffer, str);
        ASSERT_EQ(res, str);
        ASSERT_EQ(
            res.size(),
            StringUtil::Length(buffer.data(), buffer.size()));
    }

    ASSERT_EQ(StringUtil::Copy({}, str), empty);
    ASSERT_EQ(StringUtil::Copy(buffer, {}), empty);
}

TEST(StringUtilTests, EndsWith_CaseSensitive)
{
    ASSERT_TRUE(StringUtil::EndsWith("abc"sv, "bc"sv));
    ASSERT_FALSE(StringUtil::EndsWith("abcd"sv, "bc"sv));
    ASSERT_FALSE(StringUtil::EndsWith("abcD"sv, "bcd"sv));
}

TEST(StringUtilTests, EndsWith_CaseInsensitive)
{
    ASSERT_TRUE(StringUtil::EndsWithI("abc"sv, "bc"sv));
    ASSERT_FALSE(StringUtil::EndsWithI("abcd"sv, "bc"sv));
    ASSERT_TRUE(StringUtil::EndsWithI("abcD"sv, "bcd"sv));
}

TEST(StringUtilTests, Equal_CaseSensitive)
{
    // const char*
    ASSERT_FALSE(StringUtil::Equal("abc", "def"));
    ASSERT_TRUE(StringUtil::Equal("abc", "abc"));

    // string_view
    ASSERT_FALSE(StringUtil::Equal("abc"sv, "def"sv));
    ASSERT_TRUE(StringUtil::Equal("abc"sv, "abc"sv));

    // sizing
    ASSERT_FALSE(StringUtil::Equal("aab"sv, "aacb"sv));
    ASSERT_FALSE(StringUtil::Equal("defg"sv, "hij"sv));
    ASSERT_TRUE(StringUtil::Equal("hij"sv, "hijk"sv));
    ASSERT_TRUE(StringUtil::Equal("hijk"sv, "hij"sv));
}

TEST(StringUtilTests, Equal_CaseInsensitive)
{
    // const char*
    ASSERT_TRUE(StringUtil::EqualI("abcd", "ABCD"));
    ASSERT_TRUE(StringUtil::EqualI("aBcD", "AbCd"));
    ASSERT_FALSE(StringUtil::EqualI("aabb", "BBCC"));
    ASSERT_FALSE(StringUtil::EqualI("AaBb", "CcBB"));

    // string_view
    ASSERT_TRUE(StringUtil::EqualI("abcd"sv, "ABCD"sv));
    ASSERT_TRUE(StringUtil::EqualI("aBcD"sv, "AbCd"sv));
    ASSERT_FALSE(StringUtil::EqualI("aabb"sv, "BBCC"sv));
    ASSERT_FALSE(StringUtil::EqualI("AaBb"sv, "CcBB"sv));

    // sizing
    ASSERT_TRUE(StringUtil::EqualI("abc"sv, "ABCD"sv));
    ASSERT_TRUE(StringUtil::EqualI("aBc"sv, "AbC"sv));
    ASSERT_FALSE(StringUtil::EqualI("aab"sv, "BBCC"sv));
    ASSERT_FALSE(StringUtil::EqualI("AaBb"sv, "CcB"sv));
}

TEST(StringUtilTests, Join)
{
    {
        std::vector<std::string> str{ "", "" };
        ASSERT_EQ(StringUtil::Join(str, "|"sv), "|"sv);
    }
    {
        std::vector<std::string> str{ "a", "b", "c" };
        ASSERT_EQ(StringUtil::Join(str, ", "sv), "a, b, c"sv);
    }
}

TEST(StringUtilTests, Length)
{
    ASSERT_EQ(StringUtil::Length("abc"), 3);

    constexpr char str[] = "abc123\0\0\0\0\0\0";
    constexpr size_t length = sizeof(str);

    ASSERT_EQ(StringUtil::Length(str, length), 6);
    ASSERT_EQ(StringUtil::ConstLength(str), 6);
}

TEST(StringUtilTests, Less_CaseSensitive)
{
    // const char*
    ASSERT_TRUE(StringUtil::Less("abc", "def"));
    ASSERT_FALSE(StringUtil::Less("abc", "abc"));

    // string_view
    ASSERT_TRUE(StringUtil::Less("abc"sv, "def"sv));
    ASSERT_FALSE(StringUtil::Less("abc"sv, "abc"sv));

    // sizing
    ASSERT_TRUE(StringUtil::Less("aab"sv, "aacb"sv));
    ASSERT_TRUE(StringUtil::Less("defg"sv, "hij"sv));
    ASSERT_FALSE(StringUtil::Less("hij"sv, "hijk"sv));
    ASSERT_FALSE(StringUtil::Less("hijk"sv, "hij"sv));
}

TEST(StringUtilTests, Less_CaseInsensitive)
{
    // const char*
    ASSERT_FALSE(StringUtil::LessI("abcd", "ABCD"));
    ASSERT_FALSE(StringUtil::LessI("aBcD", "AbCd"));
    ASSERT_TRUE(StringUtil::LessI("aabb", "BBCC"));
    ASSERT_TRUE(StringUtil::LessI("AaBb", "CcBB"));
    ASSERT_FALSE(StringUtil::LessI("dd", "CC"));
    ASSERT_FALSE(StringUtil::LessI("ddEE", "ccBB"));

    // string_view
    ASSERT_FALSE(StringUtil::LessI("abcd"sv, "ABCD"sv));
    ASSERT_FALSE(StringUtil::LessI("aBcD"sv, "AbCd"sv));
    ASSERT_TRUE(StringUtil::LessI("aabb"sv, "BBCC"sv));
    ASSERT_TRUE(StringUtil::LessI("AaBb"sv, "CcBB"sv));
    ASSERT_FALSE(StringUtil::LessI("dd"sv, "CC"sv));
    ASSERT_FALSE(StringUtil::LessI("ddEE"sv, "ccBB"sv));

    // sizing
    ASSERT_FALSE(StringUtil::LessI("abc"sv, "ABCD"sv));
    ASSERT_FALSE(StringUtil::LessI("aBc"sv, "AbC"sv));
    ASSERT_TRUE(StringUtil::LessI("aab"sv, "BBCC"sv));
    ASSERT_TRUE(StringUtil::LessI("AaBb"sv, "CcB"sv));
    ASSERT_FALSE(StringUtil::LessI("ddee"sv, "CCbb"sv));
    ASSERT_FALSE(StringUtil::LessI("ddEE"sv, "ccBB"sv));
}

TEST(StringUtilTests, Split)
{
    ASSERT_EQ(
        StringUtil::Split(
            "abc|123"sv,
            "|"sv),
        std::vector<std::string>({
            "abc",
            "123",
        }));

    ASSERT_EQ(
        StringUtil::Split(
            "abc|123|def|456"sv,
            "|"sv,
            3),
        std::vector<std::string>({
            "abc",
            "123",
            "def",
        }));

    ASSERT_EQ(
        StringUtil::Split(
            "abc|123|def|456"sv,
            ""sv,
            3),
        std::vector<std::string>({
            "abc|123|def|456",
        }));

    std::vector<std::string> empty;

    ASSERT_EQ(
        StringUtil::Split(
            "abc|123"sv,
            "|"sv,
            0),
        empty);

    ASSERT_EQ(
        StringUtil::Split(
            ""sv,
            "|"sv,
            1),
        empty);
}

TEST(StringUtilTests, StartsWith_CaseSensitive)
{
    ASSERT_TRUE(StringUtil::StartsWith("abc"sv, "ab"sv));
    ASSERT_FALSE(StringUtil::StartsWith("abcd"sv, "efg"sv));
    ASSERT_FALSE(StringUtil::StartsWith("aBcD"sv, "ab"sv));
}

TEST(StringUtilTests, StartsWith_CaseInsensitive)
{
    ASSERT_TRUE(StringUtil::StartsWithI("abc"sv, "AB"sv));
    ASSERT_FALSE(StringUtil::StartsWithI("abcd"sv, "BC"sv));
    ASSERT_TRUE(StringUtil::StartsWithI("abcD"sv, "abcd"sv));
}

TEST(StringUtilTests, ToFloatingPoint)
{
    FUSION_ASSERT_RESULT(
        StringUtil::ToFloatingPoint("3.14"sv),
        [](double value) {
            ASSERT_FLOAT_EQ(value, 3.14f);
        });

    FUSION_ASSERT_RESULT(
        StringUtil::ToFloatingPoint("2.1128"sv),
        [](double value) {
            ASSERT_FLOAT_EQ(value, 2.1128f);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToFloatingPoint("abc123"sv),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "not a number"sv);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToFloatingPoint(""sv),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "empty string"sv);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToFloatingPoint(
            "3.14"sv,
            double(-1),
            double(1)),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "out of range"sv);
        });
}

TEST(StringUtilTests, ToLower)
{
    std::string test;

    test = "ABCDE";
    StringUtil::ToLower(test);
    ASSERT_EQ(test, "abcde"sv);

    test = "AbCdE";
    StringUtil::ToLower(test);
    ASSERT_EQ(test, "abcde"sv);

    test = "AB#DE";
    StringUtil::ToLower(test);
    ASSERT_EQ(test, "ab#de"sv);
}

TEST(StringUtilTests, ToLowerCopy)
{
    ASSERT_EQ(StringUtil::ToLowerCopy("ABCDE"), "abcde"sv);
    ASSERT_EQ(StringUtil::ToLowerCopy("AbCdE"), "abcde"sv);
    ASSERT_EQ(StringUtil::ToLowerCopy("AB#DE"), "ab#de"sv);
}

TEST(StringUtilTests, ToSignedNumber)
{
    FUSION_ASSERT_RESULT(
        StringUtil::ToSignedNumber("123456789"sv),
        [](int64_t value) {
            ASSERT_EQ(value, 123456789);
        });

    FUSION_ASSERT_RESULT(
        StringUtil::ToSignedNumber("-123456789"sv),
        [](int64_t value) {
            ASSERT_EQ(value, -123456789);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToSignedNumber(""sv),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "empty string"sv);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToSignedNumber(
            "123456789"sv,
            10,
            -1000,
            1000),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "out of range"sv);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToSignedNumber("12345abc"sv),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "not a number"sv);
        });
}

TEST(StringUtilTests, ToUnsignedNumber)
{
    FUSION_ASSERT_RESULT(
        StringUtil::ToUnsignedNumber("123456789"sv),
        [](uint64_t value) {
            ASSERT_EQ(value, 123456789);
        });

    FUSION_ASSERT_RESULT(
        StringUtil::ToUnsignedNumber("0"sv),
        [](uint64_t value) {
            ASSERT_EQ(value, 0);
        });

    FUSION_ASSERT_RESULT(
        StringUtil::ToUnsignedNumber("12345678987654321"sv),
        [](uint64_t value) {
            ASSERT_EQ(value, 12345678987654321);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToUnsignedNumber(""sv),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "empty string"sv);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToUnsignedNumber(
            "123456789"sv,
            10,
            1000),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "out of range"sv);
        });

    FUSION_ASSERT_FAILURE(
        StringUtil::ToUnsignedNumber("12345abc"sv),
        [](const Failure& f) {
            ASSERT_EQ(f.Message(), "not a number"sv);
        });
}

TEST(StringUtilTests, ToUpper)
{
    std::string test;

    test = "abcde";
    StringUtil::ToUpper(test);
    ASSERT_EQ(test, "ABCDE"sv);

    test = "AbCdE";
    StringUtil::ToUpper(test);
    ASSERT_EQ(test, "ABCDE"sv);

    test = "AB#DE";
    StringUtil::ToUpper(test);
    ASSERT_EQ(test, "AB#DE"sv);
}

TEST(StringUtilTests, ToUpperCopy)
{
    ASSERT_EQ(StringUtil::ToUpperCopy("abCde"), "ABCDE"sv);
    ASSERT_EQ(StringUtil::ToUpperCopy("AbCdE"), "ABCDE"sv);
    ASSERT_EQ(StringUtil::ToUpperCopy("ab#de"), "AB#DE"sv);
}

TEST(StringUtilTests, Trim)
{
    constexpr auto tokens = "az\r\n"sv;
    std::string test;

    test = "   abc 123\r\n";
    StringUtil::Trim(test);
    ASSERT_EQ(test, "abc 123"sv);

    test = "\r\nabc\n123  .. ...  \n";
    StringUtil::Trim(test);
    ASSERT_EQ(test, "abc\n123  .. ..."sv);

    test = "abcdef\r  12345\r\nba    ";
    StringUtil::Trim(test, tokens);
    ASSERT_EQ(test, "bcdef\r  12345\r\nba    "sv);
}

TEST(StringUtilTests, TrimCopy)
{
    constexpr auto tokens = "az\r\n"sv;

    ASSERT_EQ(StringUtil::TrimCopy(
        "   abc 123\r\n"sv),
        "abc 123"sv);

    ASSERT_EQ(StringUtil::TrimCopy(
        "\r\nabc\n123  .. ...  \n"sv),
        "abc\n123  .. ..."sv);

    ASSERT_EQ(StringUtil::Trim(
        "abcdef\r  12345\r\nba    "sv, tokens),
        "bcdef\r  12345\r\nba    "sv);
}

TEST(StringBuilderTests, FixedStringBuilder)
{
    std::array<char, 32> buffer;
    FixedStringBuilder builder(buffer);

    // Initial state
    ASSERT_EQ(builder.Capacity(), buffer.size() - 1);
    ASSERT_EQ(builder.Size(), 0);
    ASSERT_EQ(builder.View(), ""sv);

    // Add some data
    builder.Append("abc123"sv);
    ASSERT_EQ(builder.Size(), 6);
    ASSERT_EQ(builder.View(), "abc123"sv);
    ASSERT_EQ(
        builder.Size(),
        StringUtil::Length(buffer.data(), buffer.size()));

    {
        // Fake adding some external data
        constexpr auto str = "def456"sv;
        memcpy(builder.Head(), str.data(), str.size());
        builder.Offset(str.size());

        ASSERT_EQ(builder.View(), "abc123def456"sv);
        ASSERT_EQ(builder.Size(), 12);
    }

    ASSERT_EQ(builder.Remaining(), 19);
    FixedStringBuilder builder2(builder.Subspan());

    // Check the new builder
    ASSERT_EQ(builder2.Capacity(), 18);

    {
        // Add some more data
        constexpr auto str = "ghi789"sv;
        builder2 += str;

        ASSERT_EQ(builder2.View(), str);
        ASSERT_EQ(builder2.Size(), str.size());
        builder.Offset(builder2.Size());
    }

    std::string str = builder.String();
    ASSERT_EQ(str, "abc123def456ghi789"sv);
}
