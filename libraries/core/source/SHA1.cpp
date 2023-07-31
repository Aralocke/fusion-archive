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

#include <Fusion/Hash.h>

#include <Fusion/Endian.h>
#include <Fusion/MemoryUtil.h>

 // Implementation based on:
 // https://github.com/stbrumme/hash-library/blob/master/sha1.cpp
 // Original implementer:
 // Stephan Brumme (http://create.stephan-brumme.com)

namespace
{
constexpr uint32_t f1(uint32_t b, uint32_t c, uint32_t d)
{
    return d ^ (b & (c ^ d)); // original: f = (b & c) | ((~b) & d);
}

constexpr uint32_t f2(uint32_t b, uint32_t c, uint32_t d)
{
    return b ^ c ^ d;
}

constexpr uint32_t f3(uint32_t b, uint32_t c, uint32_t d)
{
    return (b & c) | (b & d) | (c & d);
}

constexpr uint32_t rotate(uint32_t a, uint32_t c)
{
    return (a << c) | (a >> (32 - c));
}

constexpr uint32_t Swap(uint32_t x)
{
    return Fusion::Endian::ConstSwap(x);
}
}

namespace Fusion
{
template<>
void MDHash<MdHashType::Sha1>::ProcessBlock(
    std::span<const uint8_t> input)
{
    // get last hash
    uint32_t a = m_digest[0],
        b = m_digest[1],
        c = m_digest[2],
        d = m_digest[3],
        e = m_digest[4];

    const auto* input32 = reinterpret_cast<const uint32_t*>(input.data());

    // convert to big endian
    std::array<uint32_t, 80U> words = { 0 };

    for (size_t i = 0; i < 16; i++)
    {
        words[i] = Swap(input32[i]);
    }

    // extend to 80 words
    for (size_t i = 16; i < 80; i++)
    {
        words[i] = rotate(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16], 1);
    }

    // first round
    for (size_t i = 0; i < 4; i++)
    {
        size_t offset = 5 * i;
        e += rotate(a, 5) + f1(b, c, d) + words[offset] + 0x5a827999; b = rotate(b, 30);
        d += rotate(e, 5) + f1(a, b, c) + words[offset + 1] + 0x5a827999; a = rotate(a, 30);
        c += rotate(d, 5) + f1(e, a, b) + words[offset + 2] + 0x5a827999; e = rotate(e, 30);
        b += rotate(c, 5) + f1(d, e, a) + words[offset + 3] + 0x5a827999; d = rotate(d, 30);
        a += rotate(b, 5) + f1(c, d, e) + words[offset + 4] + 0x5a827999; c = rotate(c, 30);
    }

    // second round
    for (size_t i = 4; i < 8; i++)
    {
        size_t offset = 5 * i;
        e += rotate(a, 5) + f2(b, c, d) + words[offset] + 0x6ed9eba1; b = rotate(b, 30);
        d += rotate(e, 5) + f2(a, b, c) + words[offset + 1] + 0x6ed9eba1; a = rotate(a, 30);
        c += rotate(d, 5) + f2(e, a, b) + words[offset + 2] + 0x6ed9eba1; e = rotate(e, 30);
        b += rotate(c, 5) + f2(d, e, a) + words[offset + 3] + 0x6ed9eba1; d = rotate(d, 30);
        a += rotate(b, 5) + f2(c, d, e) + words[offset + 4] + 0x6ed9eba1; c = rotate(c, 30);
    }

    // third round
    for (size_t i = 8; i < 12; i++)
    {
        size_t offset = 5 * i;
        e += rotate(a, 5) + f3(b, c, d) + words[offset] + 0x8f1bbcdc; b = rotate(b, 30);
        d += rotate(e, 5) + f3(a, b, c) + words[offset + 1] + 0x8f1bbcdc; a = rotate(a, 30);
        c += rotate(d, 5) + f3(e, a, b) + words[offset + 2] + 0x8f1bbcdc; e = rotate(e, 30);
        b += rotate(c, 5) + f3(d, e, a) + words[offset + 3] + 0x8f1bbcdc; d = rotate(d, 30);
        a += rotate(b, 5) + f3(c, d, e) + words[offset + 4] + 0x8f1bbcdc; c = rotate(c, 30);
    }

    // fourth round
    for (size_t i = 12; i < 16; i++)
    {
        size_t offset = 5 * i;
        e += rotate(a, 5) + f2(b, c, d) + words[offset] + 0xca62c1d6; b = rotate(b, 30);
        d += rotate(e, 5) + f2(a, b, c) + words[offset + 1] + 0xca62c1d6; a = rotate(a, 30);
        c += rotate(d, 5) + f2(e, a, b) + words[offset + 2] + 0xca62c1d6; e = rotate(e, 30);
        b += rotate(c, 5) + f2(d, e, a) + words[offset + 3] + 0xca62c1d6; d = rotate(d, 30);
        a += rotate(b, 5) + f2(c, d, e) + words[offset + 4] + 0xca62c1d6; c = rotate(c, 30);
    }

    // update hash
    m_digest[0] += a;
    m_digest[1] += b;
    m_digest[2] += c;
    m_digest[3] += d;
    m_digest[4] += e;
}

template<>
void MDHash<MdHashType::Sha1>::ProcessBuffer()
{
    // Process the final block in the buffer, this assumes <64 bytes

    size_t padding = m_bufferSize * 8U;  // number of bits
    padding++;  // always append 1 bit

    // Number of bits must be (numBits % 512) = 448
    size_t lower11Bits = padding & 511;
    if (lower11Bits <= 448)
    {
        padding += 448 - lower11Bits;
    }
    else
    {
        padding += 512 + 448 - lower11Bits;
    }
    padding /= 8U;  // convert back to bytes;

    // Used for adding additional data
    BufferStorage extra = { 0 };

    if (m_bufferSize < m_buffer.size())
    {
        // Append a "1" bit, 128 => binary 10000000
        m_buffer[m_bufferSize] = 128;
    }
    else
    {
        extra[0] = 128;
    }

    size_t i = 0;
    for (i = m_bufferSize + 1; i < m_buffer.size(); i++)
    {
        m_buffer[i] = 0;
    }

    // add message length in bits as 64 bit number
    uint64_t msgBits = 8 * (m_size + m_bufferSize);
    // find right position
    uint8_t* addLength = nullptr;
    if (padding < m_buffer.size())
    {
        addLength = m_buffer.data() + padding;
    }
    else
    {
        addLength = extra.data() + padding - m_buffer.size();
    }

    // must be big endian
    *addLength++ = static_cast<uint8_t>((msgBits >> 56) & 0xFF);
    *addLength++ = static_cast<uint8_t>((msgBits >> 48) & 0xFF);
    *addLength++ = static_cast<uint8_t>((msgBits >> 40) & 0xFF);
    *addLength++ = static_cast<uint8_t>((msgBits >> 32) & 0xFF);
    *addLength++ = static_cast<uint8_t>((msgBits >> 24) & 0xFF);
    *addLength++ = static_cast<uint8_t>((msgBits >> 16) & 0xFF);
    *addLength++ = static_cast<uint8_t>((msgBits >> 8) & 0xFF);
    *addLength = static_cast<uint8_t>(msgBits & 0xFF);

    // Process blocks
    ProcessBlock(m_buffer);

    // flowed over into a second block ?
    if (padding > m_buffer.size())
    {
        ProcessBlock(extra);
    }
}

template<>
void MDHash<MdHashType::Sha1>::Finish(
    std::span<uint8_t> digest)
{
    Digest output;

    DigestStorage old = { 0 };

    // Save the old hash if the buffer still has remaining data
    for (size_t i = 0; i < m_digest.size(); ++i)
    {
        old[i] = m_digest[i];
    }

    // Process the remaining bytes
    ProcessBuffer();

    uint8_t* p = output.data;
    for (size_t i = 0; i < m_digest.size(); i++)
    {
        *p++ = (m_digest[i] >> 24) & 0xFF;
        *p++ = (m_digest[i] >> 16) & 0xFF;
        *p++ = (m_digest[i] >> 8) & 0xFF;
        *p++ = m_digest[i] & 0xFF;

        // restore old hash
        m_digest[i] = old[i];
    }

    // Finalize the output into the users buffer capped at whatever
    // size we have been given.
    memcpy(
        digest.data(),
        output.data,
        std::min<size_t>(digest.size(), Digest::SIZE));
}

template<>
void MDHash<MdHashType::Sha1>::Reset()
{
    m_buffer.fill(0);
    m_digest.fill(0);

    m_size = 0;
    m_bufferSize = 0;

    m_digest[0] = 0x67452301;
    m_digest[1] = 0xefcdab89;
    m_digest[2] = 0x98badcfe;
    m_digest[3] = 0x10325476;
    m_digest[4] = 0xc3d2e1f0;
}
}  // namespace fusion
