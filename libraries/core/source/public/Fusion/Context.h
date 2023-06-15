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

#include <Fusion/DateTime.h>
#include <Fusion/Types.h>

#include <atomic>
#include <memory>

namespace Fusion
{
//
//
//
class Context final
{
public:
    //
    //
    //
    Context();

    //
    //
    //
    ~Context();

    //
    //
    //
    template<typename Rep, typename Length>
    Context(std::chrono::duration<Rep, Length> at);

    //
    //
    //
    Context(const Context& ctx);

    //
    //
    //
    Context& operator=(const Context& ctx);

    //
    //
    //
    Context(Context&& ctx) noexcept;

    //
    //
    //
    Context& operator=(Context&& ctx) noexcept;

    //
    //
    //
    template<typename Rep, typename Length>
    void CancelAfter(std::chrono::duration<Rep, Length> at);

    //
    //
    //
    template<typename Rep, typename Duration>
    void CancelAt(std::chrono::time_point<Rep, Duration> at);

    //
    //
    //
    void Cancel();

    //
    //
    //
    bool IsCancelled() const;

    //
    //
    //
    void ReleaseDeadline();

private:
    struct State
    {
        std::atomic<bool> flag{ false };
        std::atomic<Clock::duration::rep> deadline{ 0 };
    };

    std::shared_ptr<State> m_state;
};
}

#define FUSION_IMPL_CONTEXT 1
#include <Fusion/Impl/Context.h>
