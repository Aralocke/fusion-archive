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

#include <Fusion/Platform.h>

#if FUSION_PLATFORM_POSIX

#include <Fusion/Internal/Network.h>

// -------------------------------------------------------------
// GetLastNetworkError                                     START
Failure Internal::GetLastNetworkFailure()
{
    const int err = errno;

    switch (err)
    {
    case ENOTCONN:
        return E_FAILURE(err);
    case EWOULDBLOCK:
        return E_NET_WOULD_BLOCK;
    case EADDRINUSE:
    case EADDRNOTAVAIL:
        return E_RESOURCE_NOT_AVAILABLE(err);
    case EALREADY:
        return E_NET_INPROGRESS(err);
    case ECONNABORTED:
    case EHOSTUNREACH:
        return E_NET_CONN_ABORTED(err);
    case ECONNREFUSED:
        return E_NET_CONN_REFUSED(err);
    case ECONNRESET:
        return E_NET_CONN_RESET(err);
    case EINPROGRESS:
        return E_NET_INPROGRESS(err);
    case ENETDOWN:
        return E_NET_CONN_ABORTED(err);
    case EISCONN:
        return E_NET_CONNECTED(err);
    case ETIMEDOUT:
        return E_NET_TIMEOUT(err);
    }
    return E_FAILURE(err);
}
// GetLastNetworkError                                       END
 // -------------------------------------------------------------
 // InetAddress                                              START
std::string_view ToString(
    const InetAddress& address,
    char* buffer,
    size_t len)
{
    static constexpr std::string_view INET_EMPTY{ "0.0.0.0" };

    if (!inet_ntop(
        AF_INET,
        address.Data(),
        buffer,
        socklen_t(len)))
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
    static constexpr std::string_view INET6_EMPTY{ "::0" };

    if (!inet_ntop(
        AF_INET6,
        address.Data(),
        buffer,
        socklen_t(len)))
    {
        return INET6_EMPTY;
    }

    return { buffer };
}
// Inet6Address                                              END
// -------------------------------------------------------------
#endif
