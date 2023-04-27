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

#include <Fusion/Unicode.h>

#include <Fusion/Platform.h>
#include <Fusion/Windows.h>

#include <cstring>

#if FUSION_PLATFORM_POSIX
constexpr size_t MB_ERROR = static_cast<size_t>(-1);
#endif  // FUSION_PLATFORM_POSIX

namespace Fusion
{
std::wstring Unicode::Utf8ToWide(std::string_view str)
{
    if (str.empty())
    {
        return {};
    }

#if FUSION_PLATFORM_WINDOWS
    int32_t size = MultiByteToWideChar(
        CP_UTF8,
        0,
        str.data(),
        static_cast<int>(str.size()),
        nullptr,
        0);

    std::wstring wide(size, L'\0');

    if (int32_t res = MultiByteToWideChar(
        CP_UTF8,
        0,
        str.data(),
        static_cast<int>(str.size()),
        wide.data(),
        static_cast<int>(wide.size())); res > 0)
    {
        return wide;
    }

    return {};
#else
    std::wstring wide(str.size(), 0);
    size_t size = mbstowcs(nullptr, str.data(), str.size());

    if (size == MB_ERROR)
    {
        return {};
    }

    std::wstring wstr(size, 0);

    (void)mbstowcs(wstr.data(), str.data(), str.size());
    return wstr;
#endif
}

std::string Unicode::WideToUtf8(std::wstring_view wstr)
{
    if (wstr.empty())
    {
        return {};
    }

#if FUSION_PLATFORM_WINDOWS
    int32_t size = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        static_cast<int>(wstr.size()),
        nullptr,
        0,
        nullptr,
        nullptr);

    std::string str(size, '\0');

    if (!WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        static_cast<int>(wstr.size()),
        str.data(),
        static_cast<int>(str.size()),
        nullptr,
        nullptr))
    {
        return {};
    }

    return str;
#else
    size_t size = wcstombs(nullptr, wstr.data(), 0);

    if (size == MB_ERROR)
    {
        return {};
    }

    std::string str(size, 0);

    (void)wcstombs(str.data(), wstr.data(), str.size());
    return str;
#endif
}
}
