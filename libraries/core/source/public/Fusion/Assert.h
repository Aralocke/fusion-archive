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

#include <Fusion/Types.h>

 /**
  * Set FUSION_ASSERTIONS_DISABLED to disable all asserts regardless of the
  * NDEBUG definition or FUSION_ASSERTIONS_ENABLED state.
  *
  * Set FUSION_ASSERTIONS_ENABLED to enable assertions if NDEBUG is set to
  * allow them.
  */

#if !defined(FUSION_ASSERTIONS_DISABLED)
#if !defined(FUSION_ASSERTIONS_ENABLED)
#if !defined(NDEBUG)
#define FUSION_ASSERTIONS_ENABLED 1
#else
#define FUSION_ASSERTIONS_ENABLED 0
#endif
#endif  // FUSION_ASSERTIONS_ENABLED
#else
#define FUSION_ASSERTIONS_ENABLED 0
#endif  // FUSION_ASSERTIONS_DISABLED

#if FUSION_ASSERTIONS_ENABLED

#include <Fusion/Macros.h>
#include <cassert>

namespace Fusion
{
//
// Assertion handler
//
void AssertHandler(
    const wchar_t* message,
    const wchar_t* condition,
    const wchar_t* filename,
    unsigned lineno);
}  // namespace Fusion

#define _FUSION_EXPAND( x ) x
#define _FUSION_ASSERT(condition) assert(condition)
#define FUSION_ASSERT_MESSAGE(condition,message) \
    do { \
        if(!(condition)) { \
            Fusion::AssertHandler( \
                FUSION_WIDE(message), \
                FUSION_WIDE(#condition), \
                FUSION_WIDE(__FILE__), \
                static_cast<unsigned>(__LINE__)); \
        } \
    } while (false)

#define FUSION_ASSERT_TYPE(_1,_2,NAME,...) NAME
#define FUSION_ASSERT(...) _FUSION_EXPAND(FUSION_ASSERT_TYPE(__VA_ARGS__, FUSION_ASSERT_MESSAGE, _FUSION_ASSERT)(__VA_ARGS__))
#else
#define FUSION_ASSERT_MESSAGE(...)
#define FUSION_ASSERT(...)
#endif  // FUSION_ASSERTIONS_ENABLED
