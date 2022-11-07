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

#include <Fusion/Platform.h>
#include <Fusion/Result.h>
#include <Fusion/StringUtil.h>

#include <span>
#include <string>
#include <vector>

namespace Fusion
{
#if FUSION_PLATFORM_WINDOWS
//
// This is temporary until proper unicode handling is implemented. For
// now this is necessary to convert the wmain args to UTF8 clean strings.
//
std::string FromWString(const wchar_t* wstr, size_t len);

#endif

//
// Implement this in the launching point to a new application.
//
// The span is guaranteed for the length of the application.
Result<void> Main(std::span<std::string_view> args);

//
// Faux main handler that will setup the basic application in a
// global way. Ultimately converts the above Result<void> into a
// platform specific return code and exit handler.
//
int _Main(std::span<std::string_view> args);
}  // namespace Fusion

#if FUSION_PLATFORM_WINDOWS
#if !defined(FUSION_IMPL_MAIN)
int wmain(int argc, wchar_t** argv)
{
    using namespace Fusion;

    std::vector<std::string> args(argc);
    std::vector<std::string_view> views(argc);

    for (int i = 0; i < argc; ++i)
    {
        size_t length = wcslen(argv[i]);

        args[i] = FromWString(argv[i], length);
        views[i] = args[i];
    }

    return _Main(views);
}
#endif
#else
#if !defined(FUSION_IMPL_MAIN)
int main(int argc, char** argv)
{
    using namespace Fusion;

    std::vector<std::string> args(argc);
    std::vector<std::string_view> views(argc);

    for (int i = 0; i < argc; ++i)
    {
        size_t length = StringUtil::Length(argv[i]);
        args[i] = { argv[i], length };
        views[i] = args.back();
    }

    return _Main(views);
}
#endif
#endif
