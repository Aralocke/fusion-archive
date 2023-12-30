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

#pragma once

#if !defined(FUSION_IMPL_NETWORK)
#error "Network impl included before main header"
#endif

#include <fmt/format.h>

namespace Fusion
{
template<size_t LENGTH>
std::string_view ToString(
    AddressFamily family,
    char(&buffer)[LENGTH])
{
    return ToString(family, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    AddressInfoFlags flag,
    char(&buffer)[LENGTH])
{
    return ToString(flag, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    MessageOption option,
    char(&buffer)[LENGTH])
{
    return ToString(option, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    PollFlags flag,
    char(&buffer)[LENGTH])
{
    return ToString(flag, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    SocketOperation operation,
    char(&buffer)[LENGTH])
{
    return ToString(operation, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    SocketOpt option,
    char(&buffer)[LENGTH])
{
    return ToString(option, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    SocketProtocol proto,
    char(&buffer)[LENGTH])
{
    return ToString(proto, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    SocketShutdownMode mode,
    char(&buffer)[LENGTH])
{
    return ToString(mode, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    SocketType type,
    char(&buffer)[LENGTH])
{
    return ToString(type, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    const InetAddress& address,
    char(&buffer)[LENGTH])
{
    return ToString(address, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    const Inet6Address& address,
    char(&buffer)[LENGTH])
{
    return ToString(address, buffer, LENGTH);
}

template<size_t LENGTH>
std::string_view ToString(
    const SocketAddress& address,
    char(&buffer)[LENGTH])
{
    return ToString(address, buffer, LENGTH);
}

template<typename SockOption>
Result<typename SockOption::Type>
Network::GetSocketOption(Socket sock) const
{
    using Type = typename SockOption::Type;

    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT);
    }

    if constexpr (std::is_same_v<Type, bool>)
    {
        // Ensure that the data storage for <bool> meets expectations

        Type optionData = {};
        size_t optionSize = SockOption::size;

        if (Result<void> result = GetSocketOption(
            sock,
            SockOption::option,
            &optionData,
            optionSize); !result)
        {
            return result.Error();
        }

        return bool(optionData == 1);
    }
    else if constexpr (std::is_same_v<Type, int32_t>)
    {
        static const size_t kDataSize = sizeof(Type);

        Type optionData = {};
        size_t optionSize = SockOption::size;

        if (Result<void> result = GetSocketOption(
            sock,
            SockOption::option,
            &optionData,
            optionSize); !result)
        {
            return result.Error();
        }

        FUSION_ASSERT(optionSize == kDataSize,
            "System expected a different data type that int32");

        return optionData;
    }
    else if constexpr (std::is_same_v<Type, Clock::duration>)
    {
        // Not Implemented
    }
    else if constexpr (std::is_same_v<Type, MulticastGroup>)
    {
        // Not Implemented
    }

    return Failure(E_NOT_SUPPORTED);
}

template<typename SockOption>
Result<void> Network::SetSocketOption(
    Socket sock,
    typename SockOption::Type value) const
{
    using Type = typename SockOption::Type;

    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT);
    }

    if constexpr (std::is_same_v<bool, Type>)
    {
        int32_t option = (value) ? 1 : 0;

        if (Result<void> result = SetSocketOption(
            sock,
            SockOption::option,
            &value,
            SockOption::size); !result)
        {
            return result.Error();
        }

        return Success;
    }
    else if constexpr (std::is_same_v<int32_t, Type>)
    {
        if (Result<void> result = SetSocketOption(
            sock,
            SockOption::option,
            &value,
            SockOption::size); !result)
        {
            return result.Error();
        }

        return Success;
    }
    else if constexpr (std::is_same_v<Type, Clock::duration>)
    {
        // Not Implemented
    }
    else if constexpr (std::is_same_v<Type, MulticastGroup>)
    {
        // Not Implemented
    }

    return Failure(E_NOT_SUPPORTED);
}

}  // namespace Fusion

template<>
struct fmt::formatter<Fusion::AddressFamily>
    : fmt::formatter<fmt::string_view>
{
    template<typename Context>
    auto format(
        const Fusion::AddressFamily& family,
        Context& ctx)
    {
        return formatter<fmt::string_view>::format(
            Fusion::ToString(family),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::AddressInfoFlags>
    : fmt::formatter<std::string>
{
    template<typename Context>
    auto format(
        const Fusion::AddressInfoFlags& flags,
        Context& ctx)
    {
        return formatter<std::string>::format(
            Fusion::FlagsToString(flags),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::InetAddress>
    : fmt::formatter<std::string>
{
    template<typename Context>
    auto format(
        const Fusion::InetAddress& address,
        Context& ctx)
    {
        return formatter<std::string>::format(
            Fusion::ToString(address),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::Inet6Address>
    : fmt::formatter<std::string>
{
    template<typename Context>
    auto format(
        const Fusion::Inet6Address& address,
        Context& ctx)
    {
        return formatter<std::string>::format(
            Fusion::ToString(address),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::MessageOption>
    : fmt::formatter<std::string>
{
    template<typename Context>
    auto format(
        const Fusion::MessageOption& options,
        Context& ctx)
    {
        return formatter<std::string>::format(
            Fusion::FlagsToString(options),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::PollFlags>
    : fmt::formatter<std::string>
{
    template<typename Context>
    auto format(
        const Fusion::PollFlags& flags,
        Context& ctx)
    {
        return formatter<std::string>::format(
            Fusion::FlagsToString(flags),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::SocketAddress>
    : fmt::formatter<std::string>
{
    template<typename Context>
    auto format(
        const Fusion::SocketAddress& address,
        Context& ctx)
    {
        return formatter<std::string>::format(
            Fusion::ToString(address),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::SocketOperation>
    : fmt::formatter<std::string>
{
    template<typename Context>
    auto format(
        const Fusion::SocketOperation& operations,
        Context& ctx)
    {
        return formatter<std::string>::format(
            Fusion::FlagsToString(operations),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::SocketOpt>
    : fmt::formatter<fmt::string_view>
{
    template<typename Context>
    auto format(
        const Fusion::SocketOpt& opt,
        Context& ctx)
    {
        return formatter<fmt::string_view>::format(
            Fusion::ToString(opt),
            ctx);
    }
};

template<Fusion::SocketOpt opt, typename T>
struct fmt::formatter<Fusion::SocketOption<opt, T>>
{
    template<typename Context>
    auto format(
        Fusion::SocketOption<opt, T>,
        Context& ctx)
    {
        return fmt::format_to(ctx, "SocketOption<{}>", opt);
    }
};

template<>
struct fmt::formatter<Fusion::SocketProtocol>
    : fmt::formatter<fmt::string_view>
{
    template<typename Context>
    auto format(
        const Fusion::SocketProtocol& protocol,
        Context& ctx)
    {
        return formatter<fmt::string_view>::format(
            Fusion::ToString(protocol),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::SocketType>
    : fmt::formatter<fmt::string_view>
{
    template<typename Context>
    auto format(
        const Fusion::SocketType& type,
        Context& ctx)
    {
        return formatter<fmt::string_view>::format(
            Fusion::ToString(type),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::AddressInfo>
    : fmt::formatter<Fusion::SocketAddress>
{
    // Out of order because it relies on the formatter for
    // SocketAddress already being defined

    template<typename Context>
    auto format(
        const Fusion::AddressInfo& address,
        Context& ctx)
    {
        return formatter<Fusion::SocketAddress>::format(
            address.address,
            ctx);
    }
};
