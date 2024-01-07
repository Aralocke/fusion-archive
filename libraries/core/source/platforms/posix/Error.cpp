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
#if FUSION_PLATFORM_POSIX

#include <Fusion/Error.h>
#include <Fusion/StringUtil.h>

#include <cerrno>
#include <cstring>

namespace Fusion
{
ErrorCode GetLastError()
{
    return static_cast<ErrorCode>(errno);
}

FUSION_PUSH_WARNINGS()
FUSION_DISABLE_GCC_CLANG_WARNING("-Wunused-function")

// The version of strerror_r on GNU compilers returns a 'const char*' rather
// than an intger of the length.
static std::string_view ErrorToString(char* buffer, size_t length, const char* src)
{
    if (src != buffer)
    {
        return StringUtil::Copy(buffer, length, src);
    }
    return { buffer };
}

static std::string_view ErrorToString(char* buffer, size_t length, int size)
{
    FUSION_UNUSED(length);

    return { buffer, size_t(size) };
}

FUSION_POP_WARNINGS()

std::string_view ToString(
    ErrorCode platformCode,
    char* buffer,
    size_t length)
{
    buffer[0] = 0;
    return ErrorToString(buffer, length, strerror_r(platformCode, buffer, length));
}

Error Error::Errno(ErrorCode platformCode)
{
    switch (platformCode)
    {
    case ENOTCONN:
        return E_FAILURE(platformCode);
    case EACCES:
        return E_ACCESS_DENIED(platformCode);
    case EADDRINUSE:
    case EADDRNOTAVAIL:
        return E_RESOURCE_NOT_AVAILABLE(platformCode);
    case EINTR:
        return E_INTERRUPTED(platformCode);
    case EMFILE:
    case ENFILE:
    case ENOBUFS:
    case ENOMEM:
        return E_INSUFFICIENT_RESOURCES(platformCode);
    case EAFNOSUPPORT:
    case EBADF:
    case EDESTADDRREQ:
    case EDOM:
    case EFAULT:
    case EINVAL:
    case EISDIR:
    case ENOTSOCK:
    case EOPNOTSUPP:
    case ENOPROTOOPT:
    case EPROTONOSUPPORT:
    case EPROTOTYPE:
        return E_INVALID_ARGUMENT(platformCode);
    default:
        break;
    }
    return E_FAILURE(platformCode);
}
}  // namespace Fusion

#endif
