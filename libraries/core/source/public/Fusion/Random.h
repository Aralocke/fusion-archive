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

#include <Fusion/Result.h>

namespace Fusion
{
//
//
//
struct XorShift32
{
    uint32_t state{ 0 };

    XorShift32();
    XorShift32(uint32_t seed);
};

//
//
//
struct XorShift128
{
    uint64_t state[2] = { 0 };

    XorShift128();
    XorShift128(uint64_t seed);
};

//
//
//
struct XorShift1024
{
    uint64_t state[16] = { 0 };
    int32_t p{ 0 };

    XorShift1024();
    XorShift1024(uint64_t seed);
};

//
//
//
class Random final
{
public:
    //
    //
    //
    static bool SupportsHardwareRandom();
    
public:
    //
    //
    //
    static uint32_t Bits32(XorShift32& state);

    //
    //
    //
    static uint32_t Bits32(XorShift128& state);

    //
    //
    //
    static uint32_t Bits32(XorShift1024& state);

public:
    //
    //
    //
    static uint64_t Bits64(XorShift32& state);

    //
    //
    //
    static uint64_t Bits64(XorShift128& state);

    //
    //
    //
    static uint64_t Bits64(XorShift1024& state);

public:
    //
    //
    //
    static double Double(XorShift32& state, uint8_t width = 24);

    //
    //
    //
    static double Double(XorShift128& state, uint8_t width = 24);

    //
    //
    //
    static double Double(XorShift1024& state, uint8_t width = 24);


public:
    //
    //
    //
    template<typename RandomState>
    static double Double(
        RandomState& state,
        double min,
        double max,
        uint8_t width = 24);

    //
    //
    //
    template<typename RandomState>
    static int32_t Int32(
        RandomState& state,
        int32_t min,
        int32_t max);

    //
    //
    //
    template<typename RandomState>
    static uint32_t UInt32(
        RandomState& state,
        uint32_t min,
        uint32_t max);

    //
    //
    //
    template<typename RandomState>
    static int64_t Int64(
        RandomState& state,
        int64_t min,
        int64_t max);
    //
    //
    //
    template<typename RandomState>
    static uint64_t UInt64(
        RandomState& state,
        uint64_t min,
        uint64_t max);

public:
    //
    //
    //
    static void Bytes(
        XorShift32& state,
        uint8_t* buffer,
        size_t size);

    //
    //
    //
    static void Bytes(
        XorShift128& state,
        uint8_t* buffer,
        size_t size);

    //
    //
    //
    static void Bytes(
        XorShift1024& state,
        uint8_t* buffer,
        size_t size);

    //
    //
    //
    template<typename RandomState, size_t N>
    static void Bytes(
        RandomState& state,
        uint8_t(&buffer)[N]);

public:
    //
    //
    //
    static Result<void> SecureBytes(uint8_t* buffer, size_t size);

    //
    //
    //
    template<size_t N>
    static Result<void> SecureBytes(uint8_t(&buffer)[N]);
};
}  // namespace Fusion

#define FUSION_IMPL_RANDOM 1
#include <Fusion/Impl/Random.h>
