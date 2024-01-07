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

#include <Fusion/Internal/Network.h>

TEST(Inet6AddressTests, Parsing)
{
    FUSION_ASSERT_ERROR(
        ParseAddress("dead::beef::1"),
        E_INVALID_ARGUMENT);
    FUSION_ASSERT_ERROR(
        ParseAddress("this::will:fail::1"),
        E_INVALID_ARGUMENT);

    FUSION_ASSERT_RESULT(
        ParseAddress("::0"),
        [](ParsedAddress address) {
            ASSERT_EQ(address.family, AddressFamily::Inet6);
        });
    FUSION_ASSERT_RESULT(
        ParseAddress("::1"),
        [](ParsedAddress address) {
            ASSERT_EQ(address.family, AddressFamily::Inet6);
        });
}

TEST(Inet6AddressTests, FromStringValidation)
{
    Inet6Address address;
    FUSION_ASSERT_RESULT(address.FromString("2000::1"));
    FUSION_ASSERT_RESULT(address.FromString("::0"));

    FUSION_ASSERT_FAILURE(address.FromString("256.256.256.256"));
    FUSION_ASSERT_FAILURE(address.FromString("this will fail"));

    FUSION_ASSERT_FAILURE(address.FromString("this:bad:app:le::1"));
    FUSION_ASSERT_FAILURE(address.FromString("::gggg"));
}

TEST(Inet6AddressTests, FromAddressTests)
{
    std::array<char, 128> buffer = { 0 };
    {
        constexpr auto STR = "2001::1"sv;
        buffer.fill(0);

        Inet6Address address;
        ASSERT_TRUE(address.FromString(STR));
        ASSERT_EQ(ToString(
            address,
            buffer.data(),
            buffer.size()), STR);
    }

    {
        constexpr auto STR = "dead:beef:cafe::1"sv;

        Inet6Address address;
        ASSERT_TRUE(address.FromString(STR));
        ASSERT_EQ(ToString(
            address,
            buffer.data(),
            buffer.size()), STR);
    }
}
