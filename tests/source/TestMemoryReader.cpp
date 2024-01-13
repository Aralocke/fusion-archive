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
#include <Fusion/Uuid.h>

class MemoryReaderTests : public testing::Test
{
public:
    static void ComparePackedHeader(
        const PackedHeader& source,
        const PackedHeader& cmp)
    {
        ASSERT_TRUE(memcmp(&source, &cmp, PACKED_HEADER_SIZE) == 0);
        ASSERT_EQ(source.length, cmp.length);
        ASSERT_EQ(source.fnv, cmp.fnv);
    }

    static void ReadPackedHeader(
        MemoryReader& reader,
        PackedHeader& header,
        bool little)
    {
        if (little)
        {
            header.length = reader.Read32_LE();
            header.fnv = reader.Read64_LE();
        }
        else
        {
            header.length = reader.Read32_BE();
            header.fnv = reader.Read64_BE();
        }
    }

    template<typename Archive>
    void VerifyArchive(const Archive& archive, MemoryReader& reader)
    {
        PackedHeader header;
        {
            MemoryReader headerReader = reader.Span(PACKED_HEADER_SIZE);
            ReadPackedHeader(headerReader, header, archive.endian == Endian::LITTLE);
            ASSERT_EQ(headerReader.Remaining(), 0);
        }
        ComparePackedHeader(archive.header, header);
        
        const PackedBlock* blocks = archive.blocks.data();
        size_t blockCount = archive.blocks.size();

        for (size_t i = 0; i < blockCount; ++i)
        {
            const PackedBlock& knownBlock = blocks[i];
            PackedBlock block;

            ReadPackedHeader(
                reader,
                block.header,
                archive.endian == Endian::LITTLE);

            ComparePackedHeader(knownBlock.header, block.header);

            block.data = reader.ReadString(block.header.length);

            ASSERT_EQ(block.data, knownBlock.data);
        }

        PackedFooter footer;
        {
            std::span<const uint8_t> hash = reader.ReadSpan(
                sizeof(PackedFooter::hash));

            ASSERT_EQ(hash.size(), sizeof(footer.hash));
            ASSERT_TRUE(memcmp(
                hash.data(),
                archive.footer.hash,
                hash.size()) == 0);
        }

        ASSERT_EQ(reader.Offset(), reader.Size());
        ASSERT_EQ(reader.Remaining(), 0);
    }

public:
#pragma pack(push, 1)
    struct Order
    {
        uint64_t orderId{ 0 };
        UUID itemId{ EmptyUuid };
        char email[32] = { 0 };
    };
    struct Batch
    {
        uint8_t magic[2] = { 0 };
        uint64_t payloadlength{ 0 };
        uint64_t batchId{ 0 };
        uint32_t orderCount{ 0 };
        Order* orders{ nullptr };
    };
#pragma pack(pop)

    static constexpr size_t kBatchSize = sizeof(Batch);
    static constexpr size_t kBatchHeader = 10;
    static constexpr size_t kPayloadSize = kBatchSize - kBatchHeader;
    static constexpr size_t kOrderSize = sizeof(Order);

    static void CalculateBatchPayload(Batch* batch)
    {
        batch->payloadlength = kPayloadSize + (batch->orderCount * kOrderSize);
    }
};

TEST_F(MemoryReaderTests, TestOrderParsing)
{
    std::vector<Order> orders(3);
    {
        orders[0].orderId = 1;
        orders[0].itemId = *UUID::V4();
        strcpy(orders[0].email, "test1@test1.com");

        orders[1].orderId = 2;
        orders[1].itemId = *UUID::V4();
        strcpy(orders[1].email, "test2@test2.com");

        orders[2].orderId = 3;
        orders[2].itemId = *UUID::V4();
        strcpy(orders[2].email, "test2@test2.com");
    }

    auto batch = std::make_unique<Batch>(Batch{
        .magic = { 0x88, 0x99 },
        .payloadlength = 0,
        .batchId = 1,
        .orderCount = uint32_t(orders.size()),
        .orders = orders.data(),
    });

    CalculateBatchPayload(batch.get());
    
    MemoryReader reader(batch.get(), batch->payloadlength + kBatchHeader);
    ASSERT_EQ(reader.Read(), 0x88);
    ASSERT_EQ(reader.Read(), 0x99);
    ASSERT_EQ(reader.Read64_LE(), batch->payloadlength);

    MemoryReader spanReader = reader.Span(batch->payloadlength);
    {
        // Check the new span has what we need
        ASSERT_EQ(spanReader.Size(), batch->payloadlength);
        ASSERT_EQ(spanReader.Offset(), 0);
        ASSERT_EQ(spanReader.Remaining(), batch->payloadlength);
        // Check that the reader has been expended
        ASSERT_EQ(reader.Remaining(), 0);
    }
    ASSERT_EQ(spanReader.Read64_LE(), batch->batchId);

    uint32_t orderCount = spanReader.Read32_LE();
    ASSERT_EQ(orderCount, batch->orderCount);

    size_t orderLength = size_t(orderCount * kOrderSize);
    MemoryReader orderReader(batch->orders, orderLength);
    ASSERT_EQ(orderReader.Size(), orderLength);

    for (uint32_t i = 0; i < orderCount; ++i)
    {
        const Order& order = batch->orders[i];

        ASSERT_EQ(orderReader.Read64_LE(), order.orderId);

        std::span<const uint8_t> uuid = orderReader.ReadSpan(UUID::SIZE);
        ASSERT_TRUE(memcmp(
            uuid.data(),
            order.itemId.bytes,
            uuid.size()) == 0);

        std::string_view email = orderReader.ReadString(sizeof(Order::email));
        ASSERT_EQ(email, order.email);
    }
}

TEST_F(MemoryReaderTests, ArchivePacking_LE_A)
{
    const auto& archive = PACKED_LE_ARCHIVE_A;
    MemoryReader reader(archive.data);

    VerifyArchive(archive, reader);
}

TEST_F(MemoryReaderTests, ArchivePacking_LE_B)
{
    const auto& archive = PACKED_LE_ARCHIVE_B;
    MemoryReader reader(archive.data);

    VerifyArchive(archive, reader);
}

TEST_F(MemoryReaderTests, ArchivePacking_BE_A)
{
    const auto& archive = PACKED_BE_ARCHIVE_A;
    MemoryReader reader(archive.data);

    VerifyArchive(archive, reader);
}

TEST_F(MemoryReaderTests, ArchivePacking_BE_B)
{
    const auto& archive = PACKED_BE_ARCHIVE_B;
    MemoryReader reader(archive.data);

    VerifyArchive(archive, reader);
}
