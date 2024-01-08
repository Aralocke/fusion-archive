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
#if FUSION_PLATFORM_LINUX

#include <Fusion/Debug.h>
#include <Fusion/Memory.h>
#include <Fusion/StringUtil.h>

#include <array>

#include <fcntl.h>

namespace Fusion
{
bool Debug::IsDebuggerPresent()
{
    // This implementation came from: https://stackoverflow.com/a/24969863

    using namespace std::string_view_literals;

    std::array<char, 4096> buffer = { 0 };

    int fd = open("/proc/self/status", O_RDONLY);
    if (fd == -1)
    {
        return false;
    }

    ssize_t size = read(fd, buffer.data(), buffer.size() - 1);
    FUSION_SCOPE_GUARD([&] { close(fd); });

    if (size <= 0)
    {
        return false;
    }

    buffer[size] = 0;
    std::vector<std::string_view> lines = StringUtil::SplitViews(
        std::string_view{ buffer.data(), size_t(size) },
        "\n"sv);

    for (std::string_view line : lines)
    {
        if (line.starts_with("TracerPid:"sv))
        {
            return std::strtoul(line.data() + 11, nullptr, 10) != 0;
        }
    }

    return false;
}
}  // namespace Fusion

#endif
