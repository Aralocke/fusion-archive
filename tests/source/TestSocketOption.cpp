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

#include <Fusion/Memory.h>

namespace Fusion
{
class SocketOptionTests : public ::testing::Test
{
public:
    std::unique_ptr<Network> network;
    Socket sock;

public:
    static bool GetSockOpt(
        Socket s,
        SocketOpt option,
        void* data,
        size_t& size)
    {
        using namespace Fusion::Internal;

#if FUSION_PLATFORM_WINDOWS
        int optlen = static_cast<int>(size);
#elif FUSION_PLATFORM_POSIX
        auto optlen = static_cast<socklen_t>(size);
#endif

        int32_t res = ::getsockopt(
            s,
            GetSocketOptLevel(option),
            GetSocketOpt(option),
            reinterpret_cast<char*>(data),
            &optlen);

        return res != SOCKET_ERROR;
    }

public:
    void SetUp() override
    {
        FUSION_ASSERT_RESULT(
            Network::Create(),
            [&](std::unique_ptr<Network> n) {
                ASSERT_TRUE(n);
                network = std::move(n);
            });

        FUSION_ASSERT_RESULT(network->CreateSocket(TCPv4),
            [&](Socket s) {
                ASSERT_TRUE(s != INVALID_SOCKET);
                sock = s;
            });
    }

    void TearDown() override
    {
        if (sock != INVALID_SOCKET)
        {
            FUSION_ASSERT_RESULT(network->Close(sock));
            sock = INVALID_SOCKET;
        }
        if (network)
        {
            network->Stop();
            network.reset();
        }
        EXPECT_FALSE(network);
    }
};
}

TEST_F(SocketOptionTests, Recvbuf)
{
    using namespace SocketOptions;

    constexpr int32_t RECV_BUF_SIZE = 16 * 1024;

    int32_t nativeValue = 0;
    int32_t libValue = 0;

    {
        nativeValue = 0;
        size_t optlen = sizeof(int);

        ASSERT_TRUE(GetSockOpt(
            sock,
            SocketOpt::RecvBuf,
            &nativeValue,
            optlen));
        ASSERT_TRUE(nativeValue > 0);
    }

    libValue = 0;
    FUSION_ASSERT_RESULT(network->GetSocketOption(
        sock,
        RecvBuf(&libValue)));
    ASSERT_EQ(libValue, nativeValue);
    FUSION_ASSERT_RESULT(network->SetSocketOption(
        sock, RecvBuf(RECV_BUF_SIZE)));

    {
        nativeValue = 0;
        size_t optlen = sizeof(int);

        ASSERT_TRUE(GetSockOpt(
            sock,
            SocketOpt::RecvBuf,
            &nativeValue,
            optlen));
        ASSERT_TRUE(nativeValue > 0);
    }

#if FUSION_PLATFORM_POSIX
    // On linux the kernel will double the size set for internal
    // book-keeping on the buffer. We adjust our math to ensure
    // that the test passes.
    ASSERT_EQ(nativeValue, RECV_BUF_SIZE * 2);
#else
    ASSERT_EQ(nativeValue, RECV_BUF_SIZE);
#endif  // FUSION_PLATFORM_POSIX

    libValue = 0;
    FUSION_ASSERT_RESULT(network->GetSocketOption(
        sock,
        RecvBuf(&libValue)));

#if FUSION_PLATFORM_POSIX
    // On linux the kernel will double the size set for internal
    // book-keeping on the buffer. We adjust our math to ensure
    // that the test passes.
    ASSERT_EQ(libValue, RECV_BUF_SIZE * 2);
#else
    ASSERT_EQ(libValue, RECV_BUF_SIZE);
#endif  // FUSION_PLATFORM_POSIX
}

TEST_F(SocketOptionTests, ReuseAddress)
{
    using namespace SocketOptions;

    bool reuse = false;
    FUSION_ASSERT_RESULT(network->GetSocketOption(
        sock, ReuseAddress(&reuse)));
    ASSERT_FALSE(reuse);

    FUSION_ASSERT_RESULT(network->SetSocketOption(
        sock,
        ReuseAddress(true)));
    FUSION_ASSERT_RESULT(network->GetSocketOption(
        sock,
        ReuseAddress(&reuse)));
    ASSERT_TRUE(reuse);
}

TEST_F(SocketOptionTests, SocketType)
{
    using namespace SocketOptions;

    Socket stream{ INVALID_SOCKET };
    FUSION_ASSERT_RESULT(network->CreateSocket(TCPv4),
        [&](Socket s) {
            ASSERT_TRUE(s != INVALID_SOCKET);
            stream = s;
        });

    FUSION_SCOPE_GUARD([&] { network->Close(stream); });

    FUSION_ASSERT_RESULT(network->GetSocketType(stream),
        [&](SocketType type) {
            ASSERT_EQ(type, SocketType::Stream);
        });

    Socket datagram{ INVALID_SOCKET };
    FUSION_ASSERT_RESULT(network->CreateSocket(UDPv4),
        [&](Socket s) {
            ASSERT_TRUE(s != INVALID_SOCKET);
            datagram = s;
        });

    FUSION_SCOPE_GUARD([&] { network->Close(datagram); });

    FUSION_ASSERT_RESULT(network->GetSocketType(datagram),
        [&](SocketType type) {
            ASSERT_EQ(type, SocketType::Datagram);
        });
}
