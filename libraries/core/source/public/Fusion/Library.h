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

#include <Fusion/Enum.h>
#include <Fusion/Result.h>
#include <Fusion/TypeTraits.h>

namespace Fusion
{
//
//
//
enum class LibraryLoadFlag : uint8_t
{
    Default = 0,
    System = (1 << 1),
    Lazy = (1 << 2),
    Now = (1 << 3),
    Global = (1 << 4),
    Local = (1 << 5),
    Deep = (1 << 6)
};
FUSION_ENUM_OPS(LibraryLoadFlag);

//
//
//
class Library final
{
public:
    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;

public:
    //
    //
    //
    static std::string FormatLibraryName(
        std::string_view name,
        std::string_view suffix = {});

    //
    //
    //
    Library();

    //
    //
    //
    ~Library();

    //
    //
    //
    Library(Library&& lib) noexcept;

    //
    //
    //
    Library& operator=(Library&& lib) noexcept;

    //
    //
    //
    void* GetRawEntrypoint(const char* symbol) const;

    //
    //
    //
    template<typename T, FUSION_REQUIRES(std::is_pointer_v<T>)>
    T GetEntrypoint(const char* symbol) const
    {
        return reinterpret_cast<T>(GetRawEntrypoint(symbol));
    }

    //
    //
    //
    bool IsLoaded() const;

    operator bool() const;

    //
    //
    //
    Result<void> Load(
        std::string_view path,
        LibraryLoadFlag flags = LibraryLoadFlag::Default);

    //
    //
    //
    Result<void> Unload();

private:
    void* m_handle{ nullptr };
};
}  // namespace Fusion
