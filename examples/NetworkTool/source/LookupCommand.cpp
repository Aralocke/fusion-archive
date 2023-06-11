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

#include <NetworkTool/LookupCommand.h>

#include <Fusion/Argparse.h>
#include <Fusion/Network.h>

#include <condition_variable>
#include <iostream>
#include <mutex>

namespace NetworkTool
{
Result<void> LookupCommand::Run(Options options)
{
    LookupCommand cmd(std::move(options));
    return cmd.Run();
}

void LookupCommand::Setup(
    ArgumentCommand& cmd,
    Options& options)
{
    using namespace std::string_view_literals;

    cmd.Help("Run the Resolver tool"sv);
}

LookupCommand::LookupCommand(Options options)
    : m_options(std::move(options))
{ }

LookupCommand::~LookupCommand()
{
    //if (resolver)
    //{
    //    resolver->Stop();
    //    resolver.reset();
    //}
    if (network)
    {
        network->Stop();
        network.reset();
    }
}

Result<void> LookupCommand::Run()
{
    if (auto result = Network::Create(); !result)
    {
        return result.Error()
            .WithContext("failed to create network");
    }
    else
    {
        network = std::move(*result);
    }

    /*
    Resolver::Options resolverOptions;
    if (auto result = Resolver::Create(
        std::move(resolverOptions),
        *network); !result)
    {
        return result.Error()
            .WithContext("failed to create resolver");
    }
    else
    {
        resolver = std::move(*result);
    }
    */

    std::mutex mutex;
    std::condition_variable cond;
    std::unique_lock lock(mutex);

    std::string address{ "google.com" };
    fmt::print(FMT_STRING("Resolving: {}\n"), address);

    /*
    resolver->Resolve(address,
        [&](Failure& error, ResolveResult& result)
        {
            fmt::print(FMT_STRING("Result for: {}\n"),
                result.request->hostname);

            if (error)
            {
                fmt::print(FMT_STRING("Failed to lookup hostname: {}\n"),
                    error);
            }
            else
            {
                fmt::print(FMT_STRING("Canonical name: {}\n"),
                    result.canonicalName);
                fmt::print(FMT_STRING("Count: {}\n"),
                    result.results.size());
                for (const AddressInfo& result : result.results)
                {
                    fmt::print(FMT_STRING("address={}\n"), result.address);
                }
            }
            cond.notify_one();
        });

    cond.wait(lock);
    fmt::print("Complete\n");
    */

    return Success;
}
}  // namespace Fusion
