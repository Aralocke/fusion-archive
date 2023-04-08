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

#pragma once

#include <Fusion/Logging.h>

#include <map>
#include <memory>
#include <mutex>

namespace Fusion::Internal
{
class LogRegistry final
{
public:
    //
    //
    //
    static LogRegistry& Instance();

public:
    LogRegistry(const LogRegistry&) = delete;
    LogRegistry& operator=(const LogRegistry&) = delete;

    //
    //
    //
    LogRegistry();

    //
    //
    //
    ~LogRegistry();

    //
    //
    //
    std::shared_ptr<Logger> Get(std::string name);

    //
    //
    //
    void Drop(std::string_view sink);

    //
    //
    //
    void DropAll();

private:
    using LoggerMap = std::map<
        std::string,
        std::shared_ptr<Logger>,
        std::less<>>;

    LoggerMap m_loggers;
    std::shared_ptr<Logger> m_root;
    std::mutex m_mutex;
};
}
