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

#if !defined(FUSION_IMPL_HEX)
#error "Hex impl included before main header"
#endif

#include <Fusion/Assert.h>
#include <Fusion/Ascii.h>
#include <Fusion/MemoryUtil.h>

#include <utility>

#include <fmt/format.h>

namespace Fusion
{
// -------------------------------------------------------------
// HexView                                                 START
#pragma region HexView

template<uint32_t N>
HexView::HexView(const Hex<N>& h)
    : HexView(h.Data(), h.Size())
{
    FUSION_ASSERT(h.Capacity() == N);
    FUSION_ASSERT(h.Size() <= N);
}

#pragma endregion HexView
// HexView                                                   END
// -------------------------------------------------------------
// Hex                                                     START
#pragma region Hex

template<uint32_t N>
Hex<N> Hex<N>::FromString(std::string_view str)
{
    FUSION_ASSERT(str.size() % 2 == 0);
    FUSION_ASSERT(str.size() <= size_t(N * 2));

    if (str.empty())
    {
        return {};
    }

    Hex<N> hex;
    std::span<uint8_t> result = Ascii::FromHexString(
        str,
        hex.data,
        N);

    if (result.empty())
    {
        return {};
    }

    hex.size = result.size();
    return hex;
}

template<uint32_t N>
Hex<N>::Hex()
    : size(0)
{
    MemoryUtil::Clear(data, N);
}

template<uint32_t N>
Hex<N>::Hex(uint32_t size)
    : Hex()
{
    FUSION_ASSERT(size <= N);
    this->size = size;
}

template<uint32_t N>
Hex<N>::Hex(const void* buffer, uint32_t size)
    : Hex()
{
    FUSION_ASSERT(size <= N);
    Assign(buffer, size);
}

template<uint32_t N>
Hex<N>::Hex(const HexView& view)
    : Hex()
{
    FUSION_ASSERT(view.Size() <= N);
    Assign(view.Data(), view.Size());
}

template<uint32_t N>
Hex<N>::Hex(std::span<const uint8_t> data)
    : Hex()
{
    FUSION_ASSERT(data.size() <= N);
    Assign(data.data(), data.size());
}

template<uint32_t N>
Hex<N>::Hex(const Hex& h)
    : Hex()
{ }

template<uint32_t N>
Hex<N>::Hex(Hex&& h) noexcept
    : Hex()
{
    Assign(h.data, h.size);
}

template<uint32_t N>
Hex<N>::~Hex() = default;

template<uint32_t N>
Hex<N>& Hex<N>::operator=(const Hex<N>& h)
{
    if (data != h.data)
    {
        Assign(h.data, h.size);
    }
    return *this;
}

template<uint32_t N>
Hex<N>& Hex<N>::operator=(Hex<N>&& h) noexcept
{
    if (data != h.data)
    {
        memset(this->data, 0, SIZE);
        memcpy(this->data, h.data, SIZE);
        memset(h.data, 0, SIZE);
        this->size = std::exchange(h.size, 0);
    }
    return *this;
}

template<uint32_t N>
void Hex<N>::Assign(const void* data, size_t size)
{
    FUSION_ASSERT(size <= size_t(UINT32_MAX));

    const size_t total = std::min(size, SIZE);
    memcpy(this->data, data, total);
    this->size = total;
}

template<uint32_t N>
void Hex<N>::Assign(std::span<const uint8_t> data)
{
    Assign(data.data(), data.size());
}

template<uint32_t N>
const uint8_t* Hex<N>::Data() const
{
    return this->data;
}

template<uint32_t N>
uint8_t* Hex<N>::Data()
{
    return this->data;
}

template<uint32_t N>
bool Hex<N>::Empty() const
{
    return this->size == 0;
}

template<uint32_t N>
Hex<N>::operator bool() const
{
    return !Empty();
}

template<uint32_t N>
size_t Hex<N>::Size() const
{
    return this->size;
}

template<uint32_t N>
Hex<N>::operator std::span<const uint8_t>() const
{
    return { this->data, this->size };
}

template<uint32_t N>
Hex<N>::operator std::span<uint8_t>()
{
    return { this->data, this->size };
}

template<uint32_t N>
Hex<N>::operator HexView() const
{
    return HexView{ Data(), Size() };
}

template<uint32_t N, uint32_t U>
bool operator==(const Hex<N>& a, const Hex<U>& b)
{
    return MemoryUtil::Equal(a.data, a.size, b.data, b.size);
}

template<uint32_t N, uint32_t U>
bool operator!=(const Hex<N>& a, const Hex<U>& b)
{
    return !operator==(a, b);
}

template<uint32_t N, uint32_t U>
bool operator<(const Hex<N>& a, const Hex<U>& b)
{
    return MemoryUtil::Less(a.data, a.size, b.data, b.size);
}

#pragma endregion Hex
// Hex                                                       END
// -------------------------------------------------------------
}  // namespace Fusion

// -------------------------------------------------------------
// Formatters                                              START
#pragma region Formatters

template<size_t N>
struct fmt::formatter<Fusion::Hex<N>>
    : fmt::formatter<fmt::string_view>
{
    auto format(const Fusion::Hex<N>& hex, format_context& ctx)
    {
        std::array<char, Fusion::Hex<N>::LENGTH> buffer = { 0 };

        return fmt::formatter<fmt::string_view>::format(
            ToString(buffer.data(), buffer.size(), hex),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::HexView>
    : fmt::formatter<fmt::string_view>
{
    auto format(const Fusion::HexView& hex, format_context& ctx)
    {
        return fmt::formatter<fmt::string_view>::format(
            ToString(hex),
            ctx);
    }
};

#pragma endregion Formatters
// Formatters                                                END
// -------------------------------------------------------------
