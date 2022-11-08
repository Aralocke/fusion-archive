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
#include <Fusion/Compiler.h>
#include <Fusion/StringUtil.h>

#include <cstring>
#include <numeric>
#include <utility>

namespace Fusion
{
const std::string_view StringUtil::WHITESPACE_TOKENS = " \r\n\t";

int StringUtil::Compare(const char* a, const char* b)
{
    for (;;)
    {
        uint8_t x = static_cast<uint8_t>(*a++);
        uint8_t y = static_cast<uint8_t>(*b++);

        if (x == 0 || x != y)
        {
            return static_cast<int>(x - y);
        }
    }

    return 0;
}

int StringUtil::Compare(const char* a, const char* b, size_t length)
{
    while (length--)
    {
        uint8_t x = static_cast<uint8_t>(*a++);
        uint8_t y = static_cast<uint8_t>(*b++);

        if (x == 0 || x != y)
        {
            return static_cast<int>(x - y);
        }
    }

    return 0;
}

int StringUtil::Compare(std::string_view a, std::string_view b)
{
    return Compare(
        a.data(),
        b.data(),
        std::min(a.size(), b.size()));
}

int StringUtil::CompareI(const char* a, const char* b)
{
    for (;;)
    {
        uint8_t x = Ascii::ToLower(*a++);
        uint8_t y = Ascii::ToLower(*b++);

        if (x == 0 || x != y)
        {
            return static_cast<int>(x - y);
        }
    }

    return 0;
}

int StringUtil::CompareI(const char* a, const char* b, size_t length)
{
    while (length--)
    {
        uint8_t x = Ascii::ToLower(*a++);
        uint8_t y = Ascii::ToLower(*b++);

        if (x == 0 || x != y)
        {
            return static_cast<int>(x - y);
        }
    }

    return 0;
}

int StringUtil::CompareI(std::string_view a, std::string_view b)
{
    return CompareI(
        a.data(),
        b.data(),
        std::min(a.size(), b.size()));
}

std::string_view StringUtil::Copy(
    char* dest,
    size_t destLen,
    const char* src)
{
    return Copy(dest, destLen, src, strlen(src));
}

std::string_view StringUtil::Copy(
    char* dst,
    size_t dstLen,
    const char* src,
    size_t srcLen)
{
    if (!dst
        || dstLen == 0
        || !src
        || srcLen == 0)
    {
        return {};
    }

    size_t count = std::min(dstLen - 1, srcLen);
    memcpy(dst, src, count);
    dst[count] = 0;

    return { dst, count };
}

std::string_view StringUtil::Copy(
    std::span<char> dest,
    std::string_view source)
{
    return Copy(dest.data(), dest.size(), source.data(), source.size());
}

bool StringUtil::EndsWith(
    std::string_view str,
    std::string_view suffix)
{
    if (suffix.size() <= str.size())
    {
        return Equal(str.substr(str.size() - suffix.size()), suffix);
    }
    return false;
}

bool StringUtil::EndsWithI(
    std::string_view str,
    std::string_view suffix)
{
    if (suffix.size() <= str.size())
    {
        return EqualI(str.substr(str.size() - suffix.size()), suffix);
    }
    return false;
}

bool StringUtil::Equal(const char* a, const char* b)
{
    return Compare(a, b) == 0;
}

bool StringUtil::Equal(const char* a, const char* b, size_t len)
{
    return Compare(a, b, len) == 0;
}

bool StringUtil::Equal(std::string_view a, std::string_view b)
{
    return Compare(a, b) == 0;
}

bool StringUtil::EqualI(const char* a, const char* b)
{
    return CompareI(a, b) == 0;
}

bool StringUtil::EqualI(const char* a, const char* b, size_t len)
{
    return CompareI(a, b, len) == 0;
}

bool StringUtil::EqualI(std::string_view a, std::string_view b)
{
    return CompareI(a, b) == 0;
}

std::string StringUtil::Join(
    std::span<std::string> values,
    std::string_view delim)
{
    std::vector<std::string_view> input;

    input.insert(
        input.begin(),
        begin(values),
        end(values));

    return Join(input, delim);
}

std::string StringUtil::Join(
    std::span<std::string_view> values,
    std::string_view delim)
{
    using namespace std::string_view_literals;

    if (values.empty())
    {
        return {};
    }

    if (delim.empty())
    {
        delim = ""sv;
    }

    size_t count = values.size();
    size_t size = std::accumulate(
        begin(values),
        end(values),
        (size_t)(0),
        [&delim](size_t size, const std::string_view& str) -> size_t
        {
            return size + str.size() + delim.size();
        });

    std::string out;
    out.reserve(size);

    for (size_t i = 0; i < count; ++i)
    {
        out.append(values[i]);
        if (i < count - 1)
        {
            out.append(delim);
        }
    }

    return out;
}

#if FUSION_IMPL_STRLEN
size_t StringUtil::Length(const char* s)
{
    const char* start = s;
    while (*start != '\0')
    {
        ++start;
    }
    return static_cast<size_t>(start - s);
}
#endif  // FUSION_IMPL_STRLEN

size_t StringUtil::Length(const char* s)
{
    return std::strlen(s);
}

size_t StringUtil::Length(const char* s, size_t len)
{
    const char* start = s;
    const char* end = s + len;

    while (start < end && *start)
    {
        ++start;
    }

    return static_cast<size_t>(start - s);
}

bool StringUtil::Less(const char* a, const char* b)
{
    return Compare(a, b) < 0;
}

bool StringUtil::Less(const char* a, const char* b, size_t len)
{
    return Compare(a, b, len) < 0;
}

bool StringUtil::Less(std::string_view a, std::string_view b)
{
    return Compare(a, b) < 0;
}

bool StringUtil::LessI(const char* a, const char* b)
{
    return CompareI(a, b) < 0;
}

bool StringUtil::LessI(const char* a, const char* b, size_t len)
{
    return CompareI(a, b, len) < 0;
}

bool StringUtil::LessI(std::string_view a, std::string_view b)
{
    return CompareI(a, b) < 0;
}

std::vector<std::string> StringUtil::Split(
    std::string_view in,
    std::string_view delim,
    size_t count)
{
    std::vector<std::string> out;

    if (in.empty() || count == 0)
    {
        return {};
    }
    if (delim.empty())
    {
        out.emplace_back(in);
        return out;
    }

    std::vector<std::string_view> views = SplitViews(in, delim, count);
    
    out.reserve(views.size());
    out.insert(out.begin(), views.begin(), views.end());

    return out;
}

std::vector<std::string_view> StringUtil::SplitViews(
    std::string_view in,
    std::string_view delim,
    size_t count)
{
    std::vector<std::string_view> result;
    std::string_view part = in;

    if (in.empty() || count == 0)
    {
        return {};
    }
    if (delim.empty())
    {
        result.push_back(in);
        return result;
    }

    while (result.size() < count)
    {
        size_t pos = part.find(delim);
        if (pos == std::string_view::npos)
        {
            break;
        }

        result.push_back(part.substr(0, pos));
        part = part.substr(pos + 1);
    }

    if (!part.empty() && result.size() < count)
    {
        result.push_back(part);
    }
    
    return result;
}

bool StringUtil::StartsWith(
    std::string_view str,
    std::string_view prefix)
{
    if (prefix.size() <= str.size())
    {
        return Equal(str.substr(0, prefix.size()), prefix);
    }
    return false;
}

bool StringUtil::StartsWithI(
    std::string_view str,
    std::string_view prefix)
{
    if (prefix.size() <= str.size())
    {
        return EqualI(str.substr(0, prefix.size()), prefix);
    }
    return false;
}

Result<double> StringUtil::ToFloatingPoint(
    std::string_view in,
    double min,
    double max)
{
    using namespace std::string_view_literals;

    if (in.empty())
    {
        return Failure{ "empty string"sv };
    }

    char* end = nullptr;
    double value = std::strtod(in.data(), &end);

    if (value == HUGE_VAL || value > max || value < min)
    {
        return Failure{ "out of range"sv };
    }
    if (end - in.data() != in.size())
    {
        return Failure{ "not a number"sv };
    }
    return value;
}

void StringUtil::ToLower(std::string& str)
{
    for (char& c : str)
    {
        c = Ascii::ToLower(c);
    }
}

std::string StringUtil::ToLowerCopy(std::string_view str)
{
    std::string tmp(str);
    ToLower(tmp);

    return tmp;
}

Result<int64_t> StringUtil::ToSignedNumber(
    std::string_view in,
    int32_t base,
    int64_t min,
    int64_t max)
{
    using namespace std::string_view_literals;

    FUSION_UNUSED(base);
    if (in.empty())
    {
        return Failure{ "empty string"sv };
    }

    const bool sign = (in.front() == '-');

    if (sign)
    {
        in = in.substr(1);
    }

    const char* start = in.data();
    const char* end = start + in.size();
    int64_t v = 0;

    while (*start && Ascii::IsDigit(*start) && start < end)
    {
        v = (v * 10) + Ascii::ToDigit(*start++);
    }

    if (sign)
    {
        v = -v;
    }
    if (v < min || v > max)
    {
        return Failure{ "out of range"sv };
    }
    if (start != end)
    {
        return Failure{ "not a number"sv };
    }
    return v;
}

Result<uint64_t> StringUtil::ToUnsignedNumber(
    std::string_view in,
    int32_t base,
    uint64_t max)
{
    using namespace std::string_view_literals;

    FUSION_UNUSED(base);
    if (in.empty())
    {
        return Failure{ "empty string"sv };
    }

    const char* start = in.data();
    const char* end = start + in.size();
    uint64_t v = 0;

    while (*start && Ascii::IsDigit(*start) && start < end)
    {
        v = (v * 10) + Ascii::ToNibble(*start++);
    }

    if (v > max)
    {
        return Failure{ "out of range"sv };
    }
    if (start != end)
    {
        return Failure{ "not a number"sv };
    }
    return v;
}

void StringUtil::ToUpper(std::string& str)
{
    for (char& c : str)
    {
        c = Ascii::ToUpper(c);
    }
}

std::string StringUtil::ToUpperCopy(std::string_view str)
{
    std::string tmp(str);
    ToUpper(tmp);

    return tmp;
}

void StringUtil::Trim(
    std::string& str,
    std::string_view tokens)
{
    TrimLeft(str, tokens);
    TrimRight(str, tokens);
}

std::string_view StringUtil::Trim(
    std::string_view str,
    std::string_view tokens)
{
    str = TrimLeft(str, tokens);

    return TrimRight(str, tokens);
}

std::string StringUtil::TrimCopy(
    std::string_view str,
    std::string_view tokens)
{
    std::string tmp(str);
    Trim(tmp, tokens);

    return tmp;
}

void StringUtil::TrimLeft(
    std::string& str,
    std::string_view tokens)
{
    str.erase(0, str.find_first_not_of(tokens));
}

std::string_view StringUtil::TrimLeft(
    std::string_view str,
    std::string_view tokens)
{
    return str.substr(str.find_first_not_of(tokens));
}

std::string StringUtil::TrimLeftCopy(
    std::string_view str,
    std::string_view tokens)
{
    std::string tmp(str);
    TrimLeft(tmp, tokens);

    return tmp;
}

void StringUtil::TrimRight(
    std::string& str,
    std::string_view tokens)
{
    if (auto pos = str.find_last_not_of(tokens);
        pos != std::string::npos)
    {
        str.resize(pos + 1);
    }
    else
    {
        str.clear();
    }
}

std::string_view StringUtil::TrimRight(
    std::string_view str,
    std::string_view tokens)
{
    auto pos = str.find_last_not_of(tokens);
    if (pos != std::string_view::npos)
    {
        str = str.substr(0, pos + 1);
    }
    else
    {
        str = {};
    }
    return str;
}

std::string StringUtil::TrimRightCopy(
    std::string_view str,
    std::string_view tokens)
{
    std::string tmp(str);
    TrimRight(tmp, tokens);

    return tmp;
}

FixedStringBuilder::FixedStringBuilder(
    char* buffer,
    size_t size)
    : m_buffer(buffer, size)
    , m_capacity(size - 1)
    , m_offset(0)
{ }

FixedStringBuilder::FixedStringBuilder(std::span<char> buffer)
    : m_buffer(buffer)
    , m_capacity(buffer.size() - 1)
    , m_offset(0)
{ }

FixedStringBuilder::~FixedStringBuilder()
{ }

void FixedStringBuilder::operator+=(char c)
{
    Append(c);
}

void FixedStringBuilder::operator+=(const char* str)
{
    Append(str, StringUtil::Length(str));
}

void FixedStringBuilder::operator+=(std::string_view str)
{
    Append(str);
}

void FixedStringBuilder::Append(char c)
{
    if (Remaining() > 1)
    {
        char* buffer = Head();
        buffer[m_offset++] = c;
        buffer[m_offset] = 0;
    }
}

void FixedStringBuilder::Append(const char* str)
{
    Append(str, StringUtil::Length(str));
}

void FixedStringBuilder::Append(const char* str, size_t length)
{
    length = std::min(length, Remaining());
    FUSION_ASSERT(m_offset + length < m_capacity);
    memcpy(Head(), str, length);
    m_offset += length;
    m_buffer[m_offset] = 0;
}

void FixedStringBuilder::Append(std::string_view str)
{
    Append(str.data(), str.size());
}

size_t FixedStringBuilder::Capacity() const
{
    return m_capacity;
}

const char* FixedStringBuilder::Data() const
{
    return m_buffer.data();
}

char* FixedStringBuilder::Data()
{
    return m_buffer.data();
}

const char* FixedStringBuilder::Head() const
{
    return m_buffer.data() + m_offset;
}

char* FixedStringBuilder::Head()
{
    return m_buffer.data() + m_offset;
}

void FixedStringBuilder::Offset(size_t size)
{
    size = std::min(Remaining(), size);
    FUSION_ASSERT(m_offset + size < m_capacity);
    m_offset += size;
    m_buffer[m_offset] = 0;
}

size_t FixedStringBuilder::Remaining() const
{
    return Capacity() - Size();
}

size_t FixedStringBuilder::Size() const
{
    return m_offset;
}

std::string FixedStringBuilder::String() const
{
    return { m_buffer.data(), m_offset };
}

std::span<char> FixedStringBuilder::Subspan()
{
    return { Head(), Remaining() };
}

std::string_view FixedStringBuilder::View() const
{
    return { m_buffer.data(), m_offset };
}
}  // namespace Fusion
