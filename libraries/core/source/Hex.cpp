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

#include <Fusion/Hex.h>

#include <iostream>

namespace Fusion
{
HexView::HexView() = default;

HexView::HexView(
    const void* data,
    size_t size)
    : m_data(static_cast<const uint8_t*>(data))
    , m_size(static_cast<uint32_t>(size))
{ }

const uint8_t* HexView::Data() const
{
    return m_data;
}

bool HexView::IsEmpty() const
{
    return m_size == 0;
}

size_t HexView::Size() const
{
    return m_size;
}

HexView::operator bool() const
{
    return !IsEmpty();
}

HexView::operator std::span<const uint8_t>() const
{
    return std::span<const uint8_t>(m_data, m_size);
}

bool operator==(const HexView& a, const HexView& b)
{
    return MemoryUtil::Equal(
        a.Data(),
        a.Size(),
        b.Data(),
        b.Size());
}

bool operator!=(const HexView& a, const HexView& b)
{
    return !operator==(a, b);
}

bool operator<(const HexView& a, const HexView& b)
{
    return MemoryUtil::Less(
        a.Data(),
        a.Size(),
        b.Data(),
        b.Size());
}

std::string ToString(const HexView& view)
{
    return Ascii::ToHexString(view.Data(), view.Size());
}

std::string_view ToString(
    char* buffer,
    size_t length,
    const HexView& view)
{
    size_t s = std::min(view.Size() * 2 + 1, length);

    return Ascii::ToHexString(
        view.Data(),
        view.Size(),
        buffer,
        s);
}

std::ostream& operator<<(std::ostream& o, const HexView& h)
{
    return o << ToString(h);
}
}  // namespace Fusion
