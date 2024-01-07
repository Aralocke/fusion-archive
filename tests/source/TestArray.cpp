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

#include <Fusion/Array.h>

TEST(ArrayTests, ConstructInitialize)
{
    using namespace Fusion;

    Array<char, 64> buffer = { 0 };
    {
        ASSERT_EQ(buffer.Size(), 64);
        ASSERT_EQ(buffer.Front(), 0);
    }
    Array<char, 3> buffer2{ {'a', 'b', 'c' } };
    {
        ASSERT_EQ(buffer2.Size(), 3);
        ASSERT_EQ(buffer2[0], 'a');
        ASSERT_EQ(buffer2[1], 'b');
        ASSERT_EQ(buffer2[2], 'c');
    }
    Array<char, 6> buffer3{ buffer2 };
    {
        ASSERT_EQ(buffer3.Size(), 6);
        ASSERT_EQ(buffer3[0], 'a');
        ASSERT_EQ(buffer3[1], 'b');
        ASSERT_EQ(buffer3[2], 'c');
    }
}

TEST(ArrayTests, Copy)
{
    using namespace Fusion;

    Array<char, 3> values { {'a', 'b', 'c' } };
    Array<char, 6> storage;
    {
        storage.Clear();
        ASSERT_EQ(storage[3], 0);
    }
    storage = values;
    {
        ASSERT_EQ(storage[0], 'a');
        ASSERT_EQ(storage[1], 'b');
        ASSERT_EQ(storage[2], 'c');
        ASSERT_EQ(storage[3], 0);
    }
    storage.Clear();
    storage = std::move(values);
    {
        ASSERT_EQ(storage[0], 'a');
        ASSERT_EQ(storage[1], 'b');
        ASSERT_EQ(storage[2], 'c');
        ASSERT_EQ(storage[3], 0);
    }
}

TEST(ArrayTests, Accessors)
{
    using namespace Fusion;

    Array<char, 3> values{ {'a', 'b', 'c' } };
    Array<char, 6> storage;
    {
        storage.Clear();
        ASSERT_EQ(storage.Front(), 0);
        ASSERT_EQ(storage.Back(), 0);
    }

    storage = values;
    {
        storage.Front() = 'd';
        storage.Back() = 'e';
    }

    ASSERT_EQ(storage.Front(), 'd');
    ASSERT_EQ(storage.Back(), 'e');
}
