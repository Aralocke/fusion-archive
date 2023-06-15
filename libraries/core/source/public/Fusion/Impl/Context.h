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

#if !defined(FUSION_IMPL_CONTEXT)
#error "Context impl included before main header"
#endif

#include <Fusion/Assert.h>

namespace Fusion
{
template<typename Rep, typename Length>
Context::Context(std::chrono::duration<Rep, Length> at)
    : m_state(std::make_shared<State>())
{
    CancelAt(Clock::now() + at);
}

template<typename Rep, typename Length>
void Context::CancelAfter(std::chrono::duration<Rep, Length> at)
{
    CancelAt(Clock::now() + at);
}

template<typename Rep, typename Duration>
void Context::CancelAt(std::chrono::time_point<Rep, Duration> at)
{
    using namespace std::chrono;

    auto now = Clock::now();
    auto deadline = now + (at - now);

    FUSION_ASSERT(m_state);
    m_state->deadline.store(deadline.time_since_epoch().count());
}
}  // namespace Fusion
