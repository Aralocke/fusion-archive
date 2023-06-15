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

#include <Fusion/Context.h>

namespace Fusion
{
Context::Context()
    : m_state(std::make_shared<State>())
{ }

Context::~Context() { }

Context::Context(const Context& ctx)
    : m_state(ctx.m_state)
{ }

Context::Context(Context&& ctx) noexcept
    : m_state(std::move(ctx.m_state))
{ }

Context& Context::operator=(const Context& ctx)
{
    if (m_state != ctx.m_state)
    {
        m_state = ctx.m_state;
    }
    return *this;
}

Context& Context::operator=(Context&& ctx) noexcept
{
    if (m_state != ctx.m_state)
    {
        m_state = std::move(ctx.m_state);
    }
    return *this;
}

void Context::Cancel()
{
    m_state->flag.store(true);
}

bool Context::IsCancelled() const
{
    if (m_state->flag.load())
    {
        return true;
    }
    if (auto deadline = m_state->deadline.load())
    {
        auto count = Clock::now().time_since_epoch().count();

        return count >= deadline;
    }
    return false;
}

void Context::ReleaseDeadline()
{
    m_state->deadline.store(0);
}
}  // namespace Fusion
