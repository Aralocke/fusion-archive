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

#include <Fusion/Platform.h>
#if FUSION_PLATFORM_WINDOWS

#include <Fusion/Library.h>
#include <Fusion/Random.h>
#include <Fusion/Windows.h>

#include <bcrypt.h>

namespace Fusion::Internal
{
bool SystemSecureBytes(
    uint8_t* buffer,
    size_t size)
{
    using BCryptGenRandomProc = decltype(BCryptGenRandom)*;

    static Library s_bcrypt;
    static BCryptGenRandomProc s_bcryptGenRandom { nullptr };

    if (!s_bcrypt.IsLoaded())
    {
        if (auto result = s_bcrypt.Load(
            "bcrypt.dll",
            LibraryLoadFlag::System); !result)
        {
            return result.Error();
        }

        s_bcryptGenRandom = (BCryptGenRandomProc)s_bcrypt.GetRawEntrypoint(
            "BCryptGenRandom");
    }

    if (!s_bcryptGenRandom)
    {
        return false;
    }

    constexpr ULONG ChunkSize = 4 * 1024;

    while (size > ChunkSize)
    {
        NTSTATUS result = s_bcryptGenRandom(
            NULL,
            buffer,
            ChunkSize,
            BCRYPT_USE_SYSTEM_PREFERRED_RNG);

        if (!BCRYPT_SUCCESS(result))
        {
            return false;
        }

        buffer += ChunkSize;
        size -= ChunkSize;
    }

    if (size > 0)
    {
        NTSTATUS result = s_bcryptGenRandom(
            NULL,
            buffer,
            static_cast<ULONG>(size),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG);

        return BCRYPT_SUCCESS(result);
    }

    return false;
}
}  // namespace Fusion

#endif
