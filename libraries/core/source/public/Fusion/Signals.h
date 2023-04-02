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

#include <Fusion/Platform.h>
#include <Fusion/Types.h>

#include <functional>

namespace Fusion
{
//
//
//
enum class Signal : int32_t
{
    NONE = 0,
    HANGUP = 1,
    INTERRUPT = 2,
#if FUSION_PLATFORM_POSIX
    QUIT = 3,
    ABORT = 6,
#endif
    KILL = 9,
    TERMINATE = 15,
#if FUSION_PLATFORM_POSIX
    USR1 = 16,
    USR2 = 17,
    CHILD = 18,
#endif
    BREAK = 21,
#if FUSION_PLATFORM_WINDOWS
    ABORT = 22,
#endif
};

//
// Callback signature for functions which should receive signal
// notofications from the operating system.
//
using SignalHandler = std::function<bool(Signal)>;

//
// Clear all registered SignalHandlers
//
void ClearSignalHandlers();

//
// Clear all SignalHandlers for a given Signal
//
void ClearSignalHandlers(Signal sig);

//
// Call the associated signal handlers.
//
void RaiseSignal(Signal sig);

//
// Register a signal handler for the given Signal.
//
void RegisterSignal(Signal sig, SignalHandler fn);

//
// Register multiple signals to the same signal handler.
//
void RegisterSignal(
    std::initializer_list<Signal> signals,
    SignalHandler fn);

}  // namespace Fusion
