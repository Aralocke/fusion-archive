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

#include <Fusion/Fixtures/SocketService.h>

namespace Fusion
{
void SocketServiceTests::Consume(
    Connection& conn,
    std::string& data)
{
    Consume(conn, data, conn.readOffset);
    ASSERT_TRUE(conn.read.empty());
    ASSERT_EQ(conn.readOffset, 0);
}

void SocketServiceTests::Consume(
    Connection& conn,
    std::string& data,
    size_t size)
{
    size_t count = std::min(size, conn.readOffset);

    data.reserve(count + 1);
    data.insert(
        begin(data),
        begin(conn.read),
        begin(conn.read) + count);
    conn.read.erase(begin(conn.read), begin(conn.read) + count);
    conn.readOffset -= count;
}

void SocketServiceTests::ProcessRead(
    Manager& m,
    Connection& c)
{
    constexpr size_t READ_SIZE = 128;

    std::vector<char> buf(READ_SIZE);

    size_t size = 0;
    FUSION_ASSERT_RESULT(
        m.network->Recv(
            c.sock,
            buf.data(),
            buf.size()),
        [&](size_t readSize)
        {
            ASSERT_TRUE(readSize);
            c.read.insert(
                begin(c.read) + c.readOffset,
                begin(buf),
                begin(buf) + readSize);
            c.readOffset += readSize;
            size = readSize;
        });

    if (size <= READ_SIZE)
    {
        FUSION_ASSERT_RESULT(
            m.service->Remove(c.sock, SocketOperation::Read));
        c.ops &= ~SocketOperation::Read;
    }
}

void SocketServiceTests::ProcessWrite(
    Manager& m,
    Connection& c)
{
    ASSERT_FALSE(c.write.empty());

    FUSION_ASSERT_RESULT(
        m.network->Send(
            c.sock,
            c.write.data(),
            c.write.size()),
        [&](size_t writeSize)
        {
            ASSERT_TRUE(writeSize);
            c.write.erase(
            c.write.begin(),
            c.write.begin() + writeSize);
            c.writeOffset -= writeSize;
        });

    if (c.write.empty())
    {
        ASSERT_EQ(c.writeOffset, 0);
        FUSION_ASSERT_RESULT(
            m.service->Remove(c.sock, SocketOperation::Write));
        c.ops &= ~SocketOperation::Write;
    }
}

void SocketServiceTests::QueueRead(
    Manager& m,
    Connection& c,
    size_t size)
{
    if (!(+(c.ops & SocketOperation::Read)))
    {
        c.ops |= SocketOperation::Read;
    }
    if (!(+(c.ops & SocketOperation::Error)))
    {
        c.ops |= SocketOperation::Error;
    }
    c.read.reserve(c.read.size() + size);

    FUSION_ASSERT_RESULT(
        m.service->Add(c.sock,
            SocketOperation::Read | SocketOperation::Error));
}

void SocketServiceTests::QueueWrite(
    Manager& m,
    Connection& c,
    std::span<char> data)
{
    if (!(+(c.ops & SocketOperation::Write)))
    {
        c.ops |= SocketOperation::Write;
    }
    if (!(+(c.ops & SocketOperation::Error)))
    {
        c.ops |= SocketOperation::Error;
    }
    c.write.reserve(c.write.size() + data.size());
    {
        c.write.insert(end(c.write), begin(data), end(data));
        c.writeOffset += data.size();
    }
    FUSION_ASSERT_RESULT(
        m.service->Add(c.sock,
            SocketOperation::Write | SocketOperation::Error));
}

void SocketServiceTests::TearDown()
{
    if (pair)
    {
        FUSION_ASSERT_RESULT(pair->Drain());
        pair->Stop().wait();
        pair.reset();
    }
    if (service)
    {
        service->Stop().wait();
        service.reset();
    }
    if (network)
    {
        network->Stop().wait();
        network.reset();
    }
    EXPECT_FALSE(network);
    EXPECT_FALSE(pair);
    EXPECT_FALSE(service);
}

void SocketServiceTests::SetUp()
{
    FUSION_ASSERT_RESULT(
        Network::Create(),
        [&](std::unique_ptr<Network> n) {
            network = std::move(n);
        });
    FUSION_ASSERT_RESULT(
        SocketPair::Create(
            *network,
            SocketPair::Type::NonBlocking),
        [&](std::unique_ptr<SocketPair> p) {
            pair = std::move(p);
        });
}
}  // namespace Fusion
