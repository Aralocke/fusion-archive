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

#include <Fusion/Memory.h>
#include <Fusion/New.h>

TEST(MemoryTests, UnsignedNarrowCast)
{
    using namespace Fusion;

    uint16_t shortValue;
#if FUSION_ASSERTIONS_ENABLED
    ASSERT_DEATH(shortValue = NarrowCast<uint16_t>(123456789),
        "narrowing of input creates invalid value");
#else
    shortValue = NarrowCast<uint16_t>(123456789);
#endif

    FUSION_UNUSED(shortValue);
}

TEST(MemoryTests, SignedNarrowCast)
{
    using namespace Fusion;

    int16_t shortValue;
#if FUSION_ASSERTIONS_ENABLED
    ASSERT_DEATH(shortValue = NarrowCast<int16_t>(-123456789),
        "narrowing of input creates invalid value");
#else
    shortValue = NarrowCast<int16_t>(-123456789);
#endif

    FUSION_UNUSED(shortValue);
}

TEST(MemoryTests, TrivialConstructPlacementNew)
{
    using namespace Fusion;

    static uint8_t s_constructed;
    s_constructed = 0;

    struct Test
    {
        bool constructed;

        Test() : constructed(true) { ++s_constructed; }
        ~Test() { --s_constructed; }
    };

    alignas(Test) uint8_t buffer[sizeof(Test)] = { 0 };

    static_assert(sizeof(Test) <= sizeof(buffer),
        "Insufficient buffer size");

    Test* ptr = nullptr;

    FUSION_SCOPE_GUARD([&] {
        if (ptr) ptr->~Test();
    });

    ptr = FUSION_PLACEMENT_NEW(buffer) Test {};

    ASSERT_TRUE(ptr);
    ASSERT_TRUE(ptr->constructed);
    ASSERT_EQ(s_constructed, 1);

    ptr->~Test();
    ASSERT_EQ(s_constructed, 0);
}
