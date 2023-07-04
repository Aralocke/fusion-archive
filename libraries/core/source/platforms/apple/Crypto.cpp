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
#if FUSION_PLATFORM_OSX

#include <Fusion/Crypto.h>
#include <Fusion/Memory.h>

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>

#include <openssl/x509.h>

namespace Fusion
{
std::string StatusToString(OSStatus status)
{
    if (CFStringRef message = SecCopyErrorMessageString(status, nullptr))
    {
        const char* cStr = CFStringGetCStringPtr(
            message,
            kCFStringEncodingUTF8);
        FUSION_SCOPE_GUARD([&]{ CFRelease(message); });

        std::string errorMsg((cStr != nullptr) ? cStr : "");
        return errorMsg;
    }
    else
    {
        return "Unknown error";
    }
}

Result <std::vector<Certificate>> Crypto::LoadSystemRootStore()
{
    std::vector<Certificate> roots;

    CFMutableDictionaryRef query = nullptr;
    CFTypeRef result = nullptr;

    FUSION_SCOPE_GUARD([&]{ if(query) CFRelease(query); });
    FUSION_SCOPE_GUARD([&]{ if (result) CFRelease(result); });

    query = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks
    );
    CFDictionaryAddValue(query, kSecClass, kSecClassCertificate);
    CFDictionaryAddValue(query, kSecMatchLimit, kSecMatchLimitAll);
    CFDictionaryAddValue(query, kSecMatchTrustedOnly, kCFBooleanTrue);
    CFDictionaryAddValue(query, kSecReturnData, kCFBooleanTrue);

    OSStatus status = SecItemCopyMatching(query, &result);
    if (status == errSecSuccess)
    {
        CFArrayRef certificates = static_cast<CFArrayRef>(result);
        CFIndex count = CFArrayGetCount(certificates);

        for (CFIndex i = 0; i < count; ++i)
        {
            CFDataRef data = static_cast<CFDataRef>(
                    CFArrayGetValueAtIndex(
                        certificates,
                        i));

            const unsigned char* certData = reinterpret_cast<const unsigned char*>(CFDataGetBytePtr(data));
            int32_t certLen = static_cast<int32_t>(CFDataGetLength(data));

            if (X509* cert = d2i_X509(nullptr, &certData, certLen))
            {
                roots.emplace_back(cert);
            }
        }
    }
    else if (status == errSecItemNotFound)
    {
        return Failure(E_NOT_FOUND)
            .WithContext("unable to find root CA store: [{}] {}",
                 status, StatusToString(status));
    }
    else
    {
        return Failure(E_FAILURE)
            .WithContext("failed to open system root CA store: [{}] {}",
                 status, StatusToString(status));
    }

    return std::move(roots);
}
}

#endif