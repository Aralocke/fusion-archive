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
#include <iosfwd>
#include <string_view>
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

}  // namespace Fusion

#define FUSION_IMPL_NETWORK 1
#include <Fusion/Impl/Network.h>
