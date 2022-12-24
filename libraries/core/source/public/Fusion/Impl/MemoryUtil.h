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

#if !defined(FUSION_IMPL_MEMORYUTIL)
#error "MemoryUtil impl included before main header"
#endif

namespace Fusion
{
template <typename T>
int MemoryUtil::Compare(const T* a, size_t aCount, const T* b, size_t bCount)
{
    int result = Compare(a, b, std::min(aCount, bCount));

    if (result != 0)
    {
        return result;
    }
    else if (aCount < bCount)
    {
        return -1;
    }
    else if (aCount > bCount)
    {
        return 1;
    }
    return 0;
}

template<typename T, size_t aCount, size_t bCount>
int MemoryUtil::Compare(const T(&a)[aCount], const T(&b)[bCount])
{
    return Compare(a, aCount, b, bCount);
}

template<typename T>
int MemoryUtil::Compare(std::span<const T> a, std::span<const T> b)
{
    return Compare(a.data(), a.size(), b.data(), b.size());
}

template <typename T>
bool MemoryUtil::Equal(const T* a, size_t aCount, const T* b, size_t bCount)
{
    return (aCount == bCount) ? Equal(a, b, aCount) : false;
}

template<typename T, size_t aCount, size_t bCount>
bool MemoryUtil::Equal(const T(&a)[aCount], const T(&b)[bCount])
{
    return Equal(a, aCount, b, bCount);
}

template<typename T>
bool MemoryUtil::Equal(std::span<const T> a, std::span<const T> b)
{
    return Equal(a.data(), a.size(), b.data(), b.size());
}

template <typename T>
bool MemoryUtil::Less(const T* a, size_t aCount, const T* b, size_t bCount)
{
    if (Less<T>(a, b, std::min(aCount, bCount)))
    {
        return true;
    }
    return aCount < bCount;
}

template <typename T, size_t aCount, size_t bCount>
bool MemoryUtil::Less(const T(&a)[aCount], const T(&b)[bCount])
{
    return Less<T>(a, aCount, b, bCount);
}

template <typename T>
bool MemoryUtil::Less(std::span<const T> a, std::span<const T> b)
{
    return Less<T>(a.data(), a.size(), b.data(), b.size());
}
}  // namespace Fusion
