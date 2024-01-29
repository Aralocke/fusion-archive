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

#pragma once

#if !defined(FUSION_IMPL_PATHUTIL)
#error "PathUtil impl included before main header"
#endif

namespace Fusion
{
template<typename ...Args>
std::string PathUtil::Join(
    std::string_view a,
    std::string_view b,
    std::string_view c,
    Args&& ...args)
{
    std::string path;
    path.reserve(3 +
        + a.size()
        + b.size()
        + c.size());

    std::ignore = Join(path, a);
    std::ignore = Join(path, b);
    std::ignore = Join(path, c);

    (Join(path, std::forward<Args>(args)), ...);
    return path;
}
}
