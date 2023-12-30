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
    template<typename T>
    bool SystemGetSockOpt(
        Socket sock,
        SocketOpt option,
        T& data)
    {
        constexpr size_t kDataSize = sizeof(T);

#if FUSION_PLATFORM_WINDOWS
        int optlen = static_cast<int>(sizeof(T));
#elif FUSION_PLATFORM_POSIX
        socklen_t optlen = static_cast<socklen_t>(sizeof(T));
#endif

        int32_t res = ::getsockopt(
            sock,
            Internal::GetSocketOptLevel(option),
            Internal::GetSocketOpt(option),
            reinterpret_cast<char*>(&data),
            &optlen);

        if (res != 0)
        {
            Failure err = Failure::Errno();
            const int32_t code = err.Error().platformCode;

            fmt::println("getsockopt(): failed '{}': {}",
                code, err.Code());

            return false;
        }

        if (size_t(optlen) != kDataSize)
        {
            fmt::println("Invalid value size of '{}' (expected: '{}') for option '{}'",
                kDataSize,
                size_t(optlen),
                option);

            return false;
        }
        return true;
    }

    template<typename T>
    bool SystemSetSockOpt(
        Socket sock,
        SocketOpt option,
        T data)
    {
#if FUSION_PLATFORM_WINDOWS
        int32_t optlen = static_cast<int32_t>(sizeof(T));
#elif FUSION_PLATFORM_POSIX
        socklen_t optlen = static_cast<socklen_t>(sizeof(T));
#endif

        return ::setsockopt(
            sock,
            Internal::GetSocketOptLevel(option),
            Internal::GetSocketOpt(option),
            reinterpret_cast<const char*>(&data),
            optlen) == 0;
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

    constexpr int32_t kBufferSize = 16 * 1024;

    int32_t nativeValue = 0;

    {
        nativeValue = 0;

        ASSERT_TRUE(SystemGetSockOpt(
            sock,
            SocketOpt::RecvBuf,
            nativeValue));
        ASSERT_TRUE(nativeValue > 0);
    }

    FUSION_ASSERT_RESULT(
        network->GetSocketOption<RecvBuf>(sock),
        [&](int32_t value) {
            ASSERT_EQ(value, nativeValue);
        });
    
    FUSION_ASSERT_RESULT(
        network->SetSocketOption<RecvBuf>(sock, kBufferSize));

    {
        nativeValue = 0;

        ASSERT_TRUE(SystemGetSockOpt(
            sock,
            SocketOpt::RecvBuf,
            nativeValue));
        ASSERT_TRUE(nativeValue > 0);
    }

#if FUSION_PLATFORM_POSIX
    // On linux the kernel will double the size set for internal
    // book-keeping on the buffer. We adjust our math to ensure
    // that the test passes.
    constexpr size_t kExpectedRecvSize = kBufferSize * 2;
#else
    constexpr size_t kExpectedRecvSize = kBufferSize;
#endif  // FUSION_PLATFORM_POSIX

    ASSERT_EQ(nativeValue, kExpectedRecvSize);

    FUSION_ASSERT_RESULT(
        network->GetSocketOption<RecvBuf>(sock),
        [&](int32_t value) {
            ASSERT_EQ(value, kExpectedRecvSize);
        });
}

TEST_F(SocketOptionTests, ReuseAddress)
{
    using namespace SocketOptions;

    FUSION_ASSERT_RESULT(
        network->GetSocketOption<ReuseAddress>(sock),
        [](bool reuse) {
            ASSERT_FALSE(reuse);
        });

    FUSION_ASSERT_RESULT(
        network->SetSocketOption<ReuseAddress>(sock, true));

    FUSION_ASSERT_RESULT(
        network->GetSocketOption<ReuseAddress>(sock),
        [](bool reuse) {
            ASSERT_TRUE(reuse);
        });
}

TEST_F(SocketOptionTests, CheckSocketType)
{
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

TEST_F(SocketOptionTests, EnsureNagleDisabled)
{
    using namespace SocketOptions;

    ASSERT_TRUE(sock != INVALID_SOCKET);

    // SOCK_STREAM sockets created by the StandardNetwork will set
    // Nagle's to false on creation. We expect TcpNoDelay to be set
    // to true here.

    bool noDelay = false;

    ASSERT_TRUE(SystemGetSockOpt(
        sock,
        SocketOpt::TcpNoDelay,
        noDelay));
    ASSERT_TRUE(noDelay);

    FUSION_ASSERT_RESULT(
        network->GetSocketOption<TcpNoDelay>(sock),
        [](bool value) {
            ASSERT_TRUE(value);
        });
}

TEST_F(SocketOptionTests, GetSetNagleValue)
{
    using namespace SocketOptions;

    Socket newSock = WSASocketW(
        AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
    ASSERT_TRUE(newSock != INVALID_SOCKET);

    FUSION_SCOPE_GUARD([&] { network->Close(newSock); });

    {
        // Check the bool type

        bool nagleCheckBool = true;
        ASSERT_TRUE(SystemGetSockOpt(
            newSock,
            SocketOpt::TcpNoDelay,
            nagleCheckBool));
        ASSERT_FALSE(nagleCheckBool);
    }

    {
        // Check Network Bool Type

        FUSION_ASSERT_RESULT(
            network->GetSocketOption<TcpNoDelay>(newSock),
            [](bool result) {
                ASSERT_FALSE(result);
            });
    }

    {
        // Disable Nagle's Algorithm

        FUSION_ASSERT_RESULT(
            network->SetSocketOption<TcpNoDelay>(
            newSock,
            true));
    }

    {
        // Check Network Bool Type

        FUSION_ASSERT_RESULT(
            network->GetSocketOption<TcpNoDelay>(newSock),
            [](bool result) {
                ASSERT_TRUE(result);
            });
    }
}
