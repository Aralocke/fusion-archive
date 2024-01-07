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

#include <Fusion/Assert.h>

#if FUSION_ASSERTIONS_ENABLED

#include <iostream>

namespace Fusion
{
void AssertHandler(
    const wchar_t* message,
    const wchar_t* condition,
    const wchar_t* filename,
    unsigned lineno)
{
#if !defined(NDEBUG)
    std::wcerr << L"Assertion '" << condition << L"' failed in '" << filename
        << L":" << lineno << L"': " << message << std::endl;
#else
    (void)filename;
    (void)lineno;

    std::wcerr << L"Assertion '" << condition << L"' failed: " << message << std::endl;
#endif

    std::terminate();
}
}  // namespace Fusion
#endif  // FUSION_ASSERTIONS_ENABLED
