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

 // Define all of the platform variables first
 // The individual overrides will unset and reset the correct
 // values for the platform.
#define FUSION_PLATFORM_APPLE 0
#define FUSION_PLATFORM_APPLE_IOS 0
#define FUSION_PLATFORM_BSD 0
#define FUSION_PLATFORM_CYGWIN 0
#define FUSION_PLATFORM_DRAGONFLY 0
#define FUSION_PLATFORM_FREEBSD 0
#define FUSION_PLATFORM_IPHONE_SIM 0
#define FUSION_PLATFORM_LINUX 0
#define FUSION_PLATFORM_NETBSD 0
#define FUSION_PLATFORM_OSX 0
#define FUSION_PLATFORM_OPENBSD 0
#define FUSION_PLATFORM_POSIX 0
#define FUSION_PLATFORM_WINDOWS 0
#define FUSION_PLATFORM_WIN32 0
#define FUSION_PLATFORM_WIN64 0
#define FUSION_PLATFORM_UNIX 0

// Only Desktop support for right now
#define FUSION_DESKTOP 1
// Future Android / iOS support
#define FUSION_MOBILE 0

// Define the nits of the build
#define FUSION_32BIT 0
#define FUSION_64BIT 0

// Determine if we're on ARM or X86
#define FUSION_PROC_ARM 0
#define FUSION_PROC_ARM_32 0
#define FUSION_PROC_ARM_64 0
#define FUSION_PROC_X86 0
#define FUSION_PROC_X86_32 0
#define FUSION_PROC_X86_64 0

// Determine endian-ness
#define FUSION_BIG_ENDIAN 0
#define FUSION_LITTLE_ENDIAN 0

// Handle the Windows Platform Defines
#if (defined(WIN32) || (defined(_WIN64) || defined(_WIN32)))
#undef FUSION_PLATFORM_WINDOWS
#define FUSION_PLATFORM_WINDOWS 1
#if defined(_WIN32)
#undef FUSION_PLATFORM_WIN32
#define FUSION_PLATFORM_WIN32 1
#endif  // defined _WIN32
#if defined(_WIN64)
#undef FUSION_PLATFORM_WIN64
#define FUSION_PLATFORM_WIN64 1
#endif  // defined _WIN64
#endif  // Windows platform defines
#if defined(__CYGWIN__) && !defined(_WIN32)
#undef FUSION_PLATFORM_CYGWIN
#define FUSION_PLATFORM_CYGWIN 1
#endif  // Cygwin Windows

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__)))
// Determine teh style of UNIX OS.
// On any UNIX platform we add a definition for the OS.
#undef FUSION_PLATFORM_UNIX
#define FUSION_PLATFORM_UNIX 1

#include <sys/param.h>

#if defined(BSD)
// The BSD definition is set for *all* BSD derivatives. The
// individual OSes each set their own values to separate themselves.
#undef FUSION_PLATFORM_BSD
#define FUSION_PLATFORM_BSD 1
#ifdef __DragonFly__
#undef FUSION_PLATFORM_DRAGONFLY
#define FUSION_PLATFORM_DRAGONFLY 1
#endif  // DragonFly
#ifdef __FreeBSD__
#undef FUSION_PLATFORM_FREEBSD
#define FUSION_PLATFORM_FREEBSD 1
#endif  // FreeBSD
#ifdef __NetBSD__
#undef FUSION_PLATFORM_NETBSD
#define FUSION_PLATFORM_NETBSD 1
#endif  // NetBSD
#ifdef __OpenBSD__
#undef FUSION_PLATFORM_OPENBSD
#define FUSION_PLATFORM_OPENBSD 1
#endif  // OpenBSD
#endif  // defined(BSD)

// All UNIX style OSes have a unistd.h header to define
// their level of POSIX compliance. We use it to determine
// if we are on a valid POSIX system and set the platform
// definition if we are.
#include <unistd.h>

#if defined(_POSIX_VERSION)
#undef FUSION_PLATFORM_POSIX
#define FUSION_PLATFORM_POSIX 1
#endif  // _POSIX_VERSION

#if (defined(__APPLE__) && defined(__MACH__))
#include <TargetConditionals.h>

#undef FUSION_PLATFORM_APPLE
#define FUSION_PLATFORM_APPLE 1

#if defined(TARGET_OS_EMBEDDED) && TARGET_OS_EMBEDDED == 1
#if TARGET_IPHONE_SIMULATOR == 1
#undef FUSION_PLATFORM_IPHONE_SIM
#define FUSION_PLATFORM_IPHONE_SIM 1
#elif TARGET_OS_IPHONE == 1
#undef FUSION_PLATFORM_APPLE_IOS
#define FUSION_PLATFORM_APPLE_IOS 1
#endif  // iOS Embedded devices
#elif TARGET_OS_MAC == 1
#undef FUSION_PLATFORM
#undef FUSION_PLATFORM_OSX
#define FUSION_PLATFORM_OSX 1
#endif  // Apple OSX
#endif  // __APPLE__

// Linux Platform Detection
// The check for __linux__ should be sufficient for the major
// distributions.
//
// Known supported Distributions:
// - Red Hat / CentOS / Fedora
// - Debian / Ubuntu
// - OpenSUSE
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__TOS_LINUX__)
#undef FUSION_PLATFORM_LINUX
#define FUSION_PLATFORM_LINUX 1
#endif // Linux

#endif  // UNIX check

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86) || defined(EMSCRIPTEN) || defined(__EMSCRIPTEN__)
#undef FUSION_PROC_X86_32
#define FUSION_PROC_X86_32 1
#undef FUSION_LITTLE_ENDIAN
#define FUSION_LITTLE_ENDIAN 1
#elif defined(__x86_64__) || defined(_M_X64)
#undef FUSION_PROC_X86_64
#define FUSION_PROC_X86_64 1
#undef FUSION_LITTLE_ENDIAN
#define FUSION_LITTLE_ENDIAN 1
#elif defined(__arm__) || defined(__arm) || defined(ARM) || defined(_ARM_) || defined(__ARM__) || defined(_M_ARM)
#undef FUSION_PROC_ARM_32
#define FUSION_PROC_ARM_32 1
#if defined(__ARMEB__)
#undef FUSION_BIG_ENDIAN
#define FUSION_BIG_ENDIAN 1
#else
#undef FUSION_LITTLE_ENDIAN
#define FUSION_LITTLE_ENDIAN 1
#endif
#elif defined(__arm64__) || defined(__arm64)
#undef FUSION_PROC_ARM_64
#define FUSION_PROC_ARM_64 1
#if defined(__ARMEB__)
#undef FUSION_BIG_ENDIAN
#define FUSION_BIG_ENDIAN 1
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#undef FUSION_BIG_ENDIAN
#define FUSION_BIG_ENDIAN 1
#else
#undef FUSION_LITTLE_ENDIAN
#define FUSION_LITTLE_ENDIAN 1
#endif
#elif defined(__AARCH64EL__)
#define FUSION_LITTLE_ENDIAN 1
#elif defined(__AARCH64EB__)
#undef FUSION_BIG_ENDIAN
#define FUSION_BIG_ENDIAN 1
#endif

#if FUSION_PROC_X86_32 || FUSION_PROC_X86_64
#undef FUSION_PROC_X86
#define FUSION_PROC_X86 1
#endif
#if FUSION_PROC_ARM_32 || FUSION_PROC_ARM_64
#undef FUSION_PROC_ARM
#define FUSION_PROC_ARM 1
#endif

#if FUSION_PROC_X86_32 || FUSION_PROC_ARM_32
#undef FUSION_32BIT
#define FUSION_32BIT 1
#elif FUSION_PROC_X86_64 || FUSION_PROC_ARM_64
#undef FUSION_64BIT
#define FUSION_64BIT 1
#endif
