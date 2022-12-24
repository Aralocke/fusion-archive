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

#include <Fusion/Tests/Tests.h>

#include <Fusion/Macros.h>
#include <Fusion/Memory.h>
#include <Fusion/MemoryUtil.h>

#include <array>

namespace Fusion
{
template<typename T, FUSION_REQUIRES(std::is_pod_v<T>)>
bool IsEmpty(const T* elements, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        if (elements[i] != 0) return false;
    }
    return true;
}

struct ComplexType
{
    int value = 0;
    int counter = 0;

    ComplexType() = default;

    ComplexType(int v) : value(v) { }

    ComplexType(const ComplexType& t)
        : value(t.value)
        , counter(t.counter + 1)
    { }

    ComplexType(ComplexType&& t) noexcept
        : value(std::exchange(t.value, 0))
        , counter(std::exchange(t.counter, t.counter - 1))
    { }

    ComplexType& operator=(const ComplexType& t)
    {
        value = t.value;
        ++counter;
        return *this;
    }

    ComplexType& operator=(ComplexType&& t) noexcept
    {
        value = std::exchange(t.value, 0);
        counter = std::exchange(t.counter, t.counter - 1);
        return *this;
    }

    friend bool operator==(
        const ComplexType& a,
        const ComplexType& b)
    {
        return a.value == b.value;
    }

    friend bool operator<(
        const ComplexType& a,
        const ComplexType& b)
    {
        return a.value < b.value;
    }
};

struct TrivialType
{
    int i;

    friend bool operator==(
        const TrivialType& a,
        const TrivialType& b)
    {
        return a.i == b.i;
    }

    friend bool operator<(
        const TrivialType& a,
        const TrivialType& b)
    {
        return a.i < b.i;
    }
};

static_assert(!std::is_trivially_constructible_v<ComplexType>,
    "ComplexType is trivially constructible");
static_assert(std::is_trivially_constructible_v<TrivialType>,
    "TrivialType is not trivially constructible");
}

TEST(MemoryUtilTests, ClearComplexTypes)
{
    using namespace Fusion;

    std::array<ComplexType, 4> values = { { 1, 2, 3, 4 } };
    {
        ASSERT_EQ(values[0].value, 1);
        ASSERT_EQ(values[1].value, 2);
        ASSERT_EQ(values[2].value, 3);
        ASSERT_EQ(values[3].value, 4);
    }

    MemoryUtil::Clear(values.data(), values.size());
    {
        ASSERT_EQ(values[0].value, 0);
        ASSERT_EQ(values[1].value, 0);
        ASSERT_EQ(values[2].value, 0);
        ASSERT_EQ(values[3].value, 0);
    }
}

TEST(MemoryUtilTests, ClearPodData)
{
    using namespace Fusion;

    std::array<int, 4> values = { { 1, 2, 3, 4 } };

    ASSERT_FALSE(IsEmpty(values.data(), values.size()));

    MemoryUtil::Clear(values.data(), values.size());
    ASSERT_TRUE(IsEmpty(values.data(), values.size()));
}

TEST(MemoryUtilTests, ClearTrivialTypes)
{
    using namespace Fusion;

    std::array<TrivialType, 4> values = { { 1, 2, 3, 4 } };
    {
        ASSERT_EQ(values[0].i, 1);
        ASSERT_EQ(values[1].i, 2);
        ASSERT_EQ(values[2].i, 3);
        ASSERT_EQ(values[3].i, 4);
    }

    MemoryUtil::Clear(values.data(), values.size());
    {
        ASSERT_EQ(values[0].i, 0);
        ASSERT_EQ(values[1].i, 0);
        ASSERT_EQ(values[2].i, 0);
        ASSERT_EQ(values[3].i, 0);
    }
}

TEST(MemoryUtilTests, CompareComplexData)
{
    using namespace Fusion;

    auto a = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });
    auto b = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });
    auto c = std::to_array<ComplexType>({ { 1 }, { 2 }, { 4 } });
    auto d = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 }, { 4 } });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_EQ(MemoryUtil::Compare<ComplexType>(a, b), 0);
    ASSERT_LT(MemoryUtil::Compare<ComplexType>(a, c), 0);
    ASSERT_EQ(MemoryUtil::Compare<ComplexType>(b, a), 0);
    ASSERT_LT(MemoryUtil::Compare<ComplexType>(b, c), 0);
    ASSERT_GT(MemoryUtil::Compare<ComplexType>(c, a), 0);
    ASSERT_GT(MemoryUtil::Compare<ComplexType>(c, b), 0);

    ASSERT_EQ(MemoryUtil::Compare<ComplexType>(d, d), 0);
    ASSERT_LT(MemoryUtil::Compare<ComplexType>(a, d), 0);
    ASSERT_GT(MemoryUtil::Compare<ComplexType>(d, a), 0);
}

TEST(MemoryUtilTests, ComparePodData)
{
    using namespace Fusion;

    auto a = std::to_array<uint8_t>({ 1, 2, 3 });
    auto b = std::to_array<uint8_t>({ 1, 2, 3 });
    auto c = std::to_array<uint8_t>({ 1, 2, 4 });
    auto d = std::to_array<uint8_t>({ 1, 2, 3, 4 });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_EQ(MemoryUtil::Compare(a.data(), b.data(), a.size()), 0);
    ASSERT_LT(MemoryUtil::Compare(a.data(), c.data(), a.size()), 0);
    ASSERT_EQ(MemoryUtil::Compare(b.data(), a.data(), b.size()), 0);
    ASSERT_LT(MemoryUtil::Compare(b.data(), c.data(), b.size()), 0);
    ASSERT_GT(MemoryUtil::Compare(c.data(), a.data(), c.size()), 0);
    ASSERT_GT(MemoryUtil::Compare(c.data(), b.data(), c.size()), 0);

    ASSERT_EQ(MemoryUtil::Compare(
        a.data(),
        a.size(),
        b.data(),
        b.size()), 0);
    ASSERT_LT(MemoryUtil::Compare(
        a.data(),
        a.size(),
        c.data(),
        c.size()), 0);
    ASSERT_EQ(MemoryUtil::Compare(
        b.data(),
        b.size(),
        a.data(),
        a.size()), 0);
    ASSERT_LT(MemoryUtil::Compare(
        b.data(),
        b.size(),
        c.data(),
        c.size()), 0);
    ASSERT_GT(MemoryUtil::Compare(
        c.data(),
        c.size(),
        a.data(),
        a.size()), 0);
    ASSERT_GT(MemoryUtil::Compare(
        c.data(),
        c.size(),
        b.data(),
        c.size()), 0);

    ASSERT_EQ(MemoryUtil::Compare(
        d.data(),
        d.size(),
        d.data(),
        d.size()), 0);
    ASSERT_LT(MemoryUtil::Compare(
        a.data(),
        a.size(),
        d.data(),
        d.size()), 0);
    ASSERT_GT(MemoryUtil::Compare(
        d.data(),
        d.size(),
        a.data(),
        a.size()), 0);
}

TEST(MemoryUtilTests, CompareTrivialData)
{
    using namespace Fusion;

    auto a = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });
    auto b = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });
    auto c = std::to_array<TrivialType>({ { 1 }, { 2 }, { 4 } });
    auto d = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 }, { 4 } });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_EQ(MemoryUtil::Compare<TrivialType>(a, b), 0);
    ASSERT_LT(MemoryUtil::Compare<TrivialType>(a, c), 0);
    ASSERT_EQ(MemoryUtil::Compare<TrivialType>(b, a), 0);
    ASSERT_LT(MemoryUtil::Compare<TrivialType>(b, c), 0);
    ASSERT_GT(MemoryUtil::Compare<TrivialType>(c, a), 0);
    ASSERT_GT(MemoryUtil::Compare<TrivialType>(c, b), 0);

    ASSERT_EQ(MemoryUtil::Compare<TrivialType>(d, d), 0);
    ASSERT_LT(MemoryUtil::Compare<TrivialType>(a, d), 0);
    ASSERT_GT(MemoryUtil::Compare<TrivialType>(d, a), 0);
}

TEST(MemoryUtilTests, ContiguousConstruct)
{
    static uint8_t s_constructed;
    s_constructed = 0;

    struct Test
    {
        bool constructed;

        Test() : constructed(true) { ++s_constructed; }
        ~Test() { --s_constructed; }
    };

    constexpr size_t elementCount = 4;
    alignas(Test) uint8_t buffer[elementCount * sizeof(Test)] = { 0 };

    Test* elements = MemoryUtil::ContiguousConstruct<Test>(
        buffer,
        elementCount);

    ASSERT_TRUE(elements);
    ASSERT_EQ(s_constructed, elementCount);

    for (uint8_t i = 0; i < elementCount; ++i)
    {
        ASSERT_TRUE(elements[i].constructed);
    }

    MemoryUtil::ContiguousDestruct(elements, elementCount);
    ASSERT_EQ(s_constructed, 0);
}

TEST(MemoryUtils, CopyComplexData)
{
    using namespace Fusion;

    auto a = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });

    ASSERT_EQ(a[0].counter, 0);
    ASSERT_EQ(a[0].value, 1);
    ASSERT_EQ(a[1].counter, 0);
    ASSERT_EQ(a[1].value, 2);
    ASSERT_EQ(a[2].counter, 0);
    ASSERT_EQ(a[2].value, 3);

    std::vector<ComplexType> values(a.size());

    ComplexType* elements = MemoryUtil::Copy(
        values.data(),
        a.data(),
        a.size());

    FUSION_UNUSED(elements);

    ASSERT_EQ(values[0].counter, 1);
    ASSERT_EQ(values[0].value, 1);
    ASSERT_EQ(values[1].counter, 1);
    ASSERT_EQ(values[1].value, 2);
    ASSERT_EQ(values[2].counter, 1);
    ASSERT_EQ(values[2].value, 3);
}

TEST(MemoryUtils, CopyTrivialData)
{
    using namespace Fusion;

    auto a = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });

    ASSERT_EQ(a[0].i, 1);
    ASSERT_EQ(a[1].i, 2);
    ASSERT_EQ(a[2].i, 3);

    std::vector<TrivialType> values(a.size());

    TrivialType* elements = MemoryUtil::Copy(
        values.data(),
        a.data(),
        a.size());

    FUSION_UNUSED(elements);

    ASSERT_EQ(values[0].i, 1);
    ASSERT_EQ(values[1].i, 2);
    ASSERT_EQ(values[2].i, 3);
}

TEST(MemoryUtils, CopyConstructComplexData)
{
    using namespace Fusion;

    auto a = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });
    constexpr size_t typeSize = sizeof(ComplexType);

    ASSERT_EQ(a[0].counter, 0);
    ASSERT_EQ(a[0].value, 1);
    ASSERT_EQ(a[1].counter, 0);
    ASSERT_EQ(a[1].value, 2);
    ASSERT_EQ(a[2].counter, 0);
    ASSERT_EQ(a[2].value, 3);

    std::vector<uint8_t> values(typeSize * a.size());

    ComplexType* elements = MemoryUtil::CopyConstruct(
        values.data(),
        a.data(),
        a.size());

    ASSERT_EQ(elements[0].counter, 1);
    ASSERT_EQ(elements[0].value, 1);
    ASSERT_EQ(elements[1].counter, 1);
    ASSERT_EQ(elements[1].value, 2);
    ASSERT_EQ(elements[2].counter, 1);
    ASSERT_EQ(elements[2].value, 3);
}

TEST(MemoryUtils, CopyConstructTrivialData)
{
    using namespace Fusion;

    auto a = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });
    constexpr size_t typeSize = sizeof(TrivialType);

    ASSERT_EQ(a[0].i, 1);
    ASSERT_EQ(a[1].i, 2);
    ASSERT_EQ(a[2].i, 3);

    std::vector<uint8_t> values(typeSize * a.size());

    TrivialType* elements = MemoryUtil::CopyConstruct(
        values.data(),
        a.data(),
        a.size());

    ASSERT_EQ(elements[0].i, 1);
    ASSERT_EQ(elements[1].i, 2);
    ASSERT_EQ(elements[2].i, 3);
}

TEST(MemoryUtilTests, EqualityComplexData)
{
    using namespace Fusion;

    auto a = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });
    auto b = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });
    auto c = std::to_array<ComplexType>({ { 1 }, { 2 }, { 4 } });
    auto d = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 }, { 4 } });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_TRUE(MemoryUtil::Equal<ComplexType>(a, b));
    ASSERT_TRUE(MemoryUtil::Equal<ComplexType>(b, a));

    ASSERT_FALSE(MemoryUtil::Equal<ComplexType>(a, c));
    ASSERT_FALSE(MemoryUtil::Equal<ComplexType>(c, a));

    ASSERT_TRUE(MemoryUtil::Equal<ComplexType>(d, d));
    ASSERT_FALSE(MemoryUtil::Equal<ComplexType>(a, d));
    ASSERT_FALSE(MemoryUtil::Equal<ComplexType>(d, c));
}

TEST(MemoryUtilTests, EqualityPodData)
{
    using namespace Fusion;

    auto a = std::to_array<uint8_t>({ 1, 2, 3 });
    auto b = std::to_array<uint8_t>({ 1, 2, 3 });
    auto c = std::to_array<uint8_t>({ 1, 2, 4 });
    auto d = std::to_array<uint8_t>({ 1, 2, 3, 4 });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_TRUE(MemoryUtil::Equal(a.data(), b.data(), a.size()));
    ASSERT_FALSE(MemoryUtil::Equal(a.data(), c.data(), a.size()));
    ASSERT_TRUE(MemoryUtil::Equal(a.data(), d.data(), a.size()));

    ASSERT_TRUE(MemoryUtil::Equal(
        a.data(),
        a.size(),
        b.data(),
        b.size()));
    ASSERT_FALSE(MemoryUtil::Equal(
        a.data(),
        a.size(),
        c.data(),
        c.size()));

    ASSERT_TRUE(MemoryUtil::Equal(
        d.data(),
        d.size(),
        d.data(),
        d.size()));
    ASSERT_FALSE(MemoryUtil::Equal(
        a.data(),
        a.size(),
        d.data(),
        d.size()));
    ASSERT_FALSE(MemoryUtil::Equal(
        d.data(),
        d.size(),
        a.data(),
        a.size()));
}

TEST(MemoryUtilTests, EqualityTrivialData)
{
    using namespace Fusion;

    auto a = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });
    auto b = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });
    auto c = std::to_array<TrivialType>({ { 1 }, { 2 }, { 4 } });
    auto d = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 }, { 4 } });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_TRUE(MemoryUtil::Equal<TrivialType>(a, b));
    ASSERT_TRUE(MemoryUtil::Equal<TrivialType>(b, a));

    ASSERT_FALSE(MemoryUtil::Equal<TrivialType>(a, c));
    ASSERT_FALSE(MemoryUtil::Equal<TrivialType>(c, a));

    ASSERT_TRUE(MemoryUtil::Equal<TrivialType>(d, d));
    ASSERT_FALSE(MemoryUtil::Equal<TrivialType>(a, d));
    ASSERT_FALSE(MemoryUtil::Equal<TrivialType>(d, c));
}

TEST(MemoryUtilTests, LessComplexData)
{
    using namespace Fusion;

    auto a = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });
    auto b = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });
    auto c = std::to_array<ComplexType>({ { 1 }, { 2 }, { 4 } });
    auto d = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 }, { 4 } });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_FALSE(MemoryUtil::Less<ComplexType>(a, b));
    ASSERT_TRUE(MemoryUtil::Less<ComplexType>(a, c));
    ASSERT_FALSE(MemoryUtil::Less<ComplexType>(b, a));
    ASSERT_TRUE(MemoryUtil::Less<ComplexType>(b, c));
    ASSERT_FALSE(MemoryUtil::Less<ComplexType>(c, a));
    ASSERT_FALSE(MemoryUtil::Less<ComplexType>(c, b));

    ASSERT_FALSE(MemoryUtil::Less<ComplexType>(d, d));
    ASSERT_TRUE(MemoryUtil::Less<ComplexType>(a, d));
    ASSERT_FALSE(MemoryUtil::Less<ComplexType>(d, a));
}

TEST(MemoryUtilTests, LessPodData)
{
    using namespace Fusion;

    auto a = std::to_array<uint8_t>({ 1, 2, 3 });
    auto b = std::to_array<uint8_t>({ 1, 2, 3 });
    auto c = std::to_array<uint8_t>({ 1, 2, 4 });
    auto d = std::to_array<uint8_t>({ 1, 2, 3, 4 });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_FALSE(MemoryUtil::Less(a.data(), b.data(), a.size()));
    ASSERT_TRUE(MemoryUtil::Less(a.data(), c.data(), a.size()));
    ASSERT_FALSE(MemoryUtil::Less(b.data(), a.data(), b.size()));
    ASSERT_TRUE(MemoryUtil::Less(b.data(), c.data(), b.size()));
    ASSERT_FALSE(MemoryUtil::Less(c.data(), a.data(), c.size()));
    ASSERT_FALSE(MemoryUtil::Less(c.data(), b.data(), c.size()));
}

TEST(MemoryUtilTests, LessTrivialData)
{
    using namespace Fusion;

    auto a = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });
    auto b = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });
    auto c = std::to_array<TrivialType>({ { 1 }, { 2 }, { 4 } });
    auto d = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 }, { 4 } });
    {
        ASSERT_EQ(a.size(), b.size());
        ASSERT_EQ(a.size(), c.size());
    }

    ASSERT_FALSE(MemoryUtil::Less<TrivialType>(a, b));
    ASSERT_TRUE(MemoryUtil::Less<TrivialType>(a, c));
    ASSERT_FALSE(MemoryUtil::Less<TrivialType>(b, a));
    ASSERT_TRUE(MemoryUtil::Less<TrivialType>(b, c));
    ASSERT_FALSE(MemoryUtil::Less<TrivialType>(c, a));
    ASSERT_FALSE(MemoryUtil::Less<TrivialType>(c, b));

    ASSERT_FALSE(MemoryUtil::Less<TrivialType>(d, d));
    ASSERT_TRUE(MemoryUtil::Less<TrivialType>(a, d));
    ASSERT_FALSE(MemoryUtil::Less<TrivialType>(d, a));
}

TEST(MemoryUtils, MoveComplexData)
{
    using namespace Fusion;

    auto a = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });

    ASSERT_EQ(a[0].counter, 0);
    ASSERT_EQ(a[0].value, 1);
    ASSERT_EQ(a[1].counter, 0);
    ASSERT_EQ(a[1].value, 2);
    ASSERT_EQ(a[2].counter, 0);
    ASSERT_EQ(a[2].value, 3);

    std::vector<ComplexType> values;
    values.resize(a.size());

    ComplexType* elements = MemoryUtil::Move(
        values.data(),
        a.data(),
        a.size());

    FUSION_UNUSED(elements);

    ASSERT_EQ(values[0].counter, 0);
    ASSERT_EQ(values[0].value, 1);
    ASSERT_EQ(values[1].counter, 0);
    ASSERT_EQ(values[1].value, 2);
    ASSERT_EQ(values[2].counter, 0);
    ASSERT_EQ(values[2].value, 3);
}

TEST(MemoryUtils, MoveTrivialData)
{
    using namespace Fusion;

    auto a = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });

    ASSERT_EQ(a[0].i, 1);
    ASSERT_EQ(a[1].i, 2);
    ASSERT_EQ(a[2].i, 3);

    std::vector<TrivialType> values;
    values.resize(a.size());

    TrivialType* elements = MemoryUtil::Move(
        values.data(),
        a.data(),
        a.size());

    FUSION_UNUSED(elements);

    ASSERT_EQ(values[0].i, 1);
    ASSERT_EQ(values[1].i, 2);
    ASSERT_EQ(values[2].i, 3);
}

TEST(MemoryUtils, MoveConstructComplexData)
{
    using namespace Fusion;

    auto a = std::to_array<ComplexType>({ { 1 }, { 2 }, { 3 } });
    constexpr size_t typeSize = sizeof(ComplexType);

    ASSERT_EQ(a[0].counter, 0);
    ASSERT_EQ(a[0].value, 1);
    ASSERT_EQ(a[1].counter, 0);
    ASSERT_EQ(a[1].value, 2);
    ASSERT_EQ(a[2].counter, 0);
    ASSERT_EQ(a[2].value, 3);

    std::vector<uint8_t> values(typeSize * a.size());

    ComplexType* elements = MemoryUtil::MoveConstruct(
        values.data(),
        a.data(),
        a.size());

    ASSERT_EQ(elements[0].counter, 0);
    ASSERT_EQ(elements[0].value, 1);
    ASSERT_EQ(elements[1].counter, 0);
    ASSERT_EQ(elements[1].value, 2);
    ASSERT_EQ(elements[2].counter, 0);
    ASSERT_EQ(elements[2].value, 3);
}

TEST(MemoryUtils, MoveConstructTrivialData)
{
    using namespace Fusion;

    auto a = std::to_array<TrivialType>({ { 1 }, { 2 }, { 3 } });
    constexpr size_t typeSize = sizeof(TrivialType);

    ASSERT_EQ(a[0].i, 1);
    ASSERT_EQ(a[1].i, 2);
    ASSERT_EQ(a[2].i, 3);

    std::vector<uint8_t> values(typeSize * a.size());

    TrivialType* elements = MemoryUtil::MoveConstruct(
        values.data(),
        a.data(),
        a.size());

    ASSERT_EQ(elements[0].i, 1);
    ASSERT_EQ(elements[1].i, 2);
    ASSERT_EQ(elements[2].i, 3);
}

TEST(MemoryUtilTests, PlacementNewConstruct)
{
    using namespace Fusion;

    struct Test
    {
        bool b;
        int i;
        float f;
    };

    alignas(Test) uint8_t buffer[sizeof(Test)];
    MemoryUtil::Clear(buffer);

    static_assert(sizeof(Test) <= sizeof(buffer),
        "Insufficient buffer size");

    Test* ptr = nullptr;

    FUSION_SCOPE_GUARD([&] {
        if (ptr) MemoryUtil::Destruct(ptr);
    });

    ptr = FUSION_PLACEMENT_NEW(buffer)
        Test { false, 28, 7.28f };

    ASSERT_TRUE(ptr);
    ASSERT_FALSE(ptr->b);
    ASSERT_EQ(ptr->i, 28);
    ASSERT_DOUBLE_EQ(ptr->f, 7.28f);
}
