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

#include <Fusion/Internal/EPollSocketService.h>

#if FUSION_PLATFORM_LINUX

#include <sys/epoll.h>

namespace Fusion::Internal
{
static SocketOperation FromSocketEvents(uint32_t events)
{
    auto ops = SocketOperation::None;

    if ((events & EPOLLIN) == EPOLLIN)
    {
        ops |= SocketOperation::Read;
    }
    if ((events & EPOLLOUT) == EPOLLOUT)
    {
        ops |= SocketOperation::Write;
    }
    if ((events & EPOLLERR) == EPOLLERR)
    {
        ops |= SocketOperation::Error;
    }

    return ops;
}

static uint32_t ToEPollEvents(SocketOperation events)
{
    uint32_t ev = 0;

    if (+(events & SocketOperation::Read))
    {
        ev |= EPOLLIN;
    }
    if (+(events & SocketOperation::Write))
    {
        ev |= EPOLLOUT;
    }
    if (+(events & SocketOperation::Error))
    {
        ev |= EPOLLERR;
    }

    return ev;
}

EPollSocketService::EPollSocketService(Network& network)
    : SocketService(Type::Epoll, Operation::Polling)
    , m_network(network)
    , m_pipe(network)
{ }

EPollSocketService::~EPollSocketService()
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

Result<void> EPollSocketService::Add(
    Socket sock,
    SocketOperation events)
{
    std::lock_guard lock(m_mutex);

    if (m_shutdown)
    {
        return Failure(E_FAILURE);
    }

    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid socket");
    }

    if (m_poll == INVALID_SOCKET)
    {
        return Failure(E_NOT_INITIALIZED)
            .WithContext("pollset not yet initialized");
    }

    if (events == SocketOperation::None)
    {
        return Success;
    }

    struct epoll_event event = { 0 };
    event.data.fd = sock;

    auto iter = m_events.find(sock);

    if (iter != m_events.end())
    {
        FUSION_ASSERT(sock == iter->first);
        SocketOperation& ops = iter->second;

        if ((ops | events) != events)
        {
            ops |= events;
        }

        event.events = ToEPollEvents(ops);

        if (epoll_ctl(
            m_poll,
            EPOLL_CTL_MOD,
            sock,
            &event) == SOCKET_ERROR)
        {
            return GetLastNetworkFailure()
                .WithContext("failed to modify socket '{}' on epoll (events={})",
                    sock, FlagsToString(ops));
        }

        m_events.emplace_hint(iter, sock, ops);
    }
    else
    {
        event.events = ToEPollEvents(events);

        if (epoll_ctl(
            m_poll,
            EPOLL_CTL_ADD,
            sock,
            &event) == SOCKET_ERROR)
        {
            return GetLastNetworkFailure()
                .WithContext("failed to add '{}' to socket '{}' on epoll",
                    FlagsToString(events), sock);
        }

        m_events.emplace(sock, events);
    }

    return Success;
}

Result<void> EPollSocketService::Close(Socket sock)
{
    std::lock_guard lock(m_mutex);

    if (m_shutdown)
    {
        return Failure(E_FAILURE);
    }

    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid socket");
    }

    if (m_poll == INVALID_SOCKET)
    {
        return Failure(E_NOT_INITIALIZED)
            .WithContext("pollset not yet initialized");
    }

    if (auto iter = m_events.find(sock); iter != m_events.end())
    {
        if (epoll_ctl(
            m_poll,
            EPOLL_CTL_DEL,
            sock,
            nullptr) == SOCKET_ERROR)
        {
            return GetLastNetworkFailure()
                .WithContext("failed to remove socket '{}' from epoll", sock);
        }

        m_events.erase(iter);
        return Success;
    }

    return Failure(E_NOT_FOUND)
        .WithContext("socket '{}' not in pollset", sock);
}

Result<std::span<SocketEvent>>
EPollSocketService::Execute(Clock::duration timeout)
{
    std::unique_lock lock(m_mutex);

    if (m_shutdown)
    {
        FUSION_ASSERT(m_events.empty());

        return Failure(E_CANCELLED);
    }

    FUSION_ASSERT(!m_events.empty());
    FUSION_ASSERT(!m_polling);

    auto duration = static_cast<int>(
        std::chrono::duration_cast<
            std::chrono::milliseconds>(timeout).count());

    m_polling = true;
    lock.unlock();

    std::vector<struct epoll_event> events(m_events.size());

    Clock::time_point start = Clock::now();

    int res = epoll_wait(
        m_poll,
        events.data(),
        static_cast<int>(events.size()),
        duration);

    Clock::time_point end = Clock::now();
    lock.lock();

    FUSION_ASSERT(m_polling);
    m_polling = false;

    if (m_shutdown)
    {
        return std::span<SocketEvent>{};
    }

    FUSION_UNUSED(start);
    FUSION_UNUSED(end);

    if (res == SOCKET_ERROR)
    {
        return GetLastNetworkFailure()
            .WithContext("epoll failed");
    }

    if (res == 0)
    {
        return std::span<SocketEvent>{};
    }

    m_results.clear();
    m_results.reserve(res);
    const auto& notify = m_pipe.Reader();

    for (int i = 0; i < res; ++i)
    {
        Socket sock = events[i].data.fd;
        SocketOperation forward = FromSocketEvents(events[i].events);

        if (sock == notify)
        {
            if (auto result = m_pipe.Drain(); !result)
            {
                return result.Error()
                    .WithContext("failed to drain the notification socket");
            }
            continue;
        }
        if (forward != SocketOperation::None)
        {
            m_results.push_back(SocketEvent{
                .sock = sock,
                .events = forward,
            });
        }
    }

    if (!m_results.empty())
    {
        return { m_results };
    }

    return std::span<SocketEvent>{};
}

void EPollSocketService::Notify()
{
    std::unique_lock lock(m_mutex);
    NotifyLocked(lock);
}

void EPollSocketService::NotifyLocked(
    const std::unique_lock<std::mutex>& lock)
{
    FUSION_ASSERT(lock.owns_lock());
    char data[1] = { 0 };

    if (!m_polling)
    {
        return;
    }

    size_t writeSize = 0;
    if (auto result = m_network.Send(
        m_pipe.Writer(),
        data,
        writeSize); !result)
    {
        if (m_shutdown)
        {
            return;
        }
    }
}

Result<void> EPollSocketService::Remove(
    Socket sock,
    SocketOperation events)
{
    std::lock_guard lock(m_mutex);

    if (m_shutdown)
    {
        return Failure(E_FAILURE);
    }

    if (sock == INVALID_SOCKET)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid socket");
    }

    if (m_poll == INVALID_SOCKET)
    {
        return Failure(E_NOT_INITIALIZED)
            .WithContext("pollset not yet initialized");
    }

    if (events == SocketOperation::None)
    {
        return Success;
    }

    struct epoll_event event = { 0 };
    event.data.fd = sock;

    if (auto iter = m_events.find(sock); iter != m_events.end())
    {
        SocketOperation& ops = iter->second;

        if ((ops & events) != SocketOperation::None)
        {
            ops &= ~events;

            if ((ops & SocketOperation::Error) == SocketOperation::Error)
            {
                // Small optimization to ensure that a socket with only an error
                // event doesn't get left in the pollset.
                ops = SocketOperation::None;
            }

            if (ops == SocketOperation::None)
            {
                if (epoll_ctl(
                    m_poll,
                    EPOLL_CTL_DEL,
                    sock,
                    nullptr) == SOCKET_ERROR)
                {
                    return GetLastNetworkFailure()
                        .WithContext("failed to remove socket '{}' from epoll", sock);
                }
                m_events.erase(iter);
            }
            else
            {
                event.events = ToEPollEvents(ops);

                if (epoll_ctl(
                    m_poll,
                    EPOLL_CTL_MOD,
                    sock,
                    &event) == SOCKET_ERROR)
                {
                    return GetLastNetworkFailure()
                        .WithContext("failed to modify socket '{}' in epoll", sock);
                }

                FUSION_ASSERT(ops != SocketOperation::None);
                m_events.emplace_hint(iter, sock, ops);
            }
        }

        return Success;
    }

    return Failure(E_NOT_FOUND)
        .WithContext("socket '{}' not found in pollset", sock);
}

Result<void> EPollSocketService::Start()
{
    std::unique_lock lock(m_mutex);

    if (m_poll != INVALID_SOCKET)
    {
        return Failure(E_NOT_SUPPORTED);
    }

    if (m_started || m_shutdown)
    {
        return Failure{ E_FAILURE };
    }

    if (auto result = m_pipe.Start(
        SocketPair::Type::NonBlocking); !result)
    {
        return result.Error();
    }

    if (m_poll = epoll_create1(0); m_poll == INVALID_SOCKET)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to initialize epoll");
    }

    m_shutdown = false;
    m_started = true;
    lock.unlock();

    if (auto result = Add(
        m_pipe.Reader(),
        SocketOperation::Read | SocketOperation::Error); !result)
    {
        return GetLastNetworkFailure()
            .WithContext("failed to add listening socket to epoll");
    }

    return Success;
}

void EPollSocketService::Stop()
{
    Stop(nullptr);
}

void EPollSocketService::Stop(std::function<void(Failure&)> fn)
{
    std::unique_lock lock(m_mutex);

    if (m_shutdown)
    {
        return;
    }

    m_shutdown = true;
    m_pipe.Stop();
    m_events.clear();
    m_results.clear();

    if (auto res = ::close(m_poll); res == SOCKET_ERROR)
    {
        const auto f = Failure::Errno();
        FUSION_UNUSED(f);
    }

    lock.unlock();
    if (fn)
    {
        Failure f(E_SUCCESS);

        fn(f);
    }
}
}  // namespace Fusion::Internal
#endif  // FUSION_PLATFORM_POSIX
