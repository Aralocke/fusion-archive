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

#include <Fusion/Hex.h>

#include <array>

namespace
{
struct HexTest
{
    std::string_view str;
    std::array<uint8_t, 16U> hash;
};

constexpr auto HexData = std::to_array<HexTest>({
    {
        "0123456789abcdef0123456789abcdef",
        {
            0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
            0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
        },
    },
    {
        "abcdef9876543210fedcba9876543210",
        {
            0xab, 0xcd, 0xef, 0x98, 0x76, 0x54, 0x32, 0x10,
            0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x21
        },
    },
});
}

TEST(HexTests, Comparisons)
{
    const auto& testA = HexData[0];
    const auto& testB = HexData[1];

    Hex<16> hexA = Hex<16>::FromString(testA.str);
    Hex<16> hexA_B(testA.hash.data(), testA.hash.size());

    {
        ASSERT_EQ(hexA, hexA_B);
    }

    Hex<16> hexB = Hex<16>::FromString(testB.str);

    {
        ASSERT_NE(hexA, hexB);
        ASSERT_LT(hexA, hexB);
    }
}

TEST(HexTests, ToString)
{
    const auto& test = HexData[0];

    Hex<16> hex = Hex<16>::FromString(test.str);
    std::string hexstr;

    {
        std::ostringstream o;
        o << hex;
        hexstr = o.str();
    }

    ASSERT_EQ(hexstr, test.str);
}

TEST(HexTests, ToStringFormatters)
{
    const auto& test = HexData[0];

    Hex<16> hex = Hex<16>::FromString(test.str);
    std::string hexstr = fmt::format("{}", hex);

    ASSERT_EQ(hexstr, test.str);
}
