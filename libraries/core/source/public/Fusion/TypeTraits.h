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

#include <Fusion/Compiler.h>
#include <Fusion/Types.h>

#include <type_traits>

#define FUSION_REQUIRED(...) std::enable_if_t<(__VA_ARGS__), decltype(nullptr)>
#define FUSION_REQUIRES(...) FUSION_REQUIRED(__VA_ARGS__) = nullptr

#if FUSION_COMPILER_CLANG
// --- START - Clang Specific
#define FUSION_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#define FUSION_IS_POD_TYPE(T) __is_pod(T)
#define FUSION_IS_TRIVIALLY_CONSTRUCTIBLE(T, ...) __is_trivially_constructible(T, ##__VA_ARGS__)
#define FUSION_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#if FUSION_CLANG_VERSION >= 60000
#define FUSION_IS_TRIVIALLY_DESTRUCTIBLE(T) __is_trivially_destructible(T)
#endif
// --- END - Clang Specific
#elif FUSION_COMPILER_GCC
// --- START - GCC Specific
#define FUSION_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#define FUSION_IS_POD_TYPE(T) __is_pod(T)
#if FUSION_GCC_VERSION >= 50100
#define FUSION_IS_TRIVIALLY_CONSTRUCTIBLE(T, ...) __is_trivially_constructible(T, ##__VA_ARGS__)
#endif
#define FUSION_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
// --- START - GCC Specific
#elif FUSION_COMPILER_MSVC
// --- START - MSVC Specific
#define FUSION_HAS_TRIVIAL_CONSTRUCTOR(T) __has_trivial_constructor(T)
#define FUSION_IS_POD_TYPE(T) __is_pod(T)
#define FUSION_IS_TRIVIALLY_CONSTRUCTIBLE(T, ...) __is_trivially_constructible(T, ##__VA_ARGS__)
#define FUSION_HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#define FUSION_IS_TRIVIALLY_DESTRUCTIBLE(T) __is_trivially_destructible(T)
// --- END - MSVC Specific
#else
#error "Unknown compiler"
#endif

namespace Fusion
{
//
//
//
template<bool b>
struct BoolConstant
{
    static constexpr bool value = b;
};

typedef BoolConstant<true> TrueType;
typedef BoolConstant<false> FalseType;

//
//
//
template<typename T>
struct IsReferenceable
    : std::negation<std::disjunction<std::is_function<T>, std::is_void<T>>>
{
};

//
//
//
template<typename T>
struct HasTrivialConstructor
    : BoolConstant<
        (FUSION_IS_POD_TYPE(T) || FUSION_HAS_TRIVIAL_CONSTRUCTOR(T))
        && std::is_default_constructible_v<T>
        && IsReferenceable<T>::value
    >
{
};

//
//
//
template<class T>
struct HasTrivialDestructor
    : BoolConstant<
        std::is_destructible_v<T>
        && FUSION_HAS_TRIVIAL_DESTRUCTOR(T)
    >
{};

template<typename T>
struct IsBitwiseEqualityComparable
    : BoolConstant<
        std::is_integral_v<T>
        || std::is_enum_v<T>
        || std::is_pointer_v<T>>
{
};

template<typename T>
struct IsLikeByte
    : BoolConstant<sizeof(T) == sizeof(uint8_t)>
{
};

template<typename T>
struct IsByteTriviallyCoppyable
    : BoolConstant<
        IsLikeByte<T>::value
        && std::is_trivially_copyable_v<T>>
{
};
}  // namespace Fusion
