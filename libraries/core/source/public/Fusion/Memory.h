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
#include <Fusion/Macros.h>
#include <Fusion/TypeTraits.h>

#include <span>
#include <string_view>
#include <utility>

namespace Fusion
{
//
//
//
template<typename T, typename U,
    FUSION_REQUIRES(std::is_signed_v<T> == std::is_signed_v<U>)>
constexpr T NarrowCast(const U& in)
{
    T out = static_cast<T>(in);
    FUSION_ASSERT(static_cast<U>(out) == in,
        "narrowing of input creates invalid value");
    return out;
}

//
//
//
template<typename T, typename U,
    FUSION_REQUIRES(std::is_signed_v<T> && !std::is_signed_v<U>)>
constexpr T NarrowCast(const U& in)
{
    T out = static_cast<T>(in);
    FUSION_ASSERT(static_cast<U>(out) == in && !(out < T{}),
        "narrowing of input creates invalid value");
    return out;
}

//
//
//
template<typename T, typename U,
    FUSION_REQUIRES(!std::is_signed_v<T> && std::is_signed_v<U>)>
constexpr T NarrowCast(const U& in)
{
    T out = static_cast<T>(in);
    FUSION_ASSERT(static_cast<U>(out) == in && !(in < U{}),
        "narrowing of input creates invalid value");
    return out;
}

//
//
//
template<typename T>
class ScopeGuard final
{
public:
    template<typename Fn>
    friend ScopeGuard<std::decay_t<Fn>> CreateScopeGuard(Fn fn);

    ~ScopeGuard() { m_obj(); }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = delete;

private:
    explicit ScopeGuard(T o) : m_obj(std::move(o)) { }

private:
    T m_obj;
};

//
//
//
template<typename Fn>
static ScopeGuard<std::decay_t<Fn>> CreateScopeGuard(Fn fn)
{
    return ScopeGuard<std::decay_t<Fn>>(std::move(fn));
}

//
//
//
class MemoryReader final
{
public:
    //
    //
    //
    MemoryReader() = default;

    //
    //
    //
    MemoryReader(std::span<const uint8_t> data);

    //
    //
    //
    MemoryReader(const void* data, size_t size)
        : m_data(reinterpret_cast<const uint8_t*>(data))
        , m_size(size)
    { }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(IsLikeByte<T>::value)>
    constexpr MemoryReader(const T* data, size_t size)
        : m_data(reinterpret_cast<const uint8_t*>(data))
        , m_size(size)
    { }

    //
    //
    //
    template<typename T, size_t N, FUSION_REQUIRES(IsLikeByte<T>::value)>
    constexpr MemoryReader(const T(&data)[N])
        : m_data(data)
        , m_size(N)
    { }

    //
    //
    //
    template<typename T, size_t N, FUSION_REQUIRES(!IsLikeByte<T>::value)>
    constexpr MemoryReader(const T(&data)[N])
        : m_data(data)
        , m_size(sizeof(T) * N)
    { }

    ~MemoryReader() = default;

public:
    //
    //
    //
    size_t Offset() const;

    //
    //
    //
    uint8_t Read();

    //
    //
    //
    uint8_t Read(size_t offset);

    //
    //
    //
    std::span<const uint8_t> ReadSpan(size_t length);

    //
    //
    //
    std::span<const uint8_t> ReadSpan(size_t offset, size_t length);

    //
    //
    //
    std::string_view ReadString(size_t length);

    //
    //
    //
    std::string_view ReadString(size_t offset, size_t length);

    //
    //
    //
    size_t Remaining() const;

    //
    //
    //
    void Reset();

    //
    //
    //
    void Seek(size_t offset);

    //
    //
    //
    size_t Size() const;

    //
    //
    //
    void Skip(size_t count);

    //
    //
    //
    MemoryReader Span(size_t size);

    //
    //
    //
    MemoryReader Span(size_t offset, size_t size);

public:
    //
    //
    //
    uint16_t Read16_LE();

    //
    //
    //
    uint16_t Read16_LE(size_t offset);

    //
    //
    //
    uint32_t Read32_LE();

    //
    //
    //
    uint32_t Read32_LE(size_t offset);

    //
    //
    //
    uint64_t Read64_LE();

    //
    //
    //
    uint64_t Read64_LE(size_t offset);

public:
    //
    //
    //
    uint16_t Read16_BE();

    //
    //
    //
    uint16_t Read16_BE(size_t offset);

    //
    //
    //
    uint32_t Read32_BE();

    //
    //
    //
    uint32_t Read32_BE(size_t offset);

    //
    //
    //
    uint64_t Read64_BE();

    //
    //
    //
    uint64_t Read64_BE(size_t offset);

private:
    const uint8_t* m_data{ nullptr };
    size_t m_offset{ 0 };
    size_t m_size{ 0 };
};

//
//
//
class MemoryWriter final
{
public:
    //
    //
    //
    constexpr MemoryWriter() = default;

    //
    //
    //
    MemoryWriter(void* data, size_t size);

    //
    //
    //
    MemoryWriter(std::span<uint8_t> data);

    //
    //
    //
    template<typename T, FUSION_REQUIRES(IsLikeByte<T>::value)>
    constexpr MemoryWriter(T* data, size_t size)
        : m_data(data)
        , m_size(size)
    { }

    //
    //
    //
    template<typename T, size_t N, FUSION_REQUIRES(IsLikeByte<T>::value)>
    constexpr MemoryWriter(T(&data)[N])
        : m_data(data)
        , m_size(N)
    { }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_trivially_constructible<T>::value)>
    constexpr MemoryWriter(T& data)
        : m_data(&data)
        , m_size(sizeof(T))
    { }

    ~MemoryWriter() = default;

public:
    //
    //
    //
    const uint8_t* Data() const;

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_trivially_constructible<T>::value)>
    MemoryWriter ObjectWriter()
    {
        return ObjectWriter<T>(m_offset);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_trivially_constructible<T>::value)>
    MemoryWriter ObjectWriter(size_t offset)
    {
        if (offset + sizeof(T) <= m_size)
        {
            Seek(offset);
            MemoryWriter writer(m_data + m_offset, sizeof(T));

            m_offset += sizeof(T);
            return writer;
        }
        return {};
    }

    //
    //
    //
    size_t Offset() const;

    //
    //
    //
    void Put(const void* data, size_t size);

    //
    //
    //
    void Put(size_t offset, const void* data, size_t size);

    //
    //
    //
    void Put(std::span<const uint8_t> data);

    //
    //
    //
    void Put(size_t offset, std::span<const uint8_t> data);

    //
    //
    //
    void PutString(const char* str, size_t length);

    //
    //
    //
    void PutString(size_t offset, const char* str, size_t length);

    //
    //
    //
    void PutString(std::string_view str);

    //
    //
    //
    void PutString(size_t offset, std::string_view str);

    //
    //
    //
    void PutZero(size_t count);

    //
    //
    //
    void PutZero(size_t offset, size_t count);

    //
    //
    //
    size_t Remaining() const;

    //
    //
    //
    void Seek(size_t offset);

    //
    //
    //
    size_t Size() const;

    //
    //
    //
    void Skip(size_t count);

public:
    //
    //
    //
    void Put16_LE(uint16_t value);

    //
    //
    //
    void Put16_LE(size_t offset, uint16_t value);

    //
    //
    //
    void Put32_LE(uint32_t value);

    //
    //
    //
    void Put32_LE(size_t offset, uint32_t value);

    //
    //
    //
    void Put64_LE(uint64_t value);

    //
    //
    //
    void Put64_LE(size_t offset, uint64_t value);

public:
    //
    //
    //
    void Put16_BE(uint16_t value);

    //
    //
    //
    void Put16_BE(size_t offset, uint16_t value);

    //
    //
    //
    void Put32_BE(uint32_t value);

    //
    //
    //
    void Put32_BE(size_t offset, uint32_t value);

    //
    //
    //
    void Put64_BE(uint64_t value);

    //
    //
    //
    void Put64_BE(size_t offset, uint64_t value);

private:
    uint8_t* m_data{ nullptr };
    size_t m_offset{ 0 };
    size_t m_size{ 0 };
};

}  // namespace Fusion

#define FUSION_SCOPE_GUARD(...) \
    auto FUSION_ANONYMOUS_SYMBOL(_guard) \
        = Fusion::CreateScopeGuard(__VA_ARGS__);

