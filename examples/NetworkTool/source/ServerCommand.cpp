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

#include <NetworkTool/ServerCommand.h>

#include <Fusion/Argparse.h>
#include <Fusion/Network.h>
#include <iostream>

namespace NetworkTool
{
Result<void> ServerCommand::Run(Options options)
{
    ServerCommand cmd(std::move(options));
    return cmd.Run();
}

void ServerCommand::Setup(
    ArgumentCommand& cmd,
    Options& options)
{
    using namespace std::string_view_literals;

    cmd.Help("Run the playground tool"sv);
}

ServerCommand::ServerCommand(Options options)
    : m_options(std::move(options))
{ }

ServerCommand::~ServerCommand()
{ }

Result<void> ServerCommand::Run()
{
    std::cout << "ServerCommand\n";

    FUSION_UNUSED(m_options);

    return Success;
}
}  // namespace Fusion
