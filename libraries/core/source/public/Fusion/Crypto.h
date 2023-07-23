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

#include <Fusion/Fwd/Crypto.h>

#include <Fusion/DateTime.h>
#include <Fusion/Result.h>

#include <string>
#include <vector>

namespace Fusion
{
//
//
//
class Crypto final
{
public:
    //
    //
    //
    static Result<std::vector<Certificate>> LoadSystemRootStore();

public:
    //
    //
    //
    static Result<void> AddCertificate(
        SSL_CTX* context,
        X509* cert);

public:
    enum class CertificateVersion
    {
        v1,
        v2,
        v3,

        Default = v3,
    };

    struct CertificateOptions
    {
        CertificateVersion version{ CertificateVersion::Default };
        uint32_t serial{ 1 };

        SystemClock::time_point notBefore{ SystemClock::now() };
        SystemClock::time_point notAfter{ SystemClock::now() + std::chrono::months(12) };

        std::string name;
        std::string issuer;

        std::string countryCode;
        std::string stateProvinceName;
        std::string localityName;
        std::string organizationName;
        std::string organizationalUnitName;
        std::string commonName;

        Certificate* signer{ nullptr };
    };

    static Result<Certificate> GenerateCertificate(CertificateOptions options);
};
}
