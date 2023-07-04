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

#if !defined(FUSION_IMPL_HTTP)
#error "Http impl included before main header"
#endif

#include <fmt/core.h>

namespace Fusion
{
}  // namespace Fusion

template<>
struct fmt::formatter<Fusion::HttpMethod>
    : fmt::formatter<fmt::string_view>
{
    template<typename Context>
    auto format(
        const Fusion::HttpMethod& method,
        Context& ctx)
    {
        return formatter<fmt::string_view>::format(
            Fusion::ToString(method),
            ctx);
    }
};
