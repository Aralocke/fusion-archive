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
#if FUSION_PLATFORM_POSIX

#include <Fusion/Environment.h>
#include <Fusion/Path.h>
#include <Fusion/PathUtil.h>
#include <Fusion/StringUtil.h>

#include <array>

#include <limits.h>
#include <pwd.h>
#include <unistd.h>

namespace Fusion
{
std::string PathUtil::GetAppCachePath()
{
    using namespace std::string_view_literals;

    std::string path = GetHomePath();

    if (path.empty())
    {
        return {};
    }

#if FUSION_PLATFORM_OSX
    return Join(path, "Library/Caches"sv);
#else
    std::string cache = Environment::Get("XDG_CACHE_HOME");

    if (!cache.empty())
    {
        return cache;
    }

    return Join(path, ".cache"sv);
#endif  // FUSION_PLATFORM_OSX
}

std::string PathUtil::GetAppConfigPath()
{
    using namespace std::string_view_literals;

    std::string path = GetHomePath();

    if (path.empty())
    {
        return {};
    }

#if FUSION_PLATFORM_OSX
    return Join(path, "Library/Preferences"sv);
#else
    std::string config = Environment::Get("XDG_CONFIG_HOME");

    if (!config.empty())
    {
        return config;
    }

    return Join(path, ".config"sv);
#endif  // FUSION_PLATFORM_OSX
}

std::string PathUtil::GetAppDataPath()
{
    using namespace std::string_view_literals;

    std::string path = GetHomePath();

    if (path.empty())
    {
        return {};
    }

#if FUSION_PLATFORM_OSX
    return Join(path, "Library/Application Support"sv);
#else
    std::string data = Environment::Get("XDG_DATA_HOME");

    if (!data.empty())
    {
        return data;
    }

    return Join(path, ".local/share"sv);
#endif  // FUSION_PLATFORM_OSX
}

std::string PathUtil::GetConfigPath()
{
    using namespace std::string_literals;

#if FUSION_PLATFORM_OSX
    return "/Library/Preferences"s;
#else
    return "/etc"s;
#endif  // FUSION_PLATFORM_OSX
}

std::string PathUtil::GetCwdPath()
{
    std::string path = Environment::Get("FUSION_CWD");

    if (!path.empty())
    {
        return path;
    }

    std::array<char, PATH_MAX + 1> buffer = { 0 };

    if (!getcwd(buffer.data(), buffer.size()))
    {
        return {};
    }

    size_t length = StringUtil::Length(
        buffer.data(),
        buffer.size());
    path.assign(buffer.data(), length);

    std::ignore = Normalize(path);
    return path;
}

#if !FUSION_PLATFORM_APPLE
std::string PathUtil::GetExecutablePath()
{
    std::array<char, PATH_MAX + 1> buffer = { 0 };

    ssize_t result = readlink(
        "/proc/self/exe",
        buffer.data(),
        buffer.size());

    if (result == -1)
    {
        return {};
    }

    std::string_view path{ buffer.data(), size_t(result) };
    return Dirname(path);
}
#endif

std::string PathUtil::GetHomePath()
{
    std::string path = Environment::Get("FUSION_HOME");

    if (!path.empty())
    {
        std::ignore = Normalize(path);
        return path;
    }

    path = Environment::Get("HOME");

    if (!path.empty())
    {
        std::ignore = Normalize(path);
        return path;
    }

    if (struct passwd* pwd = getpwuid(getuid()); pwd)
    {
        return Normalize(pwd->pw_dir);
    }

    if (struct passwd* pwd = getpwuid(geteuid()); pwd)
    {
        return Normalize(pwd->pw_dir);
    }

    return {};
}

std::string PathUtil::GetProgramFilesPath(bool use64)
{
    FUSION_UNUSED(use64);
    return {};
}

std::string PathUtil::GetTempPath()
{
    std::string path = Environment::Get("TMPDIR");

    if (!path.empty())
    {
        std::ignore = Normalize(path);
        return path;
    }

    return "/tmp";
}
}  // namespace Fusion

#endif
