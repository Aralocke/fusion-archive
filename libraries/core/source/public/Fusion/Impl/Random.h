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

#if !defined(FUSION_IMPL_RANDOM)
#error "Random impl included before main header"
#endif

namespace Fusion
{
template<typename RandomState, size_t N>
void Random::Bytes(
    RandomState& state,
    uint8_t(&buffer)[N])
{
    Bytes(state, buffer, N);
}

template<typename RandomState>
double Random::Double(
    RandomState& state,
    double min,
    double max,
    uint8_t width)
{
    return Double(state, width) * (max - min) + min;
}

template<typename RandomState>
int32_t Random::Int32(
    RandomState& state,
    int32_t min,
    int32_t max)
{
    return int32_t(UInt32(
        state,
        uint32_t(min),
        uint32_t(max)));
}

template<typename RandomState>
int64_t Random::Int64(
    RandomState& state,
    int64_t min,
    int64_t max)
{
    return int64_t(UInt64(
        state,
        uint64_t(min),
        uint64_t(max)));
}

template<size_t N>
Result<void> Random::SecureBytes(uint8_t(&buffer)[N])
{
    return SecureBytes(buffer, N);
}

template<typename RandomState>
uint32_t Random::UInt32(
    RandomState& state,
    uint32_t min,
    uint32_t max)
{
    uint32_t value = Bits32(state);
    uint32_t ret = (max - min + 1);

    if (ret)
    {
        value %= ret;
    }

    return value + min;
}

template<typename RandomState>
uint64_t Random::UInt64(
    RandomState& state,
    uint64_t min,
    uint64_t max)
{
    uint64_t value = Bits64(state);
    uint64_t ret = (max - min + 1);

    if (ret)
    {
        value %= ret;
    }

    return value + min;
}
}  // namespace Fusion
