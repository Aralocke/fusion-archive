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

#include <Fusion/Platform.h>
#if FUSION_PLATFORM_APPLE

#include <Fusion/Internal/Thread.h>

#include <pthread.h>

namespace Fusion
{
Thread::Id Thread::CurrentThreadId()
{
    uint64_t id = 0;
    pthread_threadid_np(pthread_self(), &id);

    return Id(id);
}

void Internal::SetThreadName(std::string_view name)
{
    FUSION_ASSERT(!name.empty());

    std::string str(name);
    pthread_setname_np(str.c_str());
}
}  // namespace Fusion

#endif  // FUSION_PLATFORM_WINDOWS
