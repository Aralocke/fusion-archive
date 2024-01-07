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

#if !defined(FUSION_IMPL_HASH)
#error "Hash impl included before main header"
#endif

#include <Fusion/Assert.h>
#include <Fusion/StringUtil.h>

#include <fmt/core.h>

namespace Fusion
{
// -------------------------------------------------------------
// FnvSeed                                                 START
#pragma region FnvSeed

template<>
struct FnvSeed<uint32_t>
{
    static constexpr uint32_t seed{ 2166136261UL };
    static constexpr uint32_t prime{ 16777619UL };
};

template<>
struct FnvSeed<uint64_t>
{
    static constexpr uint64_t seed{ 14695981039346656037ULL };
    static constexpr uint64_t prime{ 1099511628211ULL };
};

#pragma endregion FnvSeed
// FnvSeed                                                   END
// -------------------------------------------------------------
// FNVA                                                    START
#pragma region FNVA

template<typename T>
T FNVA<T>::Hash(
    const void* data,
    size_t length,
    T seed,
    const T prime)
{
    const auto* d = static_cast<const uint8_t*>(data);

    while (length-- > 0)
    {
        seed = (seed * prime) ^ *d++;
    }

    return seed;
}

#pragma endregion FNVA
// FNVA                                                      END
// -------------------------------------------------------------
// FNV1A                                                   START
#pragma region FNV1A

template<typename T>
T FNV1A<T>::Hash(
    const void* data,
    size_t length,
    T seed,
    const T prime)
{
    const auto* d = static_cast<const uint8_t*>(data);

    while (length-- > 0)
    {
        seed = (seed ^ *d++) * prime;
    }

    return seed;
}

#pragma endregion FNV1A
// FNV1A                                                     END
// -------------------------------------------------------------
// FNV                                                     START
#pragma region FNV

template<typename Algorithm>
typename FNV<Algorithm>::Type FNV<Algorithm>::Hash(
    const void* data,
    size_t size)
{
    FNV<Algorithm> fnv;
    fnv.Data(data, size);
    return fnv.Done();
}

template<typename Algorithm>
FNV<Algorithm>::FNV()
    : m_seed(FnvSeed<Type>::seed)
{ }

template<typename Algorithm>
FNV<Algorithm>::~FNV() { }

template<typename Algorithm>
void FNV<Algorithm>::Data(const void* data, size_t size)
{
    m_seed = Algorithm::Hash(
        data,
        size,
        m_seed,
        FnvSeed<Type>::prime);
}

template<typename Algorithm>
void FNV<Algorithm>::String(const char* str)
{
    Data(str, StringUtil::Length(str));
}

template<typename Algorithm>
void FNV<Algorithm>::Reset()
{
    m_seed = FnvSeed<Type>::seed;
}

#pragma endregion FNV
// FNV                                                       END
// -------------------------------------------------------------
// XXHASH                                                  START
#pragma region XXHASH

template<typename IntegralType>
template<typename T>
IntegralType XXHASH<IntegralType>::Hash(
    std::span<const T> data,
    IntegralType seed)
{
    return HashInternal(data.data(), data.size_bytes(), seed);
}

#pragma endregion XXHASH
// XXHASH                                                    END
// -------------------------------------------------------------
// MDHash                                                  START
#pragma region MDHash

template<MdHashType Type>
MDHash<Type>::MDHash()
{
    Reset();
}

template<MdHashType Type>
void MDHash<Type>::Finish(Digest& digest)
{
    Finish(std::span<uint8_t>{ digest.data, Digest::SIZE });
    digest.size = Digest::SIZE;
}

template<MdHashType Type>
template<size_t size>
void MDHash<Type>::Finish(uint8_t(&digest)[size])
{
    Finish(std::span<uint8_t>{ digest, size });
}

template<MdHashType Type>
void MDHash<Type>::Process(std::string_view s)
{
    Process(s.data(), s.size());
}

template<MdHashType Type>
void MDHash<Type>::Process(std::span<const uint8_t> s)
{
    Process(s.data(), s.size());
}

template<MdHashType Type>
void MDHash<Type>::Process(const char* data)
{
    Process(data, StringUtil::Length(data));
}

template<MdHashType Type>
void MDHash<Type>::Process(
    const void* buffer,
    size_t size)
{
    const auto* inputU8 = static_cast<const uint8_t*>(buffer);
    size_t inputSize = size;
    size_t offset = 0;

    while (m_bufferSize + inputSize >= m_buffer.size())
    {
        // Consume as many full blocks as possible from the input stream.
        size_t available = std::min<size_t>(
            m_buffer.size(),
            m_buffer.size() - m_bufferSize);

        FUSION_ASSERT(available <= m_buffer.size());
        FUSION_ASSERT(offset <= size);

        memcpy(
            m_buffer.data() + m_bufferSize,
            inputU8 + offset,
            available);

        m_bufferSize += available;
        inputSize -= available;
        offset += available;

        FUSION_ASSERT(m_bufferSize == m_buffer.size());

        ProcessBlock(m_buffer);
        m_size += m_buffer.size();
        m_bufferSize = 0;
    }
    if (inputSize > 0)
    {
        // Consume remaining data and put it into the buffer.
        FUSION_ASSERT(inputSize <= m_buffer.size());
        FUSION_ASSERT(m_bufferSize == 0);

        memcpy(
            m_buffer.data(),
            inputU8 + offset,
            inputSize);

        m_bufferSize += inputSize;
    }
}

template<MdHashType Type>
template<size_t size>
void MDHash<Type>::Process(const uint8_t(&data)[size])
{
    Process(data, size);
}

#pragma endregion MDHash
// MDHash                                                    END
// -------------------------------------------------------------
// HMAC                                                    START
#pragma region HMAC

template<typename Method>
void HMAC(
    std::span<const uint8_t> key,
    std::span<const uint8_t> data,
    typename Method::Digest& output)
{
    HMAC(
        key,
        data,
        std::span<uint8_t>(output.Data(), output.Capacity()),
        Method::HashType);

    output.size = output.Capacity();
}

#pragma endregion HMAC
// HMAC                                                      END
// -------------------------------------------------------------
}  // namespace Fusion
