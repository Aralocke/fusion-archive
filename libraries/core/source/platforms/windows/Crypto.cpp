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

#include <Fusion/Platform.h>
#if FUSION_PLATFORM_WINDOWS

#include <Fusion/Crypto.h>
#include <Fusion/Memory.h>
#include <Fusion/Windows.h>

#include <openssl/x509.h>

#include <wincrypt.h>

namespace Fusion
{
Result<std::vector<Certificate>> Crypto::LoadSystemRootStore()
{
    std::vector<Certificate> roots;

    DWORD flags = CERT_SYSTEM_STORE_CURRENT_USER
        | CERT_STORE_OPEN_EXISTING_FLAG
        | CERT_STORE_READONLY_FLAG;

    if (HCERTSTORE store = CertOpenStore(
        CERT_STORE_PROV_SYSTEM_W,
        0,
        0,
        flags,
        L"ROOT"))
    {
        PCCERT_CONTEXT context = nullptr;

        FUSION_SCOPE_GUARD([&] { CertFreeCertificateContext(context); });
        FUSION_SCOPE_GUARD([&] { CertCloseStore(store, 0); });

        while ((context = CertEnumCertificatesInStore(store, context)) != nullptr)
        {
            const unsigned char* bytes = context->pbCertEncoded;
            if (X509* certificate = d2i_X509(nullptr, &bytes, long(context->cbCertEncoded)))
            {
                roots.emplace_back(certificate);
            }
        }
    }
    else
    {
        return Failure::Errno()
            .WithContext("failed to open system root CA store");
    }

    return std::move(roots);
}
}  // namespace Fusion

#endif  // FUSION_PLATFORM_WINDOWS
