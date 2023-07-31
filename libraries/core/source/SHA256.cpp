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
 // https://github.com/stbrumme/hash-library/blob/master/sha256.cpp
 // Original implementer:
 // Stephan Brumme (http://create.stephan-brumme.com)

namespace
{
constexpr uint32_t rotate(uint32_t a, uint32_t c)
{
    return (a >> c) | (a << (32 - c));
}

constexpr uint32_t f1(uint32_t e, uint32_t f, uint32_t g)
{
    uint32_t term1 = rotate(e, 6) ^ rotate(e, 11) ^ rotate(e, 25);
    uint32_t term2 = (e & f) ^ (~e & g); //(g ^ (e & (f ^ g)))
    return term1 + term2;
}

constexpr uint32_t f2(uint32_t a, uint32_t b, uint32_t c)
{
    uint32_t term1 = rotate(a, 2) ^ rotate(a, 13) ^ rotate(a, 22);
    uint32_t term2 = ((a | b) & c) | (a & b); //(a & (b ^ c)) ^ (b & c);
    return term1 + term2;
}

constexpr uint32_t Swap(uint32_t x)
{
    return Fusion::Endian::ConstSwap(x);
}
}

namespace Fusion
{
template<>
void MDHash<MdHashType::Sha256>::ProcessBlock(
    std::span<const uint8_t> input)
{
    // get last hash
    uint32_t a = m_digest[0],
        b = m_digest[1],
        c = m_digest[2],
        d = m_digest[3],
        e = m_digest[4],
        f = m_digest[5],
        g = m_digest[6],
        h = m_digest[7];

    // data represented as 16x 32-bit words
    const auto* input32 = reinterpret_cast<const uint32_t*>(input.data());

    // convert to big endian
    uint32_t words[64];

    size_t i = 0;
    for (i = 0; i < 16; i++)
    {
        words[i] = Swap(input32[i]);
    }

    uint32_t x, y; // temporaries

    // first round
    x = h + f1(e, f, g) + 0x428a2f98 + words[0]; y = f2(a, b, c); d += x; h = x + y;
    x = g + f1(d, e, f) + 0x71374491 + words[1]; y = f2(h, a, b); c += x; g = x + y;
    x = f + f1(c, d, e) + 0xb5c0fbcf + words[2]; y = f2(g, h, a); b += x; f = x + y;
    x = e + f1(b, c, d) + 0xe9b5dba5 + words[3]; y = f2(f, g, h); a += x; e = x + y;
    x = d + f1(a, b, c) + 0x3956c25b + words[4]; y = f2(e, f, g); h += x; d = x + y;
    x = c + f1(h, a, b) + 0x59f111f1 + words[5]; y = f2(d, e, f); g += x; c = x + y;
    x = b + f1(g, h, a) + 0x923f82a4 + words[6]; y = f2(c, d, e); f += x; b = x + y;
    x = a + f1(f, g, h) + 0xab1c5ed5 + words[7]; y = f2(b, c, d); e += x; a = x + y;

    // secound round
    x = h + f1(e, f, g) + 0xd807aa98 + words[8]; y = f2(a, b, c); d += x; h = x + y;
    x = g + f1(d, e, f) + 0x12835b01 + words[9]; y = f2(h, a, b); c += x; g = x + y;
    x = f + f1(c, d, e) + 0x243185be + words[10]; y = f2(g, h, a); b += x; f = x + y;
    x = e + f1(b, c, d) + 0x550c7dc3 + words[11]; y = f2(f, g, h); a += x; e = x + y;
    x = d + f1(a, b, c) + 0x72be5d74 + words[12]; y = f2(e, f, g); h += x; d = x + y;
    x = c + f1(h, a, b) + 0x80deb1fe + words[13]; y = f2(d, e, f); g += x; c = x + y;
    x = b + f1(g, h, a) + 0x9bdc06a7 + words[14]; y = f2(c, d, e); f += x; b = x + y;
    x = a + f1(f, g, h) + 0xc19bf174 + words[15]; y = f2(b, c, d); e += x; a = x + y;

    // extend to 24 words
    for (; i < 24; i++)
    {
        words[i] = words[i - 16] +
            (rotate(words[i - 15], 7) ^ rotate(words[i - 15], 18) ^ (words[i - 15] >> 3)) +
            words[i - 7] +
            (rotate(words[i - 2], 17) ^ rotate(words[i - 2], 19) ^ (words[i - 2] >> 10));
    }

    // third round
    x = h + f1(e, f, g) + 0xe49b69c1 + words[16]; y = f2(a, b, c); d += x; h = x + y;
    x = g + f1(d, e, f) + 0xefbe4786 + words[17]; y = f2(h, a, b); c += x; g = x + y;
    x = f + f1(c, d, e) + 0x0fc19dc6 + words[18]; y = f2(g, h, a); b += x; f = x + y;
    x = e + f1(b, c, d) + 0x240ca1cc + words[19]; y = f2(f, g, h); a += x; e = x + y;
    x = d + f1(a, b, c) + 0x2de92c6f + words[20]; y = f2(e, f, g); h += x; d = x + y;
    x = c + f1(h, a, b) + 0x4a7484aa + words[21]; y = f2(d, e, f); g += x; c = x + y;
    x = b + f1(g, h, a) + 0x5cb0a9dc + words[22]; y = f2(c, d, e); f += x; b = x + y;
    x = a + f1(f, g, h) + 0x76f988da + words[23]; y = f2(b, c, d); e += x; a = x + y;

    // extend to 32 words
    for (; i < 32; i++)
    {
        words[i] = words[i - 16] +
            (rotate(words[i - 15], 7) ^ rotate(words[i - 15], 18) ^ (words[i - 15] >> 3)) +
            words[i - 7] +
            (rotate(words[i - 2], 17) ^ rotate(words[i - 2], 19) ^ (words[i - 2] >> 10));
    }

    // fourth round
    x = h + f1(e, f, g) + 0x983e5152 + words[24]; y = f2(a, b, c); d += x; h = x + y;
    x = g + f1(d, e, f) + 0xa831c66d + words[25]; y = f2(h, a, b); c += x; g = x + y;
    x = f + f1(c, d, e) + 0xb00327c8 + words[26]; y = f2(g, h, a); b += x; f = x + y;
    x = e + f1(b, c, d) + 0xbf597fc7 + words[27]; y = f2(f, g, h); a += x; e = x + y;
    x = d + f1(a, b, c) + 0xc6e00bf3 + words[28]; y = f2(e, f, g); h += x; d = x + y;
    x = c + f1(h, a, b) + 0xd5a79147 + words[29]; y = f2(d, e, f); g += x; c = x + y;
    x = b + f1(g, h, a) + 0x06ca6351 + words[30]; y = f2(c, d, e); f += x; b = x + y;
    x = a + f1(f, g, h) + 0x14292967 + words[31]; y = f2(b, c, d); e += x; a = x + y;

    // extend to 40 words
    for (; i < 40; i++)
        words[i] = words[i - 16] +
        (rotate(words[i - 15], 7) ^ rotate(words[i - 15], 18) ^ (words[i - 15] >> 3)) +
        words[i - 7] +
        (rotate(words[i - 2], 17) ^ rotate(words[i - 2], 19) ^ (words[i - 2] >> 10));

    // fifth round
    x = h + f1(e, f, g) + 0x27b70a85 + words[32]; y = f2(a, b, c); d += x; h = x + y;
    x = g + f1(d, e, f) + 0x2e1b2138 + words[33]; y = f2(h, a, b); c += x; g = x + y;
    x = f + f1(c, d, e) + 0x4d2c6dfc + words[34]; y = f2(g, h, a); b += x; f = x + y;
    x = e + f1(b, c, d) + 0x53380d13 + words[35]; y = f2(f, g, h); a += x; e = x + y;
    x = d + f1(a, b, c) + 0x650a7354 + words[36]; y = f2(e, f, g); h += x; d = x + y;
    x = c + f1(h, a, b) + 0x766a0abb + words[37]; y = f2(d, e, f); g += x; c = x + y;
    x = b + f1(g, h, a) + 0x81c2c92e + words[38]; y = f2(c, d, e); f += x; b = x + y;
    x = a + f1(f, g, h) + 0x92722c85 + words[39]; y = f2(b, c, d); e += x; a = x + y;

    // extend to 48 words
    for (; i < 48; i++)
        words[i] = words[i - 16] +
        (rotate(words[i - 15], 7) ^ rotate(words[i - 15], 18) ^ (words[i - 15] >> 3)) +
        words[i - 7] +
        (rotate(words[i - 2], 17) ^ rotate(words[i - 2], 19) ^ (words[i - 2] >> 10));

    // sixth round
    x = h + f1(e, f, g) + 0xa2bfe8a1 + words[40]; y = f2(a, b, c); d += x; h = x + y;
    x = g + f1(d, e, f) + 0xa81a664b + words[41]; y = f2(h, a, b); c += x; g = x + y;
    x = f + f1(c, d, e) + 0xc24b8b70 + words[42]; y = f2(g, h, a); b += x; f = x + y;
    x = e + f1(b, c, d) + 0xc76c51a3 + words[43]; y = f2(f, g, h); a += x; e = x + y;
    x = d + f1(a, b, c) + 0xd192e819 + words[44]; y = f2(e, f, g); h += x; d = x + y;
    x = c + f1(h, a, b) + 0xd6990624 + words[45]; y = f2(d, e, f); g += x; c = x + y;
    x = b + f1(g, h, a) + 0xf40e3585 + words[46]; y = f2(c, d, e); f += x; b = x + y;
    x = a + f1(f, g, h) + 0x106aa070 + words[47]; y = f2(b, c, d); e += x; a = x + y;

    // extend to 56 words
    for (; i < 56; i++)
        words[i] = words[i - 16] +
        (rotate(words[i - 15], 7) ^ rotate(words[i - 15], 18) ^ (words[i - 15] >> 3)) +
        words[i - 7] +
        (rotate(words[i - 2], 17) ^ rotate(words[i - 2], 19) ^ (words[i - 2] >> 10));

    // seventh round
    x = h + f1(e, f, g) + 0x19a4c116 + words[48]; y = f2(a, b, c); d += x; h = x + y;
    x = g + f1(d, e, f) + 0x1e376c08 + words[49]; y = f2(h, a, b); c += x; g = x + y;
    x = f + f1(c, d, e) + 0x2748774c + words[50]; y = f2(g, h, a); b += x; f = x + y;
    x = e + f1(b, c, d) + 0x34b0bcb5 + words[51]; y = f2(f, g, h); a += x; e = x + y;
    x = d + f1(a, b, c) + 0x391c0cb3 + words[52]; y = f2(e, f, g); h += x; d = x + y;
    x = c + f1(h, a, b) + 0x4ed8aa4a + words[53]; y = f2(d, e, f); g += x; c = x + y;
    x = b + f1(g, h, a) + 0x5b9cca4f + words[54]; y = f2(c, d, e); f += x; b = x + y;
    x = a + f1(f, g, h) + 0x682e6ff3 + words[55]; y = f2(b, c, d); e += x; a = x + y;

    // extend to 64 words
    for (; i < 64; i++)
    {
        words[i] = words[i - 16] +
            (rotate(words[i - 15], 7) ^ rotate(words[i - 15], 18) ^ (words[i - 15] >> 3)) +
            words[i - 7] +
            (rotate(words[i - 2], 17) ^ rotate(words[i - 2], 19) ^ (words[i - 2] >> 10));
    }

    // eigth round
    x = h + f1(e, f, g) + 0x748f82ee + words[56]; y = f2(a, b, c); d += x; h = x + y;
    x = g + f1(d, e, f) + 0x78a5636f + words[57]; y = f2(h, a, b); c += x; g = x + y;
    x = f + f1(c, d, e) + 0x84c87814 + words[58]; y = f2(g, h, a); b += x; f = x + y;
    x = e + f1(b, c, d) + 0x8cc70208 + words[59]; y = f2(f, g, h); a += x; e = x + y;
    x = d + f1(a, b, c) + 0x90befffa + words[60]; y = f2(e, f, g); h += x; d = x + y;
    x = c + f1(h, a, b) + 0xa4506ceb + words[61]; y = f2(d, e, f); g += x; c = x + y;
    x = b + f1(g, h, a) + 0xbef9a3f7 + words[62]; y = f2(c, d, e); f += x; b = x + y;
    x = a + f1(f, g, h) + 0xc67178f2 + words[63]; y = f2(b, c, d); e += x; a = x + y;

    // update hash
    m_digest[0] += a;
    m_digest[1] += b;
    m_digest[2] += c;
    m_digest[3] += d;
    m_digest[4] += e;
    m_digest[5] += f;
    m_digest[6] += g;
    m_digest[7] += h;
}

template<>
void MDHash<MdHashType::Sha256>::ProcessBuffer()
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
void MDHash<MdHashType::Sha256>::Finish(
    std::span<uint8_t> digest)
{
    Digest output;
    DigestStorage old = { 0 };

    // Save the old hash if the buffer still has remaining data
    for (uint8_t i = 0; i < m_digest.size(); ++i)
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
void MDHash<MdHashType::Sha256>::Reset()
{
    m_buffer.fill(0);
    m_digest.fill(0);

    m_size = 0;
    m_bufferSize = 0;

    m_digest[0] = 0x6a09e667;
    m_digest[1] = 0xbb67ae85;
    m_digest[2] = 0x3c6ef372;
    m_digest[3] = 0xa54ff53a;
    m_digest[4] = 0x510e527f;
    m_digest[5] = 0x9b05688c;
    m_digest[6] = 0x1f83d9ab;
    m_digest[7] = 0x5be0cd19;
}

}  // namespace fusion
