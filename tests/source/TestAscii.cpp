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

#include <Fusion/Ascii.h>
#include <array>
#include <string>

TEST(AsciiTests, FromDigit)
{
    // Positive
    ASSERT_EQ(Ascii::FromDigit(0), '0');
    ASSERT_EQ(Ascii::FromDigit(1), '1');
    ASSERT_EQ(Ascii::FromDigit(2), '2');
    ASSERT_EQ(Ascii::FromDigit(3), '3');
    ASSERT_EQ(Ascii::FromDigit(4), '4');
    ASSERT_EQ(Ascii::FromDigit(5), '5');
    ASSERT_EQ(Ascii::FromDigit(6), '6');
    ASSERT_EQ(Ascii::FromDigit(7), '7');
    ASSERT_EQ(Ascii::FromDigit(8), '8');
    ASSERT_EQ(Ascii::FromDigit(9), '9');
    // Negative
    ASSERT_FALSE(Ascii::FromDigit(10));
}

TEST(AsciiTests, FromHexChar)
{
    // Positive
    ASSERT_EQ(Ascii::FromHexChar('0'), 0);
    ASSERT_EQ(Ascii::FromHexChar('1'), 1);
    ASSERT_EQ(Ascii::FromHexChar('2'), 2);
    ASSERT_EQ(Ascii::FromHexChar('3'), 3);
    ASSERT_EQ(Ascii::FromHexChar('4'), 4);
    ASSERT_EQ(Ascii::FromHexChar('5'), 5);
    ASSERT_EQ(Ascii::FromHexChar('6'), 6);
    ASSERT_EQ(Ascii::FromHexChar('7'), 7);
    ASSERT_EQ(Ascii::FromHexChar('8'), 8);
    ASSERT_EQ(Ascii::FromHexChar('9'), 9);
    ASSERT_EQ(Ascii::FromHexChar('a'), 10);
    ASSERT_EQ(Ascii::FromHexChar('B'), 11);
    ASSERT_EQ(Ascii::FromHexChar('c'), 12);
    ASSERT_EQ(Ascii::FromHexChar('D'), 13);
    ASSERT_EQ(Ascii::FromHexChar('e'), 14);
    ASSERT_EQ(Ascii::FromHexChar('F'), 15);
    // Negative
    ASSERT_FALSE(Ascii::FromHexChar('G'));
}

TEST(AsciiTests, FromHexString)
{
    constexpr auto data = std::to_array<uint8_t>({
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab,
        0xcd, 0xef, 0x01, 0x23, 0x45, 0x67,
        0x89, 0xab, 0xcd, 0xef });
   
    constexpr auto str = "0123456789abcdef0123456789abcdef"sv;

    std::array<uint8_t, 128> buffer = { 0 };
    std::span<uint8_t> empty{};

    auto EqualSpan = [](
        std::span<const uint8_t> a,
        std::span<const uint8_t> b) -> bool
    {
        return memcmp(
            a.data(),
            b.data(),
            std::min(a.size(), b.size())) == 0;
    };

    // Positive
    ASSERT_TRUE(
        EqualSpan(
            Ascii::FromHexString(
                str,
                buffer.data(),
                buffer.size()),
            data));

    ASSERT_TRUE(
        EqualSpan(
            Ascii::FromHexString(
                str,
                buffer.data(),
                4),
            data));

    // Negative
    ASSERT_TRUE(
        EqualSpan(
            Ascii::FromHexString(
                {},
                buffer.data(),
                4),
            empty));

    ASSERT_TRUE(
        EqualSpan(
            Ascii::FromHexString(
                str,
                0,
                buffer.size()),
            empty));

    ASSERT_TRUE(
        EqualSpan(
            Ascii::FromHexString(
                str,
                buffer.data(),
                0),
            empty));
}

TEST(AsciiTests, IsAlpha)
{
    // Positive
    ASSERT_TRUE(Ascii::IsAlpha('a'));
    ASSERT_TRUE(Ascii::IsAlpha('B'));
    ASSERT_TRUE(Ascii::IsAlpha('y'));
    ASSERT_TRUE(Ascii::IsAlpha('Z'));

    // Negative
    ASSERT_FALSE(Ascii::IsAlpha('0'));
    ASSERT_FALSE(Ascii::IsAlpha('#'));
}

TEST(AsciiTests, IsAlphaNum)
{
    // Positive
    ASSERT_TRUE(Ascii::IsAlphaNum('a'));
    ASSERT_TRUE(Ascii::IsAlphaNum('B'));
    ASSERT_TRUE(Ascii::IsAlphaNum('y'));
    ASSERT_TRUE(Ascii::IsAlphaNum('Z'));
    ASSERT_TRUE(Ascii::IsAlphaNum('1'));
    ASSERT_TRUE(Ascii::IsAlphaNum('9'));

    // Negative
    ASSERT_FALSE(Ascii::IsAlphaNum('$'));
    ASSERT_FALSE(Ascii::IsAlphaNum('#'));
}

TEST(AsciiTests, IsDigit)
{
    // Positive
    ASSERT_TRUE(Ascii::IsDigit('0'));
    ASSERT_TRUE(Ascii::IsDigit('9'));

    // Negative
    ASSERT_FALSE(Ascii::IsDigit('#'));
    ASSERT_FALSE(Ascii::IsDigit('A'));
}

TEST(AsciiTests, IsFalseString)
{
    // Positive
    ASSERT_TRUE(Ascii::IsFalseString("False"sv));
    ASSERT_TRUE(Ascii::IsFalseString("false"sv));
    ASSERT_TRUE(Ascii::IsFalseString("no"sv));
    ASSERT_TRUE(Ascii::IsFalseString("n"sv));
    ASSERT_TRUE(Ascii::IsFalseString("0"sv));

    // Negative
    ASSERT_FALSE(Ascii::IsFalseString("True"sv));
}

TEST(AsciiTests, IsHex)
{
    // Positive
    ASSERT_TRUE(Ascii::IsHex('0'));
    ASSERT_TRUE(Ascii::IsHex('9'));
    ASSERT_TRUE(Ascii::IsHex('a'));
    ASSERT_TRUE(Ascii::IsHex('F'));

    // Negative
    ASSERT_FALSE(Ascii::IsHex('#'));
    ASSERT_FALSE(Ascii::IsHex('G'));
    ASSERT_FALSE(Ascii::IsHex('h'));
}

TEST(AsciiTests, IsLower)
{
    // Positive
    ASSERT_TRUE(Ascii::IsLower('a'));
    ASSERT_TRUE(Ascii::IsLower('b'));
    ASSERT_TRUE(Ascii::IsLower('y'));
    ASSERT_TRUE(Ascii::IsLower('z'));

    // Negative
    ASSERT_FALSE(Ascii::IsLower('A'));
    ASSERT_FALSE(Ascii::IsLower('Z'));
    ASSERT_FALSE(Ascii::IsLower('0'));
    ASSERT_FALSE(Ascii::IsLower('#'));
}

TEST(AsciiTests, IsSpace)
{
    // Positive
    ASSERT_TRUE(Ascii::IsSpace(' '));
    ASSERT_TRUE(Ascii::IsSpace('\t'));
    ASSERT_TRUE(Ascii::IsSpace('\n'));
    ASSERT_TRUE(Ascii::IsSpace('\r'));
    ASSERT_TRUE(Ascii::IsSpace('\v'));
    ASSERT_TRUE(Ascii::IsSpace('\f'));

    // Negative
    ASSERT_FALSE(Ascii::IsSpace('$'));
    ASSERT_FALSE(Ascii::IsSpace('A'));
}

TEST(AsciiTests, IsTrueString)
{
    ASSERT_TRUE(Ascii::IsTrueString("True"sv));
    ASSERT_TRUE(Ascii::IsTrueString("true"sv));
    ASSERT_TRUE(Ascii::IsTrueString("yes"sv));
    ASSERT_TRUE(Ascii::IsTrueString("y"sv));
    ASSERT_TRUE(Ascii::IsTrueString("1"sv));
    ASSERT_FALSE(Ascii::IsTrueString("False"sv));
}

TEST(AsciiTests, IsUpper)
{
    // Positive
    ASSERT_TRUE(Ascii::IsUpper('A'));
    ASSERT_TRUE(Ascii::IsUpper('B'));
    ASSERT_TRUE(Ascii::IsUpper('Y'));
    ASSERT_TRUE(Ascii::IsUpper('Z'));

    // Negative
    ASSERT_FALSE(Ascii::IsUpper('a'));
    ASSERT_FALSE(Ascii::IsUpper('z'));
    ASSERT_FALSE(Ascii::IsUpper('0'));
    ASSERT_FALSE(Ascii::IsUpper('#'));
}

TEST(AsciiTests, ToDigit)
{
    // Positive
    ASSERT_EQ(Ascii::ToDigit('0'), 0);
    ASSERT_EQ(Ascii::ToDigit('1'), 1);
    ASSERT_EQ(Ascii::ToDigit('2'), 2);
    ASSERT_EQ(Ascii::ToDigit('3'), 3);
    ASSERT_EQ(Ascii::ToDigit('4'), 4);
    ASSERT_EQ(Ascii::ToDigit('5'), 5);
    ASSERT_EQ(Ascii::ToDigit('6'), 6);
    ASSERT_EQ(Ascii::ToDigit('7'), 7);
    ASSERT_EQ(Ascii::ToDigit('8'), 8);
    ASSERT_EQ(Ascii::ToDigit('9'), 9);

    // Negative
    ASSERT_EQ(Ascii::ToDigit('#'), 0);
    ASSERT_EQ(Ascii::ToDigit('A'), 0);
    ASSERT_EQ(Ascii::ToDigit('z'), 0);
}

TEST(AsciiTests, ToHexByte)
{
    ASSERT_EQ(Ascii::ToHexByte(1, 2), 0x12);
    ASSERT_EQ(Ascii::ToHexByte(3, 4), 0x34);
    ASSERT_EQ(Ascii::ToHexByte(5, 6), 0x56);
    ASSERT_EQ(Ascii::ToHexByte(7, 8), 0x78);
    ASSERT_EQ(Ascii::ToHexByte(9, 0xA), 0x9a);
    ASSERT_EQ(Ascii::ToHexByte(0xB, 0xC), 0xBC);
    ASSERT_EQ(Ascii::ToHexByte(0xD, 0xE), 0xDE);
    ASSERT_EQ(Ascii::ToHexByte(0xF, 0xF), 0xFF);
}

TEST(AsciiTests, BytesToHexString)
{
    constexpr auto data = std::to_array<uint8_t>({
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab,
        0xcd, 0xef, 0x01, 0x23, 0x45, 0x67,
        0x89, 0xab, 0xcd, 0xef });

    std::array<char, 128> buffer = { 0 };

    // Positive
    ASSERT_EQ(Ascii::ToHexString(
        data.data(),
        data.size(),
        buffer.data(),
        buffer.size()), "0123456789abcdef0123456789abcdef"sv);

    ASSERT_EQ(Ascii::ToHexString(
        data.data(),
        data.size(),
        buffer.data(),
        8), "012345"sv);

    // Negative
    ASSERT_EQ(Ascii::ToHexString(
        nullptr,
        data.size(),
        buffer.data(),
        0), ""sv);

    ASSERT_EQ(Ascii::ToHexString(
        data.data(),
        0,
        buffer.data(),
        buffer.size()), ""sv);

    ASSERT_EQ(Ascii::ToHexString(
        data.data(),
        data.size(),
        buffer.data(),
        0), ""sv);
}

TEST(AsciiTests, NumberToHexString)
{
    std::array<char, 128> buffer = { 0 };
    ASSERT_EQ(Ascii::ToHexString(
        9000,
        buffer.data(),
        buffer.size()), "2328"sv);

    ASSERT_EQ(Ascii::ToHexString(
        3735928559,
        buffer.data(),
        buffer.size()), "deadbeef"sv);

    ASSERT_EQ(Ascii::ToHexString(
        9000,
        nullptr,
        buffer.size()), ""sv);

    ASSERT_EQ(Ascii::ToHexString(
        9000,
        buffer.data(),
        0), ""sv);

    ASSERT_EQ(Ascii::ToHexString(
        0,
        buffer.data(),
        buffer.size()), "0"sv);
}

TEST(AsciiTests, ToLower)
{
    ASSERT_EQ(Ascii::ToLower('A'), 'a');
    ASSERT_EQ(Ascii::ToLower('B'), 'b');
    ASSERT_EQ(Ascii::ToLower('Y'), 'y');
    ASSERT_EQ(Ascii::ToLower('Z'), 'z');
    ASSERT_EQ(Ascii::ToLower('a'), 'a');
    ASSERT_EQ(Ascii::ToLower('z'), 'z');
    ASSERT_EQ(Ascii::ToLower('0'), '0');
    ASSERT_EQ(Ascii::ToLower('#'), '#');
}

TEST(AsciiTests, ToOctalString)
{
    std::array<char, 128> buffer = { 0 };
    ASSERT_EQ(Ascii::ToOctalString(
        3735928559,
        buffer.data(),
        buffer.size()), "33653337357"sv);

    ASSERT_EQ(Ascii::ToOctalString(
        3735928559,
        nullptr,
        buffer.size()), ""sv);

    ASSERT_EQ(Ascii::ToOctalString(
        3735928559,
        buffer.data(),
        0), ""sv);

    ASSERT_EQ(Ascii::ToOctalString(
        0,
        buffer.data(),
        buffer.size()), "0"sv);
}

TEST(AsciiTests, ToNibble)
{
    // Positive
    ASSERT_EQ(Ascii::ToNibble('a'), 0xA);
    ASSERT_EQ(Ascii::ToNibble('0'), 0x0);
    ASSERT_EQ(Ascii::ToNibble('9'), 0x9);
    ASSERT_EQ(Ascii::ToNibble('F'), 0xF);

    // Negative
    ASSERT_FALSE(Ascii::ToNibble('G'));
    ASSERT_FALSE(Ascii::ToNibble('#'));
}

TEST(AsciiTests, ToUpper)
{
    ASSERT_EQ(Ascii::ToUpper('a'), 'A');
    ASSERT_EQ(Ascii::ToUpper('b'), 'B');
    ASSERT_EQ(Ascii::ToUpper('y'), 'Y');
    ASSERT_EQ(Ascii::ToUpper('z'), 'Z');
    ASSERT_EQ(Ascii::ToUpper('A'), 'A');
    ASSERT_EQ(Ascii::ToUpper('Z'), 'Z');
    ASSERT_EQ(Ascii::ToUpper('0'), '0');
    ASSERT_EQ(Ascii::ToUpper('#'), '#');
}
