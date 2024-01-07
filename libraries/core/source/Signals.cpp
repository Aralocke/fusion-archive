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

#include <Fusion/Internal/Signals.h>

#include <algorithm>

namespace Fusion
{
SignalManager SignalManager::s_instance{};

SignalManager& SignalManager::Instance()
{
    return s_instance;
}

void SignalManager::Receiver(int32_t signum)
{
    // Triggers the receiver from the system
    SignalManager::Instance()
        .ReceiveSignal(signum);
}

void ClearSignalHandlers()
{
    SignalManager::Instance()
        .Clear();
}

void ClearSignalHandlers(Signal signal)
{
    SignalManager::Instance()
        .Clear(signal);
}

void RaiseSignal(Signal signal)
{
    SignalManager::Instance()
        .Raise(signal);
}

void RegisterSignal(
    Signal sig,
    SignalHandler fn)
{
    SignalManager::Instance()
        .Register(SignalManager::Handler{
            .signum = static_cast<int32_t>(sig),
            .signal = sig,
            .fn = std::move(fn),
        });
}

void RegisterSignal(
    std::initializer_list<Signal> signals,
    SignalHandler fn)
{
    for (auto sig : signals)
    {
        RegisterSignal(sig, fn);
    }
}

SignalManager::~SignalManager()
{ }

void SignalManager::Clear()
{
    std::lock_guard lock(m_mutex);

    m_handlers.clear();
}

void SignalManager::Clear(Signal signal)
{
    std::lock_guard lock(m_mutex);

    m_handlers.erase(
        std::remove_if(
            begin(m_handlers),
            end(m_handlers),
            [signal](const Handler& h) {
                return h.signal == signal;
            }),
        end(m_handlers));
}

void SignalManager::Raise(Signal sig)
{
    ReceiveSignal(static_cast<int32_t>(sig));
}

void SignalManager::Register(Handler handler)
{
    std::lock_guard lock(m_mutex);

    const int32_t signum = handler.signum;
    m_handlers.push_back(std::move(handler));

    if (m_handlers.size() > 1)
    {
        std::sort(
            begin(m_handlers),
            end(m_handlers),
            [](const Handler& l, const Handler& r) {
                return l.signal < r.signal;
            });
    }

    RegisterSignal(signum);
}

void SignalManager::ReceiveSignal(int32_t signum)
{
    std::unique_lock lock(m_mutex);

    std::vector<Handler> handlers;
    handlers.reserve(m_handlers.size());

    const Signal sig = MapEvent(signum);

    m_handlers.erase(
        std::remove_if(
            begin(m_handlers),
            end(m_handlers),
            [&](Handler& h) {
                if (h.signal == sig)
                {
                    handlers.push_back(std::move(h));
                    return true;
                }
                return false;
            }),
        end(m_handlers));

    lock.unlock();
    for (auto& handler : handlers)
    {
        if (handler.fn(handler.signal))
        {
            Register(std::move(handler));
        }
    }
    handlers.clear();
}

void SignalManager::RegisterSignal(int32_t signum)
{
    if (m_signals.count(signum) > 0)
    {
        // Already handling signum
        return;
    }

    AddSignalToSet(signum);
    m_signals.insert(signum);
}
}  // namespace Fusion
