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
#include <Fusion/Global.h>
#include <Fusion/Http.h>
#include <Fusion/Json.h>
#include <Fusion/Network.h>

#include <condition_variable>
#include <iostream>
#include <mutex>

#include <fmt/format.h>

#define FMT_FORMAT(message,...) \
    fmt::format(FMT_STRING(message), __VA_ARGS__)

namespace NetworkTool
{
Result<void> LookupCommand::Run(
    GlobalOptions& globalOptions,
    Options options)
{
    LookupCommand cmd(globalOptions, std::move(options));
    return cmd.Run();
}

void LookupCommand::Setup(
    ArgumentCommand& cmd,
    Options& options)
{
    using namespace std::string_view_literals;

    cmd.AddArgument(options.record, "record", 'r')
        .Default("A")
        .Help("DNS record type to lookup");

    cmd.AddArgument(options.server, "server", 's')
        .Default("https://dns.google")
        .Help("DOH server to use for lookup");

    cmd.AddPositional(options.address, "address")
        .Help("Hostname to lookup");

    cmd.Help("Run the Resolver tool"sv);
}

LookupCommand::LookupCommand(
    GlobalOptions& globalOptions,
    Options options)
    : m_globalOptions(globalOptions)
    , m_options(std::move(options))
{ }

LookupCommand::~LookupCommand()
{
    if (httpclient)
    {
        httpclient->Stop();
        httpclient.reset();
    }
    if (network)
    {
        network->Stop();
        network.reset();
    }
}

Result<void> LookupCommand::Run()
{
    Global global(Global::Params{
        .initializeCurl = true,
        .initializeOpenssl = true,
    });

    if (Result<void> result = global.Start(
        m_globalOptions); !result)
    {
        return result.Error()
            .WithContext("Failed to initialize global state");
    }

    if (auto result = Network::Create(); !result)
    {
        return result.Error()
            .WithContext("failed to create network");
    }
    else
    {
        network = std::move(*result);
    }

    HttpClient::Params httpClientParams{
        .captureCertDetails = true,
    };

    std::unique_ptr<HttpClient> httpClient;
    if (auto result = HttpClient::Create(std::move(httpClientParams)); !result)
    {
        return result.Error().WithContext("failed to create HttpClient");
    }
    else
    {
        httpClient = std::move(*result);
    }

    std::string url = FMT_FORMAT("{}/resolve?name={}&type=A&do=1",
        m_options.server,
        m_options.address);

    HttpClient::RequestOptions options;
    {
        options.url = url;
        options.method = HttpMethod::Get;
    }
    HttpClient::Request request;

    if (auto result = httpClient->SendRequest(
        std::move(options)); !result)
    {
        return result.Error()
            .WithContext("Failed to send HTTP request");
    }
    else
    {
        request = std::move(*result);
    }

    HttpClient::Response response = request.response.get();

    if (response.error)
    {
        return Failure(E_FAILURE)
            .WithContext("request failed: {}", response.error);
    }

    simdjson::dom::parser parser;
    simdjson::dom::element doc;

    auto result = parser.parse(response.body);
    std::ignore = result.get(doc);

    PrettyPrint(std::cout, doc);

    return Success;
}
}  // namespace Fusion
