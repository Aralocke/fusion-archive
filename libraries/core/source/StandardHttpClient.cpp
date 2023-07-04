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

#include <Fusion/Internal/StandardHttpClient.h>

#include <Fusion/Assert.h>
#include <Fusion/Crypto.h>
#include <Fusion/Network.h>
#include <Fusion/StringUtil.h>

#include <array>

#include <curl/curl.h>

#include <fmt/format.h>

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

namespace Fusion::Internal
{
struct StandardHttpClient::RequestData
{
    HttpClient::RequestOptions options;

    uint64_t id{ UINT64_MAX };
    bool paused{ false };

    std::promise<HttpClient::Response> response;
};

struct StandardHttpClient::State
{
    // HttpClient Params
    Params* params{ nullptr };
    std::span<Certificate> systemRoots;

    // Curl State
    CURLM* handle{ nullptr };
    curl_slist* hosts{ nullptr };   
};

struct StandardHttpClient::RequestContext
{
    uint64_t id{ UINT64_MAX };

    State* client{ nullptr };
    RequestData* data{ nullptr };

    HttpClient::Response response;

    CURL* handle{ nullptr };
    curl_slist* headers{ nullptr };

    std::array<char, CURL_ERROR_SIZE> error = {};
};

static int32_t HandleDebugFn(
    CURL* handle,
    curl_infotype type,
    char* data,
    size_t size,
    void* userdata)
{
    using namespace std::string_view_literals;

    using RequestContext = StandardHttpClient::RequestContext;
    using RequestOptions = HttpClient::RequestOptions;

    FUSION_UNUSED(handle);
    FUSION_ASSERT(userdata);

    RequestContext* context = static_cast<RequestContext*>(userdata);
    FUSION_ASSERT(context->data);

    const RequestOptions& options = context->data->options;

    std::string_view text(data, size);
    std::span<const uint8_t> span{
        reinterpret_cast<const uint8_t*>(data),
        size
    };

    switch (type)
    {
    case CURLINFO_TEXT:
    {
        options.debugFn(text, {});
        return 0;
    }
    case CURLINFO_HEADER_OUT:
    {
        text = "=> Send header"sv;
        break;
    }
    case CURLINFO_DATA_OUT:
    {
        text = "=> Send data"sv;
        break;
    }
    case CURLINFO_SSL_DATA_OUT:
    {
        text = "=> Send SSL data";
        break;
    }
    case CURLINFO_HEADER_IN:
    {
        text = "<= Recv header"sv;
        break;
    }
    case CURLINFO_DATA_IN:
    {
        text = "<= Recv data"sv;
        break;
    }
    case CURLINFO_SSL_DATA_IN:
    {
        text = "<= Recv SSL data"sv;
        break;
    }
    default:
        return 0;
    }

    options.debugFn(text, span);
    return 0;
}

static size_t HandleHeaderFn(
    char* buffer,
    size_t size,
    size_t nitems,
    void* userdata)
{
    using RequestContext = StandardHttpClient::RequestContext;
    using RequestOptions = HttpClient::RequestOptions;
    using Response = HttpClient::Response;

    FUSION_ASSERT(userdata);
    RequestContext* context = reinterpret_cast<RequestContext*>(userdata);
    FUSION_ASSERT(context->data);

    const RequestOptions& options = context->data->options;
    Response& response = context->response;

    std::string_view header(buffer, size * nitems);

    if (auto colon = header.find(':');
        colon != std::string_view::npos)
    {
        std::string key = StringUtil::ToLowerCopy(
            StringUtil::Trim(header.substr(0, colon)));
        std::string value = StringUtil::TrimCopy(
            header.substr(colon + 1));

        if (options.headerFn)
        {
            if (!options.headerFn(key, value))
            {
                return 0;
            }
        }

        response.headers.push_back(
            HttpClient::Header{
                .name = std::move(key),
                .value = std::move(value),
            });
    }
    else if (options.statusFn)
    {
        if (!options.statusFn(header))
        {
            return 0;
        }
    }

    return size * nitems;
}

static int32_t HandleProgressFn(
    void* userdata,
    curl_off_t dltotal,
    curl_off_t dlnow,
    curl_off_t ultotal,
    curl_off_t ulnow)
{
    using RequestContext = StandardHttpClient::RequestContext;
    using RequestOptions = HttpClient::RequestOptions;

    FUSION_ASSERT(userdata);
    RequestContext* context = static_cast<RequestContext*>(userdata);
    FUSION_ASSERT(context->data);

    const RequestOptions& options = context->data->options;

    if (options.progressFn)
    {
        if (!options.progressFn(
            static_cast<int64_t>(dltotal),
            static_cast<int64_t>(dlnow),
            static_cast<int64_t>(ultotal),
            static_cast<int64_t>(ulnow)))
        {
            return 1;
        }
    }

    return CURLE_OK;
}

static int32_t HandleReadFn(
    char* buffer,
    size_t size,
    size_t nitems,
    void* userdata)
{
    using RequestContext = StandardHttpClient::RequestContext;
    using RequestOptions = HttpClient::RequestOptions;

    FUSION_ASSERT(userdata);
    RequestContext* context = static_cast<RequestContext*>(userdata);

    FUSION_ASSERT(context->data);
    RequestOptions& options = context->data->options;

    if (!options.readFn)
    {
        return 0;
    }

    bool more = false;
    size_t total = size * nitems;
    if (!options.readFn(buffer, total, more))
    {
        return CURL_READFUNC_ABORT;
    }

    if (!more)
    {
        options.readFn = nullptr;
    }
    else if (total == 0)
    {
        context->data->paused = true;
        return CURL_READFUNC_PAUSE;
    }

    return total;
}

static int32_t HandleSeekFn(
    void* userdata,
    curl_off_t offset,
    int32_t origin)
{
    using RequestContext = StandardHttpClient::RequestContext;
    using RequestOptions = HttpClient::RequestOptions;

    FUSION_ASSERT(userdata);
    RequestContext* context = static_cast<RequestContext*>(userdata);

    FUSION_ASSERT(context->data);

    const RequestOptions& options = context->data->options;

    if (!options.seekFn(offset, origin))
    {
        return CURL_SEEKFUNC_FAIL;
    }

    return CURL_SEEKFUNC_OK;
}

static int32_t HandleSocketFn(
    void* userdata,
    curl_socket_t fd,
    curlsocktype purpose)
{
    using RequestContext = StandardHttpClient::RequestContext;
    using RequestOptions = HttpClient::RequestOptions;

    FUSION_ASSERT(userdata);
    RequestContext* context = static_cast<RequestContext*>(userdata);

    FUSION_ASSERT(context->data);

    const RequestOptions& options = context->data->options;

    if (purpose != CURLSOCKTYPE_IPCXN)
    {
        return CURL_SOCKOPT_OK;
    }

    if (options.socketFn)
    {
        if (!options.socketFn(fd))
        {
            return 1;
        }
    }

    return CURLE_OK;
}

static int32_t HandleSslFn(
    CURL* handle,
    void* ssl_ctx,
    void* userdata)
{
    using State = StandardHttpClient::State;

    FUSION_UNUSED(handle);
    FUSION_ASSERT(userdata);
    FUSION_ASSERT(ssl_ctx);

    const State* state = static_cast<State*>(userdata);
    SSL_CTX* ctx = static_cast<SSL_CTX*>(ssl_ctx);

    for (const Certificate& certificate : state->systemRoots)
    {
        if (Result<void> result = Crypto::AddCertificate(
            ctx,
            certificate.get()); !result)
        {
            continue;
        }
    }

    return CURLE_OK;
}

static size_t HandleWriteFn(
    char* data,
    size_t size,
    size_t nitems,
    void* userdata)
{
    using RequestContext = StandardHttpClient::RequestContext;
    using RequestOptions = HttpClient::RequestOptions;
    using Response = HttpClient::Response;

    FUSION_ASSERT(userdata);

    RequestContext* context = static_cast<RequestContext*>(userdata);

    FUSION_ASSERT(context->data);

    Response& response = context->response;
    const RequestOptions& options = context->data->options;

    size_t total = size * nitems;

    if (options.writeFn)
    {
        bool pause = false;
        if (!options.writeFn(data, total, pause))
        {
            return 0;
        }
        if (pause)
        {
            context->data->paused = true;
            return CURL_WRITEFUNC_PAUSE;
        }
    }
    else
    {
        if (response.body.empty())
        {
            double contentLength = -1;
            curl_easy_getinfo(
                context->handle,
                CURLINFO_CONTENT_LENGTH_DOWNLOAD,
                &contentLength);

            if (contentLength > 0)
            {
                response.body.reserve(size_t(std::llround(contentLength)));
            }
        }

        response.body.append(data, total);
    }

    return total;
}

static void CompleteRequest(
    StandardHttpClient::RequestContext* context,
    int32_t result,
    bool captureCertificates)
{
    using Response = HttpClient::Response;

    FUSION_ASSERT(context);
    Response& response = context->response;

    response.errorCode = result;
    response.error = E_CANCELLED;
    response.error.WithContext(context->error.data());

    long httpCode = 0;
    curl_easy_getinfo(context->handle, CURLINFO_RESPONSE_CODE, &httpCode);
    response.httpCode = int32_t(httpCode);

    double uploadBytes = 0;
    curl_easy_getinfo(context->handle, CURLINFO_SIZE_UPLOAD, &uploadBytes);
    response.uploadedBytes = std::llround(uploadBytes);

    double downloadBytes = 0;
    curl_easy_getinfo(context->handle, CURLINFO_SIZE_DOWNLOAD, &downloadBytes);
    response.downloadedBytes = std::llround(downloadBytes);

    curl_easy_getinfo(context->handle, CURLINFO_NAMELOOKUP_TIME, &response.dnsLookupTime);
    curl_easy_getinfo(context->handle, CURLINFO_CONNECT_TIME, &response.connectTime);
    curl_easy_getinfo(context->handle, CURLINFO_APPCONNECT_TIME, &response.initTime);
    curl_easy_getinfo(context->handle, CURLINFO_PRETRANSFER_TIME, &response.preTransferTime);
    curl_easy_getinfo(context->handle, CURLINFO_STARTTRANSFER_TIME, &response.startTransferTime);
    curl_easy_getinfo(context->handle, CURLINFO_TOTAL_TIME, &response.totalTime);
    curl_easy_getinfo(context->handle, CURLINFO_REDIRECT_TIME, &response.redirectTime);

    if (result == CURLE_OK && (httpCode >= 200 && httpCode <= 399))
    {
        response.error = E_SUCCESS;
    }

    if (captureCertificates)
    {
        curl_certinfo* certs = nullptr;
        if (curl_easy_getinfo(context->handle, CURLINFO_CERTINFO, &certs) == CURLE_OK)
        {
            FUSION_ASSERT(certs);
            response.certificates.reserve(certs->num_of_certs);

            for (int32_t i = 0; i < certs->num_of_certs; ++i)
            {
                for (curl_slist* data = certs->certinfo[i]; data; data = data->next)
                {
                    if (strncmp(data->data, "Cert:", 5) != 0)
                    {
                        continue;
                    }

                    std::unique_ptr<BIO, Deleter> bio(BIO_new(BIO_s_mem()));
                    BIO_puts(bio.get(), data->data + 5);

                    Certificate cert(PEM_read_bio_X509(
                        bio.get(), nullptr, nullptr, nullptr));

                    if (!cert)
                    {
                        continue;
                    }

                    response.certificates.push_back(std::move(cert));
                }
            }
        }
    }
}

StandardHttpClient::StandardHttpClient(Params&& params)
    : m_params(std::move(params))
{
    if (m_params.loadSystemRoots)
    {
        if (auto roots = Crypto::LoadSystemRootStore(); roots)
        {
            m_systemRoots = std::move(*roots);
        }
    }

    if (m_params.userAgent.empty())
    {
        m_params.userAgent = "Fusion HttpClient";
    }
}

StandardHttpClient::~StandardHttpClient()
{
    Stop(nullptr);
}

void StandardHttpClient::CancelRequest(uint64_t id)
{
    RequestAction action;
    action.type = RequestAction::Type::Cancel;
    action.id = id;
    std::future<Result<void>> future = action.notify.get_future();

    {
        std::unique_lock lock(m_mutex);

        if (m_shutdown)
        {
            return;
        }

        Enqueue(lock, std::move(action));
    }

    future.wait();
}

void StandardHttpClient::CreateRequestContext(RequestData& data)
{
    using namespace std::string_view_literals;

    using RequestOptions = HttpClient::RequestOptions;

    FUSION_ASSERT(m_requestData.contains(data.id));
    FUSION_ASSERT(!m_requestContexts.contains(data.id));

    RequestOptions& options = data.options;
    RequestContext* context = &m_requestContexts[data.id];
    context->client = m_state.get();
    context->data = &data;
    context->handle = curl_easy_init();
    context->id = data.id;

    switch (options.method)
    {
    case HttpMethod::Get:
        curl_easy_setopt(context->handle, CURLOPT_HTTPGET, 1L);
        break;
    case HttpMethod::Head:
        curl_easy_setopt(context->handle, CURLOPT_NOBODY, 1L);
        break;
    default:
        curl_easy_setopt(
            context->handle,
            CURLOPT_CUSTOMREQUEST,
            ToString(options.method).data());
        break;
    }

    curl_easy_setopt(context->handle, CURLOPT_PRIVATE, context);
    curl_easy_setopt(context->handle, CURLOPT_ERRORBUFFER, context->error.data());
    curl_easy_setopt(context->handle, CURLOPT_FAILONERROR, 0);
    curl_easy_setopt(context->handle, CURLOPT_FOLLOWLOCATION, options.followRedirects ? 1 : 0);

    curl_easy_setopt(context->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(context->handle, CURLOPT_TCP_FASTOPEN, 0);

    curl_easy_setopt(context->handle, CURLOPT_FORBID_REUSE, options.closeConnection);

    curl_easy_setopt(context->handle, CURLOPT_NOPROXY, "*");
    curl_easy_setopt(context->handle, CURLOPT_SSL_VERIFYPEER, options.verifyCerts ? 1 : 0);
    curl_easy_setopt(context->handle, CURLOPT_SSL_VERIFYHOST, options.verifyCerts ? 2 : 0);

    curl_easy_setopt(context->handle, CURLOPT_URL, options.url.c_str());

    bool userAgentHeader = false;
    bool expectHeader = false;

    for (const HttpClient::Header& header : options.headers)
    {
        std::string h = fmt::format(FMT_STRING("{}: {}"),
            header.name, header.value);
        context->headers = curl_slist_append(context->headers, h.c_str());

        if (StringUtil::CompareI(header.name, "User-Agent"sv) == 0)
        {
            userAgentHeader = true;
        }
        else if (StringUtil::CompareI(header.name, "Expect"sv) == 0)
        {
            expectHeader = true;
        }
    }

    if (!userAgentHeader)
    {
        context->headers = curl_slist_append(
            context->headers,
            m_params.userAgent.c_str());
    }

    if (!expectHeader)
    {
        if (options.method == HttpMethod::Post
            || options.method == HttpMethod::Put)
        {
            context->headers = curl_slist_append(
                context->headers,
                "Expect: ");
        }
    }

    curl_easy_setopt(context->handle, CURLOPT_HTTPHEADER, context->headers);

    if (!options.username.empty() && options.password.empty())
    {
        curl_easy_setopt(context->handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(
            context->handle,
            CURLOPT_USERNAME,
            options.username.c_str());
        curl_easy_setopt(
            context->handle,
            CURLOPT_PASSWORD,
            options.password.c_str());
    }

    curl_easy_setopt(context->handle, CURLOPT_WRITEFUNCTION, HandleWriteFn);
    curl_easy_setopt(context->handle, CURLOPT_WRITEDATA, context);

    if (options.readFn)
    {
        curl_easy_setopt(context->handle, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(context->handle, CURLOPT_READFUNCTION, HandleReadFn);
        curl_easy_setopt(context->handle, CURLOPT_READDATA, context);

        if (options.seekFn)
        {
            curl_easy_setopt(context->handle, CURLOPT_SEEKFUNCTION, HandleSeekFn);
            curl_easy_setopt(context->handle, CURLOPT_SEEKDATA, context);
        }
    }

    if (options.maxDownloadSpeed != 0)
    {
        curl_easy_setopt(
            context->handle,
            CURLOPT_MAX_RECV_SPEED_LARGE,
            static_cast<curl_off_t>(options.maxDownloadSpeed));
    }

    if (options.maxUploadSpeed != 0)
    {
        curl_easy_setopt(
            context->handle,
            CURLOPT_MAX_SEND_SPEED_LARGE,
            static_cast<curl_off_t>(options.maxUploadSpeed));
    }

    curl_easy_setopt(context->handle, CURLOPT_HEADERFUNCTION, HandleHeaderFn);
    curl_easy_setopt(context->handle, CURLOPT_HEADERDATA, context);

    curl_easy_setopt(context->handle, CURLOPT_SSL_CTX_FUNCTION, HandleSslFn);
    curl_easy_setopt(context->handle, CURLOPT_SSL_CTX_DATA, m_state.get());

    if (!m_params.certBundle.empty())
    {
        curl_easy_setopt(context->handle, CURLOPT_CAINFO, m_params.certBundle.c_str());
    }
    else
    {
        curl_easy_setopt(context->handle, CURLOPT_CAINFO, nullptr);
    }

    if (options.progressFn)
    {
        curl_easy_setopt(context->handle, CURLOPT_XFERINFOFUNCTION, HandleProgressFn);
        curl_easy_setopt(context->handle, CURLOPT_XFERINFODATA, context);
        curl_easy_setopt(context->handle, CURLOPT_NOPROGRESS, 0);
    }

    if (options.debugFn)
    {
        curl_easy_setopt(context->handle, CURLOPT_DEBUGFUNCTION, HandleDebugFn);
        curl_easy_setopt(context->handle, CURLOPT_DEBUGDATA, context);
        curl_easy_setopt(context->handle, CURLOPT_VERBOSE, 1);
    }

    curl_easy_setopt(context->handle, CURLOPT_TCP_KEEPALIVE, options.keepAlive ? 1 : 0);
    curl_easy_setopt(context->handle, CURLOPT_TCP_KEEPIDLE, 60);
    curl_easy_setopt(context->handle, CURLOPT_TCP_KEEPINTVL, 1);

    uint32_t connectTimout = std::chrono::duration_cast<
        std::chrono::seconds>(options.connectTimeout).count();
    curl_easy_setopt(context->handle, CURLOPT_CONNECTTIMEOUT, connectTimout);

    uint32_t timeout = std::chrono::duration_cast<
        std::chrono::seconds>(options.timeout).count();
    curl_easy_setopt(context->handle, CURLOPT_TIMEOUT, timeout);

    if (m_state->hosts)
    {
        curl_easy_setopt(context->handle, CURLOPT_RESOLVE, m_state->hosts);
    }

    if (options.compression)
    {
        curl_easy_setopt(context->handle, CURLOPT_ACCEPT_ENCODING, "");
    }

    if (m_params.captureCertDetails)
    {
        curl_easy_setopt(context->handle, CURLOPT_CERTINFO, 1);
    }

    if (data.paused)
    {
        curl_easy_pause(context->handle, CURLPAUSE_ALL);
    }

    if (options.socketFn)
    {
        curl_easy_setopt(context->handle, CURLOPT_SOCKOPTFUNCTION, HandleSocketFn);
        curl_easy_setopt(context->handle, CURLOPT_SOCKOPTDATA, context);
    }

    curl_multi_add_handle(m_state->handle, context->handle);
}

void StandardHttpClient::Enqueue(
    std::unique_lock<std::mutex>& lock,
    RequestAction&& action)
{
    FUSION_ASSERT(lock.owns_lock());
    FUSION_ASSERT(m_state);

    m_queue.push(std::forward<RequestAction>(action));
    lock.unlock();

    curl_multi_wakeup(m_state->handle);
}

Result<HttpClient::Request> StandardHttpClient::Execute(
    RequestOptions&& options)
{
    std::unique_lock lock(m_mutex);

    if (m_shutdown)
    {
        return Failure(E_CANCELLED)
            .WithContext("request submitted after shutdown");
    }

    uint64_t id = m_nextId++;
    auto [it, inserted] = m_requestData.try_emplace(
        id,
        RequestData{
            .options = std::move(options),
            .id = id,
        });

    FUSION_ASSERT(inserted);
    RequestData& data = it->second;

    {
        Enqueue(lock, RequestAction{
            .type = RequestAction::Type::Send ,
            .id = id,
        });
        FUSION_ASSERT(!lock.owns_lock());
    }

    Request request;
    request.id = id;
    request.response = data.response.get_future();

    return std::move(request);
}

void StandardHttpClient::FinalizeRequest(RequestContext& context)
{
    if (context.handle)
    {
        curl_multi_remove_handle(m_state->handle, context.handle);
        curl_easy_cleanup(context.handle);
    }

    if (context.headers)
    {
        curl_slist_free_all(context.headers);
    }

    context.data->response.set_value(std::move(context.response));

    uint64_t id = context.id;
    m_requestContexts.erase(id);
    m_requestData.erase(id);

    UpdatePendingRequests();
}

void StandardHttpClient::HandleCancelAction(uint64_t id)
{
    // We assume this function is called from the protection of
    // the 'm_mutex' lock.

    if (m_requestContexts.contains(id))
    {
        RequestContext& context = m_requestContexts[id];
        context.response.error = E_CANCELLED;

        FinalizeRequest(context);
    }
    else if (m_pendingRequests.contains(id))
    {
        HttpClient::Response response;
        response.error = E_CANCELLED;
        {
            RequestData& data = m_requestData[id];
            data.response.set_value(std::move(response));
        }
        m_pendingRequests.erase(id);
        m_requestData.erase(id);
    }
}

void StandardHttpClient::HandlePauseAction(uint64_t id)
{
    // We assume this function is called from the protection of
    // the 'm_mutex' lock.

    if (m_requestContexts.contains(id))
    {
        RequestContext& context = m_requestContexts[id];

        curl_easy_pause(context.handle, CURLPAUSE_ALL);
        context.data->paused = true;
    }
    else if (m_pendingRequests.contains(id))
    {
        RequestData& data = m_requestData[id];
        data.paused = true;
    }
}

void StandardHttpClient::HandleResumeAction(uint64_t id)
{
    // We assume this function is called from the protection of
    // the 'm_mutex' lock.

    if (m_requestContexts.contains(id))
    {
        RequestContext& context = m_requestContexts[id];

        curl_easy_pause(context.handle, CURLPAUSE_CONT);
        context.data->paused = false;
    }
    else if (m_pendingRequests.contains(id))
    {
        RequestData& data = m_requestData[id];
        data.paused = false;
    }
}

void StandardHttpClient::HandleSendAction(uint64_t id)
{
    // We assume this function is called from the protection of
    // the 'm_mutex' lock.

    FUSION_ASSERT(m_requestData.contains(id));
    if (m_requestContexts.size() < m_params.maxConcurrentRequests)
    {
        RequestData& data = m_requestData[id];
        CreateRequestContext(data);
    }
    else
    {
        FUSION_ASSERT(!m_pendingRequests.contains(id));
        m_pendingRequests.emplace(id);
    }
}

void StandardHttpClient::PauseRequest(uint64_t id)
{
    RequestAction action;
    action.type = RequestAction::Type::Pause;
    action.id = id;
    std::future<Result<void>> future = action.notify.get_future();

    {
        std::unique_lock lock(m_mutex);

        if (m_shutdown)
        {
            return;
        }

        Enqueue(lock, std::move(action));
    }

    future.wait();
}

void StandardHttpClient::PerformTransfers()
{
    int32_t count = 0;
    CURLMcode result = curl_multi_perform(m_state->handle, &count);

    if (result != CURLM_OK)
    {
        Failure f{ Failure::Errno() };
        f.WithContext("curl_multi_perform failed with '{}': {}",
            int32_t(result),
            curl_multi_strerror(result));
    }
    else
    {
        UpdateTransfers();
    }
}

bool StandardHttpClient::PollNetwork()
{
    int32_t err = 0;
    CURLMcode result = curl_multi_poll(
        m_state->handle,  // curl multi handle
        nullptr,          // extra fds
        0,                // extra fds count
        1000,             // timeout ms
        &err);            // network return code

    Failure f(err);
    f.WithContext("curl_multi_poll failed with '{}': {}",
        int32_t(result),
        curl_multi_strerror(result));

    // If this returns false it kills the polling thread

    return true;
}

void StandardHttpClient::ProcessQueue()
{
    while (!m_queue.empty())
    {
        RequestAction action = std::move(m_queue.front());
        m_queue.pop();

        switch (action.type)
        {
        case RequestAction::Type::Cancel:
            HandleCancelAction(action.id);
            break;
        case RequestAction::Type::Pause:
            HandlePauseAction(action.id);
            break;
        case RequestAction::Type::Resume:
            HandleResumeAction(action.id);
            break;
        case RequestAction::Type::Send:
            HandleSendAction(action.id);
            break;
        default:
        {
            action.notify.set_value(Failure(E_CANCELLED));
            break;
        }
        }
    }
}

void StandardHttpClient::ResumeRequest(uint64_t id)
{
    RequestAction action;
    action.type = RequestAction::Type::Resume;
    action.id = id;
    std::future<Result<void>> future = action.notify.get_future();

    {
        std::unique_lock lock(m_mutex);

        if (m_shutdown)
        {
            return;
        }

        Enqueue(lock, std::move(action));
    }

    future.wait();
}

bool StandardHttpClient::RunOnce()
{
    std::unique_lock lock(m_mutex);

    if (m_shutdown)
    {
        m_shutdownComplete = true;

        lock.unlock();
        m_cond.notify_one();

        return false;
    }

    {
        ProcessQueue();
        PerformTransfers();
    }

    lock.unlock();
    if (!PollNetwork())
    {
        return false;
    }

    return true;
}

Result<void> StandardHttpClient::Start()
{
    std::unique_lock lock(m_mutex);
    
    if (m_state)
    {
        return Failure(E_INVALID_ARGUMENT);
    }

    m_state = std::make_unique<State>();
    m_state->handle = curl_multi_init();
    m_state->params = &m_params;
    m_state->systemRoots = m_systemRoots;

    if (KnownHosts* knownHosts = m_params.knownHosts)
    {
        for (const auto& [hostname, addresses] : *knownHosts)
        {
            FUSION_UNUSED(hostname);
            FUSION_UNUSED(addresses);
        }
    }

    curl_multi_setopt(
        m_state->handle,
        CURLMOPT_MAX_HOST_CONNECTIONS,
        m_params.maxPerHostConnections);

    m_thread = std::thread([&] {
        while (RunOnce());
    });

    return Success;
}

void StandardHttpClient::Stop(std::function<void()> fn)
{
    std::unique_lock lock(m_mutex);

    if (m_shutdown)
    {
        FUSION_ASSERT(!m_state);
        FUSION_ASSERT(m_shutdownComplete);

        if (fn)
        {
            fn();
        }

        return;
    }

    m_shutdown = true;
    curl_multi_wakeup(m_state->handle);
    m_cond.wait(lock, [&] { return m_shutdownComplete.load(); });

    if (m_thread.joinable())
    {
        m_thread.join();
    }

    std::vector<uint64_t> ids;
    ids.reserve(m_requestData.size());

    for (const auto& [id, data] : m_requestData)
    {
        FUSION_UNUSED(data);
        ids.push_back(id);
    }

    for (uint64_t id : ids)
    {
        HandleCancelAction(id);
    }

    FUSION_ASSERT(m_pendingRequests.empty());
    FUSION_ASSERT(m_requestContexts.empty());

    while (!m_queue.empty())
    {
        RequestAction action = std::move(m_queue.front());
        m_queue.pop();

        Response response;
        response.error = E_CANCELLED;

        FUSION_ASSERT(m_requestData.contains(action.id));
        RequestData& data = m_requestData[action.id];

        data.response.set_value(std::move(response));
        action.notify.set_value(Success);
    }

    m_requestData.clear();

    curl_multi_cleanup(m_state->handle);
    curl_slist_free_all(m_state->hosts);
    m_state.reset();

    lock.unlock();

    if (fn)
    {
        fn();
    }
}

void StandardHttpClient::UpdateTransfers()
{
    while (true)
    {
        int32_t count = 0;
        CURLMsg* msg = curl_multi_info_read(m_state->handle, &count);

        if (!msg)
        {
            break;
        }

        RequestContext* context = nullptr;
        curl_easy_getinfo(
            msg->easy_handle,
            CURLINFO_PRIVATE,
            reinterpret_cast<char**>(&context));

        FUSION_ASSERT(context);

        CompleteRequest(
            context,
            msg->data.result,
            m_params.captureCertDetails);

        FinalizeRequest(*context);
    }
}

void StandardHttpClient::UpdatePendingRequests()
{
    while (!m_pendingRequests.empty()
        && m_requestContexts.size() < m_params.maxConcurrentRequests)
    {
        auto it = m_pendingRequests.begin();
        RequestData& data = m_requestData[*it];
        CreateRequestContext(data);
        m_pendingRequests.erase(it);
    }
}
}
