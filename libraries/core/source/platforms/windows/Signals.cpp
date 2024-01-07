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
#if FUSION_PLATFORM_WINDOWS

#include <Fusion/Internal/Signals.h>
#include <Fusion/Error.h>
#include <Fusion/Macros.h>
#include <fusion/Windows.h>

namespace Fusion
{
Signal SignalManager::MapEvent(int32_t signum)
{
    switch (signum)
    {
    case CTRL_BREAK_EVENT:
        return Signal::HANGUP;
    case CTRL_C_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    case CTRL_CLOSE_EVENT:
        return Signal::INTERRUPT;
    default:
        return Signal::TERMINATE;
    }
}

static BOOL WINAPI CtrlHandler(DWORD dwCtrltype)
{
    auto signum = static_cast<int32_t>(dwCtrltype);

    RaiseSignal(SignalManager::MapEvent(signum));

    return TRUE;
}

void SignalManager::AddSignalToSet(int32_t signum)
{
    // Expects to be called from within the class mutex

    if (m_consoleInstalled)
    {
        return;
    }

    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        const Error err = Error::Errno();
        FUSION_UNUSED(err);

        return;
    }

    m_consoleInstalled = true;
}
}  // namespace Fusion
#endif
