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

#include <Fusion/Debug.h>

#include <sys/sysctl.h>

namespace Fusion
{
bool Debug::IsDebuggerPresent()
{
#ifndef _NDEBUG
    // Apple Recommendation
    // https://developer.apple.com/library/archive/qa/qa1361/_index.html
    int pid[4] = {};
    size_t length = 4;

    if (sysctlnametomib("kern.proc.pid", pid, &length) != 0)
    {
        return false;
    }

    kinfo_proc kpInfo = {};
    length = sizeof(kpInfo);
    pid[3] = getpid();

    if (sysctl(pid, 4, &kpInfo, &length, nullptr, 0) != 0)
    {
        return false;
    }

    return (kpInfo.kp_proc.p_flag & P_TRACED) != 0;
#else
    // Do not support this function if we are not running in a debug
    // build.
    return false;
#endif
}
}  // namespace Fusion

#endif
