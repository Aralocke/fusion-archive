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

//
// Macros for Clang Thread Safety Analysis
// https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
// https://llvm.org/devmtg/2011-11/Hutchins_ThreadSafety.pdf

//
// For guarding data
//
// member value guarded by a mutex
#define FUSION_GUARDED_BY(attr) FUSION_THREAD_ATTRIBUTE(guarded_by(attr))
// member value guarded by a pthread_mutex
#define FUSION_PT_GUARDED_BY(attr) FUSION_THREAD_ATTRIBUTE(pt_guarded_by(attr))

//
// For guarding functions
//
// Cannot hold lock while calling a function
#define FUSION_EXCLUDES(...) FUSION_THREAD_ATTRIBUTE(locks_excluded(__VA_ARGS__))
// Lock must be held before calling function
#define FUSION_REQUIRES_LOCK(...) FUSION_THREAD_ATTRIBUTE(requires_capability(__VA_ARGS__))
// Function annotation that a shared lock is required
#define FUSION_REQUIRES_LOCK_SHARED(...) FUSION_THREAD_ATTRIBUTE(requires_shared_capability(__VA_ARGS__))

//
// For deadlock detection
//
#define FUSION_ACQUIRED_BEFORE(...) FUSION_THREAD_ATTRIBUTE(acquired_before(__VA_ARGS__))
#define FUSION_ACQUIRED_AFTER(...) FUSION_THREAD_ATTRIBUTE(acquired_after(__VA_ARGS__))

// For annotating functions which operate on mutex objects
// Nothing below here is likely to be used unless you implement
// custom mutex / lock types
#define FUSION_CAPABILITY(attr) FUSION_THREAD_ATTRIBUTE(capability(attr))
#define FUSION_SCOPED_CAPABILITY FUSION_THREAD_ATTRIBUTE(scoped_lockable)
#define FUSION_ACQUIRE(...) FUSION_THREAD_ATTRIBUTE(acquire_capability(__VA_ARGS__))
#define FUSION_ACQUIRE_SHARED(...) FUSION_THREAD_ATTRIBUTE(acquire_shared_capability(__VA_ARGS__))
#define FUSION_RELEASE(...) FUSION_THREAD_ATTRIBUTE(release_capability(__VA_ARGS__))
#define FUSION_RELEASE_SHARED(...) FUSION_THREAD_ATTRIBUTE(release_shared_capability(__VA_ARGS__))
#define FUSION_TRY_ACQUIRE(...) FUSION_THREAD_ATTRIBUTE(try_acquire_capability(__VA_ARGS__))
#define FUSION_TRY_ACQUIRE_SHARED(...) FUSION_THREAD_ATTRIBUTE(try_acquire_shared_capability(__VA_ARGS__))
#define FUSION_ASSERT_CAPABILITY(attr) FUSION_THREAD_ATTRIBUTE(assert_capability(attr))
#define FUSION_ASSERT_SHARED_CAPABILITY(attr) FUSION_THREAD_ATTRIBUTE(assert_shared_capability(attr))
#define FUSION_RETURN_CAPABILITY(attr) FUSION_THREAD_ATTRIBUTE(lock_returned(attr))
