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

#include <memory>

struct evp_pkey_st;
struct ssl_ctx_st;
struct x509_st;

typedef struct evp_pkey_st EVP_PKEY;
typedef struct ssl_ctx_st SSL_CTX;
typedef struct x509_st X509;

namespace Fusion
{
struct Deleter
{
    template<typename T>
    void operator()(T* ptr) const;
};

using Certificate = std::unique_ptr<X509, Deleter>;
}  // namespace Fusion
