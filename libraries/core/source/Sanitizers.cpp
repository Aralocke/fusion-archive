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

#include <Fusion/Sanitizers.h>

#include <Fusion/Compiler.h>
#include <Fusion/Environment.h>
#include <Fusion/Macros.h>

namespace Fusion
{
#if FUSION_ADDRESS_SANITIZER_ENABLED
static void ConfigureAddressSanitizer(std::string_view program)
{
    using namespace std::string_view_literals;

    std::string asan = Environment::Get("ASAN_OPTIONS");

    if (!asan.empty())
    {
        return;
    }

    // Set the default options we want to specify for the address sanitizer.
    // For now 'detect_stack_use_after_return' is disabled by compile flag on Windows because
    // it causes issues with 'std::locale' crashes with MSVC's STL.

    Environment::Set("ASAN_OPTIONS",
        "halt_on_error=1" ":"
        "print_legend=1" ":"
        "detect_stack_use_after_return=1"
    );

    // Determine the program name and set the variable to write out a dump
    // file if a crash occurs.

    std::string prog(program);
    prog += ".asan.dmp"sv;

    Environment::Set("ASAN_SAVE_DUMPS", std::move(prog));
}
#endif

#if FUSION_THREAD_SANITIZER_ENABLED
static void ConfigureThreadSanitizer(std::string_view program)
{
    FUSION_UNUSED(program);
}
#endif

#if FUSION_UNDEFINED_SANITIZER_ENABLED
static void ConfigureUndefinedSanitizer(std::string_view program)
{
    FUSION_UNUSED(program);
}
#endif

void ConfigureSanitizerEnvironment(std::string_view program)
{
#if FUSION_ADDRESS_SANITIZER_ENABLED
    ConfigureAddressSanitizer(program);
#endif
#if FUSION_THREAD_SANITIZER_ENABLED
    ConfigureThreadSanitizer(program);
#endif
#if FUSION_UNDEFINED_SANITIZER_ENABLED
    ConfigureUndefinedSanitizer(program);
#endif
}
}
