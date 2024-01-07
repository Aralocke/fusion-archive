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

#include <Fusion/Result.h>

#include <iosfwd>
#include <span>
#include <string>

namespace Fusion
{
class UUID final
{
public:
    static Result<UUID> FromString(std::string_view str);
    static Result<UUID> V4();

public:
    static constexpr size_t SIZE = 16U;
    static constexpr size_t LENGTH = 36U + 1;  // 32 + 4 Dashes

public:
    uint8_t Version() const;

public:
    alignas(uint8_t) uint8_t bytes[SIZE] = { 0 };
};

constexpr UUID EmptyUuid;

bool operator==(const UUID& left, const UUID& right);
bool operator!=(const UUID& left, const UUID& right);
bool operator<(const UUID& left, const UUID& right);
bool operator<=(const UUID& left, const UUID& right);
bool operator>(const UUID& left, const UUID& right);
bool operator>=(const UUID& left, const UUID& right);

std::string ToString(const UUID& uuid);

std::string_view ToString(
    std::span<char> buffer,
    const UUID& uuid);

std::ostream& operator<<(std::ostream& o, const UUID& uuid);
}  // namespace Fusion

#define FUSION_IMPL_UUID 1
#include <Fusion/Impl/Uuid.h>
