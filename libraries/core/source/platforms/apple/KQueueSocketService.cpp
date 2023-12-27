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

#include <Fusion/Internal/KQueueSocketService.h>

#if FUSION_PLATFORM_APPLE
namespace Fusion::Internal
{
KQueueSocketService::KQueueSocketService(Network& network)
    : SocketService(Type::Kqueue, Operation::Polling)
    , m_network(network)
    , m_pipe(network)
{ }

KQueueSocketService::~KQueueSocketService()
{
}

Result<void> KQueueSocketService::Add(
    Socket sock,
    SocketOperation operation)
{
    return Failure{ E_NOT_IMPLEMENTED };
}

Result<void> KQueueSocketService::Close(Socket sock)
{
    return Failure{ E_NOT_IMPLEMENTED };
}

void KQueueSocketService::Notify()
{}

Result<std::span<SocketEvent>>
KQueueSocketService::Execute(Clock::duration timeout)
{
    FUSION_UNUSED(m_network);

    return Failure{ E_NOT_IMPLEMENTED };
}

Result<void> KQueueSocketService::Remove(
    Socket sock,
    SocketOperation events)
{
    return Failure{ E_NOT_IMPLEMENTED };
}

Result<void> KQueueSocketService::Start()
{
    return Failure{ E_NOT_IMPLEMENTED };
}

std::future<Result<void>> KQueueSocketService::Stop()
{
    std::promise<Result<void>> promise;
    std::future<Result<void>> future = promise.get_future();

    promise.set_value(Success);
    return future;
}
}  // namespace Fusion::Internal
#endif  // FUSION_PLATFORM_APPLE
