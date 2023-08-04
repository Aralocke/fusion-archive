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

#include <Fusion/Uuid.h>

#include <algorithm>
#include <vector>

TEST(UuidTests, Initialize)
{
    UUID uuidA, uuidB;

    FUSION_ASSERT_RESULT(UUID::V4(),
        [&](UUID uuid)
        {
            uuidA = std::move(uuid);
        });
    FUSION_ASSERT_RESULT(UUID::V4(),
        [&](UUID uuid)
        {
            uuidB = std::move(uuid);
        });

    ASSERT_NE(uuidA, EmptyUuid);
    ASSERT_EQ(uuidA.Version(), 4U);

    ASSERT_NE(uuidB, EmptyUuid);
    ASSERT_EQ(uuidB.Version(), 4U);

    ASSERT_NE(uuidA, uuidB);
}

TEST(UuidTests, CopyAndMove)
{
    constexpr std::string_view STRINGA
        = "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa";

    UUID uuidA;
    {
        UUID uuidB;
        FUSION_ASSERT_RESULT(
            UUID::FromString(STRINGA),
            [&](UUID&& uuid)
            {
                uuidB = std::move(uuid);
            });

        uuidA = uuidB;
    }

    ASSERT_EQ(ToString(uuidA), STRINGA);
}

TEST(UuidTests, UUIDFromString)
{
    std::array<char, UUID::LENGTH> buffer = { 0 };
    constexpr std::string_view STRINGA
        = "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa";

    UUID uuidA;
    FUSION_ASSERT_RESULT(
        UUID::FromString(STRINGA),
        [&](UUID&& uuid)
        {
            uuidA = std::move(uuid);
        });

    ASSERT_EQ(ToString(buffer, uuidA), STRINGA);

    constexpr std::string_view STRINGB
        = "bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb";

    UUID uuidB;
    FUSION_ASSERT_RESULT(
        UUID::FromString(STRINGB),
        [&](UUID&& uuid)
        {
            uuidB = std::move(uuid);
        });

    ASSERT_EQ(ToString(buffer, uuidB), STRINGB);

    constexpr std::string_view STRINGZ
        = "zzzzzzzz-zzzz-zzzz-zzzz-zzzzzzzzzzzz";

    FUSION_ASSERT_FAILURE(UUID::FromString(STRINGZ));
}

TEST(UuidTests, Compare)
{
    constexpr std::string_view STRINGA
        = "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa";

    UUID uuidA;
    FUSION_ASSERT_RESULT(
        UUID::FromString(STRINGA),
        [&](UUID&& uuid)
        {
            uuidA = std::move(uuid);
        });

    constexpr std::string_view STRINGB
        = "bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb";

    UUID uuidB;
    FUSION_ASSERT_RESULT(
        UUID::FromString(STRINGB),
        [&](UUID&& uuid)
        {
            uuidB = std::move(uuid);
        });

    ASSERT_EQ(uuidA, uuidA);
    ASSERT_NE(uuidA, uuidB);
    ASSERT_TRUE(uuidA < uuidB);
    ASSERT_TRUE(uuidB > uuidA);
}

TEST(UuidTests, Sorting)
{
    constexpr std::string_view STRINGA
        = "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa";

    UUID uuidA;
    FUSION_ASSERT_RESULT(
        UUID::FromString(STRINGA),
        [&](UUID&& uuid)
        {
            uuidA = std::move(uuid);
        });

    constexpr std::string_view STRINGB
        = "bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb";

    UUID uuidB;
    FUSION_ASSERT_RESULT(
        UUID::FromString(STRINGB),
        [&](UUID&& uuid)
        {
            uuidB = std::move(uuid);
        });

    constexpr std::string_view STRINGC
        = "cccccccc-cccc-cccc-cccc-cccccccccccc";

    UUID uuidC;
    FUSION_ASSERT_RESULT(
        UUID::FromString(STRINGC),
        [&](UUID&& uuid)
        {
            uuidC = std::move(uuid);
        });

    constexpr std::string_view STRINGD
        = "dddddddd-dddd-dddd-dddd-dddddddddddd";

    UUID uuidD;
    FUSION_ASSERT_RESULT(
        UUID::FromString(STRINGD),
        [&](UUID&& uuid)
        {
            uuidD = std::move(uuid);
        });

    std::vector<UUID> v({ uuidB, uuidD, uuidA, uuidC });
    std::sort(v.begin(), v.end());

    ASSERT_TRUE(v[0] < v[1]);
    ASSERT_TRUE(v[1] < v[2]);
    ASSERT_TRUE(v[2] < v[3]);
}
