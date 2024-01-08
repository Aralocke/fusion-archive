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

#include <Fusion/Internal/Thread.h>

#include <Fusion/Compiler.h>
#include <Fusion/Debug.h>
#include <Fusion/Unicode.h>

namespace Fusion
{
Thread::Id Thread::CurrentThreadId()
{
    return Id(GetCurrentThreadId());
}

void SetThreadNameInternal(const char* name)
{
    // We were unable to set the thread name with the preferred APIs
    // We fall back to the exception method.
    //
    // https://learn.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2022

    if (!Debug::IsDebuggerPresent())
    {
        // This method does nothing if a debugger is not attached.
        return;
    }

    FUSION_PUSH_WARNINGS();
    FUSION_DISABLE_MSVC_WARNING(4611 6320 6322);

#pragma pack(push,8)
    struct ThreadNameInfo
    {
        DWORD dwType;     // Must be 0x1000.
        LPCSTR szName;    // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags;    // Reserved for future use, must be zero.
    };
#pragma pack(pop)

    jmp_buf env;
    if (setjmp(env) == 0)
    {
        __try
        {
            ThreadNameInfo info = {
                .dwType = 0x1000,
                .szName = name,
                .dwThreadID = DWORD(-1),
                .dwFlags = 0,
            };

            RaiseException(
                0x406D1388,
                0,
                sizeof(info) / sizeof(ULONG_PTR),
                PULONG_PTR(&info));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }

        longjmp(env, 1);
    }

    FUSION_POP_WARNINGS();
}

void Internal::SetThreadName(std::string_view name)
{
    if (name.empty())
    {
        return;
    }

    if (HMODULE win32 = GetModuleHandleW(L"kernel32.dll"))
    {
        using SetThreadDescriptionFn = HRESULT(WINAPI*)(
            HANDLE hThread,
            PCWSTR lpThreadDescription
        );

        auto SetThreadDescriptionFunc = SetThreadDescriptionFn(
            GetProcAddress(win32, "SetThreadDescription"));

        if (SetThreadDescriptionFunc
            && SetThreadDescriptionFunc(
                GetCurrentThread(),
                Unicode::Utf8ToWide(name).c_str()) == S_OK)
        {
            // Successfully set thread name
            return;
        }
    }

    // Split the exception approach into a separate function to support
    // Structured Exception Handling which doe snot allow C++ objects
    // with destructors.

    std::string str(name);
    SetThreadNameInternal(str.c_str());
}
}  // namespace Fusion

#endif  // FUSION_PLATFORM_WINDOWS
