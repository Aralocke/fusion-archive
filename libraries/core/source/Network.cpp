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

#include <Fusion/Assert.h>
#include <Fusion/StringUtil.h>

#include <iostream>
#include <string>
#include <vector>

namespace Fusion
{
// -------------------------------------------------------------
// AddressFamily                                           START
int32_t Internal::GetAddressFamily(AddressFamily family)
{
    static constexpr uint32_t s_addressFamily[size_t(AddressFamily::_Count)] = {
        AF_UNSPEC,
        AF_UNSPEC,
        AF_INET,
        AF_INET6,
        AF_UNIX,
    };
        
    return s_addressFamily[size_t(family)];
}

AddressFamily Internal::GetAddressFamily(int32_t family)
{
    switch (family)
    {
    case AF_INET:
        return AddressFamily::Inet4;
    case AF_INET6:
        return AddressFamily::Inet6;
    case AF_UNIX:
        return AddressFamily::Unix;
    case AF_UNSPEC:
        return AddressFamily::Unspecified;
    }

    return AddressFamily::None;
}

std::string_view ToString(AddressFamily family)
{
    using namespace std::string_view_literals;

    static constexpr std::string_view s_familyStrings[size_t(AddressFamily::_Count)] = {
        "None"sv,
        "Unspecified"sv,
        "Inet4"sv,
        "Inet6"sv,
        "Unix"sv,
    };

    return s_familyStrings[size_t(family)];
}

std::string_view ToString(
    AddressFamily family,
    char* buffer,
    size_t length)
{
    auto value = ToString(family);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream& o, AddressFamily family)
{
    return o << ToString(family);
}
// AddressFamily                                             END
// -------------------------------------------------------------
// AddressInfoFlags                                        START
std::string FlagsToString(AddressInfoFlags flags)
{
    using namespace std::string_view_literals;

    std::vector<std::string_view> strings;
    strings.reserve(8);

    if (+(flags & AddressInfoFlags::AddressConfig))
    {
        strings.emplace_back(ToString(AddressInfoFlags::AddressConfig));
    }
    if (+(flags & AddressInfoFlags::All)
        || +(flags & AddressInfoFlags::AddressMapped))
    {
        strings.emplace_back(ToString(AddressInfoFlags::AddressMapped));
    }
    if (+(flags & AddressInfoFlags::CanonicalName))
    {
        strings.emplace_back(ToString(AddressInfoFlags::CanonicalName));
    }
    if (+(flags & AddressInfoFlags::Fdqn))
    {
        strings.emplace_back(ToString(AddressInfoFlags::Fdqn));
    }
    if (+(flags & AddressInfoFlags::NumericHost))
    {
        strings.emplace_back(ToString(AddressInfoFlags::NumericHost));
    }
    if (+(flags & AddressInfoFlags::NumericService))
    {
        strings.emplace_back(ToString(AddressInfoFlags::NumericService));
    }
    if (+(flags & AddressInfoFlags::Passive))
    {
        strings.emplace_back(ToString(AddressInfoFlags::Passive));
    }

    if (strings.empty())
    {
        return "None";
    }

    return StringUtil::Join(strings, ", "sv);
}

int32_t Internal::GetAddressInfoFlags(AddressInfoFlags flags)
{
    int32_t r = 0;

    if (+(flags & AddressInfoFlags::Passive))
    {
        r |= AI_PASSIVE;
    }
    if (+(flags & AddressInfoFlags::CanonicalName))
    {
        r |= AI_CANONNAME;
    }
    if (+(flags & AddressInfoFlags::NumericHost))
    {
        r |= AI_NUMERICHOST;
    }
    if (+(flags & AddressInfoFlags::NumericService))
    {
        r |= AI_NUMERICSERV;
    }
    if (+(flags & AddressInfoFlags::All))
    {
        r |= AI_ALL;
    }
    if (+(flags & AddressInfoFlags::AddressConfig))
    {
        r |= AI_ADDRCONFIG;
    }
    if (+(flags & AddressInfoFlags::AddressMapped))
    {
        r |= AI_V4MAPPED;
    }

#if FUSION_PLATFORM_WINDOWS
    if (+(flags & AddressInfoFlags::Fdqn))
    {
        r |= AI_FQDN;
    }
#endif  // FUSION_PLATFORM_WINDOWS

    return r;
}

std::string_view ToString(AddressInfoFlags flag)
{
    using namespace std::string_view_literals;

    switch (flag)
    {
    case AddressInfoFlags::AddressConfig:
        return "AddressConfig"sv;
    case AddressInfoFlags::AddressMapped:
        return "AddressMapped"sv;
    case AddressInfoFlags::CanonicalName:
        return "CanonicalName"sv;
    case AddressInfoFlags::Fdqn:
        return "Fdqn"sv;
    case AddressInfoFlags::NumericHost:
        return "NumericHost"sv;
    case AddressInfoFlags::NumericService:
        return "NumericService"sv;
    case AddressInfoFlags::Passive:
        return "Passive"sv;
    case AddressInfoFlags::All:
        return "All"sv;
    }

    return "None"sv;
}

std::string_view ToString(
    AddressInfoFlags flag,
    char* buffer,
    size_t length)
{
    auto value = ToString(flag);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream& o, AddressInfoFlags flags)
{
    return o << FlagsToString(flags);
}
// AddressInfoFlags                                          END
// -------------------------------------------------------------
// MessageOption                                           START
std::string FlagsToString(MessageOption options)
{
    using namespace std::string_view_literals;

    if (options == MessageOption::None)
    {
        return "None";
    }

    std::vector<std::string_view> strings;
    strings.reserve(4);

    if (+(options & MessageOption::Confirm))
    {
        strings.emplace_back(ToString(MessageOption::Confirm));
    }
    if (+(options & MessageOption::NoSignal))
    {
        strings.emplace_back(ToString(MessageOption::NoSignal));
    }
    if (+(options & MessageOption::OutOfBand))
    {
        strings.emplace_back(ToString(MessageOption::OutOfBand));
    }
    if (+(options & MessageOption::Peek))
    {
        strings.emplace_back(ToString(MessageOption::Peek));
    }

    return StringUtil::Join(strings, ", "sv);
}

std::string_view ToString(MessageOption option)
{
    using namespace std::string_view_literals;

    switch (option)
    {
    case MessageOption::Confirm:
        return "Confirm"sv;
    case MessageOption::NoSignal:
        return "NoSignal"sv;
    case MessageOption::OutOfBand:
        return "OutOfBand"sv;
    case MessageOption::Peek:
        return "Peek"sv;
    case MessageOption::None:
        return "None"sv;
    }

    return "None"sv;
}

int32_t Internal::GetMessageOption(MessageOption opt)
{
    int32_t r = 0;

#if FUSION_PLATFORM_POSIX
    if (+(opt & MessageOption::Confirm))
    {
        r |= MSG_CONFIRM;
    }
    if (+(opt & MessageOption::NoSignal))
    {
        r |= MSG_NOSIGNAL;
    }
#endif  // FUSION_PLATFORM_POSIX

    if (+(opt & MessageOption::OutOfBand))
    {
        r |= MSG_OOB;
    }
    if (+(opt & MessageOption::Peek))
    {
        r |= MSG_PEEK;
    }

    return r;
}

std::string_view ToString(
    MessageOption option,
    char* buffer,
    size_t length)
{
    auto value = ToString(option);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream & o, MessageOption options)
{
    return o << FlagsToString(options);
}
// MessageOption                                             END
// -------------------------------------------------------------
// PollFlags                                               START
std::string FlagsToString(PollFlags flags)
{
    using namespace std::string_view_literals;

    if (flags == PollFlags::None)
    {
        return "None";
    }

    std::vector<std::string_view> strings;
    strings.reserve(5);

    if (+(flags & PollFlags::Error))
    {
        strings.emplace_back(ToString(PollFlags::Error));
    }
    if (+(flags & PollFlags::HangUp))
    {
        strings.emplace_back(ToString(PollFlags::HangUp));
    }
    if (+(flags & PollFlags::Invalid))
    {
        strings.emplace_back(ToString(PollFlags::Invalid));
    }
    if (+(flags & PollFlags::Read))
    {
        strings.emplace_back(ToString(PollFlags::Read));
    }
    if (+(flags & PollFlags::Write))
    {
        strings.emplace_back(ToString(PollFlags::Write));
    }

    return StringUtil::Join(strings, ", "sv);
}

std::string_view ToString(PollFlags flags)
{
    using namespace std::string_view_literals;

    switch (flags)
    {
    case PollFlags::Error:
        return "Error"sv;
    case PollFlags::HangUp:
        return "HangUp"sv;
    case PollFlags::Invalid:
        return "Invalid"sv;
    case PollFlags::Read:
        return "Read"sv;
    case PollFlags::Write:
        return "Write"sv;
    }

    return "None"sv;
}

std::string_view ToString(
    PollFlags flag,
    char* buffer,
    size_t length)
{
    auto value = ToString(flag);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream& o, PollFlags flags)
{
    return o << FlagsToString(flags);
}
// PollFlags                                                 END
// -------------------------------------------------------------
// SocketOperation                                         START
std::string FlagsToString(SocketOperation operations)
{
    using namespace std::string_view_literals;

    if (operations == SocketOperation::None)
    {
        return "None";
    }

    std::vector<std::string_view> strings;
    strings.reserve(3);

    if (+(operations & SocketOperation::Read))
    {
        strings.emplace_back(ToString(SocketOperation::Read));
    }
    if (+(operations & SocketOperation::Write))
    {
        strings.emplace_back(ToString(SocketOperation::Write));
    }
    if (+(operations & SocketOperation::Error))
    {
        strings.emplace_back(ToString(SocketOperation::Error));
    }

    return StringUtil::Join(strings, ", "sv);
}

std::string_view ToString(SocketOperation operations)
{
    using namespace std::string_view_literals;

    switch (operations)
    {
    case SocketOperation::Error:
        return "Error"sv;
    case SocketOperation::Read:
        return "Read"sv;
    case SocketOperation::Write:
        return "Write"sv;
    }

    return "None"sv;
}

std::string_view ToString(
    SocketOperation operation,
    char* buffer,
    size_t length)
{
    auto value = ToString(operation);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream& o, SocketOperation operations)
{
    return o << FlagsToString(operations);
}
// SocketOperation                                           END
// -------------------------------------------------------------
// SocketOpt                                               START
int32_t Internal::GetSocketOptLevel(SocketOpt option)
{
    static const int32_t s_socketLevels[size_t(SocketOpt::_Count)] = {
        int32_t(SOL_SOCKET),   // Broadcast
        int32_t(SOL_SOCKET),   // Debug
        int32_t(SOL_SOCKET),   // DontRoute
        int32_t(SOL_SOCKET),   // KeepAlive
        int32_t(SOL_SOCKET),   // Linger
        int32_t(IPPROTO_IP),   // Multicast
        int32_t(IPPROTO_IP),   // MulticastLoopback
        int32_t(IPPROTO_IP),   // MulticastTTL
        int32_t(IPPROTO_TCP),  // NoDelay
        int32_t(SOL_SOCKET),   // OobInline
        int32_t(SOL_SOCKET),   // RecvBuf
        int32_t(SOL_SOCKET),   // RecvLowMark
        int32_t(SOL_SOCKET),   // RecvTimeout
        int32_t(SOL_SOCKET),   // ReuseAddress
        int32_t(SOL_SOCKET),   // ReusePort
        int32_t(SOL_SOCKET),   // SendBuf
        int32_t(SOL_SOCKET),   // SendLowMark
        int32_t(SOL_SOCKET),   // SendTimeout
        int32_t(SOL_SOCKET),   // SocketError
        int32_t(IPPROTO_TCP),  // TcpKeepAlive
        int32_t(IPPROTO_TCP),  // TcpKeepCount
        int32_t(IPPROTO_TCP),  // TcpKeepIdle
        int32_t(IPPROTO_TCP),  // TcpKeepInterval
        int32_t(IPPROTO_IP),   // TimeToLive
    };

    return s_socketLevels[size_t(option)];
}

int32_t Internal::GetSocketOpt(SocketOpt option)
{
    static const int32_t s_socketOptions[size_t(SocketOpt::_Count)] = {
        int32_t(SO_BROADCAST),       // Broadcast
        int32_t(SO_DEBUG),           // Debug
        int32_t(SO_DONTROUTE),       // DontRoute
        int32_t(SO_KEEPALIVE),       // KeepAlive
        int32_t(SO_LINGER),          // Linger
        int32_t(IP_ADD_MEMBERSHIP),  // Multicast
        int32_t(IP_MULTICAST_LOOP),  // MulticastLoopback
        int32_t(IP_MULTICAST_TTL),   // MulticastTTL
        int32_t(TCP_NODELAY),        // NoDelay
        int32_t(SO_OOBINLINE),       // OobInline
        int32_t(SO_RCVBUF),          // RecvBuf
        int32_t(SO_RCVLOWAT),        // RecvLowMark
        int32_t(SO_RCVTIMEO),        // RecvTimeout
        int32_t(SO_REUSEADDR),       // ReuseAddress
#if FUSION_PLATFORM_POSIX
        int32_t(SO_REUSEPORT),       // ReusePort
#else
        int32_t(-1),                 // ReusePort
#endif
        int32_t(SO_SNDBUF),          // SendBuf
        int32_t(SO_SNDLOWAT),        // SendLowMark
        int32_t(SO_SNDTIMEO),        // SendTimeout
        int32_t(SO_ERROR),           // SocketError
        int32_t(TCP_KEEPALIVE),      // TcpKeepAlive
        int32_t(TCP_KEEPCNT),        // TcpKeepCount
        int32_t(TCP_KEEPIDLE),       // TcpKeepIdle
        int32_t(TCP_KEEPINTVL),      // TcpKeepInterval
        int32_t(IP_TTL),             // TimeToLive
    };

    return s_socketOptions[size_t(option)];
}

std::string_view ToString(SocketOpt option)
{
    using namespace std::string_view_literals;

    static constexpr std::string_view s_optStrings[size_t(SocketOpt::_Count)] = {
        "SO_BROADCAST"sv,       // Broadcast
        "SO_DEBUG"sv,           // Debug
        "SO_DONTROUTE"sv,       // DontRoute
        "SO_KEEPALIVE"sv,       // KeepAlive
        "SO_LINGER"sv,          // Linger
        "IP_ADD_MEMBERSHIP"sv,  // Multicast
        "IP_MULTICAST_LOOP"sv,  // MulticastLoopback
        "IP_MULTICAST_TTL"sv,   // MulticastTTL
        "SO_NODELAY"sv,         // NoDelay
        "SO_OOBINLINE"sv,       // OobInline
        "SO_RECVBUF"sv,         // RecvBuf
        "SO_RCVLOWAT"sv,        // RecvLowMark
        "SO_RCVTIMEO"sv,        // RecvTimeout
        "SO_REUSEADDR"sv,       // ReuseAddress
        "SO_REUSEPORT"sv,       // ReusePort
        "SO_SENDBUF"sv,         // SendBuf
        "SO_SNDLOWAT"sv,        // SendLowMark
        "SO_SNDTIMEO"sv,        // SendTimeout
        "SO_SOCKERR"sv,         // SocketError
        "TCP_KEEPALIVE"sv,      // TcpKeepAlive
        "TCP_KEEPCNT"sv,        // TcpKeepCount
        "TCP_KEEPIDLE"sv,       // TcpKeepIdle
        "TCP_KEEPINTVL"sv,      // TcpKeepInterval
        "IP_TTL"sv,             // TimeToLive
    };

    return s_optStrings[size_t(option)];
}

std::string_view ToString(
    SocketOpt option,
    char* buffer,
    size_t length)
{
    auto value = ToString(option);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream& o, SocketOpt option)
{
    return o << ToString(option);
}
// SocketOpt                                                 END
// -------------------------------------------------------------
// SocketProtocol                                          START
int32_t Internal::GetSocketProtocol(SocketProtocol protocol)
{
    static const int32_t s_socketProtocols[] = {
        int32_t(IPPROTO_NONE),
        int32_t(IPPROTO_TCP),
        int32_t(IPPROTO_UDP),
        int32_t(IPPROTO_ICMP),
        int32_t(IPPROTO_IP),
        int32_t(IPPROTO_RAW),
    };

    return s_socketProtocols[static_cast<size_t>(protocol)];
}

SocketProtocol Internal::GetSocketProtocol(int32_t protocol)
{
    switch (protocol)
    {
    case int32_t(IPPROTO_TCP):
        return SocketProtocol::Tcp;
    case int32_t(IPPROTO_UDP):
        return SocketProtocol::Udp;
    case int32_t(IPPROTO_ICMP):
        return SocketProtocol::Icmp;
    case int32_t(IPPROTO_IP):
        return SocketProtocol::Ip;
    case int32_t(IPPROTO_RAW):
        return SocketProtocol::Raw;
    case int32_t(IPPROTO_NONE):
        return SocketProtocol::None;
    }

    return SocketProtocol::None;
}

std::string_view ToString(SocketProtocol protocol)
{
    using namespace std::string_view_literals;

    static constexpr std::string_view s_protocols[size_t(SocketProtocol::_Count)] = {
        "None"sv,
        "Icmp"sv,
        "Ip"sv,
        "Raw"sv,
        "Tcp"sv,
        "Udp"sv,
    };

    return s_protocols[size_t(protocol)];
}

std::string_view ToString(
    SocketProtocol protocol,
    char* buffer,
    size_t length)
{
    auto value = ToString(protocol);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream& o, SocketProtocol protocol)
{
    return o << ToString(protocol);
}
// SocketProtocol                                            END
// -------------------------------------------------------------
// SocketShutdownMode                                      START
std::string_view ToString(SocketShutdownMode mode)
{
    using namespace std::string_view_literals;

    static constexpr std::string_view s_modeStrings[size_t(SocketShutdownMode::_Count)] = {
        "None"sv,
        "Read"sv,
        "Write"sv,
        "Both"sv,
    };

    return s_modeStrings[size_t(mode)];
}

std::string_view ToString(
    SocketShutdownMode mode,
    char* buffer,
    size_t length)
{
    auto value = ToString(mode);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream& o, SocketShutdownMode mode)
{
    return o << ToString(mode);
}
// SocketShutdownMode                                        END
// -------------------------------------------------------------
// SocketType                                              START
int32_t Internal::GetSocketType(SocketType type)
{
    static const int32_t s_socketTypes[] = {
        int32_t(SOCKET_ERROR),
        int32_t(SOCK_DGRAM),
        int32_t(SOCK_RAW),
        int32_t(SOCK_STREAM),
    };

    return s_socketTypes[static_cast<size_t>(type)];
}

SocketType Internal::GetSocketType(int32_t type)
{
    switch (type)
    {
    case int32_t(SOCK_DGRAM):
        return SocketType::Datagram;
    case int32_t(SOCK_RAW):
        return SocketType::Raw;
    case int32_t(SOCK_STREAM):
        return SocketType::Stream;
    }

    return SocketType::None;
}

std::string_view ToString(SocketType type)
{
    using namespace std::string_view_literals;

    static constexpr std::string_view s_typeStrings[size_t(SocketType::_Count)] = {
        "None"sv,
        "Datagram"sv,
        "Raw"sv,
        "Stream"sv,
    };

    return s_typeStrings[size_t(type)];
}

std::string_view ToString(
    SocketType type,
    char* buffer,
    size_t length)
{
    auto value = ToString(type);
    size_t len = std::min(length, value.size() + 1);

    return StringUtil::Copy(buffer, len, value.data(), value.size());
}

std::ostream& operator<<(std::ostream& o, SocketType type)
{
    return o << ToString(type);
}
// SocketType                                                END
}  // namespace Fusion