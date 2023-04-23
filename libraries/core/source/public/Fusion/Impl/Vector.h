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

#if !defined(FUSION_IMPL_VECTOR)
#error "Vector impl included before main header"
#endif

#include <Fusion/Assert.h>
#include <algorithm>

namespace Fusion
{
template<typename T>
EmbeddedVector<T>::EmbeddedVector(
    T* buffer,
    uint32_t capacity)
    : m_data(buffer)
    , m_offset(0)
    , m_capacity(capacity)
{ }

template<typename T>
EmbeddedVector<T>::EmbeddedVector(
    EmbeddedVector&& vec,
    T* vecBuffer,
    uint32_t vecSize,
    T* buffer,
    uint32_t capacity)
{
    Move(std::move(vec), vecBuffer, vecSize, buffer, capacity);
}

template<typename T>
EmbeddedVector<T>::~EmbeddedVector()
{
    if (m_offset != 0)
    {
        MemoryUtil::ContiguousDestruct(begin(), m_offset);
    }

    Free();
}

template<typename T>
const T& EmbeddedVector<T>::operator[](uint32_t index) const
{
    FUSION_ASSERT(index < m_offset);
    return m_data[index];
}

template<typename T>
T& EmbeddedVector<T>::operator[](uint32_t index)
{
    FUSION_ASSERT(index < m_offset);
    return m_data[index];
}

template<typename T>
void EmbeddedVector<T>::Assign(
    uint32_t size,
    T* buffer,
    uint32_t capacity)
{
    if (buffer)
    {
        Reset(buffer, capacity);
    }
    else
    {
        Clear();
    }

    Reserve(size);
}

template<typename T>
const T& EmbeddedVector<T>::At(uint32_t index) const
{
    FUSION_ASSERT(index < m_offset);
    return m_data[index];
}

template<typename T>
T& EmbeddedVector<T>::At(uint32_t index)
{
    FUSION_ASSERT(index < m_offset);
    return m_data[index];
}

template<typename T>
const T& EmbeddedVector<T>::Back() const
{
    FUSION_ASSERT(!Empty());
    return m_data[m_offset - 1];
}

template<typename T>
T& EmbeddedVector<T>::Back()
{
    FUSION_ASSERT(!Empty());
    return m_data[m_offset - 1];
}

template<typename T>
typename EmbeddedVector<T>::const_iterator EmbeddedVector<T>::begin() const
{
    return m_data;
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::begin()
{
    return m_data;
}

template<typename T>
uint32_t EmbeddedVector<T>::Capacity() const
{
    return m_capacity;
}

template<typename T>
typename EmbeddedVector<T>::const_iterator EmbeddedVector<T>::cbegin() const
{
    return m_data;
}

template<typename T>
typename EmbeddedVector<T>::const_iterator EmbeddedVector<T>::cend() const
{
    return &m_data[m_offset];
}

template<typename T>
void EmbeddedVector<T>::Clear()
{
    Resize(0);
}

template<typename T>
const T* EmbeddedVector<T>::Data() const
{
    return m_data;
}

template<typename T>
T* EmbeddedVector<T>::Data()
{
    return m_data;
}

template<typename T>
template<typename ...Args>
typename EmbeddedVector<T>::iterator
EmbeddedVector<T>::EmplaceBack(Args&& ...args)
{
    return PushBack(std::forward<Args>(args)...);
}

template<typename T>
bool EmbeddedVector<T>::Empty() const
{
    return m_offset == 0;
}

template<typename T>
typename EmbeddedVector<T>::const_iterator EmbeddedVector<T>::end() const
{
    return &m_data[m_offset];
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::end()
{
    return &m_data[m_offset];
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Erase(
    iterator pos)
{
    return Erase(pos, end());
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Erase(
    iterator first,
    iterator last)
{
    FUSION_ASSERT(first <= last);
    FUSION_ASSERT(first >= begin() && first <= end());
    FUSION_ASSERT(last >= begin() && last <= end());

    iterator it = std::move(last, end(), first);
    m_offset = static_cast<size_t>(it - begin());
    MemoryUtil::ContiguousDestruct(it, std::distance(first, last));

    return it;
}

template<typename T>
void EmbeddedVector<T>::Free()
{
    if (m_data && m_dynamic)
    {
        std::allocator<T>{}.deallocate(m_data, m_capacity);
    }
}

template<typename T>
const T& EmbeddedVector<T>::Front() const
{
    FUSION_ASSERT(!Empty());
    return *m_data;
}

template<typename T>
T& EmbeddedVector<T>::Front()
{
    FUSION_ASSERT(!Empty());
    return *m_data;
}

template<typename T>
size_t EmbeddedVector<T>::IndexOf(const T& value) const
{
    const T* ptr = &value;

    FUSION_ASSERT(ptr >= begin() && ptr <= end());

    uintptr_t index = static_cast<uintptr_t>(ptr - begin());
    FUSION_ASSERT(index < static_cast<uintptr_t>(m_offset));

    return static_cast<size_t>(index);
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    const T* data,
    size_t count)
{
    return Insert(end(), data, count);
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    T* data,
    size_t count)
{
    return Insert(end(), data, count);
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    std::span<const T> data)
{
    return Insert(end(), data);
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    std::span<T> data)
{
    return Insert(end(), data);
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    const_iterator at,
    const T* data,
    size_t count)
{
    FUSION_ASSERT(at >= begin() && at <= end());

    size_t pos = static_cast<size_t>(std::distance(cbegin(), at));

    if (m_offset + count >= m_capacity)
    {
        Reserve(m_offset + count, true);
    }

    MemoryUtil::ContiguousMoveOverlapping(
        m_data + pos + count,
        m_data + pos,
        m_offset - pos);

    m_offset += count;
    return MemoryUtil::CopyConstruct(m_data + pos, data, count);
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    const_iterator at,
    T* data,
    size_t count)
{
    FUSION_ASSERT(at >= begin() && at <= end());

    size_t pos = static_cast<size_t>(std::distance(cbegin(), at));

    if (m_offset + count >= m_capacity)
    {
        Reserve(m_offset + count, true);
    }

    MemoryUtil::ContiguousMoveOverlapping(
        m_data + pos + count,
        m_data + pos,
        m_offset - pos);

    m_offset += count;
    return MemoryUtil::MoveConstruct(m_data + pos, data, count);
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    const_iterator at,
    std::span<const T> data)
{
    return Insert(at, data.data(), data.size());
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    const_iterator at,
    std::span<T> data)
{
    return Insert(at, data.data(), data.size());
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::Insert(
    const_iterator at,
    std::initializer_list<T> values)
{
    return Insert(at, values.begin(), values.size());
}

template<typename T>
bool EmbeddedVector<T>::IsEmbedded() const
{
    return bool(m_dynamic == 0);
}

template<typename T>
void EmbeddedVector<T>::Move(
    EmbeddedVector&& vec,
    T* vecBuffer,
    uint32_t vecSize,
    T* buffer,
    uint32_t capacity)
{
    if (buffer)
    {
        Reset(buffer, capacity);
    }
    if (vec.m_dynamic)
    {
        m_data = std::exchange(vec.m_data, nullptr);
        m_offset = std::exchange(vec.m_offset, 0);
        m_capacity = vec.m_capacity;
    }
    else
    {
        Reserve(vec.Size());
        Insert(vec.Data(), vec.Size());
    }

    vec.Reset(vecBuffer, vecSize);
}

template<typename T>
T EmbeddedVector<T>::PopBack()
{
    FUSION_ASSERT(m_offset > 0);
    T& original = m_data[--m_offset];
    T value = std::move(original);
    MemoryUtil::Destruct(&original);
    return value;
}

template<typename T>
void EmbeddedVector<T>::PopBack(uint32_t count)
{
    FUSION_ASSERT(m_offset > 0);
    while (m_offset && count--)
    {
        MemoryUtil::Destruct(&m_data[--m_offset]);
    }
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::PushBack(
    const T& value)
{
    Reserve(m_offset + 1, false);
    T* ptr = FUSION_PLACEMENT_NEW(m_data[m_offset++]) T(value);

    return ptr;
}

template<typename T>
template<typename ...Args>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::PushBack(
    Args&& ...args)
{
    Reserve(m_offset + 1, false);
    T* ptr = FUSION_PLACEMENT_NEW(&m_data[m_offset++])
        T(std::forward<Args>(args)...);

    return ptr;
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::PushBack(
    std::span<const T> data)
{
    return Insert(end(), data.data(), data.size());
}

template<typename T>
typename EmbeddedVector<T>::iterator EmbeddedVector<T>::PushBack(
    std::span<T> data)
{
    return Insert(end(), data.data(), data.size());
}

template<typename T>
void EmbeddedVector<T>::Realloc(uint32_t size)
{
    size_t capacity = std::min<size_t>(size, MaxSize);

    T* buffer = std::allocator<T>{}.allocate(capacity);
    MemoryUtil::ContiguousMoveOrCopyConstruct(buffer, m_data, m_offset);
    MemoryUtil::ContiguousDestruct(m_data, m_offset);
    Free();

    m_data = buffer;
    m_capacity = capacity;
    m_dynamic = 1;
}

template<typename T>
void EmbeddedVector<T>::Reserve(uint32_t size)
{
    Reserve(size, true);
}

template<typename T>
void EmbeddedVector<T>::Reset(T* buffer, uint32_t capacity)
{
    if (m_offset)
    {
        MemoryUtil::ContiguousDestruct(begin(), m_offset);
    }

    m_data = buffer;
    m_capacity = capacity;
    m_offset = 0;
    m_dynamic = 0;
}

template<typename T>
void EmbeddedVector<T>::Reserve(uint32_t size, bool exact)
{
    if (size > m_capacity)
    {
        Realloc(
            (exact)
                ? size
                : std::max<size_t>(size, (m_capacity + m_capacity) / 2)
        );
    }
}

template<typename T>
void EmbeddedVector<T>::Resize(uint32_t size, const T& value)
{
    size = std::min(size, MaxSize);

    if (size > m_offset)
    {
        Reserve(size, true);
        for (size_t i = m_offset; i < size; ++i)
        {
            T* ptr = FUSION_PLACEMENT_NEW(&m_data[i]) T(value);
            FUSION_UNUSED(ptr);
        }
    }
    else if (m_offset != 0)
    {
        MemoryUtil::ContiguousDestruct(
            begin() + size,
            m_offset - size);
    }

    m_offset = size;
}

template<typename T>
size_t EmbeddedVector<T>::Size() const
{
    return m_offset;
}

template<typename T>
EmbeddedVector<T>::operator std::span<const T>() const
{
    return { Data(), Size() };
}

template<typename T>
EmbeddedVector<T>::operator std::span<T>()
{
    return { Data(), Size() };
}

// ------------------

template<typename T, size_t Length>
EmbeddedVector<T, Length>::EmbeddedVector()
    : Base(reinterpret_cast<T*>(m_storage), Length)
{ }

template<typename T, size_t Length>
template<size_t N>
EmbeddedVector<T, Length>::EmbeddedVector(const T(&in)[N])
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
    Base::Reserve(N);
    Base::Insert(Base::end(), in, N);
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>::EmbeddedVector(std::span<const T> in)
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
    Base::Reserve(in.size());
    Base::Insert(Base::end(), in.data(), in.size());
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>::EmbeddedVector(const T* in, size_t size)
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
    Base::Reserve(size);
    Base::Insert(Base::end(), in, size);
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>::EmbeddedVector(size_t count, const T& in)
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
    Base::Resize(count, in);
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>::EmbeddedVector(const EmbeddedVector<T, Length>& vec)
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
    Base::Reserve(vec.Size());
    Base::Insert(Base::end(), vec.Data(), vec.Size());
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>::EmbeddedVector(EmbeddedVector<T, Length>&& vec) noexcept
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
     Base::Move(
         std::move(vec),
         reinterpret_cast<T*>(vec.m_storage),
         Length,
         nullptr,
         0);
}

template<typename T, size_t Length>
template<size_t N>
EmbeddedVector<T, Length>::EmbeddedVector(const EmbeddedVector<T, N>& vec)
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
    Base::Reserve(vec.Size());
    Base::Insert(Base::end(), vec.Data(), vec.Size());
}

template<typename T, size_t Length>
template<size_t N>
EmbeddedVector<T, Length>::EmbeddedVector(EmbeddedVector<T, N>&& vec)
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
        Base::Move(
            std::move(vec),
            reinterpret_cast<T*>(vec.m_storage),
            N,
            nullptr,
            0);
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>::EmbeddedVector(std::initializer_list<T> values)
    : Base(reinterpret_cast<T*>(m_storage), Length)
{
    Base::Reserve(values.size());
    Base::Insert(Base::end(), values.begin(), values.size());
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>& EmbeddedVector<T, Length>::operator=(
    const EmbeddedVector<T, Length>& vec)
{
    if (*this != vec)
    {
        Base::Assign(
            vec.Size(),
            reinterpret_cast<T*>(m_storage),
            Length);
        Base::Insert(Base::end(), vec.Data(), vec.Size());
    }
    return *this;
}

template<typename T, size_t Length>
template<size_t N>
EmbeddedVector<T, Length>& EmbeddedVector<T, Length>::operator=(
    const EmbeddedVector<T, N>& vec)
{
    Base::Assign(
        vec.Size(),
        reinterpret_cast<T*>(m_storage),
        Length);
    Base::Insert(Base::end(), vec.Data(), vec.Size());
    return *this;
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>& EmbeddedVector<T, Length>::operator=(
    EmbeddedVector<T, Length>&& vec) noexcept
{
    if (*this != vec)
    {
        Base::Move(
            std::move(vec),
            reinterpret_cast<T*>(vec.m_storage),
            Length,
            reinterpret_cast<T*>(m_storage),
            Length);
    }
    return *this;
}

template<typename T, size_t Length>
template<size_t N>
EmbeddedVector<T, Length>& EmbeddedVector<T, Length>::operator=(
    EmbeddedVector<T, N>&& vec)
{
    Base::Move(
        std::move(vec),
        reinterpret_cast<T*>(vec.m_storage),
        N,
        reinterpret_cast<T*>(m_storage),
        Length);

    return *this;
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>& EmbeddedVector<T, Length>::operator=(std::span<const T> data)
{
    Base::Assign(
        data.size(),
        reinterpret_cast<T*>(m_storage),
        Length);
    Base::Insert(Base::end(), data.data(), data.size());
    return *this;
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>& EmbeddedVector<T, Length>::operator=(std::span<T> data)
{
    Base::Reserve(data.size());
    Base::Insert(Base::end(), data.data(), data.size());

    return *this;
}

template<typename T, size_t Length>
EmbeddedVector<T, Length>& EmbeddedVector<T, Length>::operator=(
    std::initializer_list<T> values)
{
    Base::Assign(
        values.Size(),
        reinterpret_cast<T*>(m_storage),
        Length);
    Base::Insert(Base::end(), values.data(), values.size());
    return *this;
}

template<typename T, size_t Length>
template<size_t N>
EmbeddedVector<T, Length>& EmbeddedVector<T, Length>::operator=(const T(&in)[N])
{
    Base::Assign(N, reinterpret_cast<T*>(m_storage), Length);
    return *this;
}

template<typename T, size_t Length>
void EmbeddedVector<T, Length>::Reset()
{
    Base::Reset(reinterpret_cast<T*>(m_storage), Length);
}

template<typename T, size_t L1, size_t L2>
bool operator==(
    const EmbeddedVector<T, L1>& a,
    const EmbeddedVector<T, L2>& b)
{
    return a.Size() == b.Size()
        && std::equal(a.begin(), a.end(), b.begin());
}

template<typename T, size_t L1, size_t L2>
bool operator==(
    const EmbeddedVector<T, L1>& vec,
    std::span<const T, L2> span)
{
    return vec.Size() == span.size()
        && std::equal(vec.begin(), vec.end(), span.begin());
}

template<typename T, size_t L1, size_t L2>
bool operator==(
    std::span<const T, L1> span,
    const EmbeddedVector<T, L2>& vec)
{
    return vec.Size() == span.size()
        && std::equal(vec.begin(), vec.end(), span.begin());
}

template<typename T, size_t L1, size_t L2>
bool operator==(
    const EmbeddedVector<T, L1>& vec,
    const Array<T, L2>& arr)
{
    return vec.Size() == arr.Size()
        && std::equal(vec.begin(), vec.end(), arr.begin());
}

template<typename T, size_t L1, size_t L2>
bool operator==(
    const Array<T, L1>& arr,
    const EmbeddedVector<T, L2>& vec)
{
    return arr.Size() == vec.Size()
        && std::equal(arr.begin(), arr.end(), vec.begin());
}
}  // namespace Fusion
