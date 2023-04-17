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

#include <Fusion/Benchmarks/LatencyCommand.h>

#include <Fusion/Argparse.h>
#include <iostream>

namespace Fusion
{
Result<void> LatencyCommand::Run(Options options)
{
    LatencyCommand cmd(std::move(options));
    return cmd.Run();
}

void LatencyCommand::Setup(
    ArgumentCommand& cmd,
    Options& options)
{
    using namespace std::string_view_literals;

    cmd.Help("Run the Latency Benchmarking Command"sv);
}

LatencyCommand::LatencyCommand(Options options)
    : m_options(std::move(options))
{ }

LatencyCommand::~LatencyCommand()
{ }

Result<void> LatencyCommand::Run()
{
    std::cout << "LatencyCommand\n";

    return Success;
}
}  // namespace Fusion
