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

#include <Fusion/Tests/Tests.h>

#include <Fusion/Internal/Network.h>

#include <thread>

namespace Fusion
{
class SocketServiceTests : public ::testing::Test
{
public:
    struct Connection
    {
        Socket sock;
        std::vector<char> read;
        size_t readOffset = 0;
        std::vector<char> write;
        size_t writeOffset = 0;
        SocketOperation ops{ SocketOperation::None };
    };

    static void Consume(Connection& conn, std::string& data);

    static void Consume(Connection& conn, std::string& data, size_t size);

    struct Manager
    {
        std::vector<Connection> conns;

        Network* network{ nullptr };
        SocketService* service{ nullptr };

        Manager(Network& n, SocketService& s)
            : network(&n)
            , service(&s)
        { }
    };

    static void ProcessRead(Manager& m, Connection& c);
    static void ProcessWrite(Manager& m, Connection& c);
    static void QueueRead(
        Manager& m,
        Connection& c,
        size_t size);
    static void QueueWrite(
        Manager& m,
        Connection& c,
        std::span<char> data);

public:
    std::unique_ptr<Network> network;
    std::unique_ptr<SocketPair> pair;
    std::unique_ptr<SocketService> service;

public:
    struct NetThread
    {
        std::condition_variable cond;
        std::mutex mutex;
        std::thread thread;
        bool running{ false };
    };

    NetThread netThread;

public:

public:
    void TearDown() override;
    void SetUp() override;
};
}  // namespace Fusion
