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

#include <Fusion/Internal/Signals.h>

#include <csignal>

namespace Fusion
{
Signal SignalManager::MapEvent(int32_t signum)
{
    switch (signum)
    {
    case SIGHUP:
        return Signal::HANGUP;
    case SIGINT:
        return Signal::INTERRUPT;
    case SIGQUIT:
        return Signal::QUIT;
    case SIGKILL:
        return Signal::KILL;
    case SIGTERM:
        return Signal::TERMINATE;
    case SIGUSR1:
        return Signal::USR1;
    case SIGUSR2:
        return Signal::USR2;
    case SIGCHLD:
        return Signal::CHILD;
    case SIGTTIN:
    case SIGSTOP:
        return Signal::BREAK;
    case SIGABRT:
        return Signal::ABORT;
    }
    return Signal::NONE;
}

void SignalManager::AddSignalToSet(int32_t signum)
{
    // Expects to be called from within the class mutex

    ::signal(signum, SignalManager::Receiver);
}
}  // namespace Fusion
#endif
