// Copyright 2015-2022 Daniel Weiner
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <NetworkTool/Main.h>
#include <NetworkTool/ClientCommand.h>
#include <NetworkTool/LookupCommand.h>
#include <NetworkTool/ServerCommand.h>

#include <Fusion/Argparse.h>
#include <Fusion/Main.h>

#include <Fusion/Generated/Version.h>
#include <Fusion/Generated/VersionCommand.h>

namespace Fusion
{
    Result<void> Main(std::span<std::string_view> args)
    {
        using namespace std::string_view_literals;
        using namespace NetworkTool;

        ArgumentParser parser(ArgumentParser::Params{
            .program = Version::Project(),
            .description = "Network Concept Exploratory Tool for select() operations"sv,
        });

        VersionCommand::Options versionOptions;
        VersionCommand::Setup(parser, versionOptions);

        ClientCommand::Options clientOptions;
        auto& clientCmd = parser.AddCommand("client"sv)
            .Action([&](const ArgumentCommand&) -> Result<void> {
                return ClientCommand::Run(std::move(clientOptions));
            });

        ClientCommand::Setup(clientCmd, clientOptions);

        LookupCommand::Options lookupOptions;
        auto& lookupCmd = parser.AddCommand("lookup"sv)
            .Action([&](const ArgumentCommand&) -> Result<void> {
                return LookupCommand::Run(std::move(lookupOptions));
            });

        LookupCommand::Setup(lookupCmd, lookupOptions);

        ServerCommand::Options serverOptions;
        auto& serverCmd = parser.AddCommand("client"sv)
            .Action([&](const ArgumentCommand&) -> Result<void> {
                return ServerCommand::Run(std::move(serverOptions));
            });

        ServerCommand::Setup(serverCmd, serverOptions);

        if (Result<void> result = parser.Parse(args); !result)
        {
            return result.Error();
        }
        if (parser.Command().empty())
        {
            parser.PrintHelp();
        }

        return Success;
    }
}  // namespace Fusion
