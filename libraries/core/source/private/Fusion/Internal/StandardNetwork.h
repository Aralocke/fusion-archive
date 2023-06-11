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

#include <Fusion/Internal/Network.h>

namespace Fusion::Internal
{
//
//
//
class StandardNetwork final : public Network
{
public:
    //
    //
    //
    StandardNetwork();

    //
    //
    //
    ~StandardNetwork() override;

    //
    //
    //
    Result<AcceptedSocketData> Accept(Socket sock) const override;

    //
    //
    //
    Result<void> Bind(Socket sock, const SocketAddress& address) const override;

    //
    //
    //
    Result<Socket> CreateSocket(
        AddressFamily family,
        SocketProtocol proto,
        SocketType type) const override;

    //
    //
    //
    Result<void> Connect(
        Socket sock,
        const SocketAddress& address) const override;

    //
    //
    //
    Result<void> Close(Socket sock) const override;

    //
    //
    //
    Result<SocketAddress> GetPeerName(Socket sock) const override;

    //
    //
    //
    Result<SocketAddress> GetSockName(Socket sock) const override;

    //
    //
    //
    Result<void> GetSocketOption(
        Socket sock,
        SocketOpt option,
        void* data,
        size_t size) const override;

    //
    //
    //
    Result<void> Listen(
        Socket sock,
        uint32_t backlog) const override;

    //
    //
    //
    Result<size_t> Recv(
        Socket sock,
        void* buffer,
        size_t length,
        MessageOption flags) const override;

    //
    //
    //
    Result<RecvFromData> RecvFrom(
        Socket sock,
        void* buffer,
        size_t length,
        MessageOption flags) const override;

    //
    //
    //
    Result<size_t> Send(
        Socket sock,
        const void* buffer,
        size_t length,
        MessageOption flags) const override;

    //
    //
    //
    Result<size_t> SendTo(
        Socket sock,
        const SocketAddress& address,
        const void* buffer,
        size_t length,
        MessageOption flags) const override;

    //
    //
    //
    Result<void> SetBlocking(
        Socket sock,
        bool blocking) const override;

    //
    //
    //
    Result<void> SetSocketOption(
        Socket sock,
        SocketOpt option,
        const void* data,
        size_t size) const override;

    //
    //
    //
    Result<void> Shutdown(
        Socket sock,
        SocketShutdownMode mode) const override;

public:
    //
    //
    //
    Result<void> Start() override;

    //
    //
    //
    void Stop() override;

    //
    //
    //
    void Stop(std::function<void(Failure&)> fn) override;
};

}  // namespace fusion
