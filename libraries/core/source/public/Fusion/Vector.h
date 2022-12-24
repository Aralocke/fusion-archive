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

#include <Fusion/Array.h>

#include <array>
#include <memory>
#include <span>

namespace Fusion
{
template<typename T, size_t Length = 0> class EmbeddedVector;

//
//
//
template<typename T>
class EmbeddedVector<T, 0>
{
public:
    EmbeddedVector() = default;
    ~EmbeddedVector();

public:
    using value_type = T;
    using size_type = uint32_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = T*;
    using const_iterator = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //
    //
    //
    const_iterator begin() const;

    //
    //
    //
    iterator begin();

    //
    //
    //
    const_iterator cbegin() const;

    //
    //
    //
    const_iterator end() const;

    //
    //
    //
    iterator end();

    //
    //
    //
    const_iterator cend() const;

public:

    //
    //
    //
    const T& operator[](uint32_t index) const;

    //
    //
    //
    T& operator[](uint32_t index);

    //
    //
    //
    const T& At(uint32_t index) const;

    //
    //
    //
    T& At(uint32_t index);

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
    uint32_t Capacity() const;

    //
    //
    //
    void Clear();

    //
    //
    //
    const T* Data() const;

    //
    //
    //
    T* Data();

    //
    //
    //
    template<typename ...Args>
    iterator EmplaceBack(Args&& ...args);

    //
    //
    //
    bool Empty() const;

    //
    //
    //
    iterator Erase(iterator first, iterator last);

    //
    //
    //
    iterator Erase(iterator pos);

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
    size_t IndexOf(const T& value) const;

    //
    // Copy Insert
    //
    iterator Insert(const T* data, size_t count);

    //
    // Emplace Insert
    //
    iterator Insert(T* data, size_t count);

    //
    //
    //
    iterator Insert(std::span<const T> data);

    //
    //
    //
    iterator Insert(std::span<T> data);

    //
    // Copy Insert At
    //
    iterator Insert(const_iterator at, const T* data, size_t count);

    //
    // Emplace Insert At
    //
    iterator Insert(const_iterator at, T* data, size_t count);

    //
    //
    //
    iterator Insert(const_iterator at, std::span<const T> data);

    //
    //
    //
    iterator Insert(const_iterator at, std::span<T> data);

    //
    //
    //
    iterator Insert(const_iterator at, std::initializer_list<T> values);

    //
    //
    //
    bool IsEmbedded() const;

    //
    //
    //
    T PopBack();

    //
    //
    //
    void PopBack(uint32_t count);

    //
    //
    //
    iterator PushBack(const T& value);

    //
    //
    //
    template<typename ...Args>
    iterator PushBack(Args&& ...args);

    //
    //
    //
    iterator PushBack(std::span<const T> data);

    //
    //
    //
    iterator PushBack(std::span<T> data);

    //
    //
    //
    void Reserve(uint32_t size);

    //
    //
    //
    void Resize(uint32_t size, const T& value = T());

    //
    //
    //
    size_t Size() const;

    //
    //
    //
    operator std::span<T>();

    //
    //
    //
    operator std::span<const T>() const;

protected:

    //
    //
    //
    EmbeddedVector(T* buffer, uint32_t capacity);

    //
    //
    //
    EmbeddedVector(
        EmbeddedVector&& vec,
        T* vBuffer,
        uint32_t vSize,
        T* buffer,
        uint32_t capacity);

    //
    //
    //
    void Assign(
        uint32_t size,
        T* buffer,
        uint32_t capacity);

    //
    //
    //
    void Free();

    //
    //
    //
    void Move(
        EmbeddedVector&& vec,
        T* vBuffer,
        uint32_t vSize,
        T* buffer,
        uint32_t capacity);

    //
    //
    //
    void Reset(T* buffer, uint32_t capacity);

    //
    //
    //
    void Reserve(uint32_t size, bool exact);

    //
    //
    //
    void Realloc(uint32_t size);

private:
    static constexpr uint32_t MaxSize = 0x7FFFFFFE;

    T* m_data = nullptr;
    uint32_t m_offset = 0;
    uint32_t m_capacity:31 = 0;
    uint32_t m_dynamic:1 = 0;
};

template<typename T, size_t Length>
class EmbeddedVector final : public EmbeddedVector<T, 0>
{
private:
    using Base = EmbeddedVector<T, 0>;

public:

    //
    //
    //
    EmbeddedVector();

    //
    //
    //
    template<size_t N>
    EmbeddedVector(const T(&in)[N]);

    //
    //
    //
    EmbeddedVector(std::span<T> in);

    //
    //
    //
    EmbeddedVector(const T* in, size_t size);

    //
    //
    //
    EmbeddedVector(size_t count, const T& in = T());

    //
    //
    //
    EmbeddedVector(const EmbeddedVector& vec);

    //
    //
    //
    EmbeddedVector(EmbeddedVector&& vec);

    //
    //
    //
    template<size_t N>
    EmbeddedVector(const EmbeddedVector<T, N>& vec);

    //
    //
    //
    template<size_t N>
    EmbeddedVector(EmbeddedVector<T, N>&& vec);

    //
    //
    //
    EmbeddedVector(std::initializer_list<T> values);

    //
    //
    //
    EmbeddedVector& operator=(const EmbeddedVector& vec);

    //
    //
    //
    template<size_t N>
    EmbeddedVector& operator=(const EmbeddedVector<T, N>& vect);

    //
    //
    //
    EmbeddedVector& operator=(EmbeddedVector&& vec);

    //
    //
    //
    template<size_t N>
    EmbeddedVector& operator=(EmbeddedVector<T, N>&& vect);

    //
    //
    //
    EmbeddedVector& operator=(std::span<const T> data);

    //
    //
    //
    EmbeddedVector& operator=(std::span<T> data);

    //
    //
    //
    EmbeddedVector& operator=(std::initializer_list<T> values);

    //
    //
    //
    template<size_t N>
    EmbeddedVector& operator=(const T(&in)[N]);

    //
    //
    //
    void Reset();

private:
    template<typename U, size_t N> friend class EmbeddedVector;

private:
    struct Storage
    {
        alignas(T) uint8_t storage[Length][sizeof(T)];

        constexpr size_t Capacity() const { return Length; }
        constexpr const T* Data() const { return reinterpret_cast<const T*>(storage); }
        T* Data() { return reinterpret_cast<T*>(storage); }
    } m_storage;
};

//
//
//
template<typename T, size_t L1, size_t L2>
bool operator==(const EmbeddedVector<T, L1>& a, const EmbeddedVector<T, L2>& b);

//
//
//
template<typename T, size_t L1, size_t L2>
bool operator==(const EmbeddedVector<T, L1>& vec, std::span<const T, L2> span);

//
//
//
template<typename T, size_t L1, size_t L2>
bool operator==(std::span<const T, L1> span, const EmbeddedVector<T, L2>& vec);

//
//
//
template<typename T, size_t L1, size_t L2>
bool operator==(const EmbeddedVector<T, L1>& vec, const Array<T, L2>& arr);

//
//
//
template<typename T, size_t L1, size_t L2>
bool operator==(const Array<T, L1>& arr, const EmbeddedVector<T, L2>& vec);


//
//
//
template<typename T>
std::strong_ordering operator<=>(
    const EmbeddedVector<T>& a,
    const EmbeddedVector<T>& b);

//
//
//
template<typename T, size_t L1, size_t L2>
std::strong_ordering operator<=>(
    const EmbeddedVector<T, L1>& a,
    const EmbeddedVector<T, L2>& b);

}  // namespace Fusion

#define FUSION_IMPL_VECTOR 1
#include <Fusion/Impl/Vector.h>
