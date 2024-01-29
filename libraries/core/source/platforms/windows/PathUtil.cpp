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

#include <Fusion/Windows.h>
#if FUSION_PLATFORM_WINDOWS

#include <Fusion/Environment.h>
#include <Fusion/Memory.h>
#include <Fusion/Path.h>
#include <Fusion/PathUtil.h>
#include <Fusion/Unicode.h>

#include <array>

#include <ShlObj.h>

namespace Fusion
{
static std::string GetKnownFolderPath(GUID folderId)
{
    PWSTR result = nullptr;
    FUSION_SCOPE_GUARD([&] { if (result) CoTaskMemFree(result); });

    if (SHGetKnownFolderPath(folderId, 0, nullptr, &result) != 0)
    {
        return {};
    }

    std::string path = Unicode::WideToUtf8(result);
    std::ignore = PathUtil::Normalize(path);

    return path;
}

std::string PathUtil::GetAppCachePath()
{
    return GetKnownFolderPath(FOLDERID_ProgramData);
}

std::string PathUtil::GetAppConfigPath()
{
    return GetKnownFolderPath(FOLDERID_LocalAppData);
}

std::string PathUtil::GetAppDataPath()
{
    return GetKnownFolderPath(FOLDERID_LocalAppData);
}

std::string PathUtil::GetConfigPath()
{
    // Windows doesn't have an equivalent to /etc.
    return {};
}

std::string PathUtil::GetCwdPath()
{
    std::string path = Environment::Get("FUSION_CWD");

    if (!path.empty())
    {
        std::ignore = Normalize(path);
        return path;
    }

    DWORD length = GetCurrentDirectoryW(0, nullptr);
    if (length == 0)
    {
        return {};
    }

    std::wstring cwd;
    cwd.resize(length - 1);

    length = GetCurrentDirectoryW(length, cwd.data());
    if (length == 0)
    {
        return {};
    }

    return Normalize(Unicode::WideToUtf8(cwd));
}

std::string PathUtil::GetExecutablePath()
{
    std::array<wchar_t, Path::MaxLength + 2> buffer = { 0 };

    DWORD length = GetModuleFileNameW(
        GetModuleHandleW(nullptr),
        buffer.data(),
        DWORD(buffer.size()));

    if (length == 0)
    {
        return {};
    }

    std::wstring_view path{ buffer.data(), size_t(length) };
    return Dirname(Unicode::WideToUtf8(path));
}

std::string PathUtil::GetHomePath()
{
    std::string path = Environment::Get("FUSION_HOME");

    if (!path.empty())
    {
        std::ignore = Normalize(path);
        return path;
    }

    path = Environment::Get("USERPROFILE");

    if (!path.empty())
    {
        std::ignore = Normalize(path);
        return path;
    }

    path = Environment::Get("HOMEPATH");
    std::string drive = Environment::Get("SYSTEMDRIVE");

    if (!path.empty() && !drive.empty())
    {
        drive.append(path);
        std::ignore = Normalize(drive);

        return drive;
    }

    return GetKnownFolderPath(FOLDERID_Profile);
}

std::string PathUtil::GetProgramFilesPath(bool use64)
{
    return GetKnownFolderPath(
        (use64)
            ? FOLDERID_ProgramFilesX64
            : FOLDERID_ProgramFilesX86);
}

std::string PathUtil::GetTempPath()
{
    std::array<wchar_t, Path::MaxLength> buffer = { 0 };

    DWORD length = GetTempPath2W(
        DWORD(buffer.size()),
        buffer.data());

    if (length == 0)
    {
        return {};
    }

    std::wstring_view view{ buffer.data(), size_t(length) };
    std::string path = Unicode::WideToUtf8(view);

    return Normalize(path);
}
}  // namespace Fusion

#endif
