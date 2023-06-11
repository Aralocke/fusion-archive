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

#include <Fusion/Platform.h>
#include <Fusion/TypeTraits.h>

#if FUSION_COMPILER_MSVC
extern "C"
{
unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64);
unsigned long __cdecl _byteswap_ulong(unsigned long);
unsigned short __cdecl _byteswap_ushort(unsigned short);
}
#endif // FUSION_COMPILER_MSVC

namespace Fusion
{
//
//
//
class Endian final
{
public:
    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_unsigned_v<T>)>
    static constexpr T ConstSwap(T x, T result = 0, size_t step = 0)
    {
        return step == sizeof(T)
            ? result
            : ConstSwap<T>(x >> 8, (result << 8) | (x & T(0xff)), step + 1);
    }

    //
    //
    //
    static inline uint16_t Swap(uint16_t v)
    {
#if FUSION_COMPILER_MSVC
        return _byteswap_ushort(v);
#else
        return (v >> 8) | (v << 8);
#endif
    }

    //
    //
    //
    static inline uint32_t Swap(uint32_t v)
    {
#if FUSION_COMPILER_MSVC
        return _byteswap_ulong(v);
#else
        return __builtin_bswap32(v);
#endif
    }

    //
    //
    //
    static inline uint64_t Swap(uint64_t v)
    {
#if FUSION_COMPILER_MSVC
        return _byteswap_uint64(v);
#else
        return __builtin_bswap64(v);
#endif
    }
};

//
//
//
template<typename T, FUSION_REQUIRES(std::is_unsigned_v<T>)>
class LittleEndian final
{
public:
    //
    //
    //
    static constexpr T Convert(T x)
    {
#if FUSION_LITTLE_ENDIAN
        return x;
#else
        return Endian::ConstSwap(x);
#endif
    }

    //
    //
    //
    static inline T Load(const T* p)
    {
#if FUSION_LITTLE_ENDIAN
        return *p;
#else
        return Endian::Swap(*p);
#endif
    }

    //
    //
    //
    static inline void Store(T* p, T x)
    {
#if FUSION_LITTLE_ENDIAN
        *p = x;
#else
        *p = Endian::Swap(x);
#endif
    }
};

//
//
//
template<typename T, FUSION_REQUIRES(std::is_unsigned_v<T>)>
class BigEndian final
{
public:
    //
    //
    //
    static constexpr T Convert(T x)
    {
#if FUSION_LITTLE_ENDIAN
        return Endian::ConstSwap(x);
#else
        return x;
#endif
    }

    //
    //
    //
    static inline T Load(const T* p)
    {
#if FUSION_LITTLE_ENDIAN
        return Endian::Swap(*p);
#else
        return *p;
#endif
    }

    //
    //
    //
    static inline void Store(T* p, T x)
    {
#if FUSION_LITTLE_ENDIAN
        *p = Endian::Swap(x);
#else
        *p = x;
#endif
    }
};

}  // namespace fusion
