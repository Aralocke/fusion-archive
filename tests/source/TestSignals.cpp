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

#include <Fusion/Tests/Tests.h>

#include <Fusion/Memory.h>
#include <Fusion/Signals.h>

TEST(SignalTests, RaiseSignal)
{
    using namespace Fusion;

    FUSION_SCOPE_GUARD([] { ClearSignalHandlers(); });

    static bool s_flagged = false;
    static uint8_t s_signals = 0;

    RegisterSignal(
        Signal::HANGUP,
        [&](Signal) -> bool
        {
            ++s_signals;
            return true;
        });

    RegisterSignal(
        Signal::TERMINATE,
        [&](Signal) -> bool
        {
            ++s_signals;
            s_flagged = true;
            return true;
        });

    RaiseSignal(Signal::HANGUP);
    ASSERT_EQ(s_signals, 1);

    RaiseSignal(Signal::TERMINATE);
    ASSERT_EQ(s_signals, 2);
    ASSERT_TRUE(s_flagged);

    ClearSignalHandlers(Signal::HANGUP);
    RaiseSignal(Signal::HANGUP);
    ASSERT_EQ(s_signals, 2);
}

TEST(SignalTests, RaiseMultipleSignals)
{
    using namespace Fusion;

    FUSION_SCOPE_GUARD([] { ClearSignalHandlers(); });

    static uint8_t s_signals = 0;

    RegisterSignal(
        { Signal::HANGUP, Signal::TERMINATE },
        [&](Signal) -> bool
        {
            ++s_signals;
            return true;
        });

    RaiseSignal(Signal::HANGUP);
    RaiseSignal(Signal::TERMINATE);
    ASSERT_EQ(s_signals, 2);

    ClearSignalHandlers(Signal::HANGUP);
    RaiseSignal(Signal::TERMINATE);
    ASSERT_EQ(s_signals, 3);
}
