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

#if !defined(FUSION_IMPL_UUID)
#error "Uuid impl included before main header"
#endif

#include <Fusion/Fwd/Hash.h>

#include <array>

#include <fmt/format.h>

namespace Fusion
{
template<typename Algorithm>
struct Hash<UUID, Algorithm>
{
    size_t operator()(const UUID& uuid) const
    {
        return size_t(typename Algorithm::Hash(uuid.bytes));
    }
};
}  // namespace Fusion

template <>
struct fmt::formatter<Fusion::UUID>
    : fmt::formatter<fmt::string_view>
{
    auto format(const Fusion::UUID& uuid, format_context& ctx)
    {
        std::array<char, Fusion::UUID::LENGTH> buffer = { 0 };

        return formatter<fmt::string_view>::format(
            Fusion::ToString(buffer, uuid),
            ctx);
    }
};
