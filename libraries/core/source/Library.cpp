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

#include <Fusion/Library.h>

#include <Fusion/Platform.h>

#include <utility>

#include <fmt/format.h>

namespace Fusion
{
std::string Library::FormatLibraryName(
    std::string_view name,
    std::string_view suffix)
{
#if FUSION_PLATFORM_WINDOWS
    constexpr const std::string_view prefix{ "" };
    constexpr const std::string_view extension{ "dll" };
#elif FUSION_PLATFORM_LINUX
    constexpr const std::string_view prefix{ "lib" };
    constexpr const std::string_view extension{ "so" };
#elif FUSION_PLATFORM_OSX
    constexpr const std::string_view prefix{ "lib" };
    constexpr const std::string_view extension{ "dylib" };
#endif

    return fmt::format(FMT_STRING("{}{}{}.{}"),
        prefix,
        name,
        suffix,
        extension);
}

Library::Library() = default;

Library::~Library()
{
    Unload();
}

Library::Library(Library&& lib) noexcept
    : m_handle(std::exchange(lib.m_handle, nullptr))
{ }

Library& Library::operator=(Library&& lib) noexcept
{
    if (m_handle != lib.m_handle)
    {
        if (m_handle != nullptr)
        {
            Unload();
        }
        m_handle = std::exchange(lib.m_handle, nullptr);
    }
    return *this;
}

bool Library::IsLoaded() const
{
    return m_handle != nullptr;
}

Library::operator bool() const
{
    return IsLoaded();
}
}  // namespace Fusion
