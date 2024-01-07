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

#include <Fusion/Internal/Network.h>

#include <array>
#include <chrono>
#include <thread>

class SocketPairTests : public testing::Test
{
public:
    std::unique_ptr<Network> network;
    std::unique_ptr<SocketPair> pair;

    void SetUp() override
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

    void TearDown() override
    {
        if (pair)
        {
            pair->Stop();
            pair.reset();
        }
        if (network)
        {
            network->Stop();
        }
    }
};

TEST_F(SocketPairTests, InitializeAndDrain)
{
    constexpr std::string_view message =
        "This is a test message that will be automatically "
        "drained by the drain call."sv;

    FUSION_ASSERT_RESULT(network->Send(
        pair->Writer(),
        message.data(),
        message.size()),
        [&](size_t writeSize) {
            ASSERT_EQ(writeSize, message.size());
        });
    
    FUSION_ASSERT_RESULT(pair->Drain());
}

TEST_F(SocketPairTests, WriteAndRead)
{
    std::array<char, 128U> buffer = { 0 };

    constexpr std::string_view message = "This is a test message"sv;

    FUSION_ASSERT_RESULT(network->Send(
        pair->Writer(),
        message.data(),
        message.size()),
        [&](size_t writeSize) {
            ASSERT_EQ(writeSize, message.size());
        });

    // Just calling read below is not enough. On some systems the kernel
    // buffer is not readable within the few cycles between instructions
    // and we get a E_OPERATION_WOULD_BLOCK. To remedy this, we create
    // a small while loop to sit on the socket until it is ready.

    size_t readSize = 0;
    Result<size_t> result;

    while (readSize < message.size())
    {
        if (result = network->Recv(
            pair->Reader(),
            buffer.data(),
            buffer.size()); !result)
        {
            if (result.Error().Error() != E_NET_WOULD_BLOCK)
            {
                break;
            }
        }
        else if (size_t count = *result ; count > 0)
        {
            readSize += count;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_TRUE(result);
    ASSERT_EQ(readSize, message.size());
    ASSERT_EQ(std::string_view(buffer.data()), message);
}
