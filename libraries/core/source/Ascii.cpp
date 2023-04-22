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

#include <Fusion/Ascii.h>
#include <Fusion/StringUtil.h>
#include <Fusion/Utilities.h>

#include <algorithm>
#include <array>

namespace Fusion
{
using namespace std::string_view_literals;

constexpr auto FALSE_STRINGS =
    std::to_array<std::string_view>({
        "disabled"sv, "false"sv, "no"sv, "n"sv, "0"sv
    });

constexpr auto TRUE_STRINGS =
    std::to_array<std::string_view>({
        "enabled"sv, "true"sv, "yes"sv, "y"sv, "1"sv
    });

constexpr auto HEX_CHARACTERS =
    std::to_array<char>({
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f',
    });

std::span<uint8_t> Ascii::FromHexString(
    std::string_view str,
    void* buffer,
    size_t size)
{
    const size_t length = str.size();

    if (!buffer
        || length == 0
        || length % 2 != 0
        || size == 0)
    {
        return {};
    }

    size_t hexLength = length / 2;
    auto* b = static_cast<const char*>(str.data());
    auto* outputU8 = static_cast<uint8_t*>(buffer);

    size_t i = 0;
    while (i < hexLength && i + 2 < size)
    {
        uint8_t high = ToNibble(*b++);
        uint8_t low = ToNibble(*b++);

        outputU8[i++] = ToHexByte(high, low);
    }

    return { outputU8, i };
}

bool Ascii::IsFalseString(std::string_view str)
{
    return std::any_of(
        std::begin(FALSE_STRINGS),
        std::end(FALSE_STRINGS),
        [str](std::string_view f)
        {
            return StringUtil::EqualI(str, f);
        });
}

bool Ascii::IsTrueString(std::string_view str)
{
    return std::any_of(
        std::begin(TRUE_STRINGS),
        std::end(TRUE_STRINGS),
        [str](std::string_view f)
        {
            return StringUtil::EqualI(str, f);
        });
}

std::string_view Ascii::ToHexString(
    uint64_t value,
    char* buffer,
    size_t length)
{
    if (!buffer || length == 0)
    {
        return {};
    }
    else if (value == 0)
    {
        buffer[0] = '0';
        length = 1;
    }
    else
    {
        char* start = buffer;
        char* end = start + length - 1;  // terminator

        while (value > 0 && start < end)
        {
            *start++ = HEX_CHARACTERS[value % 16];
            value /= 16;
        }

        length = static_cast<size_t>(start - buffer);
        Utils::Reverse(buffer, start);
    }

    buffer[length] = 0;
    return { buffer, length };
}

std::string_view Ascii::ToHexString(
    const void* bytes,
    size_t size,
    char* buffer,
    size_t length)
{
    if (size == 0 || length == 0)
    {
        return {};
    }

    length = std::min(length - 1, size * 2 + 1);  // terminator
    auto* inputU8 = static_cast<const uint8_t*>(bytes);

    size_t i = 0;
    while (i + 2 <= length)
    {
        buffer[i++] = HEX_CHARACTERS[(*inputU8 >> 4) & 0xF];
        buffer[i++] = HEX_CHARACTERS[*inputU8++ & 0xF];
    }

    buffer[i] = 0;
    return { buffer, i };
}

std::string_view Ascii::ToOctalString(
    uint64_t value,
    char* buffer,
    size_t length)
{
    if (!buffer || length == 0)
    {
        return {};
    }
    else if (value == 0)
    {
        buffer[0] = '0';
        length = 1;
    }
    else
    {
        char* start = buffer;
        char* end = start + length - 1;  // terminator

        while (value > 0 && start < end)
        {
            *start++ = HEX_CHARACTERS[value % 8];
            value /= 8;
        }

        Utils::Reverse(buffer, start);
        length = static_cast<size_t>(start - buffer);
    }

    buffer[length] = 0;
    return { buffer, length };
}

}  // namespace Fusion
