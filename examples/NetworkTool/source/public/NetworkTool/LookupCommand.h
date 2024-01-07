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

#include <Fusion/Fwd/Argparse.h>
#include <Fusion/Fwd/Network.h>
#include <Fusion/Result.h>

using namespace Fusion;

namespace NetworkTool
{
class LookupCommand final
{
public:
    struct Options
    {};

public:
    static Result<void> Run(Options options);
    static void Setup(ArgumentCommand& cmd, Options& options);

public:
    LookupCommand(Options options);
    ~LookupCommand();

    Result<void> Run();

private:
    Options m_options;

private:
    std::unique_ptr<Network> network;
    //std::unique_ptr<Resolver> resolver;
};
}  // namespace
