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

#include <Fusion/Macros.h>
#include <Fusion/TypeTraits.h>

#define FUSION_ENUM_OPS(T) \
    FUSION_PUSH_WARNINGS() \
    FUSION_DISABLE_GCC_CLANG_WARNING("-Wunused-function") \
    constexpr inline T operator~(T a) \
    { \
        using U = std::underlying_type_t<T>; \
        return T(~U(a)); \
    } \
    constexpr inline T operator&(T a, T b) \
    { \
        using U = std::underlying_type_t<T>; \
        return T(U(a) & U(b)); \
    } \
    constexpr inline T operator|(T a, T b) \
    { \
        using U = std::underlying_type_t<T>; \
        return T(U(a) | U(b)); \
    } \
    constexpr inline T operator^(T a, T b) \
    { \
        using U = std::underlying_type_t<T>; \
        return T(U(a) ^ U(b)); \
    } \
    inline T& operator&=(T& a, T b) \
    { \
        using U = std::underlying_type_t<T>; \
        return a = T(U(a) & U(b)); \
    } \
    inline T& operator|=(T& a, T b) \
    { \
        using U = std::underlying_type_t<T>; \
        return a = T(U(a) | U(b)); \
    } \
    inline T& operator^=(T& a, T b) \
    { \
        using U = std::underlying_type_t<T>; \
        return a = T(U(a) ^ U(b)); \
    } \
    constexpr inline bool operator!(T a) \
    { \
        return a == T{}; \
    } \
    constexpr inline std::underlying_type_t<T> operator+(T a) \
    { \
        using U = std::underlying_type_t<T>; \
        return U(a); \
    } \
    FUSION_POP_WARNINGS()

namespace Fusion
{
//
//
//
class FlagUtil final
{
public:
    template<typename FlagSet>
    static bool IsSet(FlagSet flags, FlagSet s)
    {
        return (flags & s) == s;
    }

    template<typename Flags, typename ...FlagSet>
    static bool IsAnySet(Flags flags, FlagSet ...args)
    {
        return ((IsSet(flags, args)), ...);
    }
};
}  // namespace Fusion
