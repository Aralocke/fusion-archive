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

#include <Fusion/Fixtures/SocketService.h>

TEST_F(SocketServiceTests, SelectStartupSHutdown)
{
    FUSION_ASSERT_RESULT(
        SocketService::Create(
            SocketService::Type::Select,
            *network),
        [&](std::unique_ptr<SocketService> s) {
            service = std::move(s);
        });

    Manager manager(*network, *service);
    {
        manager.conns.push_back({
            .sock = pair->Reader(),
        });
        manager.conns.push_back({
            .sock = pair->Writer(),
        });
    }

    std::string message = "write this first";
    std::span<SocketEvent> events;

    Connection& one = manager.conns[0];
    Connection& two = manager.conns[1];

    QueueRead(manager, one, 128);
    QueueWrite(manager, two, message);

    // Process the connections.
    // Only connection 'two' has data ready to write so it should immeadietly
    // come up for action.

    FUSION_ASSERT_RESULT(
        service->Execute(std::chrono::milliseconds(100)),
        [&](auto ev) {
            events = std::move(ev);
        });

    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events[0].sock, two.sock);
    ASSERT_TRUE(+(events[0].events, SocketOperation::Write));
    ProcessWrite(manager, two);

    // At this point the service should trigger the read operation on
    // connection 'one'.
    FUSION_ASSERT_RESULT(
        service->Execute(std::chrono::milliseconds(100)),
        [&](auto ev) {
            events = std::move(ev);
        });

    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events[0].sock, one.sock);
    ASSERT_TRUE(+(events[0].events, SocketOperation::Read));
    ProcessRead(manager, one);
    {
        std::string data;
        Consume(one, data, message.size());
        ASSERT_EQ(data, message);
    }
}
