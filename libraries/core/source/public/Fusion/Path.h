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

#pragma once

#include <Fusion/Platform.h>

namespace Fusion
{
//
//
//
class Path final
{
public:
    //
    // Path max length on platforms:
    // Windows - 260 (code units, even for UTF-16 [260*2 bytes] or UTF-32 [260*4 bytes]
    // OSX - 1024 (APFS may allow slightly longer)
    // Linux - 4096 (some filesystems allow longer)
    //
#if FUSION_PLATFORM_WINDOWS
    const static size_t MaxLength = 260;
#elif FUSION_PLATFORM_OSX
    const static size_t MaxLength = 1024;
#else
    const static size_t MaxLength = 4096;
#endif

    //
    // Constants for MaxPaths using UTF-16 and UTF-32 on Windows.
    // Linux (and Posix) is standardized on UTF-8 and so these constants reflect equality
    // with the values on those platforms.
    //
#if FUSION_PLATFORM_WINDOWS
    const static size_t MaxLength16 = size_t(MaxLength * 2);
    const static size_t MaxLength32 = size_t(MaxLength * 4);
#else
    const static size_t MaxLength16 = MaxLength;
    const static size_t MaxLength32 = MaxLength;
#endif

public:
    //
    //
    //
    static const char SeparatorDrive = ':';

    //
    //
    //
    static const char SeparatorUnix = '/';

    //
    //
    //
    static const char SeparatorWindows = '\\';

    //
    //
    //
#if FUSION_PLATFORM_WINDOWS
    static const char Separator = SeparatorWindows;
#else
    static const char Separator = SeparatorUnix;
#endif
};
}  // namespace Fusion
