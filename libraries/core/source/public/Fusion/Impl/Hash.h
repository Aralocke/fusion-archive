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

#if !defined(FUSION_IMPL_HASH)
#error "Hash impl included before main header"
#endif

#include <Fusion/StringUtil.h>

#include <fmt/core.h>

namespace Fusion
{
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

// -------------------------------------------------------------
// FNVA                                                    START
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
// FNVA                                                      END
// -------------------------------------------------------------
// FNV1A                                                   START
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
// FNV1A                                                     END
// -------------------------------------------------------------
// FNV                                                     START
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
// FNV                                                       END
// -------------------------------------------------------------
// XXHASH                                                  START
template<typename IntegralType>
template<typename T>
IntegralType XXHASH<IntegralType>::Hash(
    std::span<const T> data,
    IntegralType seed)
{
    return HashInternal(data.data(), data.size_bytes(), seed);
}
// XXHASH                                                     END
// --------------------------------------------------------------
}  // namespace Fusion
