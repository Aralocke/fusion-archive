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

#pragma once

#include <Fusion/Network.h>
#include <Fusion/Windows.h>

#include <array>
#include <variant>

#if FUSION_PLATFORM_WINDOWS
 // FUSION_PLATFORM_WINDOWS -- START
#pragma warning(disable: 4221)

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>

#ifdef ERROR
#undef ERROR
#endif
#ifdef INVALID_SOCKET
#undef INVALID_SOCKET
#endif

// FUSION_PLATFORM_WINDOWS -- END
#elif FUSION_PLATFORM_POSIX
 // FUSION_PLATFORM_POSIX -- START
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif  // SOCKET_ERROR
#ifndef NO_ERROR
#define NO_ERROR 0L
#endif
// FUSION_PLATFORM_POSIX -- END
#else
#error "Unsupported operating system"
#endif

namespace Fusion::Internal
{
#if FUSION_PLATFORM_WINDOWS
//
//
//
class WSAInitializer final
{
public:
    WSAInitializer();
    ~WSAInitializer();

private:
    WSADATA m_wsadata;

private:
    static WSAInitializer s_instance;
};
#endif

//
//
//
class Fcntl final
{
public:
    //
    //
    //
    static Result<int32_t> GetFlags(Socket sock);

    //
    //
    //
    static Result<void> SetFlags(
        Socket sock,
        int32_t flags);
};

//
//
//
class Ioctl final
{
public:
    //
    //
    //
    static Result<void> SetOption(
        Socket sock,
        int64_t option,
        uint64_t value);
};

//
// Storage for storing any socket address structure
//
using SockAddrStorage = std::array<uint8_t, sizeof(sockaddr_storage)>;

//
// Storage size required for a INET4 address structure
//
constexpr size_t ADDR4_SOCKLEN = sizeof(sockaddr_in);

//
// Storage size required for a INET6 address structure
//
constexpr size_t ADDR6_SOCKLEN = sizeof(sockaddr_in6);

//
// Storage size required for a UNIX address structure
// This value is set to 0 on platforms which do not support unix:// style
// sockets.
//
#if FUSION_PLATFORM_POSIX
constexpr size_t ADDRUNIX_SOCKLEN = sizeof(sockaddr_un);
#else
constexpr size_t ADDRUNIX_SOCKLEN = 0;
#endif // FUSION_PLATFORM_POSIX

//
// Convert a 'AddressFamily' enum to a platform specific address family
// value (that represents an INT).
//
int32_t GetAddressFamily(AddressFamily family);

//
// Map a platform specific address family integer to an 'AddressFamily'
// enum value.
//
AddressFamily GetAddressFamily(int32_t family);

//
// Map AddressInfoFlags values to platform specifc values represented by
// an integer.
//
int32_t GetAddressInfoFlags(AddressInfoFlags flags);

//
// Return the current error code as a converted Failure type with an
// underlying Error object filled in.
//
Failure GetLastNetworkFailure();

//
// Map MessageOption values to their platform specific values represented
// by an integer.
//
int32_t GetMessageOption(MessageOption options);

//
// Map PollFlags values to their platform specific values represented
// by an integer.
//
int32_t GetPollFlags(PollFlags flags);

//
// Map platform specific poll flags to a 'PollFlags' enum value.
//
PollFlags GetPollFlags(int32_t flags);

//
// Map SocketOpt values to their corresponding platform specific socket
// level value.
//
int32_t GetSocketOptLevel(SocketOpt option);

//
// Map 'SocketOpt' values to their platform specific SO_* values.
//
int32_t GetSocketOpt(SocketOpt option);

//
// Map 'SocketProtocol' values to their corresponding platform specific
// integer values.
//
int32_t GetSocketProtocol(SocketProtocol protocol);

//
// Map platform specific socket protocol values to their respective
// 'SocketProtocol' values.
//
SocketProtocol GetSocketProtocol(int32_t protocol);

//
// Map 'SocketType' values to their platform specific integer values.
//
int32_t GetSocketType(SocketType type);

//
// Map socket type platform specific values to their respective 'SocketType'
// values.
//
SocketType GetSocketType(int32_t type);
}  // namespace Fusion
