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

#include <Fusion/Fwd/Argparse.h>
#include <Fusion/Fwd/Http.h>
#include <Fusion/Fwd/Network.h>
#include <Fusion/Result.h>

using namespace Fusion;

namespace Fusion
{
    struct GlobalOptions;
}

namespace NetworkTool
{
class LookupCommand final
{
public:
    struct Options
    {
        std::string address;
        std::string record;
        std::string server;
    };

public:
    static Result<void> Run(GlobalOptions& globalOptions, Options options);
    static void Setup(ArgumentCommand& cmd, Options& options);

public:
    LookupCommand(GlobalOptions& globalOptions, Options options);
    ~LookupCommand();

    Result<void> Run();

private:
    GlobalOptions& m_globalOptions;
    Options m_options;

private:
    std::unique_ptr<Network> network;
    std::unique_ptr<HttpClient> httpclient;
};
}  // namespace
