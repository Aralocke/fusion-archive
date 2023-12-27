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

#include <Fusion/Platform.h>
#if FUSION_PLATFORM_POSIX

#include <Fusion/Internal/StandardNetwork.h>

#include <fcntl.h>

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
    auto length = static_cast<socklen_t>(buffer.size());
    auto* addr = reinterpret_cast<sockaddr*>(buffer.data());

    if (client.sock = ::accept(
       server,
       addr,
       &length); !client.sock)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to accept() on '{}'", server);
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
        sock,
        addr,
        static_cast<socklen_t>(length)); result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to bind socket '{}' to '{}'",
                sock, address);
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

    if (::close(sock) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to close socket '{}'", sock);
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
    auto length = buffer.size();
    auto* addr = address.ToSockAddr(buffer.data(), length);

    if (!addr)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket address");
    }

    if (::connect(
        sock,
        addr,
        static_cast<socklen_t>(length)) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to connect socket '{}' to '{}'",
                sock, address);
    }

    return Success;
}

Result<Socket> StandardNetwork::CreateSocket(
    AddressFamily family,
    SocketProtocol proto,
    SocketType type) const
{
    Socket sock = ::socket(
        Internal::GetAddressFamily(family),
        Internal::GetSocketType(type),
        Internal::GetSocketProtocol(proto));

    if (!sock)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to create socket for {}(family={},protocol={})",
                 type, family, proto);
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
    auto length = static_cast<socklen_t>(buffer.size());
    auto* addr = reinterpret_cast<sockaddr*>(buffer.data());

    if (::getpeername(
        sock,
        addr,
        &length) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to get peer name for '{}'", sock);
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
    auto length = static_cast<socklen_t>(buffer.size());
    auto* addr = reinterpret_cast<sockaddr*>(buffer.data());

    if (::getsockname(
        sock,
        addr,
        &length) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to get socket name for '{}'", sock);
    }

    SocketAddress address;
    address.FromSockAddr(addr);

    return address;
}

Result<void> StandardNetwork::GetSocketOption(
    Socket sock,
    SocketOpt option,
    void* output,
    size_t size) const
{
    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT);
    }

    socklen_t length = static_cast<socklen_t>(size);

    int res = ::getsockopt(
        sock,
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
        sock,
        static_cast<int32_t>(backlog)) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to listen on '{}' (backlog={})",
                sock, backlog);
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

    ssize_t result = ::recv(
        sock,
        buffer,
        size,
        GetMessageOption(flags));

    if (result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed recv() from '{}' (flags={}) for '{}' bytes",
                sock, flags, size);
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
        return {};
    }

    SockAddrStorage buf = { 0 };
    auto length = static_cast<socklen_t>(buf.size());
    auto* addr = reinterpret_cast<sockaddr*>(buf.data());

    ssize_t result = ::recvfrom(
        sock,
        buffer,
        size,
        GetMessageOption(flags),
        addr,
        &length);

    if (result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed recvfrom() from '{}' (flags={}) for '{}' bytes",
                sock, flags, size);
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

    ssize_t result = ::send(
        sock,
        buffer,
        size,
        GetMessageOption(flags));

    if (result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed send() from '{}' (flags={}) for '{}' bytes",
                sock, flags, size);
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

    ssize_t result = ::sendto(
        sock,
        buffer,
        size,
        GetMessageOption(flags),
        addr,
        static_cast<socklen_t>(length));

    if (result == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed sendto() to '{}' (address={},flags={}) for '{}' bytes",
                sock, address, flags, size);
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

    int32_t flags = 0;

    if (auto result = Fcntl::GetFlags(sock); !result)
    {
        return GetLastNetworkFailure()
            .WithContext("failed fcntl get-flags for '{}'", sock);
    }
    else
    {
        flags = *result;
    }

    flags = !blocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    if (auto result = Fcntl::SetFlags(sock, flags); !result)
    {
        return GetLastNetworkFailure()
            .WithContext("failed fcntl set-flags for '{}'", sock);
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
        sock,
        GetSocketOptLevel(option),
        GetSocketOpt(option),
        reinterpret_cast<const char*>(data),
        static_cast<socklen_t>(size));

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

    int32_t how = 0;
    switch (mode)
    {
        case SocketShutdownMode::Read:
            how = SHUT_RD;
            break;
        case SocketShutdownMode::Write:
            how = SHUT_WR;
            break;
        case SocketShutdownMode::Both:
            how = SHUT_RDWR;
            break;
        default:
            return Failure{ E_INVALID_ARGUMENT }
                .WithContext("unknown shutdown mode");
    }

    if (::shutdown(sock, how) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to shutdown() socket '{}' ({})", sock, how);
    }

    return Success;
}

}  // namespace Fusion::Internal

#endif
