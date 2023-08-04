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

#include <Fusion/Uuid.h>

#include <Fusion/Ascii.h>
#include <Fusion/MemoryUtil.h>
#include <Fusion/Random.h>

#include <iostream>

namespace Fusion
{
constexpr std::string_view UUID_FMT =
    "{:02x}{:02x}{:02x}{:02x}"
    "-"
    "{:02x}{:02x}"
    "-"
    "{:02x}{:02x}"
    "-"
    "{:02x}{:02x}"
    "-"
    "{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}";

Result<UUID> UUID::FromString(std::string_view str)
{
    UUID uuid;
    memset(uuid.bytes, 0, UUID::SIZE);

    if (str.size() % 2 != 0)
    {
        return Failure(E_INVALID_ARGUMENT);
    }

    const char* start = str.data();
    const char* end = start + str.size();
    size_t index = 0;

    while (start < end && *start)
    {
        FUSION_ASSERT(index <= UUID::SIZE);

        if (*start == '-')
        {
            ++start;
            continue;
        }
        else if (!Ascii::IsHex(*start)
            || !Ascii::IsHex(*(start + 1)))
        {
            break;
        }

        uint8_t high = Ascii::ToNibble(*start++);
        uint8_t low = Ascii::ToNibble(*start++);

        uuid.bytes[index++] = (high << 4) | low;
    }

    if (index == UUID::SIZE)
    {
        return uuid;
    }

    return Failure(E_INVALID_ARGUMENT)
        .WithContext("Failed to parse '{}'", str);
}

Result<UUID> UUID::V4()
{
    UUID uuid;
    if (auto result = Random::SecureBytes(uuid.bytes); !result)
    {
        return result.Error();
    }

    // Set bits for version 4

    uuid.bytes[6] &= 0x0F;  // Clear the upper 4 bits
    uuid.bytes[6] |= 0x40;  // Set the version to 0100 (version 4)

    return uuid;
}

uint8_t UUID::Version() const
{
    return uint8_t((bytes[6] & 0xF0) >> 4);
}

bool operator==(const UUID& left, const UUID& right)
{
    return MemoryUtil::Equal(left.bytes, right.bytes, UUID::SIZE);
}

bool operator!=(const UUID& left, const UUID& right)
{
    return !MemoryUtil::Equal(left.bytes, right.bytes, UUID::SIZE);
}

bool operator<(const UUID& left, const UUID& right)
{
    return MemoryUtil::Less(left.bytes, right.bytes, UUID::SIZE);
}

bool operator<=(const UUID& left, const UUID& right)
{
    return memcmp(left.bytes, right.bytes, UUID::SIZE) <= 0;
}

bool operator>(const UUID& left, const UUID& right)
{
    return memcmp(left.bytes, right.bytes, UUID::SIZE) > 0;
}

bool operator>=(const UUID& left, const UUID& right)
{
    return memcmp(left.bytes, right.bytes, UUID::SIZE) >= 0;
}

std::string ToString(const UUID& uuid)
{
    return fmt::format(FMT_STRING(UUID_FMT),
        uuid.bytes[0],
        uuid.bytes[1],
        uuid.bytes[2],
        uuid.bytes[3],
        uuid.bytes[4],
        uuid.bytes[5],
        uuid.bytes[6],
        uuid.bytes[7],
        uuid.bytes[8],
        uuid.bytes[9],
        uuid.bytes[10],
        uuid.bytes[11],
        uuid.bytes[12],
        uuid.bytes[13],
        uuid.bytes[14],
        uuid.bytes[15]);
}

std::string_view ToString(
    std::span<char> buffer,
    const UUID& uuid)
{
    auto result = fmt::format_to_n(buffer.data(), buffer.size(),
        FMT_STRING(UUID_FMT),
        uuid.bytes[0],
        uuid.bytes[1],
        uuid.bytes[2],
        uuid.bytes[3],
        uuid.bytes[4],
        uuid.bytes[5],
        uuid.bytes[6],
        uuid.bytes[7],
        uuid.bytes[8],
        uuid.bytes[9],
        uuid.bytes[10],
        uuid.bytes[11],
        uuid.bytes[12],
        uuid.bytes[13],
        uuid.bytes[14],
        uuid.bytes[15]);

    return { buffer.data(), result.size};
}

std::ostream& operator<<(std::ostream& o, const UUID& uuid)
{
    std::array<char, UUID::LENGTH> buf = { 0 };

    return o << ToString(buf, uuid);
}
}  // namespace Fusion
