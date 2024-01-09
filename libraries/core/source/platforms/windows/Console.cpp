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

#include <Fusion/Console.h>

namespace Fusion
{
// -------------------------------------------------------------
// Console                                                 START
HANDLE GetTargetHandle(Console::Target target)
{
    switch (target)
    {
    case Console::Target::Stdin:
        return GetStdHandle(STD_INPUT_HANDLE);
    case Console::Target::Stdout:
        return GetStdHandle(STD_OUTPUT_HANDLE);
    case Console::Target::Stderr:
        return GetStdHandle(STD_ERROR_HANDLE);
    default:
        return nullptr;
    }
}

Console::Features Console::GetFeatures(Target target)
{
    Features features;
    features.interactive = IsInteractive(target);


    return features;
}

bool Console::IsInteractive(Target target)
{
    HANDLE handle = GetTargetHandle(target);
    DWORD type = GetFileType(handle);

    if (type == FILE_TYPE_CHAR)
    {
        return true;
    }
    if (type == FILE_TYPE_PIPE)
    {
        return false;
    }

    struct FileInfo
    {
        FILE_NAME_INFO info;
        WCHAR data[1024];
    };

    FileInfo info = { };
    if (!GetFileInformationByHandleEx(
        handle,
        FileNameInfo,
        &info,
        sizeof(info)))
    {
        return false;
    }

    std::wstring_view filename(
        info.info.FileName,
        size_t(info.info.FileNameLength));

    if (filename.starts_with(L"\\msys-")
        || filename.starts_with(L"\\cygwin-"))
    {
        return filename.find(L"-ptr") != std::wstring_view::npos;
    }

    return false;
}
// Console                                                   END
// -------------------------------------------------------------
}  // namespace Fusion

#endif  // FUSION_PLATFORM_WINDOWS
