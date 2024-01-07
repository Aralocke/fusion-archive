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
#include <Fusion/Signals.h>

#include <mutex>
#include <set>
#include <vector>

namespace Fusion
{
class SignalManager final
{
public:
    SignalManager(const SignalManager&) = delete;
    SignalManager& operator=(SignalManager&&) = delete;

public:
    static SignalManager& Instance();
    static void Receiver(int32_t signum);

public:
    ~SignalManager();

    void Clear();
    void Clear(Signal signal);
    void Raise(Signal signal);

    struct Handler
    {
        int32_t signum = -1;
        Signal signal = Signal::NONE;
        SignalHandler fn;
    };

    void Register(Handler handler);

private:
    SignalManager() = default;

    void AddSignalToSet(int32_t signum);
    void ReceiveSignal(int32_t signum);
    void RegisterSignal(int32_t signum);

private:
    std::vector<Handler> m_handlers;
    std::set<int32_t> m_signals;
    mutable std::mutex m_mutex;

public:
    static Signal MapEvent(int32_t signum);

private:
#if FUSION_PLATFORM_WINDOWS
    bool m_consoleInstalled = false;
#endif  // FUSION_PLATFORM_WINDOWS

private:
    static SignalManager s_instance;
};
}  // namespace Fusion
