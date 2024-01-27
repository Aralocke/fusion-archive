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
#include <Fusion/Unicode.h>

namespace Fusion
{
std::string Environment::Get(
    std::string var,
    std::string_view defaultValue)
{
    if (var.empty())
    {
        return std::string(defaultValue);
    }

    std::lock_guard lock(s_envMutex);

    std::wstring v = Unicode::Utf8ToWide(var);
    DWORD length = GetEnvironmentVariableW(
        v.c_str(),
        nullptr,
        0);

    if (length == 0)
    {
        return std::string(defaultValue);
    }

    std::wstring value;
    value.resize(size_t(length));
    DWORD result = GetEnvironmentVariableW(
        v.c_str(),
        value.data(),
        length);

    if (result == 0)
    {
        return std::string(defaultValue);
    }

    value.resize(result);
    return Unicode::WideToUtf8(value);
}

bool Environment::Set(
    std::string var,
    std::string value)
{
    if (var.empty())
    {
        return false;
    }

    std::wstring lookupVariable = Unicode::Utf8ToWide(var);
    std::wstring setValue;

    if (!value.empty())
    {
        setValue = Unicode::Utf8ToWide(value);
    }

    std::lock_guard lock(s_envMutex);

    if (!SetEnvironmentVariableW(
        lookupVariable.c_str(),
        setValue.c_str()))
    {
        return false;
    }

    return true;
}

bool Environment::Unset(std::string var)
{
    return Set(std::move(var), "");
}

}  // namespace Fusion

#endif
