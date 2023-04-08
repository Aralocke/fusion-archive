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

#include <Fusion/Platform.h>
#if FUSION_PLATFORM_POSIX

#include <Fusion/Assert.h>
#include <Fusion/Macros.h>
#include <Fusion/ThreadUtil.h>

#include <thread>

namespace Fusion
{
size_t ThreadUtil::GetCurrentThreadId()
{
    return static_cast<size_t>(
        std::hash<std::thread::id>()(
            std::this_thread::get_id()));
}

void ThreadUtil::SetAffinity(uint64_t affinity)
{}

void ThreadUtil::SetName(std::string_view name)
{
    if (name.empty())
    {
        return;
    }

    // Posix thread name length needs to be 16 (inc/ terminator)
    // or less.

    size_t length = std::min<size_t>(15, name.size());
    std::string tmp(name.data(), length);

    int rc = pthread_setname_np(pthread_self(), tmp.c_str());

    FUSION_ASSERT(!rc);
    FUSION_UNUSED(rc);
}

}  // namespace Fusion

#endif
