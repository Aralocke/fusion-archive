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

#include <Fusion/Playground/Main.h>
#include <Fusion/Playground/PlayCommand.h>

#include <Fusion/Argparse.h>
#include <Fusion/Global.h>
#include <Fusion/Main.h>

#include <Fusion/Generated/Version.h>
#include <Fusion/Generated/VersionCommand.h>

#include <iostream>

namespace Fusion
{
    Result<void> Main(std::span<std::string_view> args)
    {
        using namespace std::string_view_literals;

        FUSION_UNUSED(std::setvbuf(stdout, nullptr, _IONBF, 0));
        FUSION_UNUSED(std::setvbuf(stderr, nullptr, _IONBF, 0));

        ArgumentParser parser(ArgumentParser::Params{
            .program = Version::Project(),
            .description = "Playground exploratory tool"sv,
        });

        GlobalOptions globalOptions;
        Global::SetupArgumentParser(parser, globalOptions);

        VersionCommand::Options versionOptions;
        VersionCommand::Setup(parser, versionOptions);

        PlayCommand::Options playOptions;
        auto& playCmd = parser.AddCommand("play"sv)
            .Action([&](const ArgumentCommand&) -> Result<void> {
                return PlayCommand::Run(
                    globalOptions,
                    std::move(playOptions));
            });

        PlayCommand::Setup(playCmd, playOptions);

        if (Result<void> result = parser.Parse(args); !result)
        {
            std::cerr << "Playground: "sv << result.Error().Message() << std::endl;
            return result.Error();
        }
        if (parser.Command().empty())
        {
            parser.PrintHelp();
        }

        return Success;
    }
}  // namespace Fusion
