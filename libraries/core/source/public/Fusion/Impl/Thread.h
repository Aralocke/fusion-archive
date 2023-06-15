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

#if !defined(FUSION_IMPL_THREAD)
#error "Thread impl included before main header"
#endif

#include <utility>

namespace Fusion
{
// -------------------------------------------------------------
// Callstack                                               START
template<typename Key, typename Value>
thread_local typename Callstack<Key, Value>::Entry*
    Callstack<Key, Value>::head = nullptr;

template<typename Key, typename Value>
thread_local size_t Callstack<Key, Value>::size{ 0 };

template<typename Key, typename Value>
Callstack<Key, Value>::Entry::Entry(Key* key)
    : key(key)
    , value(reinterpret_cast<Value*>(this))
    , next(Callstack<Key, Value>::head)
{
    Callstack<Key, Value>::head = this;
    ++Callstack<Key, Value>::size;
}

template<typename Key, typename Value>
Callstack<Key, Value>::Entry::Entry(
    Key* key,
    Value& value)
    : key(key)
    , value(&value)
    , next(Callstack<Key, Value>::head)
{
    Callstack<Key, Value>::head = this;
    ++Callstack<Key, Value>::size;
}

template<typename Key, typename Value>
Callstack<Key, Value>::Entry::~Entry()
{
    Callstack<Key, Value>::head = this->next;
    --Callstack<Key, Value>::size;
}

template<typename Key, typename Value>
Callstack<Key, Value>::Iterator::Iterator(Entry* entry)
    : m_entry(entry)
{ }

template<typename Key, typename Value>
typename Callstack<Key, Value>::Iterator&
Callstack<Key, Value>::Iterator::operator++()
{
    if (m_entry)
    {
        m_entry = m_entry->next;
    }
    return *this;
}

template<typename Key, typename Value>
bool Callstack<Key, Value>::Iterator::operator !=(
    const Iterator& it) const
{
    return m_entry != it.m_entry;
}

template<typename Key, typename Value>
const typename Callstack<Key, Value>::Entry*
Callstack<Key, Value>::Iterator::operator*() const
{
    return m_entry;
}

template<typename Key, typename Value>
bool Callstack<Key, Value>::Contains(const Key* key)
{
    return Get(key) != nullptr;
}

template<typename Key, typename Value>
const Value* Callstack<Key, Value>::Get(const Key* key)
{
    for (const Entry* entry = head; entry;)
    {
        if (entry->key == key)
        {
            return entry->value;
        }
        entry = entry->next;
    }
    return nullptr;
}
// Callstack                                                 END
// -------------------------------------------------------------
// ThreadSafe                                              START
template<typename T>
ThreadSafe<T>::ThreadSafe(T t)
    : m_obj(std::move(t))
{ }

template<typename T>
template<typename ...Args>
ThreadSafe<T>::ThreadSafe(Args&& ...args)
    : m_obj{ std::forward<Args>(args)... }
{ }

template<typename T>
template<typename Fn>
auto ThreadSafe<T>::operator()(Fn&& fn) const
    -> std::invoke_result_t<Fn, T&>
{
    std::lock_guard lock(m_mutex);

    return fn(m_obj);
}
// ThreadSafe                                                END
// -------------------------------------------------------------
// ThreadPool::Result                                      START
template<typename T>
ThreadPool::Future<T>::Future(
    ThreadPool& pool,
    uint64_t id,
    std::future<Result<T>>&& future)
    : m_pool(pool)
    , m_future(std::move(future))
    , m_id(id)
{ }

template<typename T>
ThreadPool::Future<T>::~Future()
{
    if (m_future.valid())
    {
        Cancel();
        m_future.wait();
    }
}

template<typename T>
void ThreadPool::Future<T>::Cancel()
{
    if (!m_future.valid())
    {
        return;
    }

    m_pool.Cancel(m_id);
}

template<typename T>
uint64_t ThreadPool::Future<T>::GetId() const
{
    return m_id;
}

template<typename T>
const std::future<Result<T>>&
ThreadPool::Future<T>::GetFuture() const
{
    return m_future;
}

template<typename T>
std::future<Result<T>>&
ThreadPool::Future<T>::GetFuture()
{
    return m_future;
}

template<typename T>
const std::future<Result<T>>*
ThreadPool::Future<T>::operator->() const
{
    return &m_future;
}

template<typename T>
std::future<Result<T>>* ThreadPool::Future<T>::operator->()
{
    return &m_future;
}

template<typename T>
std::future<Result<T>> ThreadPool::Future<T>::Release()
{
    return std::exchange(m_future, {});
}
// ThreadPool::Result                                        END
// -------------------------------------------------------------
// ThreadPool::Strand                                      START
// ThreadPool::Strand                                        END
// -------------------------------------------------------------
// ThreadPool                                              START
template<typename Fn>
auto ThreadPool::Await(
    Fn&& fn,
    Priority priority)
    -> Future<Unwrapped<std::invoke_result_t<Fn>>>
{
    static_assert(std::is_invocable_v<Fn>, "");

    using ReturnType = std::invoke_result_t<Fn>;
    using Type = Unwrapped<ReturnType>;
    using PromiseType = std::promise<Result<Type>>;

    auto promise = std::make_shared<PromiseType>();
    auto future = promise->get_future();

    CompletionHandler completer = [
            promise = std::move(promise)
        ](void* result, Failure* error)
        {
            if (error)
            {
                promise->set_value(*error);
            }
            else if constexpr (std::is_void_v<ReturnType>)
            {
                promise->set_value(Failure(E_SUCCESS));
            }
            else
            {
                promise->set_value(
                    std::move(
                        *static_cast<ReturnType*>(result)));
            }
        };

    uint64_t id = SubmitTask(completer, [
            fn = std::forward<Fn>(fn)
        ](CompletionHandler& completer) mutable
        {
            if constexpr (std::is_void_v<ReturnType>)
            {
                Failure reason(E_SUCCESS);

                fn();
                completer(nullptr, &reason);
            }
            else
            {
                ReturnType result = fn();

                completer(&result, nullptr);
            }
        }, priority);

    return { *this, id, std::move(future) };
}

template<typename Fn>
auto ThreadPool::Dispatch(
    Fn&& fn,
    Priority priority)
    ->std::future<
        Fusion::Result<
            Unwrapped<std::invoke_result_t<Fn>>>>
{
    return Post(std::forward<Fn>(fn), priority).Release();
}
// ThreadPool                                                END
// -------------------------------------------------------------
}  // namespace Fusion
