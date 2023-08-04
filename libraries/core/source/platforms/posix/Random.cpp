#include <Fusion/Platform.h>
#if FUSION_PLATFORM_POSIX

#include <Fusion/Internal/Random.h>

#include <Fusion/Memory.h>

#include <fcntl.h>
#include <unistd.h>

namespace Fusion::Internal
{
bool SystemSecureBytes(
    uint8_t* buffer,
    size_t size)
{
    if (!buffer || size == 0)
    {
        return false;
    }

    int32_t handle = open("/dev/urandom", O_RDONLY, mode_t(0));
    FUSION_SCOPE_GUARD([&]{ if (handle != -1) close(handle); });

    if (handle == -1)
    {
        return false;
    }

    size_t chunkSize = std::min<size_t>(
        size,
        1 * 1024 * 1024);  // 1MB

    while (size > chunkSize)
    {
        int32_t result = read(handle, buffer, chunkSize);

        if (size_t(result) != chunkSize)
        {
            return false;
        }

        buffer += chunkSize;
        size -= chunkSize;
    }

    if (size > 0)
    {
        int32_t result = read(handle, buffer, chunkSize);

        return size_t(result) == size;
    }

    return true;
}
}

#endif
