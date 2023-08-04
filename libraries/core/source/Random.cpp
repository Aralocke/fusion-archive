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

#include <Fusion/Internal/Random.h>

#include <Fusion/Compiler.h>
#include <Fusion/DateTime.h>
#include <Fusion/Platform.h>

// Define this to disable hardware random if the system
// supports it.
//
// #define FUSION_DISABLE_HARDWARE_RANDOM 1

namespace Fusion
{
// ------------------------------------------------------------
// Utilities                                              START
#pragma region Utilities

constexpr uint32_t Mix32(uint32_t v)
{
    v ^= v >> 16;
    v *= 0x85ebca6b;
    v ^= v >> 13;
    v *= 0xc2b2ae35;
    v ^= v >> 16;
    return v;
}

constexpr uint64_t Mix64(uint64_t v)
{
    v ^= v >> 33;
    v *= 0xff51afd7ed558ccdULL;
    v ^= v >> 33;
    v *= 0xc4ceb9fe1a85ec53ULL;
    v ^= v >> 33;
    return v;
}

constexpr uint32_t Mix(int32_t v)
{
    return Mix32(uint32_t(v));
}

constexpr uint64_t Mix(int64_t v)
{
    return Mix32(uint64_t(v));
}

constexpr uint64_t rotl(const uint64_t v, int32_t k)
{
    return (v << k) | (v >> (64 - k));
}

#pragma endregion Utilities
// Utilities                                                END
// ------------------------------------------------------------
// RdRandX86                                              START
#pragma region rdrand

#if FUSION_PROC_X86_32

    using RdRandIntegerType = unsigned int;

#if FUSION_COMPILER_MSVC
    extern "C" int _rdrand64_step(unsigned int*);
#define FUSION_RDRAND_STEP _rdrand64_step
#elif FUSION_COMPILER_CLANG && FUSION_HAS_BUILTIN(__builtin_ia32_rdrand32_step)
#define FUSION_RDRAND_STEP __builtin_ia32_rdrand32_step
#elif FUSION_COMPILER_GCC && FUSION_COMPILER_GCC >= 40900
#define FUSION_RDRAND_STEP __builtin_ia32_rdrand32_step
#endif

#elif FUSION_PROC_X86_64
    using RdRandIntegerType = unsigned long long;

#if FUSION_COMPILER_MSVC
    extern "C" int _rdrand64_step(unsigned __int64*);
    #define FUSION_RDRAND_STEP _rdrand64_step
#elif FUSION_COMPILER_CLANG && FUSION_HAS_BUILTIN(__builtin_ia32_rdrand64_step)
    #define FUSION_RDRAND_STEP __builtin_ia32_rdrand64_step
#elif FUSION_COMPILER_GCC && FUSION_COMPILER_GCC >= 40900
    #define FUSION_RDRAND_STEP __builtin_ia32_rdrand64_step
#endif

#endif

#if defined(FUSION_RDRAND_STEP)
#if FUSION_COMPILER_GCC || FUSION_COMPILER_CLANG
__attribute__((__target__("rdrnd")))
#endif
static bool _RdRand(RdRandIntegerType& i)
{
    RdRandIntegerType v;
    return FUSION_RDRAND_STEP(&v)
        || FUSION_RDRAND_STEP(&v)
        || FUSION_RDRAND_STEP(&v)
        || FUSION_RDRAND_STEP(&v)
        || FUSION_RDRAND_STEP(&v) ? i = v, true : false;
}
#endif

#pragma endregion rdrand
// RdRandX86                                                END
// ------------------------------------------------------------
// HardwareRandom                                         START
#pragma region HardwareRegion

static bool SupportsHardwareRandom()
{
#if !defined(FUSION_DISABLE_HARDWARE_RANDOM)
#if !defined(FUSION_RDRAND_STEP)
    return false;
#else
    RdRandIntegerType i;
    RdRandIntegerType j;

    return !_RdRand(i)
        || !_RdRand(j)
        || j != i
        || !_RdRand(j)
        || j != i
        || !_RdRand(j)
        || j != i;
#endif
#else
    return false;
#endif
}

static bool GetHardwareSecureBytes(uint8_t* buffer, size_t size)
{
    if (!Random::SupportsHardwareRandom())
    {
        return false;
    }

#if defined(FUSION_RDRAND_STEP)
    constexpr size_t chunkSize = sizeof(chunkSize);

    while (size >= chunkSize)
    {
        RdRandIntegerType value = 0;

        if (!_RdRand(value))
        {
            return false;
        }

        memcpy(buffer, &value, chunkSize);

        buffer += chunkSize;
        size -= chunkSize;
    }

    if (size > 0)
    {
        RdRandIntegerType value = 0;

        if (!_RdRand(value))
        {
            return false;
        }

        memcpy(buffer, &value, size);
    }

    return true;
#else
    return false;
#endif
}

#pragma endregion HardwareRandom
// HardwareRandom                                           END
// ------------------------------------------------------------
// RandomState                                            START
#pragma region RandomState

template<typename T>
static constexpr T DefaultSeed()
{
    return T(Clock::now().time_since_epoch().count());
}

XorShift32::XorShift32()
    : XorShift32(DefaultSeed<uint32_t>())
{ }

XorShift32::XorShift32(uint32_t seed)
    : state{ (seed) ? seed : 1 }
{ }

XorShift128::XorShift128()
    : XorShift128(DefaultSeed<uint64_t>())
{ }

XorShift128::XorShift128(uint64_t seed)
{
    if (!seed)
    {
        seed = 1;
    }

    state[0] = Mix64(seed);
    state[1] = Mix64(state[0]);
}

XorShift1024::XorShift1024()
    : XorShift1024(DefaultSeed<uint64_t>())
{ }

XorShift1024::XorShift1024(uint64_t seed)
{
    if (!seed)
    {
        seed = 1;
    }

    for (uint32_t i = 0; i < 16; ++i)
    {
        seed = Mix64(seed);
        state[i] = seed;
    }
}

#pragma endregion RandomState
// RandomState                                              END
// ------------------------------------------------------------
// Random                                                 START
#pragma region Random

uint32_t Random::Bits32(XorShift32& state)
{
    uint32_t seed = state.state;
    uint32_t s = seed;

    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;

    state.state = s;
    return s + seed;
}

uint32_t Random::Bits32(XorShift128& state)
{
    return static_cast<uint32_t>(Bits64(state));
}

uint32_t Random::Bits32(XorShift1024& state)
{
    return static_cast<uint32_t>(Bits64(state));
}

uint64_t Random::Bits64(XorShift32& state)
{
    uint32_t high = Bits32(state);
    uint32_t low = Bits32(state);

    return (uint64_t(high) << 32) | low;
}

uint64_t Random::Bits64(XorShift128& state)
{
    uint64_t s0 = state.state[0];
    uint64_t s1 = state.state[1];
    uint64_t result = s0 + s1;

    s1 ^= s0;
    state.state[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
    state.state[1] = rotl(s1, 36);

    return result;
}

uint64_t Random::Bits64(XorShift1024& state)
{
    const uint64_t s0 = state.state[state.p];
    uint64_t s1 = state.state[state.p = (state.p + 1) & 15];

    s1 ^= s1 << 31;
    state.state[state.p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);

    return state.state[state.p] * 0x9e3779b97f4a7c13;
}

void Random::Bytes(
    XorShift32& state,
    uint8_t* buffer,
    size_t size)
{
    constexpr uint8_t chunkSize = sizeof(uint32_t);

    if (!buffer || size == 0)
    {
        return;
    }

    while (size > chunkSize)
    {
        uint32_t value = Bits32(state);

        memcpy(buffer, &value, chunkSize);

        buffer += chunkSize;
        size -= chunkSize;
    }

    if (size > 0)
    {
        uint32_t value = Bits32(state);

        memcpy(buffer, &value, size);
    }
}

void Random::Bytes(
    XorShift128& state,
    uint8_t* buffer,
    size_t size)
{
    constexpr uint8_t chunkSize = sizeof(uint64_t);

    if (!buffer || size == 0)
    {
        return;
    }

    while (size > chunkSize)
    {
        uint64_t value = Bits64(state);

        memcpy(buffer, &value, chunkSize);

        buffer += chunkSize;
        size -= chunkSize;
    }

    if (size > 0)
    {
        uint64_t value = Bits64(state);

        memcpy(buffer, &value, size);
    }
}

void Random::Bytes(
    XorShift1024& state,
    uint8_t* buffer,
    size_t size)
{
    constexpr uint8_t chunkSize = sizeof(uint64_t);

    if (!buffer || size == 0)
    {
        return;
    }

    while (size > chunkSize)
    {
        uint64_t value = Bits64(state);

        memcpy(buffer, &value, chunkSize);

        buffer += chunkSize;
        size -= chunkSize;
    }

    if (size > 0)
    {
        uint64_t value = Bits64(state);

        memcpy(buffer, &value, size);
    }
}

double Random::Double(XorShift32& state, uint8_t width)
{
    uint32_t w = 1 << (width);

    return (Bits32(state) & (w - 1)) / float(w);
}

double Random::Double(XorShift128& state, uint8_t width)
{
    uint32_t w = 1 << (width);

    return (Bits64(state) & (w - 1)) / float(w);
}

double Random::Double(XorShift1024& state, uint8_t width)
{
    uint32_t w = 1 << (width);

    return (Bits64(state) & (w - 1)) / float(w);
}

Result<void> Random::SecureBytes(uint8_t* buffer, size_t size)
{
    if (!GetHardwareSecureBytes(buffer, size))
    {
        if (!Internal::SystemSecureBytes(buffer, size))
        {
            return Failure(E_FAILURE);
        }
    }

    return Success;
}

bool Random::SupportsHardwareRandom()
{
#if !defined(FUSION_DISABLE_HARDWARE_RANDOM)
    static bool s_RdRandSupported = Fusion::SupportsHardwareRandom();

    if (!s_RdRandSupported)
    {
        // The system doesn't support RdRand so we need to use a
        // fall back.
        return false;
    }

    return true;
#else
    return false;
#endif
}

#pragma endregion Random
// Random                                                   END
// ------------------------------------------------------------
}
