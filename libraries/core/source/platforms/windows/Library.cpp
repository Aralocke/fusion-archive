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

#include <Fusion/Windows.h>
#if FUSION_PLATFORM_WINDOWS

#include <Fusion/Library.h>
#include <Fusion/Unicode.h>

namespace Fusion
{
void* Library::GetRawEntrypoint(const char* symbol) const
{
    FUSION_ASSERT(IsLoaded());

    HMODULE hModule = reinterpret_cast<HMODULE>(m_handle);
    return GetProcAddress(hModule, symbol);
}

Result<void> Library::Load(
    std::string_view path,
    LibraryLoadFlag flags)
{
    FUSION_ASSERT(!IsLoaded());

    int32_t f = 0;
    if (+(flags & LibraryLoadFlag::System))
    {
        f |= LOAD_LIBRARY_SEARCH_SYSTEM32;
    }

    std::wstring wpath = Unicode::Utf8ToWide(path);
    HMODULE hModule = LoadLibraryExW(wpath.c_str(), NULL, f);

    if (!hModule)
    {
        return Failure::Errno()
            .WithContext("failed to load library '{}'", path);
    }

    m_handle = hModule;
    return Success;
}

Result<void> Library::Unload()
{
    if (!IsLoaded())
    {
        return Failure(E_NOT_INITIALIZED);
    }

    HMODULE hModule = reinterpret_cast<HMODULE>(m_handle);

    if (hModule)
    {
        FreeLibrary(hModule);
    }

    m_handle = nullptr;
    return Success;
}
}  // namespace Fusion

#endif
