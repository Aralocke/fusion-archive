/**
 * Copyright 2015-2022 Daniel Weiner
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

#include <Fusion/Internal/Network.h>

#if FUSION_PLATFORM_WINDOWS

namespace Fusion
{
// -------------------------------------------------------------
// WSAInitializer                                          START
//
Internal::WSAInitializer Internal::WSAInitializer::s_instance{};

Internal::WSAInitializer::WSAInitializer()
{
    if (WSAStartup(MAKEWORD(2, 2), &m_wsadata) != 0)
    {
        ::exit(EXIT_FAILURE);
    }
    if (HIBYTE(m_wsadata.wVersion) != 2 || LOBYTE(m_wsadata.wVersion) != 2)
    {
        ::exit(EXIT_FAILURE);
    }
}

Internal::WSAInitializer::~WSAInitializer()
{
    WSACleanup();
}
// WSAInitializer                                            END
// -------------------------------------------------------------
// Fcntl                                                   START
Result<int32_t> Internal::Fcntl::GetFlags(Socket sock)
{
    FUSION_UNUSED(sock);

    return Failure{ E_NOT_SUPPORTED };
}

Result<void> Internal::Fcntl::SetFlags(Socket sock, int32_t flags)
{
    FUSION_UNUSED(sock);
    FUSION_UNUSED(flags);

    return Failure{ E_NOT_SUPPORTED };
}
// Fcntl                                                     END
// -------------------------------------------------------------
// GetLastNetworkError                                     START
Failure Internal::GetLastNetworkFailure()
{
    const int err = WSAGetLastError();

    switch (err)
    {
    case ERROR_SUCCESS:
        return E_SUCCESS;
    case EAI_FAIL:
    case EAI_NONAME:
    case WSAENOTCONN:
        return E_FAILURE(err);
    case WSAEACCES:
        return E_ACCESS_DENIED(err);
    case EAI_MEMORY:
    case WSAEMFILE:
    case WSAENOBUFS:
        return E_INSUFFICIENT_RESOURCES(err);
    case WSAEINTR:
        return E_INTERRUPTED(err);
    case ERROR_INVALID_PARAMETER:
    case WSATYPE_NOT_FOUND:  // EAI_SERVICE
    case WSAEBADF:
    case WSAEFAULT:
    case WSAEINVAL:  // EAI_BADFLAGS
    case WSAENOTSOCK:
    case WSAEAFNOSUPPORT:  // EAI_FAMILY
    case WSAEOPNOTSUPP:
    case WSAEPROTONOSUPPORT:  // EAI_SOCKTYPE
    case WSAEPROTOTYPE:
    case WSAESOCKTNOSUPPORT:
        return E_INVALID_ARGUMENT(err);
    case WSAEISCONN:
        return E_NET_CONNECTED(err);
    case WSAETIMEDOUT:
    case WSAECONNABORTED:
    case WSAENETUNREACH:
    case WSAEHOSTUNREACH:
        return E_NET_CONN_ABORTED(err);
    case WSAECONNREFUSED:
        return E_NET_CONN_REFUSED(err);
    case WSAECONNRESET:
        return E_NET_CONN_RESET(err);
    case WSAEALREADY:
        return E_NET_INPROGRESS(err);
    case WSAENETDOWN:
        return E_NET_NETWORK_DOWN(err);
    case WSAEWOULDBLOCK:
        return E_NET_WOULD_BLOCK(err);
    case WSAESHUTDOWN:
    case WSANOTINITIALISED:
        return E_NOT_INITIALIZED(err);
    case WSAEADDRINUSE:
    case WSAEADDRNOTAVAIL:
        return E_RESOURCE_NOT_AVAILABLE(err);
    case EAI_AGAIN:
        return E_NET_AGAIN(err);
    }
    return Failure{ err };
}
// GetLastNetworkError                                       END
// -------------------------------------------------------------
// PollFlags                                               START
int32_t Internal::GetPollFlags(PollFlags flags)
{
    int32_t val = 0;

    if (+(flags & PollFlags::Read))
    {
        val |= POLLRDNORM;
    }
    if (+(flags & PollFlags::Write))
    {
        val |= POLLWRNORM;
    }
    return val;
}

PollFlags Internal::GetPollFlags(int32_t flags)
{
    PollFlags events = PollFlags::None;

    if (flags & POLLERR)
    {
        events |= PollFlags::Error;
    }
    if (flags & POLLHUP)
    {
        events |= PollFlags::HangUp;
    }
    if (flags & POLLNVAL)
    {
        events |= PollFlags::Invalid;
    }
    if (flags & POLLRDNORM)
    {
        events |= PollFlags::Read;
    }
    if (flags & POLLWRNORM)
    {
        events |= PollFlags::Write;
    }

    return events;
}
// PollFlags                                                 END
// -------------------------------------------------------------
// InetAddress                                             START
std::string_view ToString(
    const InetAddress& address,
    char* buffer,
    size_t len)
{
    constexpr std::string_view INET_EMPTY{ "0.0.0.0" };

    if (!inet_ntop(AF_INET, address.Data(), buffer, len))
    {
        return INET_EMPTY;
    }

    return { buffer };
}
// InetAddress                                               END
// -------------------------------------------------------------
// Inet6Address                                            START
std::string_view ToString(
    const Inet6Address& address,
    char* buffer,
    size_t len)
{
    constexpr std::string_view INET6_EMPTY{ "::0" };

    if (!inet_ntop(AF_INET6, address.Data(), buffer, len))
    {
        return INET6_EMPTY;
    }

    return { buffer };
}
// Inet6Address                                              END
// -------------------------------------------------------------
// Ioctl                                                   START
Result<void> Internal::Ioctl::SetOption(
    Socket sock,
    int64_t option,
    uint64_t value)
{
    if (sock == INVALID_SOCKET)
    {
        return Failure{ E_INVALID_ARGUMENT }
            .WithContext("invalid socket");
    }

    if (::ioctlsocket(
        static_cast<SOCKET>(sock),
        static_cast<long>(option),
        reinterpret_cast<u_long*>(&value)) == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to set option '{}' ({}) on '{}'",
                option, value, sock);
    }

    return Success;
}
// Ioctl                                                     END
// -------------------------------------------------------------
}  // namespace fusion

#endif
