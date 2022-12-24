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

#if !defined(FUSION_IMPL_ARRAY)
#error "Array impl included before main header"
#endif

#include <Fusion/Assert.h>
#include <Fusion/MemoryUtil.h>

namespace Fusion
{
template<typename T, size_t N>
Array<T, N>::Array()
{
    for (size_t i = 0; i < N; ++i)
    {
        void* buffer = &m_storage[i];
        T* ptr = new (buffer) T;
        FUSION_UNUSED(ptr);
    }
}

template<typename T, size_t N>
template<size_t SZ>
Array<T, N>::Array(const Array<T, SZ>& arr)
{
    static_assert(SZ <= N, "source array is larger than destination");
    MemoryUtil::CopyConstruct(m_storage, arr.Data(), SZ);
}

template<typename T, size_t N>
template<size_t SZ>
Array<T, N>::Array(Array<T, SZ>&& arr)
{
    static_assert(SZ <= N, "source array is larger than destination");
    MemoryUtil::MoveConstruct(m_storage, arr.Data(), SZ);
}

template<typename T, size_t N>
Array<T, N>::Array(std::initializer_list<T> values)
{
    FUSION_ASSERT(values.size() <= N,
        "initializer list is too large for array");

    const T* value = values.begin();
    for (size_t i = 0; i < values.size(); ++i)
    {
        void* buffer = &m_storage[i];
        T* ptr = new (buffer) T(*value++);
        FUSION_UNUSED(ptr);
    }
    for (size_t i = values.size(); i < N; ++i)
    {
        void* buffer = &m_storage[i];
        T* ptr = new (buffer) T();
        FUSION_UNUSED(ptr);
    }
}

template<typename T, size_t N>
Array<T, N>::~Array()
{
    T* ptr = Data();

    for (size_t i = 0; i < N; ++i)
    {
        MemoryUtil::Destruct(&ptr[i]);
    }
}

template<typename T, size_t N>
const T& Array<T, N>::operator[](size_t index) const
{
    FUSION_ASSERT(index <= N);
    const T* ptr = Data();

    return ptr[index];
}

template<typename T, size_t N>
T& Array<T, N>::operator[](size_t index)
{
    FUSION_ASSERT(index <= N);
    T* ptr = Data();

    return ptr[index];
}

template<typename T, size_t N>
template<size_t SZ>
Array<T, N>& Array<T, N>::operator=(const Array<T, SZ>& arr)
{
    static_assert(SZ <= N, "source array is larger than destination");

    const T* input = arr.Data();
    T* output = Data();

    for (size_t i = 0; i < SZ; ++i)
    {
        output[i] = input[i];
    }

    return *this;
}

template<typename T, size_t N>
template<size_t SZ>
Array<T, N>& Array<T, N>::operator=(Array<T, SZ>&& arr)
{
    static_assert(SZ <= N, "source array is larger than destination");

    T* input = arr.Data();
    T* output = Data();

    for (size_t i = 0; i < SZ; ++i)
    {
        output[i] = std::move(input[i]);
    }

    return *this;
}

template<typename T, size_t N>
const T& Array<T, N>::Back() const
{
    const T* ptr = Data();

    return ptr[N - 1];
}

template<typename T, size_t N>
T& Array<T, N>::Back()
{
    T* ptr = Data();

    return ptr[N - 1];
}

template<typename T, size_t N>
const T* Array<T, N>::begin() const
{
    return Data();
}

template<typename T, size_t N>
T* Array<T, N>::begin()
{
    return Data();
}

template<typename T, size_t N>
size_t Array<T, N>::Capacity() const
{
    return N;
}

template<typename T, size_t N>
void Array<T, N>::Clear()
{
    memset(m_storage, 0, sizeof(T) * N);
}

template<typename T, size_t N>
const T* Array<T,N>::Data() const
{
    return reinterpret_cast<const T*>(m_storage);
}

template<typename T, size_t N>
T* Array<T, N>::Data()
{
    return reinterpret_cast<T*>(m_storage);
}

template<typename T, size_t N>
bool Array<T, N>::Empty() const
{
    return begin() == end();
}

template<typename T, size_t N>
const T* Array<T, N>::end() const
{
    const T* ptr = Data();

    return ptr + N;
}

template<typename T, size_t N>
T* Array<T, N>::end()
{
    T* ptr = Data();

    return ptr + N;
}

template<typename T, size_t N>
const T& Array<T, N>::Front() const
{
    const T* ptr = Data();

    return *ptr;
}

template<typename T, size_t N>
T& Array<T, N>::Front()
{
    T* ptr = Data();

    return *ptr;
}

template<typename T, size_t N>
size_t Array<T, N>::Size() const
{
    return N;
}

template<typename T>
class Array<T, 0> final
{
public:
    Array() = default;
    ~Array() override = default;

    Array& operator=(const Array&) { return *this; }
    Array& operator=(Array&&) { return *this; }

public:
    T* begin() { return nullptr; }
    const T* begin() const { return nullptr; }
    T* end() { return nullptr; }
    const T* end() const { return nullptr; }

public:
    T* Data() { return nullptr; }
    const T* Data() const { return nullptr; }
    size_t Size() const { return 0; }
};
}
