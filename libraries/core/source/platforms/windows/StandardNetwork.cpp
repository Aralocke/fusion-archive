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

#include <Fusion/Internal/StandardNetwork.h>

#if FUSION_PLATFORM_WINDOWS

#include <array>
#include <cerrno>
#include <cstring>
#include <tuple>

namespace Fusion::Internal
{
Result<Network::AcceptedSocketData>
StandardNetwork::Accept(Socket server) const
{
    AcceptedSocketData client;

    if (server == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    SockAddrStorage buffer = { 0 };
    int length = static_cast<int>(buffer.size());
    auto* addr = reinterpret_cast<sockaddr*>(buffer.data());

    client.sock = ::accept(
        static_cast<SOCKET>(server),
        addr,
        &length);

    if (!client.sock)
    {
        return GetLastNetworkFailure();
    }

    client.address.FromSockAddr(addr);
    return client;
}

Result<void> StandardNetwork::Bind(
    Socket sock,
    const SocketAddress& address) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    SockAddrStorage buffer = { 0 };
    size_t length = buffer.size();
    auto* addr = address.ToSockAddr(buffer.data(), length);

    if (!addr)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket address");
    }

    if (int result = ::bind(
        static_cast<SOCKET>(sock),
        addr,
        static_cast<int>(length)); result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    return Success;
}

Result<void> StandardNetwork::Close(Socket sock) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    if (::closesocket(static_cast<SOCKET>(sock)) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    return Success;
}

Result<void> StandardNetwork::Connect(
    Socket sock,
    const SocketAddress& address) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    SockAddrStorage buffer = { 0 };
    size_t length = buffer.size();
    auto* addr = address.ToSockAddr(buffer.data(), length);

    if (!addr)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket address");
    }

    if (::connect(
        static_cast<SOCKET>(sock),
        addr,
        static_cast<int>(length)) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    return Success;
}

Result<Socket> StandardNetwork::CreateSocket(
    AddressFamily family,
    SocketProtocol proto,
    SocketType type) const
{
    Socket sock = WSASocketW(
        Internal::GetAddressFamily(family),
        Internal::GetSocketType(type),
        Internal::GetSocketProtocol(proto),
        nullptr,
        0,
        0);

    if (!sock)
    {
        return GetLastNetworkFailure();
    }

    return sock;
}

Result<SocketAddress>
StandardNetwork::GetPeerName(Socket sock) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    SockAddrStorage buffer = { 0 };
    int length = static_cast<int>(buffer.size());
    auto* addr = reinterpret_cast<sockaddr*>(buffer.data());

    if (::getpeername(
        static_cast<SOCKET>(sock),
        addr,
        &length) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    SocketAddress address;
    address.FromSockAddr(addr);

    return address;
}

Result<SocketAddress>
StandardNetwork::GetSockName(Socket sock) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    SockAddrStorage buffer = { 0 };
    int length = static_cast<int>(buffer.size());
    auto* addr = reinterpret_cast<sockaddr*>(buffer.data());

    if (::getsockname(
        static_cast<SOCKET>(sock),
        addr,
        &length) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    SocketAddress address;
    address.FromSockAddr(addr);

    return address;
}

Result<void> StandardNetwork::GetSocketOption(
    Socket sock,
    SocketOpt option,
    void* output,
    size_t& size) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT);
    }

    int length = static_cast<int>(size);
    int res = ::getsockopt(
        static_cast<SOCKET>(sock),
        GetSocketOptLevel(option),
        GetSocketOpt(option),
        reinterpret_cast<char*>(output),
        &length);

    if (res == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to get socket option '{}' for '{}'",
                option,
                sock);
    }

    size = static_cast<size_t>(length);
    return Success;
}

Result<void> StandardNetwork::Listen(
    Socket sock,
    uint32_t backlog) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    if (::listen(
        static_cast<SOCKET>(sock),
        static_cast<int32_t>(backlog)) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    return Success;
}

Result<size_t> StandardNetwork::Recv(
    Socket sock,
    void* buffer,
    size_t size,
    MessageOption flags) const
{
    FUSION_ASSERT(buffer);
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    if (size == 0)
    {
        return 0;
    }

    int result = ::recv(
        static_cast<SOCKET>(sock),
        static_cast<char*>(buffer),
        static_cast<int>(size),
        GetMessageOption(flags));

    if (result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    if (result == 0)
    {
        return Failure(E_NET_DISCONNECTED);
    }

    return size_t(result);
}

Result<Network::RecvFromData> StandardNetwork::RecvFrom(
    Socket sock,
    void* buffer,
    size_t size,
    MessageOption flags) const
{
    FUSION_ASSERT(buffer);
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    if (size == 0)
    {
        return 0;
    }

    SockAddrStorage buf = { 0 };
    int length = static_cast<int>(buf.size());
    auto* addr = reinterpret_cast<sockaddr*>(buf.data());

    int result = ::recvfrom(
        static_cast<SOCKET>(sock),
        static_cast<char*>(buffer),
        static_cast<int>(size),
        GetMessageOption(flags),
        addr,
        &length);

    if (result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    RecvFromData data;
    data.address.FromSockAddr(addr);
    data.buffer = buffer;
    data.size = size;
    data.received = size_t(result);

    return data;
}

Result<size_t> StandardNetwork::Send(
    Socket sock,
    const void* buffer,
    size_t size,
    MessageOption flags) const
{
    FUSION_ASSERT(buffer);
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    if (size == 0)
    {
        return 0;
    }

    int result = ::send(
        static_cast<SOCKET>(sock),
        static_cast<const char*>(buffer),
        static_cast<int>(size),
        GetMessageOption(flags));

    if (result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    return size_t(result);
}

Result<size_t> StandardNetwork::SendTo(
    Socket sock,
    const SocketAddress& address,
    const void* buffer,
    size_t size,
    MessageOption flags) const
{
    FUSION_ASSERT(buffer);
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    if (size == 0)
    {
        return 0;
    }

    SockAddrStorage buf = { 0 };
    size_t length = buf.size();
    auto* addr = address.ToSockAddr(buf.data(), length);

    if (!addr)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket address");
    }

    int result = ::sendto(
        static_cast<SOCKET>(sock),
        static_cast<const char*>(buffer),
        static_cast<int>(size),
        GetMessageOption(flags),
        addr,
        static_cast<int>(length));

    if (result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    return size_t(result);
}

Result<void> StandardNetwork::SetBlocking(
    Socket sock,
    bool blocking) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    uint32_t status = blocking ? 0 : 1;

    if (auto err = Ioctl::SetOption(sock, FIONBIO, status); !err)
    {
        return err;
    }

    return Success;
}

Result<void> StandardNetwork::SetSocketOption(
    Socket sock,
    SocketOpt option,
    const void* data,
    size_t size) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT);
    }

    int res = ::setsockopt(
        static_cast<SOCKET>(sock),
        GetSocketOptLevel(option),
        GetSocketOpt(option),
        reinterpret_cast<const char*>(data),
        static_cast<int>(size));

    if (res == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to get socket option '{}' for '{}'",
                option,
                sock);
    }

    return Success;
}

Result<void> StandardNetwork::Shutdown(
    Socket sock,
    SocketShutdownMode mode) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    int how = 0;
    switch (mode)
    {
    case SocketShutdownMode::Read:
        how = SD_RECEIVE;
        break;
    case SocketShutdownMode::Write:
        how = SD_SEND;
        break;
    case SocketShutdownMode::Both:
        how = SD_BOTH;
        break;
    default:
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("unknown shutdown mode");
    }

    if (::shutdown(static_cast<SOCKET>(sock), how) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure();
    }

    return Success;
}
}
#endif  // FUSION_PLATFORM_POSIX
