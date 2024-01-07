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

#include <string>

namespace Fusion
{
//
//
//
class Unicode final
{
public:
    //
    //
    //
    static std::wstring Utf8ToWide(std::string_view str);

    //
    //
    //
    static std::string WideToUtf8(std::wstring_view wstr);
};
}  // namespace Fusion
