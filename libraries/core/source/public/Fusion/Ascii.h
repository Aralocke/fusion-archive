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

#pragma once

#include <Fusion/Assert.h>

#include <span>
#include <string_view>

namespace Fusion
{
class Ascii final
{
public:
    //
    //
    //
    static constexpr char FromDigit(uint8_t v)
    {
        return (v <= 9)
            ? static_cast<char>(v + '0')
            : 0;
    }

    //
    //
    //
    static constexpr uint8_t FromHexChar(char b)
    {
        if (IsHex(b))
        {
            if (IsDigit(b))
            {
                return (b - '0');
            }
            if (IsLower(b) || IsUpper(b))
            {
                return (10 + ToLower(b) - 'a');
            }
        }
        return 0;
    }

    //
    //
    //
    static std::span<uint8_t> FromHexString(
        std::string_view str,
        void* buffer,
        size_t size);

    //
    //
    //
    static constexpr bool IsAlpha(uint8_t c)
    {
        return IsUpper(c) || IsLower(c);
    }

    //
    //
    //
    static constexpr bool IsAlphaNum(uint8_t c)
    {
        return IsDigit(c) || IsAlpha(c);
    }

    //
    //
    //
    static constexpr bool IsDigit(uint8_t c)
    {
        return c >= 48 && c <= 57;
    }

    //
    //
    //
    static bool IsFalseString(std::string_view str);

    //
    //
    //
    static constexpr bool IsHex(uint8_t c)
    {
        return (c >= 97 && c <= 102)
            || (c >= 65 && c <= 70)
            || (c >= 48 && c <= 57);
    }

    //
    //
    //
    static constexpr bool IsLower(uint8_t c)
    {
        return c >= 97 && c <= 122;
    }

    //
    //
    //
    static constexpr bool IsSpace(uint8_t c)
    {
        return (c == ' ' || c == '\t' || c == '\n' || c == '\r'
            || c == '\v' || c == '\f');
    }

    //
    //
    //
    static bool IsTrueString(std::string_view str);

    //
    //
    //
    static constexpr bool IsUpper(uint8_t c)
    {
        return c >= 65 && c <= 90;
    }

    //
    //
    //
    static constexpr uint8_t ToDigit(uint8_t c)
    {
        return IsDigit(c) ? (c - '0') : 0;
    }

    //
    //
    //
    static constexpr uint8_t ToHexByte(uint8_t high, uint8_t low)
    {
        return (high << 4) | low;
    }

    //
    //
    //
    static std::string_view ToHexString(
        uint64_t value,
        char* buffer,
        size_t length);

    //
    //
    //
    static std::string_view ToHexString(
        const void* bytes,
        size_t size,
        char* buffer,
        size_t length);

    //
    //
    //
    static std::string ToHexString(const void* bytes, size_t size);

    //
    //
    //
    static constexpr uint8_t ToLower(uint8_t c)
    {
        return IsUpper(c) ? c - ('A' - 'a') : c;
    }

    //
    //
    //
    static std::string_view ToOctalString(
        uint64_t value,
        char* buffer,
        size_t length);

    //
    //
    //
    static constexpr uint8_t ToNibble(uint8_t c)
    {
        if (IsHex(c))
        {
            if (IsDigit(c))
            {
                return c - '0';
            }
            if (IsLower(c) || IsUpper(c))
            {
                return ToLower(c) - 'a' + 10;
            }
        }
        return 0;
    }

    //
    //
    //
    static constexpr char ToUpper(char c)
    {
        return IsLower(c) ? c + ('A' - 'a') : c;
    }
    
};
}  // namespace Fusion
