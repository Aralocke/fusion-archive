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

#pragma once

#include <Fusion/Macros.h>
#include <Fusion/Result.h>

#include <span>
#include <string>
#include <vector>

namespace Fusion
{
class StringUtil final
{
public:
    //
    // String compare case-sensitive
    //
    static int Compare(const char* a, const char* b);

    //
    // String compare case-sensitive
    //
    static int Compare(const char* a, const char* b, size_t len);

    //
    // String compare case-sensitive
    //
    static int Compare(std::string_view a, std::string_view b);

    //
    // String compare case insensitive
    //
    static int CompareI(const char* a, const char* b);

    //
    // String compare case insensitive
    //
    static int CompareI(const char* a, const char* b, size_t len);

    //
    // String compare case insensitive
    //
    static int CompareI(std::string_view a, std::string_view b);

    //
    // Copy a string into a given buffer
    //
    static std::string_view Copy(
        char* dest,
        size_t destLen,
        const char* src);

    //
    // Copy a string into a given buffer
    //
    static std::string_view Copy(
        char* dst,
        size_t dstLen,
        const char* src,
        size_t srcLen);

    //
    // Copy a string into a given span
    //
    static std::string_view Copy(
        std::span<char> dest,
        std::string_view source);

    //
    // Check if a given string contains the given suffix (case sensitive)
    //
    static bool EndsWith(std::string_view str, std::string_view suffix);

    //
    // Check if a given string contains the given suffix (case-insensitive)
    //
    static bool EndsWithI(std::string_view str, std::string_view suffix);

    //
    // String equal compare
    //
    static bool Equal(const char* a, const char* b);

    //
    // String equal compare
    //
    static bool Equal(const char* a, const char* b, size_t len);

    //
    // String equal compare
    //
    static bool Equal(std::string_view a, std::string_view b);

    //
    // String equal compare
    //
    template<size_t aSize, size_t bSize>
    static bool Equal(const char(&a)[aSize], const char(&b)[bSize])
    {
        return Equal(a, b, std::min(aSize, bSize));
    }

    //
    // String equal compare insensitive
    //
    static bool EqualI(const char* a, const char* b);

    //
    // String equal compare insensitive
    //
    static bool EqualI(const char* a, const char* b, size_t len);

    //
    // String equal compare insensitive
    //
    static bool EqualI(std::string_view a, std::string_view b);

    //
    // String equal compare insensitive
    //
    template<size_t aSize, size_t bSize>
    static bool EqualI(const char(&a)[aSize], const char(&b)[bSize])
    {
        return EqualI(a, b, std::min(aSize, bSize));
    }

    //
    // Join a span of string_views into a single string separated by delimiter
    //
    static std::string Join(
        std::span<std::string_view> values,
        std::string_view delim);

    //
    // Join a span of strings into a single string separated by delimiter
    //
    static std::string Join(
        std::span<std::string> values,
        std::string_view delim);

    //
    // String length
    //
    static size_t Length(const char* s);

    //
    // String length
    //
    static size_t Length(const char* s, size_t len);

    //
    // Const String length
    //
    constexpr static size_t ConstLength(const char* s)
    {
        const char* start = s;
        while (*start != '\0')
        {
            ++start;
        }
        return static_cast<size_t>(start - s);
    }

    //
    // Const String length
    //
    template<size_t Size>
    constexpr static size_t ConstLength(const char(&s)[Size])
    {
        FUSION_UNUSED_LOCAL(s);
        return Size;
    }

    //
    // String less compare
    //
    static bool Less(const char* a, const char* b);

    //
    // String less compare
    //
    static bool Less(const char* a, const char* b, size_t len);

    //
    // String less compare
    //
    static bool Less(std::string_view a, std::string_view b);

    //
    // String less compare
    //
    template<size_t aSize, size_t bSize>
    static bool Less(const char(&a)[aSize], const char(&b)[bSize])
    {
        return Less(a, b, std::min(aSize, bSize));
    }

    //
    // String less compare insensitive
    //
    static bool LessI(const char* a, const char* b);

    //
    // String less compare insensitive
    //
    static bool LessI(const char* a, const char* b, size_t len);

    //
    // String less compare insensitive
    //
    static bool LessI(std::string_view a, std::string_view b);

    //
    // String less compare insensitive
    //
    template<size_t aSize, size_t bSize>
    static bool LessI(const char(&a)[aSize], const char(&b)[bSize])
    {
        return LessI(a, b, std::min(aSize, bSize));
    }

    template<typename T>
    static Result<T> ParseNumber(
        std::string_view in,
        int32_t base = 10);

    //
    // Split a string into a vector of strings separated by delimiter
    //
    static std::vector<std::string> Split(
        std::string_view in,
        std::string_view delim,
        size_t count = (size_t)(-1));

    //
    // Split a string into a vector of string views separated by delimiter
    //
    static std::vector<std::string_view> SplitViews(
        std::string_view in,
        std::string_view delim,
        size_t count = (size_t)(-1));

    //
    // Check if a given string contains the given prefix (case-sensitive)
    //
    static bool StartsWith(std::string_view str, std::string_view prefix);

    //
    // Check if a given string contains the given prefix (case-insensitive)
    //
    static bool StartsWithI(std::string_view str, std::string_view prefix);

    //
    // Convert a string to a floating point value.
    //
    static Result<double> ToFloatingPoint(
        std::string_view in,
        double min = std::numeric_limits<double>::lowest(),
        double max = std::numeric_limits<double>::max());

    //
    // Convert the input string into all lower case characters in-place.
    //
    // Note: This function should only be used on ASCII plaintext strings.
    //
    static void ToLower(std::string& str);

    //
    // Convert the input string into all lower case characters and return
    // a copy of the input string.
    //
    // Note: This function should only be used on ASCII plaintext strings.
    //
    static std::string ToLowerCopy(std::string_view str);

    static Result<int64_t> ToSignedNumber(
        std::string_view in,
        int32_t base = 10,
        int64_t min = std::numeric_limits<int64_t>::min(),
        int64_t max = std::numeric_limits<int64_t>::max());

    static Result<uint64_t> ToUnsignedNumber(
        std::string_view in,
        int32_t base = 10,
        uint64_t max = std::numeric_limits<uint64_t>::max());

    //
    // Convert the input string into all upper case characters in-place.
    //
    // Note: This function should only be used on ASCII plaintext strings.
    //
    static void ToUpper(std::string& str);

    //
    // Convert the input string into all upper case characters and return
    // a copy of the input string.
    //
    // Note: This function should only be used on ASCII plaintext strings.
    //
    static std::string ToUpperCopy(std::string_view str);

    static const std::string_view WHITESPACE_TOKENS;

    //
    // Trim the given whitespace characters from the left and right sides
    // of the input string in-place.
    //
    static void Trim(
        std::string& str,
        std::string_view tokens = WHITESPACE_TOKENS);

    //
    // Trim the given whitespace characters from the left and right sides
    // of the input string and return a new view of the input string.
    //
    static std::string_view Trim(
        std::string_view str,
        std::string_view tokens = WHITESPACE_TOKENS);

    //
    // Trim the given whitespace characters from the left and right sides
    // of the input string and return a new copy of the input string.
    //
    static std::string TrimCopy(
        std::string_view str,
        std::string_view tokens = WHITESPACE_TOKENS);

    //
    // Trim the given whitespace characters from the left side of the input
    // string in-place.
    //
    static void TrimLeft(
        std::string& str,
        std::string_view tokens = WHITESPACE_TOKENS);

    //
    // Trim the given whitespace characters from the left and side
    // of the input string and return a new view of the input string.
    //
    static std::string_view TrimLeft(
        std::string_view str,
        std::string_view tokens = WHITESPACE_TOKENS);

    //
    // Trim the given whitespace characters from the left and side
    // of the input string and return a new copy of the input string.
    //
    static std::string TrimLeftCopy(
        std::string_view str,
        std::string_view tokens = WHITESPACE_TOKENS);

    //
    // Trim the given whitespace characters from the right side of the input
    // string in-place.
    //
    static void TrimRight(
        std::string& str,
        std::string_view tokens = WHITESPACE_TOKENS);

    //
    // Trim the given whitespace characters from the right and side
    // of the input string and return a new view of the input string.
    //
    static std::string_view TrimRight(
        std::string_view str,
        std::string_view tokens = WHITESPACE_TOKENS);

    //
    // Trim the given whitespace characters from the right and side
    // of the input string and return a new copy of the input string.
    //
    static std::string TrimRightCopy(
        std::string_view str,
        std::string_view tokens = WHITESPACE_TOKENS);
};

class FixedStringBuilder final
{
public:
    FixedStringBuilder(char* buffer, size_t size);
    FixedStringBuilder(std::span<char> buffer);
    ~FixedStringBuilder();

    void operator+=(char c);
    void operator+=(const char* str);
    void operator+=(std::string_view str);

    void Append(char c);
    void Append(const char* str);
    void Append(const char* str, size_t length);
    void Append(std::string_view str);

    size_t Capacity() const;

    const char* Data() const;
    char* Data();

    const char* Head() const;
    char* Head();

    void Offset(size_t size);

    size_t Remaining() const;

    size_t Size() const;

    std::string String() const;

    std::span<char> Subspan();

    std::string_view View() const;

private:
    std::span<char> m_buffer;
    size_t m_capacity = 0;
    size_t m_offset = 0;
};
}  // namespace Fusion

#define FUSION_IMPL_STRINGUTILS 1
#include <Fusion/Impl/StringUtil.h>
