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

#include <Fusion/Hash.h>

#include <xxhash.h>

namespace Fusion
{
// -------------------------------------------------------------
// XXHASH                                                  START
template<>
uint32_t XXHASH<uint32_t>::Hash(
    const void* data,
    size_t size,
    uint32_t seed)
{
    return XXH32(data, size, seed);
}

template<>
uint32_t XXHASH<uint32_t>::HashInternal(
    const void* data,
    size_t size,
    uint32_t seed)
{
    return XXH32(data, size, seed);
}

template<>
uint64_t XXHASH<uint64_t>::Hash(
    const void* data,
    size_t size,
    uint64_t seed)
{
    return XXH64(data, size, seed);
}

template<>
uint64_t XXHASH<uint64_t>::HashInternal(
    const void* data,
    size_t size,
    uint64_t seed)
{
    return XXH64(data, size, seed);
}
// XXHASH                                                     END
// --------------------------------------------------------------
}  // namespace Fusion
