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

#include <Fusion/Library.h>

#include <dlfcn.h>
#include <unistd.h>

namespace Fusion
{
void* Library::GetRawEntrypoint(const char* symbol) const
{
    FUSION_ASSERT(IsLoaded());

    return dlsym(m_handle, symbol);
}

Result<void> Library::Load(
    std::string_view path,
    LibraryLoadFlag flags)
{
    FUSION_ASSERT(!IsLoaded());

    int32_t fl = 0;
    fl |= (+(flags & LibraryLoadFlag::Now)) ? RTLD_NOW : RTLD_LAZY;
    fl |= (+(flags & LibraryLoadFlag::Global)) ? RTLD_GLOBAL : RTLD_LOCAL;

#if !FUSION_PLATFORM_APPLE
    if (+(flags & LibraryLoadFlag::Deep))
    {
        fl |= RTLD_DEEPBIND;
    }
#endif

    std::string p(path);

    void* handle = dlopen(p.c_str(), fl);

    if (!handle)
    {
        return Failure::Errno()
            .WithContext("failed to load library '{}'", path);
    }

    m_handle = handle;
    return Success;
}

Result<void> Library::Unload()
{
    if (!IsLoaded())
    {
        return Failure(E_NOT_INITIALIZED);
    }

    dlclose(m_handle);
    m_handle = nullptr;

    return Success;
}
}  // namespace Fusion

#endif
