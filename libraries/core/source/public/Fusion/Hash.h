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

#include <Fusion/Fwd/Hash.h>

#include <Fusion/Platform.h>
#include <Fusion/Types.h>
#include <Fusion/TypeTraits.h>

#include <span>

namespace Fusion
{
//
//
//
template<typename T>
struct FNVA
{
    typedef T Type;

    //
    //
    //
    static T Hash(
        const void* data,
        size_t length,
        T seed,
        const T prime);
};

//
//
//
template<typename T>
struct FNV1A
{
    typedef T Type;

    //
    //
    //
    static T Hash(
        const void* data,
        size_t length,
        T seed,
        const T prime);
};

//
//
//
template<typename Algorithm>
class FNV final
{
public:
    typedef typename Algorithm::Type Type;

public:
    //
    //
    //
    static Type Hash(const void* data, size_t size);

public:
    //
    //
    //
    FNV();

    //
    //
    //
    ~FNV();

    //
    //
    //
    void Data(const void* data, size_t size);

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_scalar<T>::value)>
    void Scalar(const T& obj)
    {
        Data(&obj, sizeof(T));
    }

    //
    //
    //
    void String(const char* str);

    //
    //
    //
    void Reset();

    //
    //
    //
    Type Done() const { return m_seed; };

private:
    Type m_seed{ 0 };
};

//
//
//
using FNV32 = FNV<FNVA<uint32_t>>;

//
//
//
using FNV64 = FNV<FNVA<uint64_t>>;

//
//
//
using FNV1A32 = FNV<FNV1A<uint32_t>>;

//
//
//
using FNV1A64 = FNV<FNV1A<uint64_t>>;

//
//
//
template<typename IntegralType>
class XXHASH final
{
public:
    //
    //
    //
    template<typename T, size_t N, FUSION_REQUIRES(std::is_scalar_v<T>)>
    static IntegralType Hash(
        const T(&data)[N],
        IntegralType seed = 0)
    {
        return HashInternal(data, N * sizeof(T), seed);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_scalar_v<T>)>
    static IntegralType Hash(
        const T* data,
        size_t count,
        IntegralType seed = 0)
    {
        return HashInternal(data, count * sizeof(T), seed);
    }

    //
    //
    //
    template<typename T>
    static IntegralType Hash(
        std::span<const T> data,
        IntegralType seed = 0);

    //
    //
    //
    static IntegralType Hash(
        const void* data,
        size_t size,
        IntegralType seed = 0);

private:
    static IntegralType HashInternal(
        const void* data,
        size_t size,
        IntegralType seed = 0);
};

//
//
//
using XXHASH32 = XXHASH<uint32_t>;

//
//
//
using XXHASH64 = XXHASH<uint64_t>;

#if FUSION_32BIT
//
//
//
template<typename T>
struct Hash<T, XXHASH32, std::enable_if_t<std::is_pod_v<T>>>
{
    size_t operator()(const T& data) const
    {
        return size_t(XXHASH32::Hash(&data, 1));
    }
};
#elif FUSION_64BIT
//
//
//
template<typename T>
struct Hash<T, XXHASH64, std::enable_if_t<std::is_pod_v<T>>>
{
    size_t operator()(const T& data) const
    {
        return size_t(XXHASH64::Hash(&data, 1));
    }
};
#endif

//
//
//
template<typename T, typename Algorithm>
struct Hash<T, Algorithm, std::enable_if_t<std::is_pod_v<T>>>
{
    size_t operator()(const T& data) const
    {
        return size_t(typename Algorithm::Hash(&data, 1));
    }
};

}  // namespace Fusion

#define FUSION_IMPL_HASH 1
#include <Fusion/Impl/Hash.h>
