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

#include <Fusion/Internal/Network.h>

#include <Fusion/Assert.h>
#include <Fusion/Endian.h>
#include <Fusion/Memory.h>
#include <Fusion/MemoryUtil.h>
#include <Fusion/StringUtil.h>

#include <Fusion/Internal/IocpSocketService.h>
#include <Fusion/Internal/EPollSocketService.h>
#include <Fusion/Internal/KQueueSocketService.h>
#include <Fusion/Internal/SelectSocketService.h>
#include <Fusion/Internal/StandardNetwork.h>

#include <algorithm>
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
    default:
        break;
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
    default:
        break;
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
    default:
        break;
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
    default:
        break;
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
        int32_t(SOL_SOCKET),   // Type
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
#if FUSION_PLATFORM_DARWIN
        int32_t(TCP_KEEPALIVE),      // TcpKeepAlive
#else
        int32_t(-1),                 // TcpKeepAlive
#endif
        int32_t(TCP_KEEPCNT),        // TcpKeepCount
#if FUSION_PLATFORM_WINDOWS
        int32_t(TCP_KEEPIDLE),       // TcpKeepIdle
#else
        int32_t(-1),                 // TcpKeepIdle
#endif
        int32_t(TCP_KEEPINTVL),      // TcpKeepInterval
        int32_t(IP_TTL),             // TimeToLive
        int32_t(SO_TYPE),            // Type
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
        "SO_TYPE"sv,            // Type
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
        int32_t(IPPROTO_ICMP),
        int32_t(IPPROTO_IP),
        int32_t(IPPROTO_RAW),
        int32_t(IPPROTO_TCP),
        int32_t(IPPROTO_UDP),
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
    default:
        break;
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
    default:
        break;
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
// -------------------------------------------------------------
// InetAddress                                             START
Result<void> InetAddress::FromDecimal(uint32_t address)
{
    m_address[0] = (address >> 24) & 0xff;
    m_address[1] = (address >> 16) & 0xff;
    m_address[2] = (address >> 8) & 0xff;
    m_address[3] = address & 0xff;

    return Success;
}

Result<void> InetAddress::FromString(std::string_view address)
{
    using namespace Internal;

    if (address.empty()
        || address.size() < 7U
        || address.size() > InetAddress::LENGTH)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid input address '{}'", address);
    }

    struct sockaddr_in saddr{};
    memset(&saddr, 0, ADDR4_SOCKLEN);

    std::string str(address);

    if (inet_pton(AF_INET, str.c_str(), &(saddr.sin_addr)) == 1)
    {
        const auto* bytes = reinterpret_cast<const uint8_t*>(
            &(saddr.sin_addr.s_addr));
        memcpy(Data(), bytes, InetAddress::SIZE);
        return Success;
    }

    return Failure(E_INVALID_ARGUMENT)
        .WithContext("failed to convert '{}' to an inet address", address);
}

void InetAddress::Assign(const void* address)
{
    memset(Data(), 0, InetAddress::SIZE);
    memcpy(Data(), address, InetAddress::SIZE);
}

Inet6Address InetAddress::AsV6() const
{
    if (IsEmpty())
    {
        return {};
    }

    Inet6Address address{};
    {
        uint8_t* addr = address.Data();

        addr[10] = 0xff;
        addr[11] = 0xff;
        addr[12] = m_address[0];
        addr[13] = m_address[1];
        addr[14] = m_address[2];
        addr[15] = m_address[3];
    }
    return address;
}

InetAddress::operator Inet6Address() const
{
    return AsV6();
}

const uint8_t* InetAddress::Data() const
{
    return m_address.data();
}

uint8_t* InetAddress::Data()
{
    return m_address.data();
}

bool InetAddress::IsEmpty() const
{
    return std::all_of(begin(m_address), end(m_address),
        [](auto u) { return u == 0; });
}

InetAddress::operator bool() const
{
    return !IsEmpty();
}

bool InetAddress::IsPrivate() const
{
    // 127.0.0.0
    // RFC1918 255.0.0.0
    // RFC1918 255.240.0.0
    // RFC1918 and RFC3927 255.255.0.0

    // RFC1918 10.0.0.0
    // RFC1918 172.16.0.0
    // RFC1918 192.168.0.0
    // RFC3927 169.254.0.0
    return false;
}

uint32_t InetAddress::ToDecimal() const
{
    return (uint32_t(m_address[0]) << 24U)
        + (uint32_t(m_address[1]) << 16U)
        + (uint32_t(m_address[2]) << 8U)
        + (uint32_t(m_address[3]));
}

bool InetAddress::operator==(const InetAddress& addr) const
{
    return MemoryUtil::Equal(Data(), addr.Data(), SIZE);
}

bool InetAddress::operator!=(const InetAddress& addr) const
{
    return !MemoryUtil::Equal(Data(), addr.Data(), SIZE);
}

bool InetAddress::operator<(const InetAddress& addr) const
{
    return MemoryUtil::Less(Data(), addr.Data(), SIZE);
}

std::string ToString(const InetAddress& address)
{
    char buffer[InetAddress::LENGTH + 1] = { 0 };
    std::string_view result = ToString(
        address,
        buffer,
        InetAddress::LENGTH);

    return std::string{ result };
}

std::ostream& operator<<(std::ostream& o, const InetAddress& address)
{
    return o << ToString(address);
}
// InetAddress                                               END
// -------------------------------------------------------------
// Inet6Address
Result<void> Inet6Address::FromString(std::string_view address)
{
    using namespace Internal;

    if (address.empty()
        || address.size() < 3U
        || address.size() > Inet6Address::LENGTH)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid input address '{}'", address);
    }

    struct sockaddr_in6 saddr{};
    memset(&saddr, 0, ADDR6_SOCKLEN);

    std::string str(address);

    if (inet_pton(AF_INET6, str.c_str(), &(saddr.sin6_addr)) == 1)
    {
        const auto* bytes = reinterpret_cast<const uint8_t*>(
            &(saddr.sin6_addr.s6_addr));

        memcpy(Data(), bytes, SIZE);
        return Success;
    }

    return Failure(E_INVALID_ARGUMENT)
        .WithContext("failed to convert '{}' to an inet6 address", address);
}

void Inet6Address::Assign(const void* address)
{
    memset(Data(), 0, Inet6Address::SIZE);
    memcpy(Data(), address, Inet6Address::SIZE);
}

InetAddress Inet6Address::AsV4() const
{
    if (IsEmpty() || !IsMappedV4())
    {
        return InetAddress{};
    }

    InetAddress address{};
    {
        uint8_t* addr = address.Data();

        addr[0] = m_address[12];
        addr[1] = m_address[13];
        addr[2] = m_address[14];
        addr[3] = m_address[15];
    }
    return address;
}

Inet6Address::operator InetAddress() const
{
    return AsV4();
}

const uint8_t* Inet6Address::Data() const
{
    return m_address.data();
}

uint8_t* Inet6Address::Data()
{
    return m_address.data();
}

bool Inet6Address::IsEmpty() const
{
    return std::all_of(begin(m_address), end(m_address),
        [](auto u) { return u == 0; });
}

Inet6Address::operator bool() const
{
    return !IsEmpty();
}

bool Inet6Address::IsMappedV4() const
{
    if (IsEmpty())
    {
        return false;
    }

    const uint8_t* bytes = Data();

    for (uint8_t i = 0; i < 10; ++i)
    {
        if (bytes[i] != 0x00)
        {
            return false;
        }
    }

    return bytes[10] == 0xff && bytes[11] == 0xff;
}

bool Inet6Address::IsPrivate() const
{
    return false;
}

bool Inet6Address::operator==(const Inet6Address& addr) const
{
    return MemoryUtil::Equal(Data(), addr.Data(), SIZE);
}

bool Inet6Address::operator!=(const Inet6Address& addr) const
{
    return !MemoryUtil::Equal(Data(), addr.Data(), SIZE);
}

bool Inet6Address::operator<(const Inet6Address& addr) const
{
    return MemoryUtil::Less(Data(), addr.Data(), SIZE);
}

std::string ToString(const Inet6Address& address)
{
    char buffer[Inet6Address::LENGTH + 1] = { 0 };
    std::string_view result = ToString(
        address,
        buffer,
        Inet6Address::LENGTH);

    return std::string{ result };
}

std::ostream& operator<<(std::ostream& o, const Inet6Address& address)
{
    return o << ToString(address);
}
// Inet6Address                                              END
// -------------------------------------------------------------
// ParseAddress                                            START
Result<ParsedAddress> ParseAddress(std::string_view address)
{
    InetAddress addr4{};
    if (auto result = addr4.FromString(address); result)
    {
        return ParsedAddress{
            .address = {addr4 },
            .family = AddressFamily::Inet4
        };
    }

    Inet6Address addr6{};
    if (auto result = addr6.FromString(address); result)
    {
        return ParsedAddress{
            .address = {addr6},
            .family = AddressFamily::Inet6
        };
    }

    return Failure(E_INVALID_ARGUMENT)
        .WithContext("unable to parse address '{}'", address);
}
// ParseAddress                                              END
// -------------------------------------------------------------
// SocketAddress                                           START
SocketAddress::SocketAddress(InetAddress address, uint32_t port)
    : m_address{ std::in_place_type<InetAddr>, address, uint16_t(port) }
    , m_family{ AddressFamily::Inet4 }
{ }

SocketAddress::SocketAddress(Inet6Address address, uint32_t port)
    : m_address{ std::in_place_type<Inet6Addr>, address, uint16_t(port) }
    , m_family{ AddressFamily::Inet6 }
{ }

AddressFamily SocketAddress::Family() const
{
    return m_family;
}

void SocketAddress::FromSockAddr(const sockaddr* addr)
{
    using namespace Internal;

    m_family = GetAddressFamily(addr->sa_family);

    switch (m_family)
    {
    case AddressFamily::Inet4:
    {
        InetAddr inet;

        const auto& in = *reinterpret_cast<const sockaddr_in*>(addr);
        inet.port = ntohs(in.sin_port);

        const auto* bytes = reinterpret_cast<const uint8_t*>(
            &(in.sin_addr.s_addr));

        inet.address.Assign(bytes);
        m_address = inet;

        break;
    }
    case AddressFamily::Inet6:
    {
        Inet6Addr inet6;

        const auto& in6 = *reinterpret_cast<const sockaddr_in6*>(addr);
        inet6.port = ntohs(in6.sin6_port);

        inet6.flowInfo = BigEndian<uint32_t>::Load(
            reinterpret_cast<const uint32_t*>(&in6.sin6_flowinfo));
        inet6.scope = BigEndian<uint32_t>::Load(
            reinterpret_cast<const uint32_t*>(&in6.sin6_scope_id));

        const auto* bytes = reinterpret_cast<const uint8_t*>(
            &(in6.sin6_addr.s6_addr));

        inet6.address.Assign(bytes);
        m_address = inet6;

        break;
    }
    case AddressFamily::Unix:
    {
#if FUSION_PLATFORM_POSIX
        UnixAddr unAddr;

        const auto& un = *reinterpret_cast<const sockaddr_un*>(addr);
        size_t length = StringUtil::Length(
            unAddr.path,
            SocketAddress::UnixAddr::LENGTH);

        memset(unAddr.path, 0, length);
        memcpy(unAddr.path, un.sun_path, length);

        m_address = unAddr;
#else
        FUSION_ASSERT(false, "Platform does not support UNIX sockets");
#endif  // FUSION_PLATFORM_POSIX
        break;
    }
    default:
        FUSION_ASSERT(false, "Unsupported address family");
        break;
    }
}

Result<void> SocketAddress::FromString(std::string_view address)
{
    if (address.empty())
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("empty socket address");
    }

    uint16_t port = 0;

    if (auto colon = address.find(':'); colon != std::string_view::npos)
    {
        auto in = address.substr(colon + 1);
        auto result = StringUtil::ParseNumber<uint16_t>(in, port);

        if (!result)
        {
            return result.Error();
        }

        port = *result;
        address = address.substr(0, colon);
    }

    Result<ParsedAddress> result = ParseAddress(address);

    if (!result)
    {
        return result.Error();
    }

    switch (result->family)
    {
    case AddressFamily::Inet4:
    {
        InetAddr& inet = Inet();

        m_family = AddressFamily::Inet4;
        inet.port = port;
        inet.address = result->address.inet;
        break;
    }
    case AddressFamily::Inet6:
    {
        Inet6Addr& inet6 = Inet6();

        inet6.port = port;
        inet6.flowInfo = 0;  // TODO: Fill out these fields at a later
        inet6.scope = 0;     //       date once the parsing cleans up.
        inet6.address = result->address.inet6;
        break;
    }
    default:
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("unable to parse '{}' to an inet4 or inet6 address", address);
    }

    return Success;
}

SocketAddress::InetAddr& SocketAddress::Inet()
{
    return std::get<InetAddr>(m_address);
}

const SocketAddress::InetAddr& SocketAddress::Inet() const
{
    return std::get<InetAddr>(m_address);
}

SocketAddress::Inet6Addr& SocketAddress::Inet6()
{
    return std::get<Inet6Addr>(m_address);
}

const SocketAddress::Inet6Addr& SocketAddress::Inet6() const
{
    return std::get<Inet6Addr>(m_address);
}

bool SocketAddress::IsEmpty() const
{
    if (IsValid())
    {
        return false;
    }

    switch (m_family)
    {
    case AddressFamily::Inet4:
        return !Inet().address.IsEmpty();
    case AddressFamily::Inet6:
        return !Inet6().address.IsEmpty();
    case AddressFamily::Unix:
        return Unix().path[0] == 0;
    default:
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("Unsupported address family: {}", m_family);
    }

    return true;
}

SocketAddress::operator bool() const
{
    return !IsEmpty();
}

bool SocketAddress::IsValid() const
{
    return m_family != AddressFamily::None;
}

sockaddr* SocketAddress::ToSockAddr(void* input, size_t& len) const
{
    using namespace Internal;

    if (!input || len == 0)
    {
        return nullptr;
    }

    auto* addr = reinterpret_cast<sockaddr*>(input);
    size_t length = len;

    switch (m_family)
    {
    case AddressFamily::Inet4:
    {
        if (length < ADDR4_SOCKLEN)
        {
            return nullptr;
        }

        len = ADDR4_SOCKLEN;
        const InetAddr& inet = Inet();

        auto& in = *reinterpret_cast<sockaddr_in*>(addr);
        memset(&in, 0, ADDR4_SOCKLEN);

        in.sin_family = GetAddressFamily(AddressFamily::Inet4);

        memcpy(&in.sin_addr, inet.address.Data(), InetAddress::SIZE);

        in.sin_port = htons(inet.port);
        break;
    }
    case AddressFamily::Inet6:
    {
        if (length < ADDR6_SOCKLEN)
        {
            return nullptr;
        }

        len = ADDR6_SOCKLEN;
        const Inet6Addr& inet6 = Inet6();

        auto& in6 = *reinterpret_cast<sockaddr_in6*>(addr);
        memset(&in6, 0, ADDR6_SOCKLEN);

        in6.sin6_family = GetAddressFamily(AddressFamily::Inet6);
        in6.sin6_port = htons(inet6.port);

        BigEndian<uint32_t>::Store(
            reinterpret_cast<uint32_t*>(&in6.sin6_flowinfo),
            inet6.flowInfo);

        BigEndian<uint32_t>::Store(
            reinterpret_cast<uint32_t*>(&in6.sin6_scope_id),
            inet6.scope);

        memcpy(
            &in6.sin6_addr,
            inet6.address.Data(),
            Inet6Address::SIZE);

        break;
    }
    case AddressFamily::Unix:
    {
#if FUSION_PLATFORM_POSIX
        if (length < ADDRUNIX_SOCKLEN)
        {
            return nullptr;
        }

        len = ADDRUNIX_SOCKLEN;
        const UnixAddr& unData = Unix();

        auto& un = *reinterpret_cast<sockaddr_un*>(addr);
        memset(&un, 0, ADDRUNIX_SOCKLEN);

        un.sun_family = GetAddressFamily(AddressFamily::Unix);
        static_assert(UnixAddr::LENGTH <= sizeof(un.sun_path),
            "SocketAddress::UNIX_LEN is too large to fit in sockaddr_un.sun_path");

        memcpy(un.sun_path, unData.path, UnixAddr::LENGTH);
#else
        FUSION_ASSERT(false, "Platform does not support UNIX sockets");
#endif // FUSION_PLATFORM_POSIX
        break;
    }
    default:
    {
        addr = nullptr;
        len = 0;

        FUSION_ASSERT(false, "Unsupported address family");
        break;
    }
    }

    return addr;
}

SocketAddress::UnixAddr& SocketAddress::Unix()
{
    return std::get<UnixAddr>(m_address);
}

const SocketAddress::UnixAddr& SocketAddress::Unix() const
{
    return std::get<UnixAddr>(m_address);
}

bool SocketAddress::operator==(const SocketAddress& address) const
{
    if (m_family == address.m_family)
    {
        switch (m_family)
        {
        case AddressFamily::Inet4:
            return Inet().port == address.Inet().port
                && MemoryUtil::Equal(
                    Inet().address.Data(),
                    address.Inet().address.Data(),
                    InetAddress::SIZE);
        case AddressFamily::Inet6:
            return Inet6().port == address.Inet6().port
                && MemoryUtil::Equal(
                    Inet6().address.Data(),
                    address.Inet6().address.Data(),
                    Inet6Address::SIZE);
        case AddressFamily::Unix:
            return MemoryUtil::Equal(
                Unix().path,
                address.Unix().path,
                UnixAddr::LENGTH);
        default:
            return false;
        }
    }
    return false;
}

bool SocketAddress::operator!=(const SocketAddress& address) const
{
    return !operator==(address);
}

bool SocketAddress::operator<(const SocketAddress& address) const
{
    if (m_family == address.m_family)
    {
        switch (m_family)
        {
        case AddressFamily::Inet4:
            return Inet().port == address.Inet().port
                && MemoryUtil::Less(
                    Inet().address.Data(),
                    address.Inet().address.Data(),
                    InetAddress::SIZE);
        case AddressFamily::Inet6:
            return Inet6().port == address.Inet6().port
                && MemoryUtil::Less(
                    Inet6().address.Data(),
                    address.Inet6().address.Data(),
                    Inet6Address::SIZE);
        case AddressFamily::Unix:
            return MemoryUtil::Less(
                Unix().path,
                address.Unix().path,
                UnixAddr::LENGTH);
        default:
            return false;
        }
    }
    return false;
}

std::string ToString(const SocketAddress& address)
{
    switch (address.Family())
    {
    case AddressFamily::Inet4:
    {
        return fmt::format(FMT_STRING("[{}]:{}"),
            address.Inet().address,
            address.Inet().port);
    }
    case AddressFamily::Inet6:
    {
        return fmt::format(FMT_STRING("[{}]:{}"),
            address.Inet6().address,
            address.Inet6().port);
    }
    case AddressFamily::Unix:
    {
        return fmt::format(FMT_STRING("unix://{}"),
            address.Unix().path);
    }
    default:
        return {};
    }
}

std::string_view ToString(
    const SocketAddress& address,
    char* buffer,
    size_t len)
{
    switch (address.Family())
    {
    case AddressFamily::Inet4:
    {
        fmt::format_to_n(buffer, len, FMT_STRING("[{}]:{}"),
            address.Inet().address,
            address.Inet().port);
        break;
    }
    case AddressFamily::Inet6:
    {
        fmt::format_to_n(buffer, len, FMT_STRING("[{}]:{}"),
            address.Inet6().address,
            address.Inet6().port);
        break;
    }
    case AddressFamily::Unix:
    {
        fmt::format_to_n(buffer, len, FMT_STRING("unix://{}"),
            address.Unix().path);
        break;
    }
    default:
        return {};
    }
    return { buffer };
}

std::ostream& operator<<(
    std::ostream& o,
    const SocketAddress& address)
{
    return o << ToString(address);
}
// SocketAddress                                             END
// -------------------------------------------------------------
// AddressInfo                                             START
void Internal::MapAddressInfo(
    const AddressInfo& addr,
    struct addrinfo& info)
{
    memset(&info, 0, ADDRINFO_LEN);

    info.ai_flags = GetAddressInfoFlags(addr.flags);

    info.ai_family = ((addr.family != AddressFamily::None))
        ? GetAddressFamily(addr.family)
        : GetAddressFamily(AddressFamily::Unspecified);

    info.ai_protocol = ((addr.protocol != SocketProtocol::None))
        ? GetSocketProtocol(addr.protocol)
        : 0;

    info.ai_socktype = ((addr.type != SocketType::None))
        ? GetSocketType(addr.type)
        : 0;

    info.ai_addrlen = 0;
    info.ai_addr = nullptr;
    info.ai_next = nullptr;
}
// AddressInfo                                               END
// -------------------------------------------------------------
// Network                                                 START
Result<std::unique_ptr<Network>> Network::Create()
{
    using namespace Fusion::Internal;

    auto network = std::make_unique<StandardNetwork>();

    if (auto result = network->Start(); !result)
    {
        return result.Error();
    }

    return network;
}

Result<Socket> Network::CreateSocket(SocketConfig config) const
{
    return CreateSocket(config.family, config.protocol, config.type);
}

Result<SocketType> Network::GetSocketType(Socket sock) const
{
    int32_t type = 0;
    Result<void> result = GetSocketOption(
        sock,
        SocketOptions::Type(&type));

    if (!result)
    {
        return result.Error()
            .WithContext("failed to query socket option 'SO_TYPE' for '{}",
                sock);
    }

    return Internal::GetSocketType(type);
}

Result<size_t> Network::Recv(
    Socket sock,
    void* buffer,
    size_t length) const
{
    return Recv(sock, buffer, length, MessageOption::None);
}

Result<Network::RecvFromData> Network::RecvFrom(
    Socket sock,
    void* buffer,
    size_t length) const
{
    return RecvFrom(sock, buffer, length, MessageOption::None);
}

Result<size_t> Network::Send(
    Socket sock,
    const void* buffer,
    size_t length) const
{
    return Send(sock, buffer, length, MessageOption::None);
}

Result<size_t> Network::SendTo(
    Socket sock,
    const SocketAddress& address,
    const void* buffer,
    size_t length) const
{
    return SendTo(sock, address, buffer, length, MessageOption::None);
}
// Network                                                   END
// -------------------------------------------------------------
// SocketPair                                              START
Result<std::unique_ptr<SocketPair>> SocketPair::Create(
    Network& net,
    Type blocking)
{
    auto pair = std::make_unique<SocketPair>(net);

    if (auto result = pair->Start(blocking); !result)
    {
        return result.Error();
    }

    return pair;
}

SocketPair::SocketPair(Network& net)
    : m_network(net)
{ }

SocketPair::~SocketPair() { Stop(); }

Result<void> SocketPair::Drain()
{
    if (m_sockets[0] == INVALID_SOCKET)
    {
        return Failure{ E_NOT_INITIALIZED };
    }

    std::array<char, 16U> buffer = { 0 };

    while (true)
    {
        if (auto result = m_network.Recv(
            Reader(),
            buffer.data(),
            buffer.size()); !result)
        {
            const auto& failure = result.Error();

            if (failure.Error() != E_SUCCESS
                && failure.Error() != E_NET_WOULD_BLOCK)
            {
                return failure;
            }
            if (failure.Error() == E_NET_WOULD_BLOCK)
            {
                break;
            }
        }
        else if (*result > 0)
        {
            continue;
        }
    }

    return Success;
}

Socket SocketPair::Reader() const
{
    return m_sockets[0];
}

Result<void> SocketPair::Start(Type blocking)
{
    if (m_sockets[0] != INVALID_SOCKET)
    {
        return Success;
    }

    Socket read{ INVALID_SOCKET };
    Socket write{ INVALID_SOCKET };

    FUSION_SCOPE_GUARD([&] { m_network.Close(read); });

    if (auto result = m_network.CreateSocket(TCPv4); !result)
    {
        return result.Error()
            .WithContext("failed to initialize notify listener");
    }
    else
    {
        // Set the newly created socket to the correct temporary.
        read = *result;
    }

    SocketAddress address{ InaddrLoopback, 0 };
    if (auto result = m_network.Bind(read, address); !result)
    {
        return result.Error()
            .WithContext("failed to bind notification listener '{}' to '{}'",
                read, address);
    }
    if (auto result = m_network.Listen(read, 1); !result)
    {
        return result.Error()
            .WithContext("failed to listen on notification listener '{}'",
                read);
    }

    SocketAddress remote;
    {
        if (auto result = m_network.GetSockName(read); !result)
        {
            return result.Error()
                .WithContext("failed to determine listening socket name '{}'",
                    read);
        }
        else
        {
            remote = *result;
        }
    }
    if (auto result = m_network.CreateSocket(TCPv4); !result)
    {
        return result.Error()
            .WithContext("failed to initialize notify writer");
    }
    else
    {
        write = *result;
    }
    if (auto result = m_network.Connect(write, remote); !result)
    {
        return result.Error()
            .WithContext("failed to connect notify writer '{}'", write);
    }
    if (auto result = m_network.SetBlocking(write, false); !result)
    {
        return result.Error()
            .WithContext("failed to set non-blocking on notification writer '{}'", write);
    }

    if (auto result = m_network.Accept(read); !result)
    {
        return result.Error()
            .WithContext("failed to connect read end of the notify pair '{}'", read);
    }
    else
    {
        m_sockets[0] = result->sock;
    }

    bool _blocking{ false };

    switch (blocking)
    {
    case Type::NonBlocking:
        _blocking = false;
        break;
    case Type::Blocking:
        _blocking = true;
        break;
    }

    m_sockets[1] = write;
    if (auto result = m_network.SetBlocking(Writer(), _blocking); !result)
    {
        return result.Error()
            .WithContext("failed to set non-blocking on notification writer '{}'", write);
    }
    if (auto result = m_network.SetBlocking(Reader(), _blocking); !result)
    {
        return result.Error()
            .WithContext("failed to set non-blocking on notification listener '{}'",
                m_sockets[0]);
    }

    return Success;
}

std::future<Result<void>> SocketPair::Stop()
{
    std::promise<Result<void>> promise;
    std::future<Result<void>> future = promise.get_future();

    if (Socket& reader = m_sockets[0]; reader != INVALID_SOCKET)
    {
        m_network.Close(reader);
        reader = INVALID_SOCKET;
    }
    if (Socket& writer = m_sockets[1]; writer != INVALID_SOCKET)
    {
        m_network.Close(writer);
        writer = INVALID_SOCKET;
    }

    promise.set_value(Success);
    return future;
}

Socket SocketPair::Writer() const
{
    return m_sockets[1];
}
// SocketPair                                                END
// -------------------------------------------------------------
// Poll                                                    START
Result<size_t> Poll(
    PollFd& fd,
    Clock::duration timeout)
{
    return Poll(&fd, 1, timeout);
}
// Poll                                                      END
// -------------------------------------------------------------
// SocketEvent                                             START
bool SocketEvent::operator==(const Socket& s) const
{
    return this->sock == s;
}

bool SocketEvent::operator!=(const Socket& s) const
{
    return this->sock != s;
}

bool SocketEvent::operator<(const Socket& s) const
{
    return this->sock < s;
}
// SocketEvent                                               END
// -------------------------------------------------------------
// SocketService                                           START
Result<std::unique_ptr<SocketService>> SocketService::Create(
    Network& network)
{
    return Create(Params{}, network);
}

Result<std::unique_ptr<SocketService>> SocketService::Create(
    Params params,
    Network& network)
{
    using namespace Fusion::Internal;

    std::unique_ptr<SocketService> service;

    switch (params.type)
    {
#if FUSION_PLATFORM_WINDOWS
    case Type::Default:
    case Type::Iocp:
    {
        service = std::make_unique<IocpSocketService>(
            std::move(params),
            network);
        break;
    }
#else
    case Type::Iocp:
        return Failure{ E_NOT_SUPPORTED };
#endif
#if FUSION_PLATFORM_LINUX
    case Type::Default:
    case Type::Epoll:
    {
        service = std::make_unique<EPollSocketService>(
            std::move(params),
            network);
        break;
    }
#else
    case Type::Epoll:
        return Failure{ E_NOT_SUPPORTED };
#endif
#if FUSION_PLATFORM_APPLE
    case Type::Default:
    case Type::Kqueue:
        service = std::make_unique<KQueueSocketService>(
            std::move(params),
            network);
        break;
#else
    case Type::Kqueue:
        return Failure{ E_NOT_SUPPORTED };
#endif
    default:
    {
        service = std::make_unique<SelectSocketService>(
            std::move(params),
            network);
        break;
    }
    }

    if (auto result = service->Start(); !result)
    {
        return result.Error()
            .WithContext("failed to start socket service");
    }

    return service;
}

SocketService::SocketService(Type type, Operation op)
    : m_type(type)
    , m_operation(op)
{ }

SocketService::~SocketService() = default;

Result<std::span<SocketEvent>> SocketService::Execute()
{
    return Execute(std::chrono::seconds(-1));
}

SocketService::Operation SocketService::GetOperation() const
{
    return m_operation;
}

SocketService::Type SocketService::GetType() const
{
    return m_type;
}
// SocketService                                             END
// -------------------------------------------------------------

}  // namespace Fusion
