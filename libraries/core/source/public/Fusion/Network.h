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

#include <Fusion/Fwd/Network.h>
#include <Fusion/Enum.h>
#include <Fusion/Result.h>

#include <array>
#include <iosfwd>
#include <string_view>
#include <variant>
namespace Fusion
{
//
//
//
enum class AddressFamily : uint8_t
{
    None = 0,

    Unspecified,
    Inet4,
    Inet6,
    Unix,

    _Count
};

//
//
//
std::string_view ToString(AddressFamily family);

//
//
//
std::string_view ToString(
    AddressFamily family,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    AddressFamily family,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    AddressFamily family);

//
//
//
enum class AddressInfoFlags : uint16_t
{
    None = 0,

    AddressConfig = 1 << 1,
    AddressMapped = 1 << 2,
    CanonicalName = 1 << 3,
    Fdqn = 1 << 4,
    NumericHost = 1 << 5,
    NumericService = 1 << 6,
    Passive = 1 << 7,

    All = 0xF,

    _Count
};
FUSION_ENUM_OPS(AddressInfoFlags);

//
//
//
std::string FlagsToString(AddressInfoFlags flags);

//
//
//
std::string_view ToString(AddressInfoFlags flag);

//
//
//
std::string_view ToString(
    AddressInfoFlags flag,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    AddressInfoFlags flag,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    AddressInfoFlags flags);

//
//
//
enum class MessageOption : uint8_t
{
    None = 0,

    Confirm = 1 << 1,
    NoSignal = 1 << 2,
    OutOfBand = 1 << 3,
    Peek = 1 << 4,

    _Count
};
FUSION_ENUM_OPS(MessageOption);

//
//
//
std::string FlagsToString(MessageOption option);

//
//
//
std::string_view ToString(MessageOption option);

//
//
//
std::string_view ToString(
    MessageOption option,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    MessageOption option,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    MessageOption option);

//
//
//
enum class PollFlags : uint8_t
{
    None = 0,

    Error = 1 << 1,
    HangUp = 1 << 2,
    Invalid = 1 << 3,
    Read = 1 << 4,
    Write = 1 << 5,

    All = (Error | Read | Write),

    _Count
};
FUSION_ENUM_OPS(PollFlags);

//
//
//
std::string FlagsToString(PollFlags flags);

//
//
//
std::string_view ToString(PollFlags flag);

//
//
//
std::string_view ToString(
    PollFlags flag,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    PollFlags flag,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    PollFlags flags);

//
//
//
enum class SocketOperation : uint8_t
{
    None = 0,

    Read = 1 << 1,
    Write = 1 << 2,
    Error = 1 << 3,
    All = 0xF,

    Accept = (Read | Error),
    ReadWrite = (Read | Write),

    _Count
};
FUSION_ENUM_OPS(SocketOperation);

//
//
//
std::string FlagsToString(SocketOperation operations);

//
//
//
std::string_view ToString(SocketOperation operation);

//
//
//
std::string_view ToString(
    SocketOperation operation,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    SocketOperation operation,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    SocketOperation operations);

//
//
//
enum class SocketOpt : uint8_t
{
    Broadcast,
    Debug,
    DontRoute,
    KeepAlive,
    Linger,
    Multicast,
    MulticastLoopback,
    MulticastTTL,
    NoDelay,
    OobInline,
    RecvBuf,
    RecvLowMark,
    RecvTimeout,
    ReuseAddress,
    ReusePort,
    SendBuf,
    SendLowMark,
    SendTimeout,
    SocketError,
    TcpKeepAlive,
    TcpKeepCount,
    TcpKeepIdle,
    TcpKeepInterval,
    TimeToLive,

    _Count
};

//
//
//
std::string_view ToString(SocketOpt option);

//
//
//
std::string_view ToString(
    SocketOpt option,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    SocketOpt option,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    SocketOpt option);

//
//
//
enum class SocketProtocol : uint8_t
{
    None = 0,
    Icmp,
    Ip,
    Raw,
    Tcp,
    Udp,

    _Count
};

//
//
//
std::string_view ToString(SocketProtocol proto);

//
//
//
std::string_view ToString(
    SocketProtocol proto,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    SocketProtocol proto,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    SocketProtocol proto);

//
//
//
enum class SocketShutdownMode : uint8_t
{
    None = 0,
    Read,
    Write,
    Both,

    _Count
};

//
//
//
std::string_view ToString(SocketShutdownMode mode);

//
//
//
std::string_view ToString(
    SocketShutdownMode mode,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    SocketShutdownMode mode,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    SocketShutdownMode mode);

//
//
//
enum class SocketType : uint8_t
{
    None = 0,
    Datagram,
    Raw,
    Stream,

    _Count
};

//
//
//
std::string_view ToString(SocketType type);

//
//
//
std::string_view ToString(
    SocketType type,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    SocketType type,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    SocketType type);

//
//
//
class SocketConfig final
{
public:
    AddressFamily family{ AddressFamily::Unspecified };
    SocketProtocol protocol{ SocketProtocol::None };
    SocketType type{ SocketType::None };

public:
    constexpr SocketConfig(
        AddressFamily family,
        SocketProtocol protocol,
        SocketType type)
        : family(family)
        , protocol(protocol)
        , type(type)
    { }

    constexpr SocketConfig operator()(AddressFamily family) const
    {
        return SocketConfig(family, protocol, type);
    }
};

//
//
//
constexpr const SocketConfig TCP{
    AddressFamily::Unspecified,
    SocketProtocol::Tcp,
    SocketType::Stream
};

//
//
//
constexpr const SocketConfig TCPv4 = TCP(AddressFamily::Inet4);

//
//
//
constexpr const SocketConfig TCPv6 = TCP(AddressFamily::Inet6);

//
//
//
constexpr const SocketConfig UDP{
    AddressFamily::Unspecified,
    SocketProtocol::Udp,
    SocketType::Datagram
};

//
//
//
constexpr const SocketConfig UDPv4 = UDP(AddressFamily::Inet4);

//
//
//
constexpr const SocketConfig UDPv6 = UDP(AddressFamily::Inet6);

//
//
//
constexpr const Error E_NET_INPROGRESS{ 100, "E_NET_INPROGRESS" };

//
//
//
constexpr const Error E_NET_AGAIN{ 101, "E_NET_AGAIN" };

//
//
//
constexpr const Error E_NET_WOULD_BLOCK{ 102, "E_NET_WOULD_BLOCK" };

//
//
//
constexpr const Error E_NET_NETWORK_DOWN{ 103, "E_NET_NETWORK_DOWN" };

//
//
//
constexpr const Error E_NET_CONN_ABORTED{ 104, "E_NET_CONN_ABORTED" };

//
//
//
constexpr const Error E_NET_CONN_REFUSED{ 105, "E_NET_CONN_REFUSED" };

//
//
//
constexpr const Error E_NET_CONN_RESET{ 106, "E_NET_CONN_RESET" };

//
//
//
constexpr const Error E_NET_CONNECTED{ 107, "E_NET_CONNECTED" };

//
//
//
constexpr const Error E_NET_DISCONNECTED{ 108, "E_NET_DISCONNECTED" };

//
//
//
constexpr const Error E_NET_TIMEOUT{ 109, "E_NET_TIMEOUT" };

//
//
//
constexpr const Error E_NET_UNSUPPORTED{ 110, "E_NET_UNSUPPORTED" };

//
//
//
constexpr const Error E_NET_SIZE_EXCEEDED{ 111, "E_NET_SIZE_EXCEEDED" };

//
//
//
class InetAddress final
{
public:
    static const uint8_t LENGTH = 16;
    static const uint8_t SIZE = 4;

public:
    InetAddress() = default;

    //
    //
    //
    constexpr InetAddress(std::array<uint8_t, SIZE> address)
        : m_address(address)
    { }

    //
    //
    //
    Result<void> FromDecimal(uint32_t address);

    //
    //
    //
    Result<void> FromString(std::string_view address);

    //
    //
    //
    void Assign(const void* address);

    //
    //
    //
    Inet6Address AsV6() const;

    //
    //
    //
    operator Inet6Address() const;

    //
    //
    //
    const uint8_t* Data() const;

    //
    //
    //
    uint8_t* Data();

    //
    //
    //
    bool IsEmpty() const;

    //
    //
    //
    operator bool() const;

    //
    //
    //
    bool IsPrivate() const;

    //
    //
    //
    uint32_t ToDecimal() const;

public:
    bool operator==(const InetAddress& addr) const;
    bool operator!=(const InetAddress& addr) const;
    bool operator<(const InetAddress& addr) const;

private:
    std::array<uint8_t, SIZE> m_address;
};

//
//
//
std::string ToString(const InetAddress& address);

//
//
//
std::string_view ToString(
    const InetAddress& address,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    const InetAddress& address,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    const InetAddress& address);

//
//
//
constexpr const InetAddress InaddrAny{ { 0, 0, 0, 0 } };

//
//
//
constexpr const InetAddress InaddrLoopback{ { 127, 0, 0, 1 } };

//
//
//
constexpr const InetAddress InaddrBroadcast{ { 255, 255, 255, 255 } };

//
//
//
class Inet6Address final
{
public:
    static const uint8_t LENGTH = 64;
    static const uint8_t SIZE = 16;

public:
    Inet6Address() = default;

    constexpr Inet6Address(std::array<uint8_t, SIZE> address)
        : m_address(address)
    { }

    //
    //
    //
    Result<void> FromString(std::string_view address);

    //
    //
    //
    void Assign(const void* address);

    //
    //
    //
    InetAddress AsV4() const;

    //
    //
    //
    operator InetAddress() const;

    //
    //
    //
    const uint8_t* Data() const;

    //
    //
    //
    uint8_t* Data();

    //
    //
    //
    bool IsEmpty() const;

    //
    //
    //
    operator bool() const;

    //
    //
    //
    bool IsMappedV4() const;

    //
    //
    //
    bool IsPrivate() const;

public:
    bool operator==(const Inet6Address& addr) const;
    bool operator!=(const Inet6Address& addr) const;
    bool operator<(const Inet6Address& addr) const;

private:
    std::array<uint8_t, SIZE> m_address;
};

//
//
//
std::string ToString(
    const Inet6Address& address);

//
//
//
std::string_view ToString(
    const Inet6Address& address,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    const Inet6Address& address,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    const Inet6Address& address);

//
//
//
constexpr const Inet6Address InaddrLoopback6 = { {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    1
} };

//
//
//
constexpr const Inet6Address InaddrLoopback6in4 = { {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0xff, 0xff, 127, 0, 0, 1
} };

//
//
//
struct MulticastGroup
{
    InetAddress address;
    InetAddress interface;
};

//
//
//
struct ParsedAddress
{
    union Address
    {
        InetAddress inet;
        Inet6Address inet6;
    };

    Address address;
    AddressFamily family{ AddressFamily::Unspecified };
};

//
//
//
Result<ParsedAddress> ParseAddress(std::string_view address);

//
//
//
class SocketAddress final
{
public:
    //
    //
    //
    struct InetAddr
    {
        InetAddress address;
        uint16_t port = 0;
    };

    //
    //
    //
    const InetAddr& Inet() const;

    //
    //
    //
    InetAddr& Inet();

    //
    //
    //
    struct Inet6Addr
    {
        Inet6Address address;
        uint16_t port = 0;
        uint32_t flowInfo = 0;
        uint32_t scope = 0;
    };

    //
    //
    //
    const Inet6Addr& Inet6() const;

    //
    //
    //
    Inet6Addr& Inet6();

    //
    //
    //
    struct UnixAddr
    {
        static const uint8_t LENGTH = 108U;

        char path[LENGTH + 1] = { 0 };
    };

    //
    //
    //
    const UnixAddr& Unix() const;

    //
    //
    //
    UnixAddr& Unix();

public:
    SocketAddress() = default;

    //
    //
    //
    SocketAddress(
        InetAddress address,
        uint32_t port);

    //
    //
    //
    SocketAddress(
        Inet6Address address,
        uint32_t port);

    //
    //
    //
    AddressFamily Family() const;

    //
    //
    //
    void FromSockAddr(const sockaddr* addr);

    //
    //
    //
    Result<void> FromString(std::string_view address);

    //
    //
    //
    bool IsEmpty() const;

    //
    //
    //
    operator bool() const;

    //
    //
    //
    bool IsValid() const;

    //
    //
    //
    sockaddr* ToSockAddr(
        void* address,
        size_t& length) const;

public:
    bool operator==(const SocketAddress& saddr) const;
    bool operator!=(const SocketAddress& saddr) const;
    bool operator<(const SocketAddress& saddr) const;

private:
    using AddressData = std::variant<
        std::monostate,
        InetAddr,
        Inet6Addr,
        UnixAddr>;

    AddressData m_address;
    AddressFamily m_family{ AddressFamily::None };
};

//
//
//
std::string ToString(const SocketAddress& address);

//
//
//
std::string_view ToString(
    const SocketAddress& address,
    char* buffer,
    size_t length);

//
//
//
template<size_t LENGTH>
std::string_view ToString(
    const SocketAddress& address,
    char(&buffer)[LENGTH]);

//
//
//
std::ostream& operator<<(
    std::ostream& o,
    const SocketAddress& address);


}  // namespace Fusion

#define FUSION_IMPL_NETWORK 1
#include <Fusion/Impl/Network.h>