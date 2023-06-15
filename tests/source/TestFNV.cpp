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

struct FnvTest
{
    uint32_t fnv32 = 0;
    uint64_t fnv64 = 0;

    const char* data = nullptr;
};

constexpr auto Fnv1aData = std::to_array<FnvTest>({
    {
        0x811c9dc5U,
        0xcbf29ce484222325U,
        ""
    },
    {
        0xe40c292cU,
        0xaf63dc4c8601ec8cU,
        "a"
    },
    {
        0x1a47e90bU,
        0xe71fa2190541574bU,
        "abc"
    },
    {
        0xb0bc0c82U,
        0x8450deb1cdc382a2U,
        "abcdefghijklmnopqrstuvwxyz"
    },
    {
        0xfff67afdU,
        0x38444eea9fc3e47dU,
        "f844e243"
    },
    {
        0xe4eb490bU,
        0xa3ed0f93813dec2bU,
        "fb3d51b378d2dbe6"
    },
    {
        0xaf9be648U,
        0x11183d0d16f186e8U,
        "e1aa29dc0ab618ca357e8cec9e3cc69f"
    },
    {
        0xbeaabe41U,
        0x4078b546cde91a21U,
        "Shields up. I recommend we transfer power to phasers and arm the photon "
        "torpedoes."
    },
    {
        0xc0fa14f8U,
        0x21431fe3cf3494b8U,
        "Damage report? Sections 27, 28 and 29 on decks four, five and six destroyed. "
        "Without our shields, at this range it is probable a photon detonation could "
        "destroy the Enterprise."
    },
    {
        0xbb79a827U,
        0x82dc1b8879a103c7U,
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

TEST(FnvTests, Fnv1a32)
{
    for (const FnvTest& test : Fnv1aData)
    {
        {
            FNV1A32 fnv1a32;
            fnv1a32.String(test.data);
            ASSERT_EQ(fnv1a32.Done(), test.fnv32);
        }
        {
            FNV1A32 fnv1a32;
            fnv1a32.Data(test.data, strlen(test.data));
            ASSERT_EQ(fnv1a32.Done(), test.fnv32);
        }
    }
}

TEST(FnvTests, Fnv1a64)
{
    for (const FnvTest& test : Fnv1aData)
    {
        {
            FNV1A64 fnv1a64;
            fnv1a64.String(test.data);
            ASSERT_EQ(fnv1a64.Done(), test.fnv64);
        }
        {
            FNV1A64 fnv1a64;
            fnv1a64.Data(test.data, strlen(test.data));
            ASSERT_EQ(fnv1a64.Done(), test.fnv64);
        }
    }
}
