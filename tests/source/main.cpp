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

#include <Fusion/Tests/Main.h>

#include <Fusion/Sanitizers.h>

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    using namespace Fusion;

    // This should be at the top most of main() before anything else jumps in.
    // We want to configure the sanitizers as soon as possible.
    ConfigureSanitizerEnvironment("FusionTests");

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
