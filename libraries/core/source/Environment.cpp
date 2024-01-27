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

#include <Fusion/Environment.h>

namespace Fusion
{
// -------------------------------------------------------------
// Environment                                             START
std::mutex Environment::s_envMutex{};

bool Environment::Has(std::string var)
{
    return !Get(std::move(var)).empty();
}
// Environment                                               END
// -------------------------------------------------------------
// TemporaryEnvironment                                    START
TemporaryEnvironment::~TemporaryEnvironment()
{
    for (auto& [key, value] : m_values)
    {
        Environment::Unset(key);
    }
}

bool TemporaryEnvironment::Set(
    std::string var,
    std::string value)
{
    bool result = Environment::Set(var, value);

    if (result)
    {
        m_values.insert_or_assign(
            std::move(var),
            std::move(value));
    }

    return result;
}

bool TemporaryEnvironment::Unset(std::string var)
{
    bool exists = Environment::Unset(var);
    bool tracked = false;

    if (auto iter = m_values.find(var); iter != m_values.end())
    {
        m_values.erase(iter);
        tracked = true;
    }

    return exists && tracked;
}
// TemporaryEnvironment                                      END
// -------------------------------------------------------------
}  // namespace Fusion
