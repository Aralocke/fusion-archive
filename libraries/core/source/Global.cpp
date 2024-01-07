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

#include <Fusion/Global.h>

#include <Fusion/Assert.h>

#include <fmt/format.h>

#include <curl/curl.h>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

namespace Fusion
{
template<typename Parser>
static void SetupParser(
    Parser& parser,
    GlobalOptions& options)
{
}

void Global::SetupArgumentParser(
    ArgumentCommand& cmd,
    GlobalOptions& options)
{
    SetupParser(cmd, options);
}

void Global::SetupArgumentParser(
    ArgumentParser& parser,
    GlobalOptions& options)
{
    SetupParser(parser, options);
}

Global::Global(Params params)
    : m_params(std::move(params))
{ }

Global::~Global()
{
    if (m_params.initializeCurl)
    {
        curl_global_cleanup();
    }
    if (m_params.initializeOpenssl)
    {
        OPENSSL_cleanup();
    }
}

Result<void> Global::InitializeCurl()
{
    FUSION_ASSERT(m_params.initializeCurl);

    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);

    if (res != CURLcode::CURLE_OK)
    {
        return Failure(E_FAILURE)
            .WithContext("cURL initialization failed: {}",
                curl_easy_strerror(res));
    }

    return Success;
}

Result<void> Global::InitializeOpenSSL()
{
    FUSION_ASSERT(m_params.initializeOpenssl);

    SSL_library_init();
    SSL_load_error_strings();

    RAND_poll();

    if (ERR_get_error() != 0)
    {
        return Failure(E_FAILURE)
            .WithContext("OpenSSL initialization failed: {}",
                ERR_error_string(ERR_get_error(), nullptr));
    }

    return Success;
}

Result<void> Global::Start(GlobalOptions& options)
{
    if (m_params.initializeOpenssl)
    {
        if (Result<void> result = InitializeOpenSSL(); !result)
        {
            return result.Error();
        }
    }

    if (m_params.initializeCurl)
    {
        if (Result<void> result = InitializeCurl(); !result)
        {
            return result.Error();
        }
    }

    return Success;
}
}  // namespace Fusion
