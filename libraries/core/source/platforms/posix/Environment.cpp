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

    const char* value = getenv(var.c_str());

    if (!value)
    {
        return std::string(defaultValue);
    }

    return { value };
}

bool Environment::Set(
    std::string var,
    std::string value)
{
    if (var.empty() || value.empty())
    {
        return false;
    }

    std::lock_guard lock(s_envMutex);

    return (setenv(var.c_str(), value.c_str(), 1) == 0);
}

bool Environment::Unset(std::string var)
{
    if (var.empty())
    {
        return false;
    }

    std::lock_guard lock(s_envMutex);

    return (unsetenv(var.c_str()) == 0);
}

}

#endif
