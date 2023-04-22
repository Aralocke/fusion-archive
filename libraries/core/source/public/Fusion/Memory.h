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

#include <Fusion/Assert.h>
#include <Fusion/Macros.h>
#include <Fusion/TypeTraits.h>

#include <utility>

namespace Fusion
{
//
//
//
template<typename T, typename U,
    FUSION_REQUIRES(std::is_signed_v<T> == std::is_signed_v<U>)>
constexpr T NarrowCast(const U& in)
{
    T out = static_cast<T>(in);
    FUSION_ASSERT(static_cast<U>(out) == in,
        "narrowing of input creates invalid value");
    return out;
}

//
//
//
template<typename T, typename U,
    FUSION_REQUIRES(std::is_signed_v<T> && !std::is_signed_v<U>)>
constexpr T NarrowCast(const U& in)
{
    T out = static_cast<T>(in);
    FUSION_ASSERT(static_cast<U>(out) == in && !(out < T{}),
        "narrowing of input creates invalid value");
    return out;
}

//
//
//
template<typename T, typename U,
    FUSION_REQUIRES(!std::is_signed_v<T> && std::is_signed_v<U>)>
constexpr T NarrowCast(const U& in)
{
    T out = static_cast<T>(in);
    FUSION_ASSERT(static_cast<U>(out) == in && !(in < U{}),
        "narrowing of input creates invalid value");
    return out;
}

//
//
//
template<typename T>
class ScopeGuard final
{
public:
    template<typename Fn>
    friend ScopeGuard<std::decay_t<Fn>> CreateScopeGuard(Fn fn);

    ~ScopeGuard() { m_obj(); }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = delete;

private:
    explicit ScopeGuard(T o) : m_obj(std::move(o)) { }

private:
    T m_obj;
};

template<typename Fn>
static ScopeGuard<std::decay_t<Fn>> CreateScopeGuard(Fn fn)
{
    return ScopeGuard<std::decay_t<Fn>>(std::move(fn));
}

}  // namespace Fusion

#define FUSION_SCOPE_GUARD(...) \
    auto FUSION_ANONYMOUS_SYMBOL(_guard) \
        = Fusion::CreateScopeGuard(__VA_ARGS__);

