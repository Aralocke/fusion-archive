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

#include <Fusion/Internal/Http.h>

#include <Fusion/Fwd/Network.h>

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Fusion::Internal
{
//
//
//
class StandardHttpClient final : public HttpClient
{
public:
    //
    //
    //
    StandardHttpClient(Params&& params);

    //
    //
    //
    ~StandardHttpClient() override;

    //
    //
    //
    void CancelRequest(uint64_t id) override;

    //
    //
    //
    void PauseRequest(uint64_t id) override;

    //
    //
    //
    void ResumeRequest(uint64_t id) override;

public:
    //
    //
    //
    Result<Request> Execute(RequestOptions&& options) override;

public:
    //
    //
    //
    Result<void> Start() override;

    //
    //
    //
    void Stop(std::function<void()> fn) override;

public:
    struct RequestContext;
    struct RequestData;

private:
    //
    //
    //
    struct RequestAction
    {
        enum class Type
        {
            None = 0,
            Cancel,
            Pause,
            Resume,
            Send,
        };

        Type type{ Type::None };
        uint64_t id{ UINT64_MAX };
        std::promise<Result<void>> notify;
    };

    //
    //
    //
    void Enqueue(
        std::unique_lock<std::mutex>& lock,
        RequestAction&& action);

private:
    //
    //
    //
    void HandleCancelAction(uint64_t id);

    //
    //
    //
    void HandlePauseAction(uint64_t id);

    //
    //
    //
    void HandleResumeAction(uint64_t id);

    //
    //
    //
    void HandleSendAction(uint64_t id);

private:
    //
    //
    //
    bool RunOnce();

private:
    //
    //
    //
    void CreateRequestContext(RequestData& data);

    //
    //
    //
    void FinalizeRequest(RequestContext& context);

    //
    //
    //
    void PerformTransfers();

    //
    //
    //
    bool PollNetwork();

    //
    //
    //
    void ProcessQueue();

    //
    //
    //
    void UpdatePendingRequests();

    //
    //
    //
    void UpdateTransfers();

private:
    Params m_params;
    std::vector<Certificate> m_systemRoots;

    std::queue<RequestAction> m_queue;
    std::condition_variable m_cond;
    std::mutex m_mutex;
    std::thread m_thread;

    // Data Tracking
    std::unordered_map<uint64_t, RequestData> m_requestData;
    std::unordered_map<uint64_t, RequestContext> m_requestContexts;
    std::set<uint64_t> m_pendingRequests;

    // State tracking
    uint64_t m_nextId{ 1 };
    std::atomic<bool> m_shutdown{ false };
    std::atomic<bool> m_shutdownComplete{ false };

public:
    struct State;

private:
    std::unique_ptr<State> m_state;
};
}
