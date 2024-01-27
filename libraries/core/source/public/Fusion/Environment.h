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

#include <Fusion/Macros.h>

#include <mutex>
#include <string>
#include <unordered_map>

namespace Fusion
{
//
//
//
class Environment final
{
public:
    //
    //
    //
    static std::string Get(
        std::string var,
        std::string_view defaultValue = {}) FUSION_EXCLUDES(s_mutex);

    //
    //
    //
    static bool Has(std::string var) FUSION_EXCLUDES(s_mutex);

    //
    //
    //
    static bool Set(std::string var, std::string value) FUSION_EXCLUDES(s_mutex);

    //
    //
    //
    static bool Unset(std::string var) FUSION_EXCLUDES(s_mutex);

private:
    static std::mutex s_envMutex;
};

//
//
//
class TemporaryEnvironment final
{
public:
    TemporaryEnvironment() = default;
    ~TemporaryEnvironment();

    //
    //
    //
    bool Set(std::string var, std::string value);

    //
    //
    //
    bool Unset(std::string var);

private:
    std::unordered_map<std::string, std::string> m_values;
};
}  // namespace Fusion
