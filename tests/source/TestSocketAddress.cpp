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

TEST(SocketAddressTests, PtrManipulationCopy)
{
    std::array<char, 128U> buffer = { 0 };

    SocketAddress saddr(InaddrLoopback, 8080);
    {
        auto str = ToString(
            saddr.Inet().address,
            buffer.data(),
            buffer.size());

        ASSERT_EQ(str, "127.0.0.1"sv);
    }
    {
        buffer.fill(0);

        auto str = ToString(saddr, buffer.data(), buffer.size());
        ASSERT_EQ(str, "[127.0.0.1]:8080"sv);
    }
}

TEST(SocketAddressTests, ObjectManipulation)
{
    std::array<char, 128U> buffer = { 0 };

    SocketAddress saddr(InaddrLoopback, 8080);
    ASSERT_EQ(
        ToString(saddr, buffer.data(), buffer.size()),
        "[127.0.0.1]:8080"sv);

    SocketAddress copy = saddr;
    ASSERT_TRUE(copy == saddr);

    SocketAddress copyConstruct(copy);
    ASSERT_TRUE(copyConstruct == saddr);
}

TEST(SocketAddressTests, IPAddressLogicalComparisons)
{
    std::array<char, 128U> buffer = { 0 };

    SocketAddress saddr(InaddrLoopback, 8080);
    {
        buffer.fill(0);

        ASSERT_EQ(
            ToString(saddr, buffer.data(), buffer.size()),
            "[127.0.0.1]:8080"sv);
        ASSERT_EQ(saddr.Family(), AddressFamily::Inet4);
        ASSERT_EQ(saddr.Inet().port, 8080);
    }

    SocketAddress saddr4(InaddrAny, 8080);
    {
        buffer.fill(0);

        ASSERT_EQ(
            ToString(saddr4, buffer.data(), buffer.size()),
            "[0.0.0.0]:8080"sv);
        ASSERT_EQ(saddr4.Family(), AddressFamily::Inet4);
        ASSERT_EQ(saddr4.Inet().port, 8080);
    }

    // SocketAddress saddr6{};
    //ASSERT_TRUE(saddr6.FromString("[::ffff:127.0.0.1]:8080"));

    //Inet6Address addr6(saddr6.inet6.addr);
    //ASSERT_TRUE(addr6.IsMappedV4());
}

TEST(SocketAddressTests, FromIPv6Sockaddr)
{
    using namespace Fusion::Internal;

    std::array<char, 128U> buffer = { 0 };

    SocketAddress localhost6(InaddrLoopback6, 8080);
    ASSERT_EQ(
        ToString(localhost6, buffer.data(), buffer.size()),
        "[::1]:8080"sv);

    SockAddrStorage addrBuf = { 0 };
    size_t length = addrBuf.size();
    auto* source6 = localhost6.ToSockAddr(addrBuf.data(), length);
    ASSERT_TRUE(source6);

    {
        SocketAddress addr6{};
        addr6.FromSockAddr(source6);
        ASSERT_EQ(addr6, localhost6);
        ASSERT_EQ(addr6.Family(), AddressFamily::Inet6);
        ASSERT_EQ(addr6.Inet6().address, localhost6.Inet6().address);
        ASSERT_EQ(addr6.Inet6().port, localhost6.Inet6().port);
    }
}

TEST(SocketAddressTests, FromIPv4Sockaddr)
{
    using namespace Fusion::Internal;

    std::array<char, 128U> buffer = { 0 };

    SocketAddress localhost(InaddrLoopback, 8080);
    ASSERT_EQ(
        ToString(localhost, buffer.data(), buffer.size()),
        "[127.0.0.1]:8080"sv);

    SockAddrStorage addrBuf = { 0 };
    size_t length = addrBuf.size();
    auto* source4 = localhost.ToSockAddr(addrBuf.data(), length);
    ASSERT_TRUE(source4);

    {
        SocketAddress addr4{};
        addr4.FromSockAddr(source4);
        ASSERT_EQ(addr4, localhost);
        ASSERT_EQ(addr4.Family(), AddressFamily::Inet4);
        ASSERT_EQ(addr4.Inet().address, localhost.Inet().address);
        ASSERT_EQ(addr4.Inet().port, localhost.Inet().port);
    }
}

TEST(SocketAddressTests, FromIPv6Address)
{
    using namespace Fusion::Internal;

    std::array<char, 128U> buffer = { 0 };

    Inet6Address localhost{};
    {
        ASSERT_TRUE(localhost.FromString("::1"sv));
        ASSERT_EQ(localhost, InaddrLoopback6);
    }

    ASSERT_EQ(ToString(
        localhost,
        buffer.data(),
        buffer.size()), "::1"sv);

    ASSERT_FALSE(localhost.IsMappedV4());

    SocketAddress address{}, source{};
    {
        source = { localhost, 8080 };

        SockAddrStorage buf = { 0 };
        size_t length = buf.size();

        auto* saddr = source.ToSockAddr(buf.data(), length);
        auto* s6Adddress = reinterpret_cast<sockaddr_in6*>(saddr);

        s6Adddress->sin6_port = htons(8080);
        address.FromSockAddr(saddr);
    }

    ASSERT_EQ(address.Family(), AddressFamily::Inet6);
    ASSERT_EQ(address.Inet6().address, localhost);
    ASSERT_EQ(address.Inet6().port, 8080U);
}

TEST(SocketAddressTests, FromIPv4Address)
{
    using namespace Fusion::Internal;

    std::array<char, 128U> buffer = { 0 };

    InetAddress localhost{};
    {
        ASSERT_TRUE(localhost.FromString("127.0.0.1"sv));
        ASSERT_EQ(localhost, InaddrLoopback);
    }

    ASSERT_EQ(ToString(
        localhost,
        buffer.data(),
        buffer.size()), "127.0.0.1"sv);

    SocketAddress address{}, source{};
    {
        source = { localhost, 8080 };

        SockAddrStorage buf = { 0 };
        size_t length = buf.size();

        auto* saddr = source.ToSockAddr(buf.data(), length);
        auto* sAdddress = reinterpret_cast<sockaddr_in*>(saddr);

        sAdddress->sin_port = htons(8080);
        address.FromSockAddr(saddr);
    }

    ASSERT_EQ(address.Family(), AddressFamily::Inet4);
    ASSERT_EQ(address.Inet().address, localhost);
    ASSERT_EQ(address.Inet().port, 8080U);
}
