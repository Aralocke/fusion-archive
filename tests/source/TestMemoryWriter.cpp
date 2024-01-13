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

#include <Fusion/Tests/Data.h>

#include <Fusion/Hash.h>
#include <Fusion/Memory.h>

class MemoryWriterTests : public testing::Test
{
public:
    template<typename Archive>
    void VerifyArchive(const Archive& archive, MemoryWriter& writer)
    {
        size_t blockCount = archive.blocks.size();
        const PackedBlock* blocks = archive.blocks.data();
        size_t length = archive.data.size();

        writer.Skip(12);  // Skip the initial archive header.

        for (size_t i = 0; i < blockCount; ++i)
        {
            const PackedBlock& block = blocks[i];

            // Write the header
            if (archive.endian == Endian::LITTLE)
            {
                writer.Put32_LE(block.header.length);
            }
            else if (archive.endian == Endian::BIG)
            {
                writer.Put32_BE(block.header.length);
            }

            uint64_t fnv64 = 0;
            {
                FNV64 hash;
                hash.Data(block.data.data(), block.data.size());
                fnv64 = hash.Done();
            }
            ASSERT_EQ(fnv64, block.header.fnv);

            if (archive.endian == Endian::LITTLE)
            {
                writer.Put64_LE(fnv64);
            }
            else if (archive.endian == Endian::BIG)
            {
                writer.Put64_BE(fnv64);
            }

            // Write the body
            writer.Put(block.data.data(), block.data.size());
        }

        // Include the offset for the SHA256.
        ASSERT_EQ(writer.Offset() + 32U, length);

        size_t encodedLength = size_t(writer.Offset() - 12U);
        ASSERT_EQ(encodedLength, archive.header.length);

        uint64_t fnv64 = 0;
        {
            FNV64 hash;
            hash.Data(writer.Data() + 12U, encodedLength);
            fnv64 = hash.Done();
        }

        ASSERT_EQ(fnv64, archive.header.fnv);
        size_t headerBlocksLength = writer.Offset();

        MemoryWriter headerWriter = writer.ObjectWriter<PackedHeader>(0);
        if (archive.endian == Endian::LITTLE)
        {
            headerWriter.Put32_LE(0, encodedLength);
            headerWriter.Put64_LE(4, fnv64);
        }
        else if (archive.endian == Endian::BIG)
        {
            headerWriter.Put32_BE(0, encodedLength);
            headerWriter.Put64_BE(4, fnv64);
        }
        ASSERT_EQ(headerWriter.Offset(), headerWriter.Size());
        ASSERT_EQ(headerWriter.Remaining(), 0);

        writer.Seek(headerBlocksLength);
        SHA256::Digest sha256;
        {
            SHA256 hash;
            hash.Process(writer.Data(), headerBlocksLength);
            hash.Finish(sha256);
        }

        ASSERT_TRUE(memcmp(
            sha256.data,
            archive.footer.hash,
            SHA256::Digest::SIZE) == 0);

        writer.Put(sha256.data, sha256.size);

        ASSERT_EQ(writer.Size(), length);

        MD5::Digest md5;
        {
            MD5 hash;
            hash.Process(writer.Data(), writer.Size());
            hash.Finish(md5);
        }

        ASSERT_TRUE(memcmp(
            md5.data,
            archive.md5.data(),
            MD5::Digest::SIZE) == 0);

        ASSERT_TRUE(memcmp(
            archive.data.data(),
            writer.Data(),
            writer.Size()) == 0);
    }
};

TEST_F(MemoryWriterTests, ArchivePacking_LE_A)
{
    const auto& archive = PACKED_LE_ARCHIVE_A;
    auto buffer = std::make_unique<uint8_t[]>(archive.data.size());

    MemoryWriter writer(buffer.get(), archive.data.size());
    VerifyArchive(archive, writer);
}

TEST_F(MemoryWriterTests, ArchivePacking_LE_B)
{
    const auto& archive = PACKED_LE_ARCHIVE_B;
    auto buffer = std::make_unique<uint8_t[]>(archive.data.size());

    MemoryWriter writer(buffer.get(), archive.data.size());
    VerifyArchive(archive, writer);
}

TEST_F(MemoryWriterTests, ArchivePacking_BE_A)
{
    const auto& archive = PACKED_BE_ARCHIVE_A;
    auto buffer = std::make_unique<uint8_t[]>(archive.data.size());

    MemoryWriter writer(buffer.get(), archive.data.size());
    VerifyArchive(archive, writer);
}

TEST_F(MemoryWriterTests, ArchivePacking_BE_B)
{
    const auto& archive = PACKED_BE_ARCHIVE_B;
    auto buffer = std::make_unique<uint8_t[]>(archive.data.size());

    MemoryWriter writer(buffer.get(), archive.data.size());
    VerifyArchive(archive, writer);
}
