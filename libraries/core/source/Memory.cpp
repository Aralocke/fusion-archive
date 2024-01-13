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
// MemoryWriter                                            START
MemoryWriter::MemoryWriter(void* data, size_t size)
    : m_data(reinterpret_cast<uint8_t*>(data))
    , m_size(size)
{ }

MemoryWriter::MemoryWriter(std::span<uint8_t> data)
    : m_data(data.data())
    , m_size(data.size())
{ }

const uint8_t* MemoryWriter::Data() const
{
    return m_data;
}

size_t MemoryWriter::Offset() const
{
    return m_offset;
}

void MemoryWriter::Put(
    const void* data,
    size_t size)
{
    Put(m_offset, data, size);
}

void MemoryWriter::Put(
    size_t offset,
    const void* data,
    size_t size)
{
    if (offset + size <= m_size)
    {
        Seek(offset);

        memcpy(m_data + m_offset, data, size);
        m_offset += size;
    }
}

void MemoryWriter::Put(std::span<const uint8_t> data)
{
    Put(m_offset, data.data(), data.size());
}

void MemoryWriter::Put(
    size_t offset,
    std::span<const uint8_t> data)
{
    Put(offset, data.data(), data.size());
}

void MemoryWriter::Put16_BE(uint16_t value)
{
    Put16_BE(m_offset, value);
}

void MemoryWriter::Put16_BE(size_t offset, uint16_t value)
{
    uint8_t values[2] = {
        uint8_t(value >> 8),
        uint8_t(value)
    };

    Put(offset, values, sizeof(values));
}

void MemoryWriter::Put16_LE(uint16_t value)
{
    Put16_LE(m_offset, value);
}

void MemoryWriter::Put16_LE(size_t offset, uint16_t value)
{
    uint8_t values[2] = {
        uint8_t(value),
        uint8_t(value >> 8)
    };

    Put(offset, values, sizeof(values));
}

void MemoryWriter::Put32_BE(uint32_t value)
{
    Put32_BE(m_offset, value);
}

void MemoryWriter::Put32_BE(size_t offset, uint32_t value)
{
    uint8_t values[4] = {
        uint8_t(value >> 24),
        uint8_t(value >> 16),
        uint8_t(value >> 8),
        uint8_t(value)
    };

    Put(offset, values, sizeof(values));
}

void MemoryWriter::Put32_LE(uint32_t value)
{
    Put32_LE(m_offset, value);
}

void MemoryWriter::Put32_LE(size_t offset, uint32_t value)
{
    uint8_t values[4] = {
        uint8_t(value),
        uint8_t(value >> 8),
        uint8_t(value >> 16),
        uint8_t(value >> 24)
    };

    Put(offset, values, sizeof(values));
}

void MemoryWriter::Put64_BE(uint64_t value)
{
    Put64_BE(m_offset, value);
}

void MemoryWriter::Put64_BE(size_t offset, uint64_t value)
{
    uint8_t values[8] = {
        uint8_t(value >> 56),
        uint8_t(value >> 48),
        uint8_t(value >> 40),
        uint8_t(value >> 32),
        uint8_t(value >> 24),
        uint8_t(value >> 16),
        uint8_t(value >> 8),
        uint8_t(value)
    };

    Put(offset, values, sizeof(values));
}

void MemoryWriter::Put64_LE(uint64_t value)
{
    Put64_LE(m_offset, value);
}

void MemoryWriter::Put64_LE(size_t offset, uint64_t value)
{
    uint8_t values[8] = {
        uint8_t(value),
        uint8_t(value >> 8),
        uint8_t(value >> 16),
        uint8_t(value >> 24),
        uint8_t(value >> 32),
        uint8_t(value >> 40),
        uint8_t(value >> 48),
        uint8_t(value >> 56)
    };

    Put(offset, values, sizeof(values));
}

void MemoryWriter::PutString(
    const char* str,
    size_t length)
{
    Put(m_offset, str, length);
}

void MemoryWriter::PutString(
    size_t offset,
    const char* str,
    size_t length)
{
    Put(offset, str, length);
}

void MemoryWriter::PutString(std::string_view str)
{
    Put(m_offset, str.data(), str.size());
}

void MemoryWriter::PutString(
    size_t offset,
    std::string_view str)
{
    Put(offset, str.data(), str.size());
}

void MemoryWriter::PutZero(size_t count)
{
    PutZero(m_offset, count);
}

void MemoryWriter::PutZero(size_t offset, size_t count)
{
    if (offset + count <= m_size)
    {
        Seek(offset);

        memset(m_data + m_offset, 0, count);
        m_offset += count;
    }
}

size_t MemoryWriter::Remaining() const
{
    return (m_size - m_offset);
}

void MemoryWriter::Seek(size_t offset)
{
    FUSION_ASSERT(offset <= m_size);
    m_offset = offset;
}

size_t MemoryWriter::Size() const
{
    return m_size;
}

void MemoryWriter::Skip(size_t count)
{
    Seek(m_offset + count);
}
// MemoryWriter                                              END
// -------------------------------------------------------------
}  // namespace Fusion
