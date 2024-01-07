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

#include <Fusion/Endian.h>

TEST(EndianTests, LittleEndian)
{
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint64_t u64 = 0;

    LittleEndian<uint16_t>::Store(&u16, 0x0123);
    LittleEndian<uint32_t>::Store(&u32, 0x01234567);
    LittleEndian<uint64_t>::Store(&u64, 0x0123456789abcdefull);

    ASSERT_EQ(LittleEndian<uint16_t>::Load(&u16), 0x0123);
    ASSERT_EQ(LittleEndian<uint32_t>::Load(&u32), 0x01234567);
    ASSERT_EQ(LittleEndian<uint64_t>::Load(&u64), 0x0123456789abcdefull);

    ASSERT_EQ(BigEndian<uint16_t>::Load(&u16), 0x2301);
    ASSERT_EQ(BigEndian<uint32_t>::Load(&u32), 0x67452301);
    ASSERT_EQ(BigEndian<uint64_t>::Load(&u64), 0xefcdab8967452301ull);
}

TEST(EndianTests, BigEndian)
{
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint64_t u64 = 0;

    BigEndian<uint16_t>::Store(&u16, 0x0123);
    BigEndian<uint32_t>::Store(&u32, 0x01234567);
    BigEndian<uint64_t>::Store(&u64, 0x0123456789abcdefull);

    ASSERT_EQ(BigEndian<uint16_t>::Load(&u16), 0x0123);
    ASSERT_EQ(BigEndian<uint32_t>::Load(&u32), 0x01234567);
    ASSERT_EQ(BigEndian<uint64_t>::Load(&u64), 0x0123456789abcdefull);

    ASSERT_EQ(LittleEndian<uint16_t>::Load(&u16), 0x2301);
    ASSERT_EQ(LittleEndian<uint32_t>::Load(&u32), 0x67452301);
    ASSERT_EQ(LittleEndian<uint64_t>::Load(&u64), 0xefcdab8967452301ull);
}
