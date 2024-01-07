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

#include <Fusion/New.h>
#include <Fusion/TypeTraits.h>

#include <cstring>
#include <span>

namespace Fusion
{
class MemoryUtil final
{
public:
    //
    //
    //
    template<typename T, FUSION_REQUIRES(
        (std::is_default_constructible_v<T> && std::is_copy_assignable_v<T>)
        && (!HasTrivialConstructor<T>::value || !std::is_trivially_copyable_v<T>)
    )>
    static void Clear(T* data, size_t count)
    {
        while (count-- > 0)
        {
            *data++ = T{};
        }
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(
        (std::is_default_constructible_v<T> && std::is_copy_assignable_v<T>)
        && (HasTrivialConstructor<T>::value && std::is_trivially_copyable_v<T>)
    )>
    static void Clear(T* data, size_t count)
    {
        memset(data, 0, count * sizeof(T));
    }

    //
    //
    //
    template <typename T, size_t N, FUSION_REQUIRES(
        std::is_default_constructible_v<T>
        && std::is_copy_assignable_v<T>)>
    static void Clear(T(&data)[N])
    {
        Clear(data, N);
    }

    //
    //
    //
    template <typename T, size_t N, FUSION_REQUIRES(
        std::is_default_constructible_v<T>
        && std::is_copy_assignable_v<T>)>
    static void Clear(std::span<T> s)
    {
        Clear(s.data(), s.size());
    }

    //
    //
    //
    template <typename T, FUSION_REQUIRES(!IsLikeByte<T>::value)>
    static int Compare(const T* a, const T* b, size_t count)
    {
        for (; count-- > 0; ++a, ++b)
        {
            if (*a < *b)
            {
                return -1;
            }
            if (*b < *a)
            {
                return 1;
            }
        }

        return 0;
    }

    //
    //
    //
    template <typename T, FUSION_REQUIRES(IsLikeByte<T>::value)>
    static int Compare(const T* a, const T* b, size_t count)
    {
        return memcmp(a, b, count * sizeof(T));
    }

    //
    //
    //
    template <typename T>
    static int Compare(const T* a, size_t aCount, const T* b, size_t bCount);

    //
    //
    //
    template<typename T, size_t aCount, size_t bCount>
    static int Compare(const T(&a)[aCount], const T(&b)[bCount]);

    //
    //
    //
    template<typename T>
    static int Compare(std::span<const T> a, std::span<const T> b);

    //
    //
    //
    template<typename T, FUSION_REQUIRES(!std::is_trivially_constructible_v<T>)>
    static T* ContiguousConstruct(void* elements, size_t count)
    {
        constexpr size_t elementSize = sizeof(T);

        uintptr_t ptr = reinterpret_cast<uintptr_t>(elements);
        uintptr_t end = ptr + (elementSize * count);

        for (; ptr < end; ptr += elementSize)
        {
            void* buf = reinterpret_cast<void*>(ptr);
            T* ptr = FUSION_PLACEMENT_NEW(buf) T;

            FUSION_UNUSED(ptr);
        }

        return reinterpret_cast<T*>(elements);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_trivially_constructible_v<T>)>
    static T* ContiguousConstruct(void* elements, size_t count)
    {
        FUSION_UNUSED(count);

        return reinterpret_cast<T*>(elements);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_trivially_destructible_v<T>)>
    static void ContiguousDestruct(T* elements, size_t count)
    {
        // No need to do anything
        FUSION_UNUSED(elements);
        FUSION_UNUSED(count);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(!std::is_trivially_destructible_v<T>)>
    static void ContiguousDestruct(T* elements, size_t count)
    {
        FUSION_UNUSED(elements);
        for (size_t i = 0; i < count; ++i)
        {
            Destruct(&elements[i]);
        }
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_move_constructible_v<T>)>
    static T* ContiguousMoveOrCopyConstruct(void* dest, T* source, size_t count)
    {
        return MoveConstruct<T>(dest, source, count);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(!std::is_move_constructible_v<T>)>
    static T* ContiguousMoveOrCopyConstruct(void* dest, T* source, size_t count)
    {
        return CopyConstruct<T>(dest, source, count);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(
        !std::is_trivially_move_constructible_v<T>
        || !HasTrivialDestructor<T>::value)>
    static void ContiguousMoveOverlapping(T* dest, T* source, size_t count)
    {
        size_t offset = 0;
        size_t dir = 1;

        if (dest > source)
        {
            offset = count - 1;
            dir = -1;
        }

        for (size_t i = 0; i < count; ++i)
        {
            const size_t index = offset + (i * dir);
            ContiguousMoveOrCopyConstruct<T>(dest + index, source + index, 1);
            Destruct<T>(source[index]);
        }
    };

    template<typename T, FUSION_REQUIRES(
        std::is_trivially_move_constructible_v<T>
        && HasTrivialDestructor<T>::value)>
    static void ContiguousMoveOverlapping(T* dest, T* source, size_t count)
    {
        if (count)
        {
            memmove(dest, source, sizeof(T) * count);
        }
    };

    //
    //
    //
    template <typename T, FUSION_REQUIRES(
        std::is_copy_assignable_v<T>
        && !std::is_trivially_copyable_v<T>)>
    static T* Copy(T* dest, const T* source, size_t count)
    {
        while (count-- > 0)
        {
            *dest++ = *source++;
        }
        return dest;
    }

    //
    //
    //
    template <typename T, FUSION_REQUIRES(
        std::is_copy_assignable_v<T>
        && std::is_trivially_copyable_v<T>)>
    static T* Copy(T* dest, const T* source, size_t count)
    {
        memcpy(dest, source, count * sizeof(T));
        return dest + count;
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(
        std::is_copy_constructible_v<T>
        && !std::is_trivially_copyable_v<T>)>
    static T* CopyConstruct(void* buffer, const T* elements, size_t count)
    {
        constexpr size_t elementSize = sizeof(T);
        T* begin = reinterpret_cast<T*>(buffer);

        if (count)
        {
            uintptr_t start = reinterpret_cast<uintptr_t>(buffer);
            uintptr_t end = start + (elementSize * count);

            for (; start < end; start += elementSize)
            {
                void* buf = reinterpret_cast<void*>(start);
                T* ptr = FUSION_PLACEMENT_NEW(buf) T(*elements++);
                FUSION_UNUSED(ptr);
            }
        }

        return begin;
    }

    template<typename T, FUSION_REQUIRES(
        std::is_copy_constructible_v<T>
        && std::is_trivially_copyable_v<T>)>
    static T* CopyConstruct(void* buffer, const T* elements, size_t count)
    {
        if (count)
        {
            memcpy(buffer, elements, count * sizeof(T));
        }
        return static_cast<T*>(buffer);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_trivially_destructible_v<T>)>
    static void Destruct(T* element)
    {
        // No need to do anything
        FUSION_UNUSED(element);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(!std::is_trivially_destructible_v<T>)>
    static void Destruct(T* element)
    {
        FUSION_UNUSED(element);
        element->~T();
    }

    //
    //
    //
    template <typename T, FUSION_REQUIRES(!IsBitwiseEqualityComparable<T>::value)>
    static bool Equal(const T* a, const T* b, size_t count)
    {
        while (count-- > 0)
        {
            if (!(*a++ == *b++))
            {
                return false;
            }
        }

        return true;
    }

    //
    //
    //
    template <typename T, FUSION_REQUIRES(IsBitwiseEqualityComparable<T>::value)>
    static bool Equal(const T* a, const T* b, size_t count)
    {
        return memcmp(a, b, count) == 0;
    }

    //
    //
    //
    template <typename T>
    static bool Equal(const T* a, size_t aCount, const T* b, size_t bCount);

    //
    //
    //
    template<typename T, size_t aCount, size_t bCount>
    static bool Equal(const T(&a)[aCount], const T(&b)[bCount]);

    //
    //
    //
    template<typename T>
    static bool Equal(std::span<const T> a, std::span<const T> b);

    //
    //
    //
    template<typename T, FUSION_REQUIRES(
        std::is_copy_assignable_v<T>
        && !IsByteTriviallyCoppyable<T>::value)>
    static void Fill(T* data, std::add_const<T>& value, size_t count)
    {
        while (count-- > 0)
        {
            *data++ = value;
        }
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(
        std::is_copy_assignable_v<T>
        && IsByteTriviallyCoppyable<T>::value)>
    static void Fill(T* data, std::add_const<T>& value, size_t count)
    {
        memset(data, *reinterpret_cast<const uint8_t*>(&value), count * sizeof(T));
    }

    //
    //
    //
    template<typename T, size_t N, FUSION_REQUIRES(std::is_copy_assignable_v<T>)>
    static void Fill(T(&data)[N], std::add_const<T>& value)
    {
        Fill(data, value, N);
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_copy_assignable_v<T>)>
    static void Fill(std::span<T> data, std::add_const<T>& value)
    {
        Fill(data.data(), value, data.size());
    }

    //
    //
    //
    template <typename T, FUSION_REQUIRES(!IsLikeByte<T>::value)>
    static bool Less(const T* a, const T* b, size_t count)
    {
        for (; count-- > 0; ++a, ++b)
        {
            if (*a < *b)
            {
                return true;
            }
            if (*b < *a)
            {
                return false;
            }
        }

        return false;
    }

    //
    //
    //
    template <typename T, FUSION_REQUIRES(IsLikeByte<T>::value)>
    static bool Less(const T* a, const T* b, size_t count)
    {
        return memcmp(a, b, count * sizeof(T)) < 0;
    }

    //
    //
    //
    template <typename T>
    static bool Less(const T* a, size_t aCount, const T* b, size_t bCount);

    //
    //
    //
    template <typename T, size_t aCount, size_t bCount>
    static bool Less(const T(&a)[aCount], const T(&b)[bCount]);

    //
    //
    //
    template <typename T>
    static bool Less(std::span<const T> a, std::span<const T> b);

    //
    //
    //
    template <typename T, FUSION_REQUIRES(
        std::is_move_assignable_v<T>
        && !std::is_trivially_copyable_v<T>)>
    static T* Move(T* dest, T* source, size_t count)
    {
        while (count-- > 0)
        {
            *dest++ = std::move(*source++);
        }
        return dest;
    }

    //
    //
    //
    template <typename T, FUSION_REQUIRES(
        std::is_move_assignable_v<T>
        && std::is_trivially_copyable_v<T>)>
    static T* Move(T* dest, T* source, size_t count)
    {
        memcpy(dest, source, count * sizeof(T));

        return dest;
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(
        std::is_move_constructible_v<T>
        && !std::is_trivially_move_constructible_v<T>)>
    static T* MoveConstruct(void* buffer, T* elements, size_t count)
    {
        constexpr size_t elementSize = sizeof(T);

        if (!count)
        {
            return reinterpret_cast<T*>(buffer);
        }

        uintptr_t addr = reinterpret_cast<uintptr_t>(buffer);
        uintptr_t term = addr + (elementSize * count);

        T* ptr = FUSION_PLACEMENT_NEW(buffer) \
            T(std::move(*elements++));

        for (addr += elementSize; addr < term; addr += elementSize)
        {
            void* buf = reinterpret_cast<void*>(addr);
            T* ret = FUSION_PLACEMENT_NEW(buf) \
                T(std::move(*elements++));

            FUSION_UNUSED(ret);
        }

        return ptr;
    }

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_trivially_move_constructible_v<T>)>
    static T* MoveConstruct(void* buffer, T* elements, size_t count)
    {
        constexpr size_t elementSize = sizeof(T);

        if (count)
        {
            memcpy(buffer, elements, count * elementSize);
        }

        return static_cast<T*>(buffer);
    }
};
}  // namespace Fusion

#define FUSION_IMPL_MEMORYUTIL 1
#include <Fusion/Impl/MemoryUtil.h>
