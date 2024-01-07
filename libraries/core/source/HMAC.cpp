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

#include <Fusion/Hash.h>

namespace Fusion::Internal
{
template<typename Method>
static void HMAC(
    std::span<const uint8_t> key,
    std::span<const uint8_t> data,
    std::span<uint8_t> output)
{
    using Digest = typename Method::Digest;

    FUSION_ASSERT(output.size() >= Digest::SIZE);

    std::array<uint8_t, 64U> hmac = { 0 };
    std::span<uint8_t> span{ hmac.data(), Digest::SIZE };

    if (key.size() <= hmac.size())
    {
        memcpy(hmac.data(), key.data(), key.size());
    }
    else
    {
        Method m;
        m.Process(key);
        m.Finish(span);
    }

    for (uint8_t& h : hmac)
    {
        h ^= 0x36;
    }

    Digest digest;
    {
        Method m;
        m.Process(hmac);
        m.Process(data);
        m.Finish(digest);
    }

    for (uint8_t& h : hmac)
    {
        h ^= 0x5C ^ 0x36;
    }

    Method m;
    m.Process(hmac);
    m.Process(digest);
    m.Finish(output);
}
}

namespace Fusion
{
void HMAC(
    std::span<const uint8_t> key,
    std::span<const uint8_t> data,
    std::span<uint8_t> output,
    MdHashType type)
{
    switch (type)
    {
    case MdHashType::Md5:
        Internal::HMAC<MD5>(key, data, output);
        break;
    case MdHashType::Sha1:
        Internal::HMAC<SHA1>(key, data, output);
        break;
    case MdHashType::Sha256:
        Internal::HMAC<SHA256>(key, data, output);
        break;
    default:
        break;
    }
}
}
