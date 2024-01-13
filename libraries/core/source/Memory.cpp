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

#include <Fusion/Memory.h>

#include <Fusion/StringUtil.h>

namespace Fusion
{
// -------------------------------------------------------------
// MemoryReader                                            START
MemoryReader::MemoryReader(std::span<const uint8_t> data)
    : MemoryReader(data.data(), data.size())
{ }

size_t MemoryReader::Offset() const
{
    return m_offset;
}

uint8_t MemoryReader::Read()
{
    if (m_offset <= m_size)
    {
        return m_data[m_offset++];
    }
    return 0;
}

uint8_t MemoryReader::Read(size_t offset)
{
    if (offset <= m_size)
    {
        Seek(offset);
        return m_data[m_offset++];
    }
    return 0;
}

uint16_t MemoryReader::Read16_BE()
{
    if (m_offset + 2 <= m_size)
    {
        uint16_t value = (uint16_t(m_data[m_offset]) << 8)
            + uint16_t(m_data[m_offset + 1]);

        m_offset += 2;
        return value;
    }
    return 0;
}

uint16_t MemoryReader::Read16_BE(size_t offset)
{
    if (offset + 2 <= m_size)
    {
        Seek(offset);
        uint16_t value = (uint16_t(m_data[m_offset]) << 8)
            + uint16_t(m_data[m_offset + 1]);

        m_offset += 2;
        return value;
    }
    return 0;
}

uint16_t MemoryReader::Read16_LE()
{
    if (m_offset + 2 <= m_size)
    {
        uint16_t value = (uint16_t(m_data[m_offset + 1]) << 8)
            + uint16_t(m_data[m_offset]);

        m_offset += 2;
        return value;
    }
    return 0;
}

uint16_t MemoryReader::Read16_LE(size_t offset)
{
    if (offset + 2 <= m_size)
    {
        Seek(offset);
        uint16_t value = (uint16_t(m_data[m_offset + 1]) << 8)
            + uint16_t(m_data[m_offset]);

        m_offset += 2;
        return value;
    }
    return 0;
}

uint32_t MemoryReader::Read32_BE()
{
    if (m_offset + 4 <= m_size)
    {
        uint32_t value = (uint32_t(m_data[m_offset]) << 24)
            + (uint32_t(m_data[m_offset + 1]) << 16)
            + (uint32_t(m_data[m_offset + 2]) << 8)
            + uint32_t(m_data[m_offset + 3]);

        m_offset += 4;
        return value;
    }
    return 0;
}

uint32_t MemoryReader::Read32_BE(size_t offset)
{
    if (offset + 4 <= m_size)
    {
        Seek(offset);
        uint32_t value = (uint32_t(m_data[m_offset]) << 24)
            + (uint32_t(m_data[m_offset + 1]) << 16)
            + (uint32_t(m_data[m_offset + 2]) << 8)
            + uint32_t(m_data[m_offset + 3]);

        m_offset += 4;
        return value;
    }
    return 0;
}

uint32_t MemoryReader::Read32_LE()
{
    if (m_offset + 4 <= m_size)
    {
        uint32_t value = (uint32_t(m_data[m_offset + 3]) << 24)
            + (uint32_t(m_data[m_offset + 2]) << 16)
            + (uint32_t(m_data[m_offset + 1]) << 8)
            + uint32_t(m_data[m_offset]);

        m_offset += 4;
        return value;
    }
    return 0;
}

uint32_t MemoryReader::Read32_LE(size_t offset)
{
    if (offset + 4 <= m_size)
    {
        Seek(offset);
        uint32_t value = (uint32_t(m_data[m_offset + 3]) << 24)
            + (uint32_t(m_data[m_offset + 2]) << 16)
            + (uint32_t(m_data[m_offset + 1]) << 8)
            + uint32_t(m_data[m_offset]);

        m_offset += 4;
        return value;
    }
    return 0;
}

uint64_t MemoryReader::Read64_BE()
{
    if (m_offset + 8 <= m_size)
    {
        uint64_t value = uint64_t(uint64_t(m_data[m_offset]) << 56)
            + uint64_t(uint64_t(m_data[m_offset + 1]) << 48)
            + uint64_t(uint64_t(m_data[m_offset + 2]) << 40)
            + uint64_t(uint64_t(m_data[m_offset + 3]) << 32)
            + uint64_t(uint64_t(m_data[m_offset + 4]) << 24)
            + uint64_t(uint64_t(m_data[m_offset + 5]) << 16)
            + uint64_t(uint64_t(m_data[m_offset + 6]) << 8)
            + uint64_t(m_data[m_offset + 7]);

        m_offset += 8;
        return value;
    }
    return 0;
}

uint64_t MemoryReader::Read64_BE(size_t offset)
{
    if (offset + 4 <= m_size)
    {
        Seek(offset);
        uint64_t value = uint64_t(uint64_t(m_data[m_offset]) << 56)
            + uint64_t(uint64_t(m_data[m_offset + 1]) << 48)
            + uint64_t(uint64_t(m_data[m_offset + 2]) << 40)
            + uint64_t(uint64_t(m_data[m_offset + 3]) << 32)
            + uint64_t(uint64_t(m_data[m_offset + 4]) << 24)
            + uint64_t(uint64_t(m_data[m_offset + 5]) << 16)
            + uint64_t(uint64_t(m_data[m_offset + 6]) << 8)
            + uint64_t(m_data[m_offset + 7]);

        m_offset += 8;
        return value;
    }
    return 0;
}

uint64_t MemoryReader::Read64_LE()
{
    if (m_offset + 8 <= m_size)
    {
        uint64_t value = uint64_t(uint64_t(m_data[m_offset + 7]) << 56)
            + uint64_t(uint64_t(m_data[m_offset + 6]) << 48)
            + uint64_t(uint64_t(m_data[m_offset + 5]) << 40)
            + uint64_t(uint64_t(m_data[m_offset + 4]) << 32)
            + uint64_t(uint64_t(m_data[m_offset + 3]) << 24)
            + uint64_t(uint64_t(m_data[m_offset + 2]) << 16)
            + uint64_t(uint64_t(m_data[m_offset + 1]) << 8)
            + uint64_t(m_data[m_offset]);

        m_offset += 8;
        return value;
    }
    return 0;
}

uint64_t MemoryReader::Read64_LE(size_t offset)
{
    if (offset + 4 <= m_size)
    {
        Seek(offset);
        uint64_t value = uint64_t(uint64_t(m_data[m_offset + 7]) << 56)
            + uint64_t(uint64_t(m_data[m_offset + 6]) << 48)
            + uint64_t(uint64_t(m_data[m_offset + 5]) << 40)
            + uint64_t(uint64_t(m_data[m_offset + 4]) << 32)
            + uint64_t(uint64_t(m_data[m_offset + 3]) << 24)
            + uint64_t(uint64_t(m_data[m_offset + 2]) << 16)
            + uint64_t(uint64_t(m_data[m_offset + 1]) << 8)
            + uint64_t(m_data[m_offset]);

        m_offset += 8;
        return value;
    }
    return 0;
}

std::span<const uint8_t> MemoryReader::ReadSpan(size_t length)
{
    return ReadSpan(m_offset, length);
}

std::span<const uint8_t> MemoryReader::ReadSpan(
    size_t offset,
    size_t length)
{
    if (offset + length <= m_size)
    {
        Seek(offset);
        std::span<const uint8_t> data{
            m_data + m_offset,
            length
        };

        m_offset += length;
        return data;
    }
    return {};
}

std::string_view MemoryReader::ReadString(size_t length)
{
    return ReadString(m_offset, length);
}

std::string_view MemoryReader::ReadString(
    size_t offset,
    size_t length)
{
    if (offset + length <= m_size)
    {
        Seek(offset);
        const char* p = reinterpret_cast<const char*>(m_data + m_offset);
        
        std::string_view str{ p, StringUtil::Length(p, length) };

        m_offset += length;
        return str;
    }
    return {};
}

size_t MemoryReader::Remaining() const
{
    return (m_size - m_offset);
}

void MemoryReader::Reset()
{
    Seek(0);
}

void MemoryReader::Seek(size_t offset)
{
    FUSION_ASSERT(offset < m_size);
    m_offset = offset;
}

size_t MemoryReader::Size() const
{
    return m_size;
}

void MemoryReader::Skip(size_t count)
{
    Seek(m_offset + count);
}

MemoryReader MemoryReader::Span(size_t size)
{
    return Span(m_offset, size);
}

MemoryReader MemoryReader::Span(
    size_t offset,
    size_t size)
{
    Seek(offset);

    FUSION_ASSERT(m_offset + size <= m_size);
    MemoryReader reader(m_data + m_offset, size);
    m_offset += size;

    return reader;
}
// MemoryReader                                              END
// -------------------------------------------------------------
}  // namespace Fusion
