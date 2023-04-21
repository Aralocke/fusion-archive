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

#include <Fusion/Internal/Network.h>

TEST(InetAddressTests, Parsing)
{
    FUSION_ASSERT_RESULT(
        ParseAddress("255.255.255.255"),
        [](ParsedAddress address) {
            ASSERT_EQ(address.family, AddressFamily::Inet4);
        });

    FUSION_ASSERT_ERROR(
        ParseAddress("256.255.255.256"),
        E_INVALID_ARGUMENT);
    FUSION_ASSERT_ERROR(
        ParseAddress("this will fail"),
        E_INVALID_ARGUMENT);

    FUSION_ASSERT_RESULT(
        ParseAddress("0.0.0.0"),
        [](ParsedAddress address) {
            ASSERT_EQ(address.family, AddressFamily::Inet4);
        });
    FUSION_ASSERT_RESULT(
        ParseAddress("127.0.0.1"),
        [](ParsedAddress address) {
            ASSERT_EQ(address.family, AddressFamily::Inet4);
        });
}

TEST(InetAddressTests, FromStringValidation)
{
    InetAddress address;
    FUSION_ASSERT_RESULT(address.FromString("127.0.0.1"));
    FUSION_ASSERT_RESULT(address.FromString("0.0.0.0"));

    FUSION_ASSERT_FAILURE(address.FromString("256.256.256.256"));
    FUSION_ASSERT_FAILURE(address.FromString("this will fail"));
}

TEST(InetAddressTests, FromAddressTests)
{
    std::array<char, 128> buffer = { 0 };
    {
        buffer.fill(0);

        InetAddress address;
        ASSERT_TRUE(address.FromString("127.0.0.1"sv));
        ASSERT_EQ(ToString(
            address,
            buffer.data(),
            buffer.size()), "127.0.0.1"sv);
    }

    {
        buffer.fill(0);

        constexpr auto STR = "192.168.255.1"sv;

        InetAddress address;
        ASSERT_TRUE(address.FromString(STR));
        ASSERT_EQ(ToString(
            address,
            buffer.data(),
            buffer.size()), STR);
    }

    {
        constexpr auto STR = "::ffff:127.0.0.1"sv;
        buffer.fill(0);

        Inet6Address address;
        ASSERT_TRUE(address.FromString(STR));
        ASSERT_EQ(ToString(
            address,
            buffer.data(),
            buffer.size()), STR);
        ASSERT_EQ(address, InaddrLoopback6in4);
        ASSERT_TRUE(address.IsMappedV4());
        ASSERT_TRUE(address.AsV4() == InaddrLoopback);
    }

    {
        InetAddress address;
        ASSERT_FALSE(address.FromString("fail"sv));
    }
}

TEST(InetAddressTests, FromDecimal)
{
    std::array<char, 128> buffer = { 0 };
    {
        buffer.fill(0);

        constexpr auto STR = "192.168.1.1"sv;

        InetAddress address;
        ASSERT_TRUE(address.FromDecimal(3232235777));
        ASSERT_EQ(address.ToDecimal(), 3232235777);
        ASSERT_EQ(ToString(
            address,
            buffer.data(),
            buffer.size()), STR);
    }

    {
        buffer.fill(0);

        constexpr auto STR = "127.0.0.1"sv;

        InetAddress address;
        ASSERT_TRUE(address.FromDecimal(2130706433));
        ASSERT_EQ(address.ToDecimal(), 2130706433);
        ASSERT_EQ(ToString(
            address,
            buffer.data(),
            buffer.size()), STR);
    }
}

TEST(InetAddressTests, V4toV6StateChanging)
{
    ASSERT_TRUE(InaddrLoopback6in4.IsMappedV4());
    ASSERT_TRUE(InaddrLoopback6in4.AsV4() == InaddrLoopback);
    ASSERT_TRUE(InaddrLoopback.AsV6() == InaddrLoopback6in4);
}
