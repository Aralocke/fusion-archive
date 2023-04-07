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
