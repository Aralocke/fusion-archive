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

// Define all of the initial compiler definitions first.
#define FUSION_COMPILER_APPLECLANG 0
#define FUSION_COMPILER_CLANG 0
#define FUSION_COMPILER_GCC 0
#define FUSION_COMPILER_INTEL 0
#define FUSION_COMPILER_MINGW 0
#define FUSION_COMPILER_MSVC 0

// The definitions for Version macros in all C++ compilers can be found here:
// https://sourceforge.net/p/predef/wiki/Compilers/

#if defined(_MSC_VER)
#undef FUSION_COMPILER_MSVC
#define FUSION_COMPILER_MSVC _MSC_VER
#elif defined(__clang__)
#undef FUSION_COMPILER_CLANG
#define FUSION_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100)
#if defined(__apple_build_version__)
#undef FUSION_COMPILER_APPLECLANG
#define FUSION_COMPILER_APPLECLANG __apple_build_version__
#endif  // __apple_build_version__
#elif defined (__MINGW32__)
#include <windef.h>
#undef FUSION_COMPILER_MINGW
#define FUSION_COMPILER_MINGW (__MINGW32_MAJOR_VERSION * 10000 + __MINGW32_MINOR_VERSION * 100)
#elif defined (__MINGW64__)
#include <windef.h>
#undef FUSION_COMPILER_MINGW
#define FUSION_COMPILER_MINGW (__MINGW64_VERSION_MAJOR * 10000 + __MINGW64_VERSION_MINOR * 100)
#elif defined (__GNUC__)
#undef FUSION_COMPILER_GCC
#define FUSION_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#elif defined (__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)
#undef FUSION_COMPILER_INTEL
#define FUSION_COMPILER_INTEL __INTEL_COMPILER
#else
#error "Unknown compiler"
#endif

/// Allow for querying 'FUSION_COMPILER(<compiler>)' in macro code.
/// Available options are all of the defined 'FUSION_COMPILER_*' macros
/// or:
/// - 'APPLECLANG'
/// - 'CLANG'
/// - 'GCC'
/// - 'INTEL'
/// - 'MINGW'
/// - 'MSVC'
#define FUSION_COMPILER(...) (FUSION_COMPILER_ ## __VA_ARGS__)

// Convert macros to strings
#define _FUSION_STRINGIZE(a) #a
#define FUSION_STRINGIZE(a) _FUSION_STRINGIZE(a)

// FUSION_CURRENT_FUNCTION based off the boost BOOST_CURRENT_FUNCTION
// implementation.
#ifndef FUSION_CURRENT_FUNCTION
#if defined(__GNUC__)
#define FUSION_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define FUSION_CURRENT_FUNCTION __FUNCSIG__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define FUSION_CURRENT_FUNCTION __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define FUSION_CURRENT_FUNCTION __func__
#elif defined(_MSC_VER)
#define FUSION_CURRENT_FUNCTION __FUNCTION__
#else
#define FUSION_CURRENT_FUNCTION "(unknown)"
#endif
#endif  // FUSION_CURRENT_FUNCTION

#if FUSION_COMPILER_CLANG
// CLANG specific macros - START
#define FUSION_HAS_RTTI __has_feature(cxx_rtti)

// Detect if we're allowed to use exceptions in this build.
// By default, we won't but a few places need to know about catching
// exceptions correctly from libraries we may use.
#define FUSION_HAS_EXCEPTIONS __has_feature(cxx_exceptions)
// Determine the default allocation alignment for this machine
#define FUSION_DEFAULT_ALLOC_ALIGNMENT __BIGGEST_ALIGNMENT__

// Platform specific compiler intrinsic
#define FUSION_PUSH_WARNINGS() _Pragma("clang diagnostic push")
#define FUSION_POP_WARNINGS() _Pragma("clang diagnostic pop")
#define FUSION_DISABLE_CLANG_WARNING(n) _Pragma(FUSION_STRINGIZE(clang diagnostic ignored n))
#define FUSION_DISABLE_GCC_WARNING(n)
#define FUSION_DISABLE_GCC_CLANG_WARNING(n) FUSION_DISABLE_CLANG_WARNING(n)
#define FUSION_DISABLE_MSVC_WARNING(n)
#define FUSION_NO_INLINE __attribute__((noinline))
#define FUSION_FORCE_INLINE __attribute__((always_inline)) inline
#define FUSION_RESTRICT __restrict__
#define FUSION_ALIGNED(N) __attribute__((aligned(N)))

#if __has_feature(cxx_alignof)
#define FUSION_ALIGNOF alignof
#else
#define FUSION_ALIGNOF __alignof__
#endif

#define FUSION_ALLOCA(size) __builtin_alloca(size)
#define FUSION_UNREACHABLE() __builtin_unreachable()
#define FUSION_THREAD_LOCAL __thread

#ifndef __has_feature
// Compatibility with non-clang compilers.
#define __has_feature(x) 0
#endif
#ifndef __has_extension
// Compatibility with pre-3.0 compilers.
#define __has_extension __has_feature
#endif

// Determine if we have address sanitizer enabled.
#if __has_feature(address_sanitizer)
#define FUSION_ADDRESS_SANITIZER_ENABLED 1
#define FUSION_SKIP_ADDRESS_SANITIZER __attribute__((no_sanitize("address")))
#else
#define FUSION_ADDRESS_SANITIZER_ENABLED 0
#define FUSION_SKIP_ADDRESS_SANITIZER
#endif
// Determine if we have thread sanitizer enabled.
#if __has_feature(thread_sanitizer)
#define FUSION_THREAD_SANITIZER_ENABLED 1
#define FUSION_SKIP_THREAD_SANITIZER __attribute__((no_sanitize("thread")))
#else
#define FUSION_THREAD_SANITIZER_ENABLED 0
#define FUSION_SKIP_THREAD_SANITIZER
#endif
// Determine if we have undefined behavior sanitizer enabled.
#if __has_feature(undefined_sanitizer)
#define FUSION_UNDEFINED_SANITIZER_ENABLED 1
#define FUSION_SKIP_UNDEFINED_SANITIZER __attribute__((no_sanitize("undefined")))
#else
#define FUSION_UNDEFINED_SANITIZER_ENABLED 0
#define FUSION_SKIP_UNDEFINED_SANITIZER
#endif

// CLANG specific macros - END
#elif FUSION_COMPILER_GCC
// GCC specific macros - START
#if defined(__GXX_RTTI)
#define FUSION_HAS_RTTI 1
#else
#define FUSION_HAS_RTTI 0
#endif

// Detect if we're allowed to use exceptions in this build.
// By default we won't but a few places need to know about catching
// exceptions correctly from libraries we may use.
#if defined(__EXCEPTIONS)
#define FUSION_HAS_EXCEPTIONS 1
#else
#define FUSION_HAS_EXCEPTIONS 0
#endif

// Determine the default allocation alignment for this machine
#define FUSION_DEFAULT_ALLOC_ALIGNMENT __BIGGEST_ALIGNMENT__

// Platform specific compiler instrinsics
#define FUSION_PUSH_WARNINGS() _Pragma("GCC diagnostic push")
#define FUSION_POP_WARNINGS() _Pragma("GCC diagnostic pop")
#define FUSION_DISABLE_CLANG_WARNING(n)
#define FUSION_DISABLE_GCC_WARNING(n) _Pragma(FUSION_STRINGIZE(GCC diagnostic ignored n))
#define FUSION_DISABLE_GCC_CLANG_WARNING(n) FUSION_DISABLE_GCC_WARNING(n)
#define FUSION_DISABLE_MSVC_WARNING(n)
#define FUSION_NO_INLINE __attribute__((noinline))
#define FUSION_FORCE_INLINE __attribute__((always_inline)) inline
#define FUSION_RESTRICT __restrict__
#define FUSION_ALIGNED(N) __attribute__((aligned(N)))
#define FUSION_ALIGNOF __alignof__
#define FUSION_ALLOCA(size) __builtin_alloca(size)
#define FUSION_UNREACHABLE() __builtin_unreachable()
#define FUSION_THREAD_LOCAL __thread

// Determine if we have address sanitizer enabled.
#ifdef __SANITIZE_ADDRESS__
#define FUSION_ADDRESS_SANITIZER_ENABLED 1
#define FUSION_SKIP_ADDRESS_SANITIZER __attribute__((no_sanitize_address))
#else
#define FUSION_ADDRESS_SANITIZER_ENABLED 0
#define FUSION_SKIP_ADDRESS_SANITIZER
#endif
// Determine if we have thread sanitizer enabled.
#ifdef __SANITIZE_THREAD__
#define FUSION_THREAD_SANITIZER_ENABLED 1
#define FUSION_SKIP_THREAD_SANITIZER __attribute__((no_sanitize_thread))
#else
#define FUSION_THREAD_SANITIZER_ENABLED 0
#define FUSION_SKIP_THREAD_SANITIZER
#endif
// Determine if we have undefined behavior sanitizer enabled.
#ifdef __SANITIZE_UNDEFINED__
#define FUSION_UNDEFINED_SANITIZER_ENABLED 1
#define FUSION_SKIP_UNDEFINED_SANITIZER __attribute__((no_sanitize_undefined))
#else
#define FUSION_UNDEFINED_SANITIZER_ENABLED 0
#define FUSION_SKIP_UNDEFINED_SANITIZER
#endif

// GCC specific macros - END
#elif FUSION_COMPILER_MSVC
// MSVC specific macros - START
#if defined(_CPPRTTI)
#define FUSION_HAS_RTTI 1
#else
#define FUSION_HAS_RTTI 0
#endif
// Detect if we're allowed to use exceptions in this build.
// By default we won't but a few places need to know about catching
// exceptions correctly from libraries we may use.
#if defined(_CPPUNWIND)
#define FUSION_HAS_EXCEPTIONS 1
#else
#define FUSION_HAS_EXCEPTIONS 0
#endif

// Determine the default allocation alignment for this machine
#ifdef __STDCPP_DEFAULT_NEW_ALIGNMENT__
#define FUSION_DEFAULT_ALLOC_ALIGNMENT __STDCPP_DEFAULT_NEW_ALIGNMENT__
#elif defined(_M_X64) || defined(_M_ARM64)
#define FUSION_DEFAULT_ALLOC_ALIGNMENT 16
#elif defined(_M_IX86) || defined(_M_ARM)
#define FUSION_DEFAULT_ALLOC_ALIGNMENT 8
#else
#error "Unable to determine alignment for this machine"
#endif

// Platform specific compiler instrinsics
#define FUSION_PUSH_WARNINGS() __pragma(warning(push))
#define FUSION_POP_WARNINGS() __pragma(warning(pop))
#define FUSION_DISABLE_CLANG_WARNING(n)
#define FUSION_DISABLE_GCC_WARNING(n)
#define FUSION_DISABLE_GCC_CLANG_WARNING(n)
#define FUSION_DISABLE_MSVC_WARNING(n) __pragma(warning(disable: n))
#define FUSION_NO_INLINE __declspec(noinline)
#define FUSION_FORCE_INLINE __forceinline
#define FUSION_RESTRICT __restrict
#define FUSION_ALIGNED(N) __declspec(align(N))
#define FUSION_ALIGNOF alignof
#define FUSION_ALLOCA(size) _alloca(size)
#define FUSION_UNREACHABLE() __assume(0)
#define FUSION_THREAD_LOCAL __declspec(thread)

// Determine if we have address sanitizer enabled.
// Microsoft docs on Address Sanitizer:
// https://learn.microsoft.com/en-us/cpp/sanitizers/asan-building?view=msvc-170
#ifdef __SANITIZE_ADDRESS__
#define FUSION_ADDRESS_SANITIZER_ENABLED 1
#define FUSION_SKIP_ADDRESS_SANITIZER __declspec(no_sanitize_address)
#else
#define FUSION_ADDRESS_SANITIZER_ENABLED 0
#define FUSION_SKIP_ADDRESS_SANITIZER
#endif
// Thread Sanitizer is not supported on Windows at all
#define FUSION_THREAD_SANITIZER_ENABLED 0
#define FUSION_SKIP_THREAD_SANITIZER
// Undefined Behavior Sanitizer is not supported on Windows at all
#define FUSION_UNDEFINED_SANITIZER_ENABLED 0
#define FUSION_SKIP_UNDEFINED_SANITIZER
// Ensure we have unicode / utf8 support enabled
static_assert(_NATIVE_WCHAR_T_DEFINED, "wchar_t support required");
// MSVC specific macros - END
#else
#error "Unsupported compiler"
#endif

#if FUSION_COMPILER_CLANG
#if FUSION_COMPILER_APPLECLANG
// APPLE CLANG Specific - START
#define FUSION_CPP17_ATTRIBUTE_FALLTHROUGH \
    (FUSION_COMPILER_APPLECLANG >= 8000000 && __cplusplus > 201402L)
#define FUSION_CPP17_ATTRIBUTE_MAYBE_UNUSED \
    (FUSION_COMPILER_APPLECLANG >= 8000000 && __cplusplus > 201402L)
#define FUSION_CPP17_ATTRIBUTE_NODISCARD \
    (FUSION_COMPILER_APPLECLANG >= 8000000 && __cplusplus > 201402L)
// APPLE CLANG Specific - END
#else
// CLANG (non-Apple) Specific - START
#define FUSION_CPP17_ATTRIBUTE_FALLTHROUGH \
    (FUSION_COMPILER_CLANG >= 30900 && __cplusplus > 201402L)
#define FUSION_CPP17_ATTRIBUTE_MAYBE_UNUSED \
    (FUSION_COMPILER_CLANG >= 30900 && __cplusplus > 201402L)
#define FUSION_CPP17_ATTRIBUTE_NODISCARD \
    (FUSION_COMPILER_CLANG >= 30900 && __cplusplus > 201402L)
// CLANG (non-Apple) Specific - END
#endif
#elif FUSION_COMPILER_GCC
// GCC Specific - START
#define FUSION_CPP17_ATTRIBUTE_FALLTHROUGH \
    (FUSION_COMPILER_GCC >= 70000 && __cplusplus > 201402L)
#define FUSION_CPP17_ATTRIBUTE_MAYBE_UNUSED \
    (FUSION_COMPILER_GCC >= 70000 && __cplusplus > 201402L)
#define FUSION_CPP17_ATTRIBUTE_NODISCARD \
    (FUSION_COMPILER_GCC >= 70000 && __cplusplus > 201402L)
// GCC Specific - END
#elif FUSION_COMPILER_MSVC
// MSVC Specific - START
#ifndef _MSVC_LANG
#define _MSVC_LANG 0
#endif
#define FUSION_CPP17_ATTRIBUTE_FALLTHROUGH \
    (FUSION_COMPILER_MSVC >= 1910 && _MSVC_LANG > 201402)
#define FUSION_CPP17_ATTRIBUTE_MAYBE_UNUSED \
    (FUSION_COMPILER_MSVC >= 1911 && _MSVC_LANG > 201402)
#define FUSION_CPP17_ATTRIBUTE_NODISCARD \
    (FUSION_COMPILER_MSVC >= 1911 && _MSVC_LANG > 201402)
// MSVC Specific - END
#else
#error "Unsupported compiler"
#endif

#if FUSION_CPP17_ATTRIBUTE_FALLTHROUGH
#define FUSION_FALLTHROUGH [[fallthrough]]
#elif FUSION_COMPILER_GCC && (FUSION_COMPILER_GCC >= 70000)
#define FUSION_FALLTHROUGH [[gnu::fallthrough]]
#else
#define FUSION_FALLTHROUGH
#endif

#if FUSION_CPP17_ATTRIBUTE_MAYBE_UNUSED
#define FUSION_MAYBE_UNUSED [[maybe_unused]]
#else
#define FUSION_MAYBE_UNUSED
#endif

#if FUSION_CPP17_ATTRIBUTE_NODISCARD
#define FUSION_NODISCARD [[nodiscard]]
#else
#define FUSION_NODISCARD
#endif

#if FUSION_THREAD_SANITIZER_ENABLED
#if FUSION_COMPILER_CLANG
#define FUSION_THREAD_ATTRIBUTE(attr) __attribute__((attr))
#else
#define FUSION_THREAD_ATTRIBUTE(attr)
#endif
#endif

namespace Fusion
{
//
//
//
class Compiler final
{
public:
    //
    //
    //
    static constexpr bool Is32Bit()
    {
#if FUSION_32BIT
        return true;
#else
        return false;
#endif
    }

    //
    //
    //
    static bool Is64Bit()
    {
#if FUSION_64BIT
        return true;
#else
        return false;
#endif
    }
};
}  // namespace Fusion
