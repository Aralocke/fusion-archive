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

#include <Fusion/Internal/Network.h>

#if FUSION_PLATFORM_WINDOWS

namespace Fusion
{
// -------------------------------------------------------------
// WSAInitializer                                          START
//
Internal::WSAInitializer Internal::WSAInitializer::s_instance{};

Internal::WSAInitializer::WSAInitializer()
{
    if (WSAStartup(MAKEWORD(2, 2), &m_wsadata) != 0)
    {
        ::exit(EXIT_FAILURE);
    }
    if (HIBYTE(m_wsadata.wVersion) != 2 || LOBYTE(m_wsadata.wVersion) != 2)
    {
        ::exit(EXIT_FAILURE);
    }
}

Internal::WSAInitializer::~WSAInitializer()
{
    WSACleanup();
}
// WSAInitializer                                            END
// -------------------------------------------------------------
// PollFlags                                               START
int32_t Internal::GetPollFlags(PollFlags flags)
{
    int32_t val = 0;

    if (+(flags & PollFlags::Read))
    {
        val |= POLLRDNORM;
    }
    if (+(flags & PollFlags::Write))
    {
        val |= POLLWRNORM;
    }
    return val;
}

PollFlags Internal::GetPollFlags(int32_t flags)
{
    PollFlags events = PollFlags::None;

    if (flags & POLLERR)
    {
        events |= PollFlags::Error;
    }
    if (flags & POLLHUP)
    {
        events |= PollFlags::HangUp;
    }
    if (flags & POLLNVAL)
    {
        events |= PollFlags::Invalid;
    }
    if (flags & POLLRDNORM)
    {
        events |= PollFlags::Read;
    }
    if (flags & POLLWRNORM)
    {
        events |= PollFlags::Write;
    }

    return events;
}
// PollFlags                                                 END
// -------------------------------------------------------------
}  // namespace fusion

#endif
