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

#include <Fusion/Internal/IocpSocketService.h>

#if FUSION_PLATFORM_WINDOWS
namespace Fusion::Internal
{
IocpSocketService::IocpSocketService(Network& network)
    : m_network(network)
    , m_pipe(network)
{ }

IocpSocketService::~IocpSocketService()
{
}

Result<void> IocpSocketService::Add(
    Socket sock,
    SocketOperation operation)
{
    return Failure{ E_NOT_IMPLEMENTED };
}

Result<void> IocpSocketService::Close(Socket sock)
{
    return Failure{ E_NOT_IMPLEMENTED };
}

void IocpSocketService::Notify()
{}

Result<std::span<SocketEvent>>
IocpSocketService::Execute(Clock::duration timeout)
{
    return Failure{ E_NOT_IMPLEMENTED };
}

Result<void> IocpSocketService::Remove(
    Socket sock,
    SocketOperation events)
{
    return Failure{ E_NOT_IMPLEMENTED };
}

Result<void> IocpSocketService::Start()
{
    return Failure{ E_NOT_IMPLEMENTED };
}

void IocpSocketService::Stop()
{
    Stop(nullptr);
}

void IocpSocketService::Stop(std::function<void(Failure&)> fn)
{
    if (fn)
    {
        Failure f(E_SUCCESS);

        fn(f);
    }
}
}  // namespace Fusion::Internal
#endif  // FUSION_PLATFORM_WINDOWS
