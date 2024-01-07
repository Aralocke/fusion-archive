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

#include <Fusion/Fwd/Hex.h>

#include <Fusion/Types.h>
#include <Fusion/TypeTraits.h>

#include <array>
#include <iosfwd>
#include <span>
#include <string_view>

namespace Fusion
{
//
//
//
class HexView final
{
public:
    //
    //
    //
    HexView();

    //
    //
    //
    HexView(const void* data, size_t size);

    //
    //
    //
    template<uint32_t N>
    HexView(const Hex<N>& h);

    //
    //
    //
    template <typename T, uint32_t N,
        FUSION_REQUIRES(IsLikeByte<T>::Value)>
    HexView(const T(&data)[N])
        : HexView(data, N)
    { }

    //
    //
    //
    template<typename T, uint32_t N,
        FUSION_REQUIRES(IsLikeByte<T>::Value)>
    HexView(std::span<const T> data)
        : HexView(data.data(), data.size())
    { }

    HexView(HexView&&) noexcept = default;
    HexView& operator=(HexView&&) noexcept = default;

    //
    //
    //
    const uint8_t* Data() const;

    //
    //
    //
    bool IsEmpty() const;
    explicit operator bool() const;

    //
    //
    //
    size_t Size() const;

    //
    //
    //
    operator std::span<const uint8_t>() const;

private:
    const uint8_t* m_data{ nullptr };
    uint32_t m_size : 31 { 0 };

};

bool operator==(const HexView& a, const HexView& b);
bool operator!=(const HexView& a, const HexView& b);
bool operator<(const HexView& a, const HexView& b);

//
//
//
std::string ToString(const HexView& h);

//
//
//
std::string_view ToString(
    char* buffer,
    size_t length,
    const HexView& h);

std::ostream& operator<<(std::ostream& o, const HexView& h);

//
//
//
template<uint32_t N>
class Hex final
{
    static_assert(N % 2 == 0, "Hex<N> must be an even number size");
public:
    //
    //
    //
    static Hex<N> FromString(std::string_view str);

public:
    static constexpr size_t LENGTH = (N * 2 + 1);
    static constexpr size_t SIZE = N;

public:
    //
    //
    //
    Hex();

    //
    //
    //
    Hex(uint32_t size);

    //
    //
    //
    Hex(const void* buffer, uint32_t size);

    //
    //
    //
    Hex(const HexView& view);

    //
    //
    //
    Hex(std::span<const uint8_t> data);

    //
    //
    //
    ~Hex();

    //
    //
    //
    Hex(const Hex& h);

    //
    //
    //
    Hex& operator=(const Hex& h);

    //
    //
    //
    Hex(Hex&& h) noexcept;

    //
    //
    //
    Hex& operator=(Hex&& h) noexcept;

    //
    //
    //
    void Assign(const void* data, size_t size);

    //
    //
    //
    void Assign(std::span<const uint8_t> data);

    //
    //
    //
    template<typename T, uint32_t U,
        FUSION_REQUIRES(IsLikeByte<T>::Value)>
    void Assign(const T(&in)[U])
    {
        static_assert(U <= N, "Unable to assign to internal buffer");
        Assign(in, U);
    }

    //
    //
    //
    constexpr size_t Capacity() const { return N; }

    //
    //
    //
    const uint8_t* Data() const;

    //
    //
    //
    uint8_t* Data();

    //
    //
    //
    bool Empty() const;
    explicit operator bool() const;

    //
    //
    //
    size_t Size() const;

    operator std::span<const uint8_t>() const;

    operator std::span<uint8_t>();

    operator HexView() const;

public:
    alignas(uint8_t) uint8_t data[N];
    uint32_t size{ 0 };
};

template<uint32_t N, uint32_t U>
bool operator==(const Hex<N>& a, const Hex<U>& b);

template<uint32_t N, uint32_t U>
bool operator!=(const Hex<N>& a, const Hex<U>& b);

template<uint32_t N, uint32_t U>
bool operator<(const Hex<N>& a, const Hex<U>& b);
}  // namespace Fusion

#define FUSION_IMPL_HEX 1
#include <Fusion/Impl/Hex.h>
