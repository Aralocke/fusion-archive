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

#include <Fusion/Hash.h>

#include <array>

struct XxhashTest
{
    uint32_t xxhash32 = 0;
    uint64_t xxhash64 = 0;

    const char* data = nullptr;
};

constexpr auto XxhashData = std::to_array<XxhashTest>({
    {
        0x02cc5d05,
        0xef46db3751d8e999,
        ""
    },
    {
        0x550d7456,
        0xd24ec4f1a98c6e5b,
        "a"
    },
    {
        0x32d153ff,
        0x44bc2cf5ad770999,
        "abc"
    },
    {
        0x63a14d5f,
        0xcfe1f278fa89835c,
        "abcdefghijklmnopqrstuvwxyz"
    },
    {
        0xe134383d,
        0x00f8636801b5422c,
        "f844e243"
    },
    {
        0xaa7ddb9f,
        0x53713ec21c330366,
        "fb3d51b378d2dbe6"
    },
    {
        0x8efd8c26,
        0x2455ca3fff494fd7,
        "e1aa29dc0ab618ca357e8cec9e3cc69f"
    },
    {
        0x0256e0a1,
        0xa88e91fc013c7e5e,
        "Shields up. I recommend we transfer power to phasers and arm the photon "
        "torpedoes."
    },
    {
        0x828e2bfa,
        0xa8b11617c826f8c6,
        "Damage report? Sections 27, 28 and 29 on decks four, five and six destroyed. "
        "Without our shields, at this range it is probable a photon detonation could "
        "destroy the Enterprise."
    },
    {
        0xe2b5ecd0,
        0xaac387cce08df321,
        "We're acquainted with the wormhole phenomenon, but this... Is a remarkable "
        "piece of bio-electronic engineering by which I see much of the EM spectrum "
        "ranging from heat and infrared through radio waves, et cetera, and forgive "
        "me if I've said and listened to this a thousand times. This planet's interior "
        "heat provides an abundance of geothermal energy. We need to neutralize the "
        "homing signal. "
        "Now what are the possibilities of warp drive ? Cmdr Riker's nervous system "
        "has been invaded by an unknown microorganism. The organisms fuse to the nerve, "
        "intertwining at the molecular level. That's why the transporter's biofilters "
        "couldn't extract it.The vertex waves show a K - complex corresponding to an "
        "REM state.The engineering section's critical. Destruction is imminent. Their "
        "robes contain ultritium, highly explosive, virtually undetectable by your "
        "transporter."
    }
});

TEST(XxhashTests, xxhash32)
{
    for (const XxhashTest& test : XxhashData)
    {
        ASSERT_EQ(XXHASH32::Hash(
            test.data,
            strlen(test.data)), test.xxhash32);
    }
}

TEST(XxhashTests, xxhash64)
{
    for (const XxhashTest& test : XxhashData)
    {
        ASSERT_EQ(XXHASH64::Hash(
            test.data,
            strlen(test.data)), test.xxhash64);
    }
}
