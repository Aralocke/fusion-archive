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

#include <Fusion/Error.h>
#include <Fusion/StringUtil.h>
#include <Fusion/Windows.h>

namespace Fusion
{
ErrorCode GetLastError()
{
    return static_cast<ErrorCode>(::GetLastError());
}

std::string_view ToString(
    ErrorCode platformCode,
    char* buffer,
    size_t length)
{
    using namespace std::string_view_literals;

    FixedStringBuilder builder(buffer, length);

    DWORD res = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        static_cast<DWORD>(platformCode),
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        static_cast<LPSTR>(builder.Head()),
        static_cast<DWORD>(builder.Remaining()),
        NULL);

    if (res == 0)
    {
        return "Unable to determine error string"sv;
    }

    builder.Offset(res);
    std::string_view message = builder.View();
    message = StringUtil::TrimRight(message);

    return message;
}

Error Error::Errno(ErrorCode platformCode)
{
    switch (platformCode)
    {
    case ERROR_SUCCESS:
        return E_SUCCESS;
    case ERROR_ACCESS_DENIED:
        return E_ACCESS_DENIED(platformCode);
    case ERROR_DISK_FULL:
        return E_DISK_FULL(platformCode);
    case ERROR_ALREADY_EXISTS:
    case ERROR_FILE_EXISTS:
        return E_EXISTS(platformCode);
    case ERROR_TOO_MANY_OPEN_FILES:
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
        return E_INSUFFICIENT_RESOURCES(platformCode);
    case ERROR_INVALID_PARAMETER:
    case ERROR_INVALID_HANDLE:
        return E_INVALID_ARGUMENT(platformCode);
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
        return E_NOT_FOUND(platformCode);
    }

    return E_FAILURE(platformCode);
}
}  // namespace Fusion

#endif
