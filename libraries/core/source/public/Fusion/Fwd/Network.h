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

#include <Fusion/Platform.h>
#include <Fusion/Types.h>

struct addrinfo;
struct sockaddr;
struct sockaddr_storage;

namespace Fusion
{
enum class AddressFamily : uint8_t;
enum class AddressInfoFlags : uint16_t;
enum class MessageOption : uint8_t;
enum class PollFlags : uint8_t;
enum class SocketOperation : uint8_t;
enum class SocketOpt : uint8_t;
enum class SocketProtocol : uint8_t;
enum class SocketShutdownMode : uint8_t;
enum class SocketType : uint8_t;

#if FUSION_PLATFORM_WINDOWS
using Socket = uintptr_t;
#elif FUSION_PLATFORM_POSIX
using Socket = int;
#else
#exit "Unsupported platform type"
#endif

constexpr Socket INVALID_SOCKET = Socket(-1);

struct MulticastGroup;
struct ParsedAddress;

class InetAddress;
class Inet6Address;
class SocketConfig;
}  // namespace Fusion
