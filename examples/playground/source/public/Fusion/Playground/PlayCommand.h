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

#include <Fusion/Result.h>

namespace Fusion
{
class ArgumentCommand;

class PlayCommand final
{
public:
    struct Options
    {};

public:
    static Result<void> Run(Options options);
    static void Setup(ArgumentCommand& cmd, Options& options);

public:
    PlayCommand(Options options);
    ~PlayCommand();

    Result<void> Run();

private:
    Options m_options;
};
}  // namespace
