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

#include <Fusion/Macros.h>

namespace Fusion
{
struct PlaceholderNewTag {};
using sizeof_t = decltype(sizeof(int));
}

// We're creating our own placeholder-new override here for use with the
// MemoryUtil and other classes. By default we would normally have to
// include <new> but this prevents us from needing to include a massive
// header in multiple places.

FUSION_NODISCARD inline void* operator new(
    Fusion::sizeof_t count,
    Fusion::PlaceholderNewTag tag,
    void* ptr) noexcept
{
    FUSION_UNUSED(count);
    FUSION_UNUSED(tag);

    return ptr;
}

FUSION_NODISCARD inline void* operator new[](
    Fusion::sizeof_t count,
    Fusion::PlaceholderNewTag tag,
    void* ptr) noexcept
{
    FUSION_UNUSED(count);
    FUSION_UNUSED(tag);

    return ptr;
}

inline void operator delete(
    void*,
    Fusion::PlaceholderNewTag,
    void*) noexcept
{
    // See MemoryUtil::Destruct
}

inline void operator delete[](
    void*,
    Fusion::PlaceholderNewTag,
    void*) noexcept
{
    // See MemoryUtil::Destruct
}

#define FUSION_PLACEMENT_NEW(ptr,...) \
    new(Fusion::PlaceholderNewTag{}, ptr, ##__VA_ARGS__)
