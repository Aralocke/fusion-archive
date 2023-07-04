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

#include <Fusion/Internal/Http.h>

#include <Fusion/Internal/StandardHttpClient.h>

#include <Fusion/Assert.h>

#include <iostream>

namespace Fusion
{
std::string_view ToString(HttpMethod method)
{
    using namespace std::string_view_literals;

    static constexpr std::string_view s_types[size_t(HttpMethod::_Count)] = {
        "None"sv,
        "Delete"sv,
        "Get"sv,
        "Head"sv,
        "Options"sv,
        "Post"sv,
        "Put"sv,
    };

    return s_types[size_t(method)];
}

std::ostream& operator<<(
    std::ostream& o,
    HttpMethod method)
{
    return o << ToString(method);
}

HttpClient::RequestOptions::RequestOptions(
    std::string_view url,
    HttpMethod method)
    : url(url)
    , method(method)
{
    FUSION_ASSERT(!url.empty());
    FUSION_ASSERT(method != HttpMethod::None);
}

void HttpClient::RequestOptions::AddHeader(
    std::string name,
    std::string value)
{
    FUSION_ASSERT(!name.empty());

    if (!name.empty())
    {
        headers.push_back(Header{
            .name = std::move(name),
            .value = std::move(value),
        });
    }
}

Result< std::unique_ptr<HttpClient>> HttpClient::Create(Params&& params)
{
    auto client = std::make_unique<Internal::StandardHttpClient>(
        std::forward<Params>(params));

    if (Result<void> result = client->Start(); !result)
    {
        return result.Error();
    }

    return std::move(client);
}

HttpClient::HttpClient() = default;

HttpClient::~HttpClient() = default;

Result<HttpClient::Request> HttpClient::SendRequest(
    RequestOptions options)
{
    if (options.url.empty())
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid request: empty url");
    }

    if (options.method == HttpMethod::None)
    {
        return Failure(E_INVALID_ARGUMENT)
            .WithContext("invalid request: unspecified HTTP type");
    }

    for (const Header& header : options.headers)
    {
        if (header.name.empty())
        {
            return Failure(E_INVALID_ARGUMENT)
                .WithContext("invalid request: empty header key");
        }
        if (header.value.empty())
        {
            return Failure(E_INVALID_ARGUMENT)
                .WithContext("invalid request: empty header value for key '{}'",
                    header.name);
        }
    }

    if (!options.body.empty())
    {
        // we have an uploading request-type
        if (options.method != HttpMethod::Post
            && options.method != HttpMethod::Put)
        {
            return Failure(E_INVALID_ARGUMENT)
                .WithContext("invalid request: request body specified on '{}' request type",
                    options.method);
        }
    }

    return Execute(std::move(options));
}

void HttpClient::Stop()
{
    Stop(nullptr);
}
}  // namespace Fusion
