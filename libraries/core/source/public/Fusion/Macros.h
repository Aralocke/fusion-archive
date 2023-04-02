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

#include <Fusion/Compiler.h>

#if FUSION_HAS_EXCEPTIONS
#define FUSION_NOEXCEPT noexcept
#else
#define FUSION_NOEXCEPT
#endif

#define _FUSION_WIDE(str) L ## str
#define FUSION_WIDE(str) _FUSION_WIDE(str)

#define _FUSION_JOIN(A, B) A ## B
#define FUSION_JOIN(A, B) _FUSION_JOIN(A, B)

#define FUSION_ANONYMOUS_SYMBOL(A) FUSION_JOIN(A, __COUNTER__)

namespace Fusion
{
    struct Ignored
    {
        template<typename T>
        constexpr const Ignored& operator=(const T&) const noexcept
        {
            // Does nothing
            return *this;
        }
    };

    constexpr Ignored Ignore{};
}  // namespace Fusion

#define FUSION_UNUSED(x) Fusion::Ignore = x;
