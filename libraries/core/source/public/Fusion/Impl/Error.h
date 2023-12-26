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

#if !defined(FUSION_IMPL_ERROR)
#error "Error impl included before main header"
#endif

namespace Fusion
{
template<typename ...Args>
Failure::Failure(fmt::format_string<Args...> format, Args&& ...args) noexcept
    : m_error(E_FAILURE)
{
    try
    {
        std::string message = fmt::format(
            format,
            std::forward<Args>(args)...);

        WithContext(std::move(message));
    }
    catch (const std::exception& ex)
    {
        WithContext(ex.what());
    }
    catch (...)
    {
        // TODO: Don't crash here

        std::abort();
    }
}

template<typename ...Args>
Failure& Failure::WithContext(
    fmt::format_string<Args...> format,
    Args&& ...args) noexcept
{
    try
    {
        std::string message = fmt::format(
            format,
            std::forward<Args>(args)...);

        WithContext(std::move(message));
    }
    catch (const std::exception& ex)
    {
        WithContext(ex.what());
    }
    catch (...)
    {
        // TODO: Don't crash here

        std::abort();
    }

    return *this;
    
}
}  // namespace Fusion

template <>
struct fmt::formatter<Fusion::Error> : fmt::formatter<std::string>
{
    auto format(const Fusion::Error& error, format_context& ctx) const
    {
        return formatter<std::string>::format(
            Fusion::ToString(error),
            ctx);
    }
};

template<>
struct fmt::formatter<Fusion::Failure> : fmt::formatter<std::string>
{
    auto format(const Fusion::Failure& failure, format_context& ctx) const
    {
        return formatter<std::string>::format(failure.Summary(), ctx);
    }
};
