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

#include <Fusion/Endian.h>
#include <Fusion/MemoryUtil.h>

// Implementation based on:
// https://github.com/stbrumme/hash-library/blob/master/md5.cpp
// Original implementer:
// Stephan Brumme (http://create.stephan-brumme.com)

namespace
{
constexpr uint32_t f1(uint32_t b, uint32_t c, uint32_t d)
{
    // original: f = (b & c) | ((~b) & d);
    return d ^ (b & (c ^ d));
}

constexpr uint32_t f2(uint32_t b, uint32_t c, uint32_t d)
{
    // original: f = (b & d) | (c & (~d));
    return c ^ (d & (b ^ c));
}

constexpr uint32_t f3(uint32_t b, uint32_t c, uint32_t d)
{
    return b ^ c ^ d;
}

constexpr uint32_t f4(uint32_t b, uint32_t c, uint32_t d)
{
    return c ^ (b | ~d);
}

constexpr uint32_t rotate(uint32_t a, uint32_t c)
{
    return (a << c) | (a >> (32 - c));
}

#if FUSION_BIG_ENDIAN
constexpr uint32_t Swap(uint32_t x) { return Fusion::Endian::ConstSwap(x); }
#else
constexpr uint32_t Swap(uint32_t x) { return x; }
#endif
}

namespace Fusion
{
template<>
void MDHash<MdHashType::Md5>::ProcessBlock(
    std::span<const uint8_t> input)
{
    // Capture the current hash values
    uint32_t a = m_digest[0];
    uint32_t b = m_digest[1];
    uint32_t c = m_digest[2];
    uint32_t d = m_digest[3];

    const auto* input32 = reinterpret_cast<const uint32_t*>(input.data());

    // first round
    uint32_t word0 = Swap(input32[0]);
    a = rotate(a + f1(b, c, d) + word0 + 0xd76aa478, 7) + b;
    uint32_t word1 = Swap(input32[1]);
    d = rotate(d + f1(a, b, c) + word1 + 0xe8c7b756, 12) + a;
    uint32_t word2 = Swap(input32[2]);
    c = rotate(c + f1(d, a, b) + word2 + 0x242070db, 17) + d;
    uint32_t word3 = Swap(input32[3]);
    b = rotate(b + f1(c, d, a) + word3 + 0xc1bdceee, 22) + c;

    uint32_t word4 = Swap(input32[4]);
    a = rotate(a + f1(b, c, d) + word4 + 0xf57c0faf, 7) + b;
    uint32_t word5 = Swap(input32[5]);
    d = rotate(d + f1(a, b, c) + word5 + 0x4787c62a, 12) + a;
    uint32_t word6 = Swap(input32[6]);
    c = rotate(c + f1(d, a, b) + word6 + 0xa8304613, 17) + d;
    uint32_t word7 = Swap(input32[7]);
    b = rotate(b + f1(c, d, a) + word7 + 0xfd469501, 22) + c;

    uint32_t word8 = Swap(input32[8]);
    a = rotate(a + f1(b, c, d) + word8 + 0x698098d8, 7) + b;
    uint32_t word9 = Swap(input32[9]);
    d = rotate(d + f1(a, b, c) + word9 + 0x8b44f7af, 12) + a;
    uint32_t word10 = Swap(input32[10]);
    c = rotate(c + f1(d, a, b) + word10 + 0xffff5bb1, 17) + d;
    uint32_t word11 = Swap(input32[11]);
    b = rotate(b + f1(c, d, a) + word11 + 0x895cd7be, 22) + c;

    uint32_t word12 = Swap(input32[12]);
    a = rotate(a + f1(b, c, d) + word12 + 0x6b901122, 7) + b;
    uint32_t word13 = Swap(input32[13]);
    d = rotate(d + f1(a, b, c) + word13 + 0xfd987193, 12) + a;
    uint32_t word14 = Swap(input32[14]);
    c = rotate(c + f1(d, a, b) + word14 + 0xa679438e, 17) + d;
    uint32_t word15 = Swap(input32[15]);
    b = rotate(b + f1(c, d, a) + word15 + 0x49b40821, 22) + c;

    // second round
    a = rotate(a + f2(b, c, d) + word1 + 0xf61e2562, 5) + b;
    d = rotate(d + f2(a, b, c) + word6 + 0xc040b340, 9) + a;
    c = rotate(c + f2(d, a, b) + word11 + 0x265e5a51, 14) + d;
    b = rotate(b + f2(c, d, a) + word0 + 0xe9b6c7aa, 20) + c;

    a = rotate(a + f2(b, c, d) + word5 + 0xd62f105d, 5) + b;
    d = rotate(d + f2(a, b, c) + word10 + 0x02441453, 9) + a;
    c = rotate(c + f2(d, a, b) + word15 + 0xd8a1e681, 14) + d;
    b = rotate(b + f2(c, d, a) + word4 + 0xe7d3fbc8, 20) + c;

    a = rotate(a + f2(b, c, d) + word9 + 0x21e1cde6, 5) + b;
    d = rotate(d + f2(a, b, c) + word14 + 0xc33707d6, 9) + a;
    c = rotate(c + f2(d, a, b) + word3 + 0xf4d50d87, 14) + d;
    b = rotate(b + f2(c, d, a) + word8 + 0x455a14ed, 20) + c;

    a = rotate(a + f2(b, c, d) + word13 + 0xa9e3e905, 5) + b;
    d = rotate(d + f2(a, b, c) + word2 + 0xfcefa3f8, 9) + a;
    c = rotate(c + f2(d, a, b) + word7 + 0x676f02d9, 14) + d;
    b = rotate(b + f2(c, d, a) + word12 + 0x8d2a4c8a, 20) + c;

    // third round
    a = rotate(a + f3(b, c, d) + word5 + 0xfffa3942, 4) + b;
    d = rotate(d + f3(a, b, c) + word8 + 0x8771f681, 11) + a;
    c = rotate(c + f3(d, a, b) + word11 + 0x6d9d6122, 16) + d;
    b = rotate(b + f3(c, d, a) + word14 + 0xfde5380c, 23) + c;

    a = rotate(a + f3(b, c, d) + word1 + 0xa4beea44, 4) + b;
    d = rotate(d + f3(a, b, c) + word4 + 0x4bdecfa9, 11) + a;
    c = rotate(c + f3(d, a, b) + word7 + 0xf6bb4b60, 16) + d;
    b = rotate(b + f3(c, d, a) + word10 + 0xbebfbc70, 23) + c;

    a = rotate(a + f3(b, c, d) + word13 + 0x289b7ec6, 4) + b;
    d = rotate(d + f3(a, b, c) + word0 + 0xeaa127fa, 11) + a;
    c = rotate(c + f3(d, a, b) + word3 + 0xd4ef3085, 16) + d;
    b = rotate(b + f3(c, d, a) + word6 + 0x04881d05, 23) + c;

    a = rotate(a + f3(b, c, d) + word9 + 0xd9d4d039, 4) + b;
    d = rotate(d + f3(a, b, c) + word12 + 0xe6db99e5, 11) + a;
    c = rotate(c + f3(d, a, b) + word15 + 0x1fa27cf8, 16) + d;
    b = rotate(b + f3(c, d, a) + word2 + 0xc4ac5665, 23) + c;

    // fourth round
    a = rotate(a + f4(b, c, d) + word0 + 0xf4292244, 6) + b;
    d = rotate(d + f4(a, b, c) + word7 + 0x432aff97, 10) + a;
    c = rotate(c + f4(d, a, b) + word14 + 0xab9423a7, 15) + d;
    b = rotate(b + f4(c, d, a) + word5 + 0xfc93a039, 21) + c;

    a = rotate(a + f4(b, c, d) + word12 + 0x655b59c3, 6) + b;
    d = rotate(d + f4(a, b, c) + word3 + 0x8f0ccc92, 10) + a;
    c = rotate(c + f4(d, a, b) + word10 + 0xffeff47d, 15) + d;
    b = rotate(b + f4(c, d, a) + word1 + 0x85845dd1, 21) + c;

    a = rotate(a + f4(b, c, d) + word8 + 0x6fa87e4f, 6) + b;
    d = rotate(d + f4(a, b, c) + word15 + 0xfe2ce6e0, 10) + a;
    c = rotate(c + f4(d, a, b) + word6 + 0xa3014314, 15) + d;
    b = rotate(b + f4(c, d, a) + word13 + 0x4e0811a1, 21) + c;

    a = rotate(a + f4(b, c, d) + word4 + 0xf7537e82, 6) + b;
    d = rotate(d + f4(a, b, c) + word11 + 0xbd3af235, 10) + a;
    c = rotate(c + f4(d, a, b) + word2 + 0x2ad7d2bb, 15) + d;
    b = rotate(b + f4(c, d, a) + word9 + 0xeb86d391, 21) + c;

    // update hash
    m_digest[0] += a;
    m_digest[1] += b;
    m_digest[2] += c;
    m_digest[3] += d;
}

template<>
void MDHash<MdHashType::Md5>::ProcessBuffer()
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

    // must be little endian
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF;

    // Process blocks
    ProcessBlock(m_buffer);

    // flowed over into a second block ?
    if (padding > m_buffer.size())
    {
        ProcessBlock(extra);
    }
}

template<>
void MDHash<MdHashType::Md5>::Finish(std::span<uint8_t> digest)
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

    uint8_t* p = output.Data();
    for (size_t i = 0; i < m_digest.size(); i++)
    {
        *p++ = m_digest[i] & 0xFF;
        *p++ = (m_digest[i] >> 8) & 0xFF;
        *p++ = (m_digest[i] >> 16) & 0xFF;
        *p++ = (m_digest[i] >> 24) & 0xFF;

        // restore old hash
        m_digest[i] = old[i];
    }

    // Finalize the output into the users buffer capped at whatever
    // size we have been given.
    memcpy(
        digest.data(),
        output.Data(),
        std::min<size_t>(digest.size(), Digest::SIZE));
}

template<>
void MDHash<MdHashType::Md5>::Reset()
{
    m_buffer.fill(0);
    m_digest.fill(0);

    m_size = 0;
    m_bufferSize = 0;

    m_digest[0] = 0x67452301;
    m_digest[1] = 0xefcdab89;
    m_digest[2] = 0x98badcfe;
    m_digest[3] = 0x10325476;
}
}  // namespace Fusion
