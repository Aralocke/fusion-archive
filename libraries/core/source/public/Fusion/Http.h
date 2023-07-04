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

#include <Fusion/Fwd/Http.h>
#include <Fusion/Fwd/Network.h>

#include <Fusion/Crypto.h>
#include <Fusion/Result.h>

#include <functional>
#include <future>
#include <iosfwd>
#include <map>
#include <span>
#include <string>
#include <vector>

namespace Fusion
{
//
//
//
enum class HttpMethod : uint8_t
{
    None = 0,

    Delete,
    Get,
    Head,
    Options,
    Post,
    Put,

    _Count
};

//
//
//
std::string_view ToString(HttpMethod method);

//
//
//
std::ostream& operator<<(std::ostream& o, HttpMethod method);

//
//
//
class HttpClient
{
public:
    //
    //
    //
    struct Params
    {
        //
        //
        //
        KnownHosts* knownHosts{ nullptr };

        //
        //
        //
        std::string certBundle;

        //
        //
        //
        bool verifyCerts{ true };

        //
        //
        //
        std::vector<std::string> trustedRoots;

        //
        //
        //
        bool loadSystemRoots{ true };

        //
        //
        //
        bool captureCertDetails{ false };

        //
        //
        //
        int32_t maxPerHostConnections{ 2 };

        //
        //
        //
        int32_t maxConcurrentRequests{ 10 };

        //
        //
        //
        std::string clientCertificate;

        //
        //
        //
        std::string clientPrivateKey;

        //
        //
        //
        std::string proxyHost;

        //
        //
        //
        std::string userAgent;
    };

    //
    //
    //
    static Result<std::unique_ptr<HttpClient>> Create(Params&& params);

public:
    //
    //
    //
    struct Header
    {
        std::string name;
        std::string value;
    };

    //
    //
    //
    struct Response
    {
        //
        //
        //
        int32_t errorCode{ INT32_MAX };

        //
        //
        //
        int32_t httpCode{ INT32_MAX };

        //
        //
        //
        Failure error{ E_FAILURE };

        //
        //
        //
        std::string body;

        //
        //
        //
        std::vector<Header> headers;

        //
        //
        //
        std::vector<Certificate> certificates;

        //
        //
        //
        uint64_t uploadedBytes{ 0 };

        //
        //
        //
        uint64_t downloadedBytes{ 0 };

        //
        //
        //
        double dnsLookupTime{ 0 };

        //
        //
        //
        double initTime{ 0 };

        //
        //
        //
        double connectTime{ 0 };

        //
        //
        //
        double preTransferTime{ 0 };

        //
        //
        //
        double startTransferTime{ 0 };

        //
        //
        //
        double totalTime{ 0 };

        //
        //
        //
        double redirectTime{ 0 };
    };

    //
    //
    //
    struct Request
    {
        //
        //
        //
        uint64_t id{ UINT64_MAX };

        //
        //
        //
        std::future<Response> response;
    };

public:
    //
    //
    //
    virtual ~HttpClient();

    //
    //
    //
    virtual void CancelRequest(uint64_t id) = 0;

    //
    //
    //
    virtual void PauseRequest(uint64_t id) = 0;

    //
    //
    //
    virtual void ResumeRequest(uint64_t id) = 0;

    //
    //
    //
    virtual Result<void> Start() = 0;

    //
    //
    //
    void Stop();

    //
    //
    //
    virtual void Stop(std::function<void()> fn) = 0;

public:
    //
    //
    //
    struct RequestOptions
    {
        //
        //
        //
        std::string url;

        //
        //
        //
        HttpMethod method{ HttpMethod::None };

        //
        //
        //
        std::vector<Header> headers;

        //
        //
        //
        std::string username;

        //
        //
        //
        std::string password;

        //
        //
        //
        std::string body;

        //
        //
        //
        Clock::duration connectTimeout{ std::chrono::seconds(30) };

        //
        //
        //
        Clock::duration timeout{ std::chrono::seconds(30) };

        //
        //
        //
        uint32_t maxDownloadSpeed{ 0 };

        //
        //
        //
        uint32_t maxUploadSpeed{ 0 };

        //
        //
        //
        bool followRedirects{ true };

        //
        //
        //
        bool keepAlive{ true };

        //
        //
        //
        bool compression{ true };

        //
        //
        //
        bool closeConnection{ false };

        //
        //
        //
        bool verifyCerts{ true };

        //
        //
        //
        std::function<void(
            std::string_view message,
            std::span<const uint8_t> data)> debugFn;

        //
        //
        //
        std::function<bool(
            std::string_view name,
            std::string_view value)> headerFn;

        //
        //
        //
        std::function<bool(
            uint64_t totalDownloadBytes,
            uint64_t currentDownloadBytes,
            uint64_t totalUploadBytes,
            uint64_t currentUploadBytes)> progressFn;

        //
        //
        //
        std::function<bool(
            void* data,
            size_t& size,
            bool& more)> readFn;

        //
        //
        //
        std::function<bool(
            int64_t offset,
            int32_t origin)> seekFn;

        //
        //
        //
        std::function<bool(Socket sock)> socketFn;

        //
        //
        //
        std::function<bool(std::string_view status)> statusFn;

        //
        //
        //
        std::function<bool(
            const void* data,
            size_t size,
            bool& pause)> writeFn;

        //
        //
        //
        RequestOptions() = default;

        //
        //
        //
        RequestOptions(std::string_view url, HttpMethod type);

        //
        //
        //
        void AddHeader(std::string name, std::string value);
    };

    //
    //
    //
    Result<Request> SendRequest(RequestOptions options);

protected:
    //
    //
    //
    HttpClient();

    //
    //
    //
    virtual Result<Request> Execute(RequestOptions&& options) = 0;
};
}  // namespace Fusion

#define FUSION_IMPL_HTTP 1
#include <Fusion/Impl/Http.h>
