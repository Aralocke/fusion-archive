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

#if !defined(FUSION_IMPL_FORMATTER)
#error "Formatter impl included before main header"
#endif

namespace Fusion
{
template<typename ...Args>
Result<std::string> Formatter::Format(
    fmt::format_string<Args...> str,
    Args&& ...args)
{
    try
    {
        std::string message = fmt::vformat(
            str,
            fmt::make_format_args(std::forward<Args>(args)...));

        return std::move(message);
    }
    catch (const std::exception& ex)
    {
        return Failure(ex.what());
    }
    catch (...)
    {
        return Failure("Unable to format string");
    }
}
}  // namespace Fusion
