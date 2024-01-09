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

#if !defined(FUSION_IMPL_CONSOLE)
#error "Console impl included before main header"
#endif

namespace Fusion
{
template<typename ...Args>
void Console::Write(
    Target target,
    fmt::format_string<Args...> format,
    Args&& ...args) noexcept
{
    try
    {
        std::string message = fmt::format(
            format,
            std::forward<Args>(args)...);

        Write(target, message);
    }
    catch (const std::exception& ex)
    { }
}
}  // namespace Fusion

template <>
struct fmt::formatter<Fusion::Console::Color>
    : fmt::formatter<std::string_view>
{
    auto format(
        const Fusion::Console::Color& color,
        format_context& ctx) const
    {
        return formatter<std::string_view>::format(
            Fusion::ToString(color),
            ctx);
    }
};
