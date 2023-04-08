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

#include <Fusion/Windows.h>
#if FUSION_PLATFORM_WINDOWS

#include <Fusion/ThreadUtil.h>

namespace Fusion
{
size_t ThreadUtil::GetCurrentThreadId()
{
    return static_cast<size_t>(::GetCurrentThreadId());
}

void ThreadUtil::SetName(std::string_view name)
{
    if (name.empty())
    {
        return;
    }

    std::string tmp(name);

    // Details here:
    // https://learn.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2022
    // 
    // Implement this after setting up the Unicode helpers.
    // 
    // HRESULT r;
    // r = SetThreadDescription(
    //     GetCurrentThread(),
    //    L"ThisIsMyThreadName!"
    // );
}
}  // namespace Fusion

#endif
