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

#include <Fusion/Ascii.h>
#include <Fusion/Hash.h>

namespace Fusion
{
void HashingTests::CheckHash(
    std::span<const uint8_t> input,
    std::string_view hash)
{
    ASSERT_EQ(
        Ascii::ToHexString(
            input.data(),
            input.size()),
        hash);
}

std::span<const HashingTests::HashTest>
HashingTests::Tests() const
{
    return HashData;
}
}

TEST_F(HashingTests, Md5)
{
    RunHashTest<MD5>(
        [&](MD5::Digest& digest, const HashTest& test)
        {
            CheckHash(digest, test.md5);
        });
}

TEST_F(HashingTests, Sha1)
{
    RunHashTest<SHA1>(
        [&](SHA1::Digest& digest, const HashTest& test)
        {
            CheckHash(digest, test.sha1);
        });
}

TEST_F(HashingTests, Sha256)
{
    RunHashTest<SHA256>(
        [&](SHA256::Digest& digest, const HashTest& test)
        {
            CheckHash(digest, test.sha256);
        });
}
