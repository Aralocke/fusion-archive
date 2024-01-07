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

#include <Fusion/Internal/SelectSocketService.h>

namespace Fusion::Internal
{
SelectSocketService::SelectSocketService(Network& network)
    : m_network(network)
    , m_pipe(network)
{ }

SelectSocketService::~SelectSocketService()
{
    std::lock_guard lock(m_mutex);

    if (m_started)
    {
        FUSION_ASSERT(m_shutdown);
        FUSION_ASSERT(!m_polling);
    }

    FUSION_ASSERT(m_events.empty());
    FUSION_ASSERT(m_results.empty());
}

Result<void> SelectSocketService::Add(
    Socket sock,
    SocketOperation events)
{
    std::unique_lock lock(m_mutex);

    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid socket");
    }

    if (events == SocketOperation::None)
    {
        return Success;
    }

    if (auto iter = std::find(
        begin(m_events),
        end(m_events),
        sock); iter != m_events.end())
    {
        if ((iter->events | events) != events)
        {
            iter->events |= events;
        }
    }
    else
    {
        m_events.push_back(SocketEvent{
            .sock = sock,
            .events = events
        });
    }

    NotifyLocked(lock);
    return Success;
}

Result<void> SelectSocketService::Close(Socket sock)
{
    std::unique_lock lock(m_mutex);

    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid socket");
    }

    if (auto iter = std::find(
        begin(m_events),
        end(m_events),
        sock); iter != m_events.end())
    {
        m_events.erase(iter);
        NotifyLocked(lock);
    }

    return Success;
}

Result<std::span<SocketEvent>>
SelectSocketService::Execute(Clock::duration timeout)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_shutdown)
    {
        FUSION_ASSERT(m_events.empty());
        FUSION_ASSERT(m_results.empty());

        return Failure(E_CANCELLED);
    }

    FUSION_ASSERT(!m_polling);

    Socket notify = m_pipe.Reader();
    fd_set errors, reads, writes;
    size_t rFds = 0, wFds = 0, eFds = 0;
    int32_t nFds = 0;

    FD_ZERO(&reads);
    FD_ZERO(&writes);
    FD_ZERO(&errors);
    {
        for (SocketEvent& ev : m_events)
        {
            nFds = std::max(nFds, int32_t(ev.sock + 1));

            if (+(ev.events & SocketOperation::Read))
            {
                if (!FD_ISSET(ev.sock, &reads))
                {
                    FD_SET(ev.sock, &reads);
                    ++rFds;
                }
            }
            if (+(ev.events & SocketOperation::Write))
            {
                if (!FD_ISSET(ev.sock, &writes))
                {
                    FD_SET(ev.sock, &writes);
                    ++wFds;
                }
            }
            if (+(ev.events & SocketOperation::Error))
            {
                if (!FD_ISSET(ev.sock, &errors))
                {
                    FD_SET(ev.sock, &errors);
                    ++eFds;
                }
            }
        }

        {
            nFds = std::max(nFds, int32_t(notify + 1));
            FD_SET(notify, &reads);
            ++rFds;
        }
    }

    FUSION_UNUSED(rFds);
    FUSION_UNUSED(wFds);
    FUSION_UNUSED(eFds);

    struct timeval* duration = nullptr;
    struct timeval storage = { 0 };

    if (timeout > Clock::duration::zero())
    {
        memset(&storage, 0, sizeof(timeval));
        duration = &storage;

        Clock::duration seconds = std::chrono::duration_cast<
            std::chrono::seconds>(timeout);

        storage.tv_sec = static_cast<long>(seconds.count());
        storage.tv_usec = static_cast<long>(
            std::chrono::duration_cast<std::chrono::microseconds>(
                timeout - seconds).count());
    }

    m_polling = true;
    lock.unlock();

    Clock::time_point start = Clock::now();
    int res = ::select(nFds, &reads, &writes, &errors, duration);
    Clock::time_point end = Clock::now();

    FUSION_UNUSED(start);
    FUSION_UNUSED(end);

    lock.lock();

    FUSION_ASSERT(m_polling);
    m_polling = false;
    m_notify = false;

    if (m_shutdown)
    {
        return {};
    }

    if (res == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to execute select()");
    }

    if (res == 0)
    {
        return {};
    }

    if (FD_ISSET(notify, &reads))
    {
        if (auto result = m_pipe.Drain(); !result)
        {
            return result.Error()
                .WithContext("failed to drain notification socket ({})",
                    m_pipe.Reader());
        }
    }

    m_results.clear();
    m_results.reserve(m_events.size());

    for (SocketEvent& ev : m_events)
    {
        SocketOperation forward = SocketOperation::None;

        if (ev.sock == notify)
        {
            continue;
        }
        if (+(ev.events & SocketOperation::Read))
        {
            if (FD_ISSET(ev.sock, &reads))
            {
                forward |= SocketOperation::Read;
            }
        }
        if (+(ev.events & SocketOperation::Write))
        {
            if (FD_ISSET(ev.sock, &writes))
            {
                forward |= SocketOperation::Write;
            }
        }
        if (+(ev.events & SocketOperation::Error))
        {
            if (FD_ISSET(ev.sock, &errors))
            {
                forward |= SocketOperation::Error;
            }
        }
        if (forward != SocketOperation::None)
        {
            m_results.push_back(SocketEvent{
                .sock = ev.sock,
                .events = forward,
            });
        }
    }
    return { m_results };
}

void SelectSocketService::Notify()
{
    std::unique_lock lock(m_mutex);
    NotifyLocked(lock);
}

void SelectSocketService::NotifyLocked(
    const std::unique_lock<std::mutex>& lock)
{
    FUSION_ASSERT(lock.owns_lock());

    constexpr char data[5] = { 'w', 'a', 'k', 'e', 0 };

    if (!m_polling)
    {
        m_notify = true;
        return;
    }

    m_network.Send(m_pipe.Writer(), data, 5);
}

Result<void> SelectSocketService::Remove(
    Socket sock,
    SocketOperation events)
{
    std::unique_lock lock(m_mutex);

    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid socket");
    }

    if (events == SocketOperation::None)
    {
        return Success;
    }

    if (auto iter = std::find(
        begin(m_events),
        end(m_events),
        sock); iter != m_events.end())
    {
        if ((iter->events & events) != SocketOperation::None)
        {
            iter->events &= ~events;
        }
        if (iter->events == SocketOperation::None)
        {
            m_events.erase(iter);
        }
        NotifyLocked(lock);
    }

    return Success;
}

Result<void> SelectSocketService::Start()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_started || m_shutdown)
    {
        return Failure{ E_FAILURE };
    }

    if (auto result = m_pipe.Start(
        SocketPair::Type::NonBlocking); !result)
    {
        return result.Error();
    }

    m_started = true;
    m_shutdown = false;
    return Success;
}

void SelectSocketService::Stop()
{
    Stop([&](Failure& result) {
        std::lock_guard l(m_mutex);
        FUSION_UNUSED(result);
        FUSION_ASSERT(m_shutdown);
        FUSION_ASSERT(m_events.empty());
    });
}

void SelectSocketService::Stop(std::function<void(Failure&)> fn)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_shutdown)
    {
        return;
    }

    m_shutdown = true;
    m_events.clear();
    m_results.clear();

    NotifyLocked(lock);

    if (m_polling)
    {
        while (m_polling) m_cond.wait(lock);
    }

    m_pipe.Stop();

    lock.unlock();
    if (fn)
    {
        Failure f(E_SUCCESS);

        fn(f);
    }
}
}  // namespace Fusion::Internal
