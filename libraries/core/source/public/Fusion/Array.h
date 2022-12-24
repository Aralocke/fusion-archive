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

#include <Fusion/Types.h>

#include <initializer_list>
#include <iterator>

namespace Fusion
{
//
//
//
template<typename T, size_t Length>
class Array
{
public:
    //
    //
    //
    Array();

    //
    //
    //
    template<size_t SZ>
    Array(const Array<T, SZ>& arr);

    //
    //
    //
    template<size_t SZ>
    Array(Array<T, SZ>&& arr);

    //
    //
    //
    Array(std::initializer_list<T> values);

    ~Array();

public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = T*;
    using const_iterator = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    const T* begin() const;
    T* begin();

    const T* end() const;
    T* end();

public:
    //
    //
    //
    const T& operator[](size_t index) const;

    //
    //
    //
    T& operator[](size_t index);

    //
    //
    //
    template<size_t SZ>
    Array& operator=(const Array<T, SZ>& arr);

    //
    //
    //
    template<size_t SZ>
    Array& operator=(Array<T, SZ>&& arr);

    //
    //
    //
    const T& Back() const;

    //
    //
    //
    T& Back();

    //
    //
    //
    size_t Capacity() const;

    //
    //
    //
    void Clear();

    //
    //
    //
    T* Data();

    //
    //
    //
    const T* Data() const;

    //
    //
    //
    bool Empty() const;

    //
    //
    //
    const T& Front() const;

    //
    //
    //
    T& Front();

    //
    //
    //
    size_t Size() const;

private:
    alignas(T) uint8_t m_storage[Length][sizeof(T)];
};

/*
namespace Internal
{
template<typename T, size_t N>
using ArrayType = Array<std::remove_cv_t<T>, N>;

template <typename T, size_t N, size_t... I>
constexpr ArrayType<T, N> ToArrayImpl(
    T(&values)[N],
    std::index_sequence<I...>)
{
    return { {values[I]...} };
}

template <class T, std::size_t N, std::size_t... I>
constexpr ArrayType<T, N> ToArrayImpl(
    T(&& values)[N],
    std::index_sequence<I...>)
{
    return { {std::move(values[I])...} };
}
}  // namespace Internal

template<typename T, size_t N>
constexpr Internal::ArrayType<T, N> ToArray(T(&values)[N])
{
    return Internal::ToArrayImpl(
        values,
        std::make_index_sequence<N>{});
}

template <typename T, size_t N>
constexpr Internal::ArrayType<T, N> ToArray(T(&&values)[N])
{
    return Internal::ToArrayImpl(
        std::move(values),
        std::make_index_sequence<N>{});
}
*/
}  // namespace Fusion

#define FUSION_IMPL_ARRAY 1
#include <Fusion/Impl/Array.h>
