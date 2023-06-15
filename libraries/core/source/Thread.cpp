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

#include <Fusion/Thread.h>

#include <Fusion/Macros.h>

#include <algorithm>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace Fusion
{
// -------------------------------------------------------------
// ThreadPool                                              START
struct ThreadPool::Task
{
    uint64_t id{ UINT64_MAX };
    TaskFn fn;
    CompletionHandler completer;

    explicit operator bool() const { return id != UINT64_MAX; }
};

struct ThreadPool::Thread
{
    std::thread thread;
    std::string_view threadName;
    Queue* queue{ nullptr };
};

class ThreadPool::Queue final
{
public:
    using TaskMap = std::multimap<Priority, Task, std::greater<>>;
    using TaskIdMap = std::unordered_map<uint64_t, TaskMap::iterator>;

public:
    uint64_t nextTaskId{ 1 };
    TaskMap tasks;
    TaskIdMap idMap;

    std::condition_variable cond;
    std::mutex mutex;
    bool running{ true };

public:
    Task PopFront()
    {
        FUSION_ASSERT(!tasks.empty());

        auto node = tasks.extract(begin(tasks));
        idMap.erase(node.mapped().id);

        return std::move(node.mapped());
    }
};

ThreadPool::ThreadPool(Options options)
    : m_options(std::move(options))
    , m_queue(std::make_unique<Queue>())
{
    size_t threadCount = (m_options.threads)
        ? std::clamp<size_t>(
            std::thread::hardware_concurrency(),
            1,
            m_options.maxThreads)
        : m_options.threads;

    m_threads.reserve(threadCount);

    for (size_t i = 0; i < threadCount; ++i)
    {
        Thread* thread = &m_threads.emplace_back(Thread{
            .threadName = m_options.threadName,
            .queue = m_queue.get(),
        });

        thread->thread = std::thread([
                thread,
                pool{ this }]()
            {
                RunThread(pool, *thread);
            });
    }
}

ThreadPool::~ThreadPool()
{
    Queue::TaskMap tasks;

    // Shutdown the queue

    {
        std::lock_guard lock(m_queue->mutex);

        m_queue->running = false;
        m_queue->tasks.swap(tasks);
        m_queue->idMap.clear();
    }

    m_queue->cond.notify_all();

    Failure reason(E_CANCELLED);

    for (auto& [priority, task] : tasks)
    {
        task.completer(nullptr, &reason);
    }

    // Join the active threads

    for (Thread& thread : m_threads)
    {
        thread.thread.join();
    }

    // Fail any tasks that might have come in while we were executing above.

    for (auto& [priority, task] : m_queue->tasks)
    {
        task.completer(nullptr, &reason);
    }
}

void ThreadPool::Cancel(uint64_t id)
{
    std::lock_guard lock(m_queue->mutex);

    auto node = m_queue->idMap.extract(id);

    if (!node)
    {
        return;
    }

    Task& task = node.mapped()->second;

    Failure reason(E_CANCELLED);
    task.completer(nullptr, &reason);

    m_queue->tasks.erase(node.mapped());
}

bool ThreadPool::IsRunningInThisThread() const
{
    return Callstack<ThreadPool>::Contains(this);
}

std::optional<ThreadPool::Task> ThreadPool::DequeueTask(
    Queue& queue)
{
    std::unique_lock lock(queue.mutex);

    queue.cond.wait(lock, [&]() {
        return !queue.tasks.empty() || !queue.running;
    });

    if (!queue.running)
    {
        return std::nullopt;
    }

    return queue.PopFront();
}

void ThreadPool::RunThread(
    ThreadPool* pool,
    Thread& thread)
{
    FUSION_CALLSTACK_ENTRY(ThreadPool, pool);

    Queue* queue = thread.queue;

    if (!thread.threadName.empty())
    {
        // ThreadUtil::SetName(thread.threadName);
    }

    while (auto task = DequeueTask(*queue))
    {
        task->fn(task->completer);
    }
}

uint64_t ThreadPool::SubmitTask(
    CompletionHandler& completer,
    TaskFn fn,
    Priority priority)
{
    std::unique_lock lock(m_queue->mutex);

    uint64_t id = m_queue->nextTaskId++;
    auto it = m_queue->tasks.emplace(priority, Task{
        .id = id,
        .fn = std::move(fn),
        .completer = std::move(completer),
    });

    m_queue->idMap.emplace(id, it);
    lock.unlock();

    m_queue->cond.notify_one();

    return id;
}
// ThreadPool                                                END
// -------------------------------------------------------------
// ThreadPool::Strand                                      START
struct ThreadPool::Strand::State
{
    ThreadPool& pool;
    ThreadPool::Queue queue;

    State(ThreadPool& p) : pool(p) { }
};

ThreadPool::Strand::Strand(ThreadPool& pool)
    : m_state(std::make_shared<State>(pool))
{ }

ThreadPool::Strand::Strand(std::shared_ptr<State> state)
    : m_state(std::move(state))
{ }

ThreadPool::Strand::~Strand()
{ }

Strand ThreadPool::Strand::Duplicate() const
{
    return Strand{ m_state };
}

bool ThreadPool::Strand::IsRunningInThisThread() const
{
    return Callstack<Strand>::Contains(this);
}

void ThreadPool::Strand::Run()
{
    FUSION_CALLSTACK_ENTRY(Strand, this);

    ThreadPool::Queue& queue = m_state->queue;

    while (true)
    {
        ThreadPool::Task task;

        std::unique_lock lock(queue.mutex);

        FUSION_ASSERT(queue.running);
        if (!queue.tasks.empty())
        {
            task = queue.PopFront();
        }
        else
        {
            queue.running = false;
        }

        lock.unlock();

        if (task)
        {
            task.fn(task.completer);
            continue;
        }

        return;
    }  
}
// ThreadPool::Strand                                        END
// -------------------------------------------------------------
}  // namespace Fusion
