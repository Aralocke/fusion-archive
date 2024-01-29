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

#include <string>

#if FUSION_PLATFORM_WINDOWS
#undef GetTempPath
#endif

namespace Fusion
{
//
//
//
class PathUtil final
{
public:
    //
    //
    //
    static std::string GetAppCachePath();

    //
    //
    //
    static std::string GetAppConfigPath();

    //
    //
    //
    static std::string GetAppDataPath();

    //
    //
    //
    static std::string GetConfigPath();

    //
    //
    //
    static std::string GetCwdPath();

    //
    //
    //
    static std::string GetExecutablePath();

    //
    //
    //
    static std::string GetHomePath();

    //
    //
    //
    static std::string GetProgramFilesPath(bool use64 = false);

    //
    //
    //
    static std::string GetTempPath();

public:
    //
    //
    //
    static std::string Basename(std::string_view path);

    //
    //
    //
    static std::string Dirname(std::string_view path);

    //
    //
    //
    static std::string Extension(std::string_view file);

    //
    //
    //
    static std::string Join(
        std::string_view left,
        std::string_view right);

    //
    //
    //
    template<typename ...Args>
    static std::string Join(
        std::string_view a,
        std::string_view b,
        std::string_view c,
        Args&& ...args);

    //
    //
    //
    static std::string& Join(
        std::string& existing,
        std::string_view segment);

    //
    //
    //
    static std::string LongPath(std::string_view path);

    //
    //
    //
    static std::string& LongPath(std::string& path);

    //
    //
    //
    static std::string Normalize(std::string_view path);

    //
    //
    //
    static std::string& Normalize(std::string& path);

    //
    //
    //
    static std::string Normcase(std::string_view path);

    //
    //
    //
    static std::string& Normcase(std::string& path);

    //
    //
    //
    static std::string RelPath(
        std::string_view path,
        std::string_view parent = {});

public:
    //
    //
    //
    static bool IsAbsolute(std::string_view path);

    //
    //
    //
    static bool IsNormalizedRoot(std::string_view path);

    //
    //
    //
    static bool IsRelative(
        std::string_view child,
        std::string_view parent);

    //
    //
    //
    static bool IsSeparator(char c);

    //
    //
    //
    static bool IsSpecialCharacter(char c);
};
}  // namespace Fusion

#define FUSION_IMPL_PATHUTIL 1
#include <Fusion/Impl/PathUtil.h>
