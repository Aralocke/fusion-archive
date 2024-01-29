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
#if FUSION_PLATFORM_APPLE

#include <Fusion/PathUtil.h>

#include <array>

#include <mach-o/dyld.h>

namespace Fusion
{
std::string PathUtil::GetExecutablePath()
{
    uint32_t length = 0;
    _NSGetExecutablePath(nullptr, &length);

    if (length == 0)
    {
        return {};
    }

    std::string path;
    path.resize(length);

    if (_NSGetExecutablePath(path.data(), &length) == 0)
    {
        return {};
    }

    return Dirname(path);
}
}  // namespace Fusion

#endif
