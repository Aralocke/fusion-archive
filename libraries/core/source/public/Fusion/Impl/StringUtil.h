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

#if !defined(FUSION_IMPL_STRINGUTILS)
#error "StringUtils impl included before main header"
#endif

#include <Fusion/Macros.h>

namespace Fusion
{
template<typename T>
Result<T> StringUtil::ParseNumber(
    std::string_view in,
    int32_t base)
{
    static constexpr T min = std::numeric_limits<T>::lowest();
    static constexpr T max = std::numeric_limits<T>::max();

    if constexpr (std::is_floating_point_v<T>)
    {
        FUSION_UNUSED(base);
        if (auto result = StringUtil::ToFloatingPoint(in, min, max))
        {
            return static_cast<T>(*result);
        }
        else
        {
            return result.Error();
        }
    }
    else if constexpr (std::is_unsigned_v<T>)
    {
        if (auto result = StringUtil::ToUnsignedNumber(in, base, max))
        {
            return static_cast<T>(*result);
        }
        else
        {
            return result.Error();
        }
    }
    else
    {
        if (auto result = StringUtil::ToSignedNumber(in, base, min, max))
        {
            return static_cast<T>(*result);
        }
        else
        {
            return result.Error();
        }
    }
}
}  // namespace Fusion
