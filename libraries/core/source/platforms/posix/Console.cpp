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

#include <Fusion/Console.h>

#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace Fusion
{
int32_t GetTargetHandle(Console::Target target)
{
    switch (target)
    {
    case Console::Target::Stdin:
        return STDIN_FILENO;
    case Console::Target::Stdout:
        return STDOUT_FILENO;
    case Console::Target::Stderr:
        return STDERR_FILENO;
    default:
        return -1;
    }
}

bool Console::IsInteractive(Console::Target target)
{
    return isatty(GetTargetHandle(target)) != 0;
}

Console::Features Console::GetFeatures(Console::Target target)
{
    Features features;
    features.interactive = IsInteractive(target);
    features.bold = true;
    features.colors = true;

    int32_t fd = GetTargetHandle(target);
    if (!features.interactive)
    {
        features.colors = false;
        features.bold = false;
    }

    struct winsize size = {};
    if (ioctl(fd, TIOCGWINSZ, &size) != -1 && size.ws_col != 0)
    {
        features.width = size.ws_col;
    }

    struct termios echo = {};
    if (tcgetattr(STDIN_FILENO, &echo) == 0)
    {
        features.echo = (echo.c_lflag & ECHO);
    }

    return features;
}

void Console::SetConsoleEcho(bool value)
{
    struct termios echo = {};
    if (tcgetattr(STDIN_FILENO, &echo) != 0)
    {
        return;
    }

    if (value)
    {
        echo.c_lflag |= ECHO;
    }
    else
    {
        echo.c_lflag &= ~ECHO;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &echo);
}
}  // namespace Fusion

#endif  // FUSION_PLATFORM_POSIX
