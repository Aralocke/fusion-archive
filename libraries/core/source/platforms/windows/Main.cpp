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
#if FUSION_PLATFORM_WINDOWS

#define FUSION_IMPL_MAIN 1
#include <Fusion/Main.h>
#include <Fusion/Windows.h>

namespace Fusion
{
std::string FromWString(const wchar_t* wstr, size_t len)
{
    if (len == 0)
    {
        return {};
    }

    std::string str(len, 0);

    int res = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr,
        static_cast<int>(len),
        str.data(),
        static_cast<int>(str.capacity()),
        nullptr,
        nullptr);

    if (res == 0)
    {
        return {};
    }

    return str;
}
}
#endif
