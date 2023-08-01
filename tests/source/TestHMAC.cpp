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

#include <Fusion/Fixtures/Hashing.h>

#include <Fusion/Hash.h>

namespace Fusion
{
std::span<const HashingTests::HmacTest>
    HashingTests::HmacTests() const
{
    return HmacData;
}
}

TEST_F(HashingTests, HmacMD5)
{
    RunHmacTest<MD5>(
        [&](MD5::Digest& digest, const HmacTest& test)
        {
            CheckHash(digest, test.md5);
        });
}

TEST_F(HashingTests, HmacSHA1)
{
    RunHmacTest<SHA1>(
        [&](SHA1::Digest& digest, const HmacTest& test)
        {
            CheckHash(digest, test.sha1);
        });
}

TEST_F(HashingTests, HmacSHA256)
{
    RunHmacTest<SHA256>(
        [&](SHA256::Digest& digest, const HmacTest& test)
        {
            CheckHash(digest, test.sha256);
        });
}
