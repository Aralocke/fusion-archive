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

#include <Fusion/Platform.h>
#if FUSION_PLATFORM_LINUX

#include <Fusion/Internal/Network.h>

#include <mutex>
#include <unordered_map>
#include <vector>

namespace Fusion::Internal
{
//
//
//
class EPollSocketService final
    : public SocketService
{
public:
    EPollSocketService(Network& network);
    ~EPollSocketService() override;

public:
    //
    //
    //
    Result<void> Add(
        Socket sock,
        SocketOperation events) override;

    //
    //
    //
    Result<void> Close(Socket sock) override;

    //
    //
    //
    void Notify() override;

    //
    //
    //
    Result<std::span<SocketEvent>> Execute(Clock::duration timeout) override;

    //
    //
    //
    Result<void> Remove(
        Socket sock,
        SocketOperation events) override;

    //
    //
    //
    Result<void> Start() override;

    //
    //
    //
    void Stop() override;

    //
    //
    //
    void Stop(std::function<void(Failure&)> fn) override;

private:
    void NotifyLocked(const std::unique_lock<std::mutex>&);

    Network& m_network;
    SocketPair m_pipe;

    bool m_polling{ false };
    bool m_shutdown{ false };
    bool m_started{ false };

    Socket m_poll{ INVALID_SOCKET };

    std::mutex m_mutex;

    std::vector<SocketEvent> m_results;
    std::unordered_map<Socket, SocketOperation> m_events;
};
}  // namespace Fusion::Internal

#endif  // FUSION_PLATFORM_POSIX
