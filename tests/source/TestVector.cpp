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

#include <Fusion/Tests/Tests.h>

#include <Fusion/Vector.h>

#include <algorithm>
#include <map>
#include <vector>

namespace Fusion
{
template<typename T>
void Compare(
    const EmbeddedVector<T>& left,
    const EmbeddedVector<T>& right)
{
    ASSERT_EQ(left.Size(), right.Size());
    for (size_t i = 0; i < left.Size(); ++i)
    {
        const T& a = left[i];
        const T& b = right[i];
        ASSERT_EQ(a, b);
    }
}
}

TEST(VectorTests, Construct)
{
    using namespace Fusion;

    Array<uint8_t, 6> valuesA = { { 'a', 'b', 'c', 'd', 'e', 'f' } };
    constexpr uint8_t valuesB[] = { 'g', 'h', 'i', 'j', 'k', 'l' };
    constexpr size_t valuesBSize = sizeof(valuesB);

    // Construct from c-array
    {
        EmbeddedVector<uint8_t, 6> buffer(valuesB);
        ASSERT_EQ(buffer.Size(), valuesBSize);
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer[0], 'g');
        ASSERT_EQ(buffer[valuesBSize - 1], 'l');
    }

    // Construct from pointer + size
    {
        EmbeddedVector<uint8_t, 6> buffer(valuesB, valuesBSize);
        ASSERT_EQ(buffer.Size(), valuesBSize);
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer[0], 'g');
        ASSERT_EQ(buffer[valuesBSize - 1], 'l');
    }

    // construct and initialize
    {
        EmbeddedVector<uint8_t, 6> buffer(8, '\0');
        ASSERT_EQ(buffer.Size(), 8);
        ASSERT_FALSE(buffer.IsEmbedded());
        ASSERT_EQ(buffer[0], '\0');
        ASSERT_EQ(buffer[7], '\0');
    }

    // Construct from span
    {
        EmbeddedVector<uint8_t, 6> buffer(valuesA);
        ASSERT_EQ(buffer.Size(), 6);
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer[0], 'a');
        ASSERT_EQ(buffer[5], 'f');
    }

    // Construct from iterators
    {
        EmbeddedVector<uint8_t, 8> buffer(valuesA.Data(), valuesA.Size());
        ASSERT_EQ(buffer.Size(), 6);
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer[0], 'a');
        ASSERT_EQ(buffer[5], 'f');
    }

    // Construct initializer list
    {
        EmbeddedVector<uint8_t, 6> buffer({ 'm', 'n', 'o' });
        ASSERT_EQ(buffer.Size(), 3);
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer[0], 'm');
        ASSERT_EQ(buffer[2], 'o');
    }
}

TEST(VectorTests, Copy)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> valuesA({ 'a', 'b', 'c', 'd', 'e', 'f' });
    EmbeddedVector<uint8_t, 8> valuesB({ 'g', 'h', 'i', 'j', 'k', 'l' });
    EmbeddedVector<uint8_t, 64> dest(valuesA);

    {
        ASSERT_EQ(valuesA.Size(), 6);
        ASSERT_EQ(dest.Size(), valuesA.Size());
        ASSERT_TRUE(dest.IsEmbedded());
        ASSERT_EQ(dest.Front(), valuesA.Front());
        ASSERT_EQ(dest.Back(), valuesA.Back());
    }

    dest.Clear();
    ASSERT_EQ(dest.Size(), 0);
    ASSERT_TRUE(dest.Empty());

    dest = valuesB;
    {
        ASSERT_EQ(valuesB.Size(), 6);
        ASSERT_EQ(dest.Size(), valuesB.Size());
        ASSERT_TRUE(dest.IsEmbedded());
        ASSERT_EQ(dest.Front(), valuesB.Front());
        ASSERT_EQ(dest.Back(), valuesB.Back());
    }
}

TEST(VectorTests, Move)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> valuesA({ 'a', 'b', 'c', 'd', 'e', 'f' });
    EmbeddedVector<uint8_t, 8> valuesB({ 'g', 'h', 'i', 'j', 'k', 'l' });

    // Move construct
    {
        EmbeddedVector<uint8_t, 8> copy(valuesA);
        EmbeddedVector<uint8_t, 64> dest(std::move(copy));

        ASSERT_EQ(valuesA.Size(), 6);
        ASSERT_EQ(dest.Size(), valuesA.Size());
        ASSERT_TRUE(dest.IsEmbedded());
        ASSERT_EQ(dest.Front(), valuesA.Front());
        ASSERT_EQ(dest.Back(), valuesA.Back());
    }

    // Move assign
    {
        EmbeddedVector<uint8_t, 8> copy(valuesB);
        EmbeddedVector<uint8_t, 64> dest;
        {
            dest = std::move(copy);
        }

        ASSERT_EQ(valuesB.Size(), 6);
        ASSERT_EQ(dest.Size(), valuesB.Size());
        ASSERT_TRUE(dest.IsEmbedded());
        ASSERT_EQ(dest.Front(), valuesB.Front());
        ASSERT_EQ(dest.Back(), valuesB.Back());
    }
}

TEST(VectorTests, Assignment)
{
    using namespace Fusion;

    constexpr auto values = std::to_array<uint8_t>({ 'g', 'h', 'i', 'j', 'k', 'l' });
    std::span<const uint8_t> valueSpan{values};

    EmbeddedVector<uint8_t, 8> buffer;
    {
        buffer = values;
    }
    EmbeddedVector<uint8_t> buffer2;
    {
        buffer2 = buffer;
    }

    ASSERT_EQ(buffer, valueSpan);
    ASSERT_EQ(buffer, buffer2);

    buffer.Reset();
    buffer = valueSpan;
    ASSERT_EQ(buffer, valueSpan);
}

TEST(VectorTests, Accessors)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> buffer({ 'g', 'h', 'i', 'j', 'k', 'l' });
    {
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 6);
        ASSERT_EQ(buffer.Capacity(), 8);
    }

    ASSERT_EQ(buffer.Front(), 'g');
    ASSERT_EQ(buffer.Back(), 'l');
    {
        buffer.Front() = 'a';
        buffer.Back() = 'f';
    }
    ASSERT_EQ(buffer[0], 'a');
    ASSERT_EQ(buffer[5], 'f');

    buffer.Insert(buffer.end(), { 'a', 'b', 'c', 'd', 'e', 'f' });
    {
        ASSERT_FALSE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 12);
        ASSERT_GE(buffer.Capacity(), 12);
    }

    ASSERT_EQ(buffer.Front(), 'a');
    ASSERT_EQ(buffer.Back(), 'f');
    {
        buffer.Front() = 'z';
        buffer.Back() = 'z';
    }
    ASSERT_EQ(buffer[0], 'z');
    ASSERT_EQ(buffer[11], 'z');
}

TEST(VectorTests, Iterators)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> buffer(8, 'd');

    ASSERT_TRUE(std::all_of(
        buffer.begin(),
        buffer.end(),
        [](const uint8_t& v) { return v == 'd'; }));

    for (uint8_t& v : buffer)
    {
        v = 'e';
    }

    ASSERT_TRUE(std::all_of(
        buffer.begin(),
        buffer.end(),
        [](const uint8_t& v) { return v == 'e'; }));
}

TEST(VectorTests, Erase)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> values({ 'a', 'b', 'c', 'd', 'e', 'f' });
    {
        ASSERT_TRUE(values.IsEmbedded());
        ASSERT_EQ(values.Size(), 6);
    }

    values.Erase(values.begin() + 3);
    {
        ASSERT_EQ(values.Size(), 3);
        ASSERT_EQ(values.Front(), 'a');
        ASSERT_EQ(values.Back(), 'c');
    }

    values.Erase(values.begin());
    ASSERT_TRUE(values.Empty());
}

TEST(VectorTests, IndexOf)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> values({ 'a', 'b', 'c', 'd', 'e', 'f' });

    const auto& front = values.Front();
    const auto& back = values.Back();

    ASSERT_EQ(values.IndexOf(front), 0);
    ASSERT_EQ(values.IndexOf(back), 5);

    const auto& value = values[3];
    ASSERT_EQ(values.IndexOf(value), 3);
}

TEST(VectorTests, PopBack)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> values({ 'a', 'b', 'c', 'd', 'e', 'f' });
    {
        ASSERT_TRUE(values.IsEmbedded());
        ASSERT_EQ(values.Size(), 6);
    }

    ASSERT_EQ(values.PopBack(), 'f');
    ASSERT_EQ(values.PopBack(), 'e');
    ASSERT_EQ(values.Size(), 4);

    values.PopBack(3);
    ASSERT_EQ(values.Size(), 1);
    ASSERT_EQ(values.Front(), 'a');
}

TEST(VectorTests, PushBack)
{
    using namespace Fusion;
    
    EmbeddedVector<uint8_t> buffer;
    EmbeddedVector<uint8_t, 8> values({ 'a', 'b', 'c', 'd', 'e', 'f' });
    {
        ASSERT_TRUE(values.IsEmbedded());
        ASSERT_EQ(values.Size(), 6);
    }

    buffer.PushBack('1');
    buffer.PushBack('2');
    {
        ASSERT_FALSE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 2);
        ASSERT_EQ(buffer.Front(), '1');
        ASSERT_EQ(buffer.Back(), '2');
    }

    buffer.PushBack(std::span<const uint8_t>(values));
    {
        ASSERT_EQ(buffer.Size(), 8);
        ASSERT_EQ(buffer.Front(), '1');
        ASSERT_EQ(buffer.Back(), 'f');
    }

    buffer.Clear();
    buffer.PushBack(std::span<uint8_t>(values));
    {
        ASSERT_EQ(buffer.Size(), 6);
        ASSERT_EQ(buffer.Front(), 'a');
        ASSERT_EQ(buffer.Back(), 'f');
    }
}

TEST(VectorTests, EmplaceBack)
{
    using namespace Fusion;
    using iter = EmbeddedVector<std::string, 8>::const_iterator;

    EmbeddedVector<std::string, 8> values;
    iter it1 = values.EmplaceBack("abc");
    iter it2 = values.EmplaceBack("def");
    iter it3 = values.EmplaceBack("ghi");

    ASSERT_EQ(*it1, "abc");
    ASSERT_EQ(*it2, "def");
    ASSERT_EQ(*it3, "ghi");

    ASSERT_EQ(values.Size(), 3);
    ASSERT_EQ(values[0], "abc");
    ASSERT_EQ(values[1], "def");
    ASSERT_EQ(values[2], "ghi");

    EmbeddedVector<uint8_t, 8> valuesB;
    ASSERT_EQ(*valuesB.EmplaceBack('9'), '9');
}

TEST(VectorTests, Reserve)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> buffer;
    {
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 0);
        ASSERT_EQ(buffer.Capacity(), 8);
    }

    buffer.Reserve(4);
    {
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 0);
        ASSERT_EQ(buffer.Capacity(), 8);
    }

    buffer.Reserve(16);
    {
        ASSERT_FALSE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 0);
        ASSERT_GE(buffer.Capacity(), 16);
    }

    // Insert some data to ensure that there is no way it could still
    // be embedded.
    {
        buffer.Insert(
            buffer.begin(),
            { '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        ASSERT_EQ(buffer.Size(), 9);
        ASSERT_EQ(buffer.Front(), '1');
        ASSERT_EQ(buffer.Back(), '9');
    }

    buffer.Reset();
    {
        // After Reset we are back to an embedded state.
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 0);
        ASSERT_EQ(buffer.Capacity(), 8);
    }
}

TEST(VectorTests, Resize)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 8> buffer;
    {
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 0);
        ASSERT_EQ(buffer.Capacity(), 8);
    }

    buffer.Resize(8);
    {
        ASSERT_TRUE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 8);
        ASSERT_EQ(buffer.Capacity(), 8);
    }

    buffer.Clear();
    buffer.Resize(16, 'd');
    {
        ASSERT_FALSE(buffer.IsEmbedded());
        ASSERT_EQ(buffer.Size(), 16);
        ASSERT_GE(buffer.Capacity(), 16);
    }

    ASSERT_TRUE(std::all_of(
        buffer.begin(),
        buffer.end(),
        [](const uint8_t& v) { return v == 'd'; }));
}

TEST(VectorTests, Comparisons)
{
    using namespace Fusion;

    EmbeddedVector<uint8_t, 64> left;
    EmbeddedVector<uint8_t, 32> right;

    Compare(left, right);
}

TEST(VectorTests, SimpleTypes)
{
    using namespace Fusion;

    struct SimpleType
    {
        bool init = false;
        int32_t value = -1;
        double pi = 3.14f;
    };

    EmbeddedVector<SimpleType, 4> values;
    {
        values.EmplaceBack(SimpleType{
            .pi = 4.567f,
        });
        values.EmplaceBack(SimpleType{
            .value = 4,
        });
        values.EmplaceBack(SimpleType{
            .init = true,
            .value = 4,
            .pi = 9.012f,
        });
    }

    ASSERT_EQ(values.Size(), 3);
    ASSERT_DOUBLE_EQ(values[0].pi, 4.567f);
    ASSERT_EQ(values[1].value, 4);

    ASSERT_TRUE(values[2].init);
    ASSERT_EQ(values[2].value, 4);
    ASSERT_DOUBLE_EQ(values[2].pi, 9.012f);
}

TEST(VectorTests, ComplexTypes)
{
    using namespace Fusion;

    struct ComplexType
    {
        std::string str;
        std::vector<int> vec;
        std::map<std::string, int> map;
    };

    EmbeddedVector<ComplexType, 3> values;
    {
        values.EmplaceBack(ComplexType{});
        values.EmplaceBack(ComplexType{
            .str = "abc123",
            .vec = { 1, 2, 3 },
            .map = { { "abc", 123 } }
        });
        values.EmplaceBack(ComplexType{
            .str = "def456",
            .vec = { 4, 5, 6 },
            .map = { { "def", 456 } }
        });
    }

    ASSERT_EQ(values.Size(), 3);
    ASSERT_TRUE(values.IsEmbedded());
    ASSERT_EQ(values.Capacity(), 3);

    // Check basic state of the stored containers
    ASSERT_EQ(values[0].str, "");
    ASSERT_EQ(values[1].str, "abc123");
    ASSERT_EQ(values[2].vec[1], 5);

    // Add a dynamic element that forces the container to move into
    // dynamic memory.

    values.EmplaceBack(ComplexType{
        .str = "ghi789",
        .vec = { 7, 8, 9 },
        .map = { { "ghi", 789 } }
    });

    ASSERT_EQ(values.Size(), 4);
    ASSERT_FALSE(values.IsEmbedded());
    ASSERT_EQ(values.Capacity(), 4);

    // Verify existing, known state
    ASSERT_EQ(values[0].str, "");
    ASSERT_EQ(values[1].str, "abc123");
    ASSERT_EQ(values[2].vec[1], 5);

    // Check the newly added element
    ASSERT_EQ(values[3].map["ghi"], 789);
}
