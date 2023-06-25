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

#include <Fusion/Fwd/Argparse.h>

#include <Fusion/Platform.h>
#include <Fusion/Result.h>

namespace Fusion
{
struct GlobalOptions
{};

class Global final
{
public:
    Global(const Global&) = delete;
    Global& operator=(const Global&) = delete;

public:
    static void SetupArgumentParser(
        ArgumentCommand& cmd,
        GlobalOptions& options);

    static void SetupArgumentParser(
        ArgumentParser& parser,
        GlobalOptions& options);

public:
    struct Params
    {
        bool initializeCurl{ false };
        bool initializeOpenssl{ false };
    };

public:
    Global(Params params);
    ~Global();

    Result<void> Start(GlobalOptions& options);

private:
    Result<void> InitializeCurl();
    Result<void> InitializeOpenSSL();

private:
    Params m_params;
};
}  // namespace Fusion
