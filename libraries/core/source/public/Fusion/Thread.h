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

#pragma once

#include <Fusion/Fwd/Thread.h>

#include <Fusion/Macros.h>
#include <Fusion/Result.h>

#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace Fusion
{
// Type-safe, thread-local storage for key/value pairs.
//
// Internal linked-list allows for storing data on the thread-local storage
// for retrieval later.
//
// Based on: https://www.crazygaze.com/blog/2016/03/11/callstack-markers-boostasiodetailcall_stack/
template<typename Key, typename Value = uint8_t>
class Callstack final
{
public:
    class Iterator;

public:
    //
    //
    //
    class Entry final
    {
    public:
        Entry(const Entry&) = delete;
        Entry& operator=(const Entry&) = delete;

    public:
        explicit Entry(Key* key);
        Entry(Key* key, Value& value);
        ~Entry();

    private:
        friend class Callstack<Key, Value>;
        friend class Callstack<Key, Value>::Iterator;

    public:
        Key* key{ nullptr };
        Value* value{ nullptr };
        Entry* next{ nullptr };
    };

public:
    //
    //
    //
    class Iterator final
    {
    public:
        explicit Iterator(Entry* entry);
        ~Iterator() = default;

        Iterator& operator++();
        bool operator !=(const Iterator& it) const;

        const Entry* operator*() const;

    private:
        Entry* m_entry{ nullptr };
    };

public:
    //
    //
    //
    static bool Contains(const Key* key);

    //
    //
    //
    static const Value* Get(const Key* key);

    //
    //
    //
    static size_t Size() { return size; }

public:
    static Iterator begin() { return Iterator(head); }
    static Iterator end() { return Iterator(nullptr); }

private:
    static thread_local Entry* head;
    static thread_local size_t size;
};

//
//
//
class Thread final
{
public:
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

public:
    using Id = uint64_t;
    static constexpr Id INVALID_THREAD_ID = (~0);

public:
    //
    //
    //
    static Id CurrentThreadId();

    //
    //
    //
    static std::string_view GetName();

    //
    //
    //
    static void SetName(std::string_view name);

public:
};

//
//
//
template<typename T>
class ThreadSafe final
{
public:
    //
    //
    //
    ThreadSafe() = default;

    //
    //
    //
    ThreadSafe(T t);

    //
    //
    //
    template<typename ...Args>
    ThreadSafe(Args&& ...args);

public:
    //
    //
    //
    template<typename Fn>
    auto operator()(Fn&& fn) const->std::invoke_result_t<Fn, T&>;

private:
    mutable T m_obj;
    mutable std::mutex m_mutex;
};

//
//
//
class ThreadPool final
{
public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

public:
    //
    //
    //
    enum class Priority
    {
        Low = 0,
        Normal,
        High,

        Default = Normal,
    };

public:
    //
    //
    //
    struct Options
    {
        //
        //
        //
        std::string_view threadName;

        //
        //
        //
        uint32_t threads{ 0 };

        //
        //
        //
        uint8_t maxThreads{ UINT8_MAX };
    };

public:
    //
    //
    //
    ThreadPool(Options options);

    //
    //
    //
    ~ThreadPool();

    //
    //
    //
    void Cancel(uint64_t id);

public:
    //
    //
    //
    template<typename T>
    class Future final
    {
    public:
        //
        //
        //
        Future(
            ThreadPool& pool,
            uint64_t id,
            std::future<Result<T>>&& future);

        //
        //
        //
        ~Future();

        //
        //
        //
        void Cancel();

        //
        //
        //
        uint64_t GetId() const;

        //
        //
        //
        const std::future<Result<T>>& GetFuture() const;

        //
        //
        //
        std::future<Result<T>>& GetFuture();

        //
        //
        //
        std::future<Result<T>> Release();

        //
        //
        //
        const std::future<Result<T>>* operator->() const;

        //
        //
        //
        std::future<Result<T>>* operator->();

    private:
        ThreadPool& m_pool;
        std::future<Result<T>> m_future;
        uint64_t m_id{ UINT64_MAX };
    };

private:
    template <typename T>
    struct Unwrap
    {
        using Type = T;
    };

    template <typename T>
    struct Unwrap<Future<T>>
    {
        using Type = T;
    };

    template <typename T>
    using Unwrapped = typename Unwrap<T>::Type;

public:
    //
    //
    //
    template<typename Fn>
    auto Dispatch(
        Fn&& fn,
        Priority priority = Priority::Default)
        -> std::future<
               Result<
                   Unwrapped<std::invoke_result_t<Fn>>>>;

    //
    //
    //
    template<typename Fn>
    auto Await(
        Fn&& fn,
        Priority priority = Priority::Default)
        -> Future<Unwrapped<std::invoke_result_t<Fn>>>;

private:
    using CompletionHandler = std::function<void(
        void* result,
        Failure* error)>;

    using TaskFn = std::function<void(
        CompletionHandler& completer)>;

private:
    class Queue;
    struct Task;
    struct Thread;

    //
    //
    //
    uint64_t SubmitTask(
        CompletionHandler& completer,
        TaskFn fn,
        Priority priority);

    //
    //
    //
    static std::optional<Task> DequeueTask(Queue& queue);

    //
    //
    //
    static void RunThread(ThreadPool* pool, Thread& thread);

public:
    //
    //
    //
    class Strand final
    {
    public:
        Strand(const Strand&) = delete;
        Strand& operator=(const Strand&) = delete;

    public:
        //
        //
        //
        Strand(ThreadPool& pool);

        //
        //
        //
        ~Strand();

        //
        //
        //
        Strand Duplicate() const;

    private:
        struct State;

    private:
        //
        //
        //
        Strand(std::shared_ptr<State> state);

        //
        //
        //
        bool IsRunningInThisThread() const;

        //
        //
        //
        void Run();

    private:
        std::shared_ptr<State> m_state;
    };

    //
    //
    //
    bool IsRunningInThisThread() const;

private:
    Options m_options;
    std::vector<Thread> m_threads;
    std::unique_ptr<Queue> m_queue;
};

//
//
//
using Strand = ThreadPool::Strand;
}  // namespace Fusion

#define FUSION_CALLSTACK_ENTRY(T, value) \
    typename Fusion::Callstack<T, decltype(value)>::Entry \
        callstackEntry(value); \
    FUSION_UNUSED(callstackEntry);

#define FUSION_IMPL_THREAD 1
#include <Fusion/Impl/Thread.h>
