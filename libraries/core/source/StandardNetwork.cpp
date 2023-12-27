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

#include <Fusion/Internal/StandardNetwork.h>

namespace Fusion::Internal
{
StandardNetwork::StandardNetwork() = default;

StandardNetwork::~StandardNetwork() = default;

Result<void> StandardNetwork::Start()
{
    return Success;
}

std::future<Result<void>> StandardNetwork::Stop()
{
    std::promise<Result<void>> promise;
    std::future<Result<void>> future = promise.get_future();

    promise.set_value(Success);
    return future;
}
}
