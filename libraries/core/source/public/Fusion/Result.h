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

#include <Fusion/Error.h>

#include <type_traits>
#include <variant>

namespace Fusion
{

//
//
//
template<typename T, typename E = Failure>
class Result final
{
public:
    using ErrorType = E;
    using ValueType = std::conditional_t<std::is_void_v<T>, std::monostate, T>;

public:

    //
    //
    //
    Result();

    //
    //
    //
    template<typename ...Args,
        std::enable_if_t<std::is_constructible_v<ValueType, Args...>, int> = 0>
    Result(Args&& ...args)
        : m_value(std::in_place_type<ValueType>, std::forward<Args>(args)...)
    { }

    //
    //
    //
    Result(const ErrorType& error);

    //
    //
    //
    Result(ErrorType&& error);

    //
    //
    //
    Result(ErrorType& error);

    //
    //
    //
    ~Result();

    //
    //
    //
    explicit operator bool() const;

    //
    //
    //
    ValueType Default();

    //
    //
    //
    ValueType Default() const;

    //
    //
    //
    ErrorType& Error();

    //
    //
    //
    const ErrorType& Error() const;

    //
    //
    //
    bool Empty() const;

    //
    //
    //
    bool Failed() const;

    //
    //
    //
    bool Succeeded() const;

    //
    //
    //
    ValueType& operator*();

    //
    //
    //
    const ValueType& operator*() const;

    //
    //
    //
    ValueType* operator->();

    //
    //
    //
    const ValueType* operator->() const;

    //
    //
    //
    ValueType& Value();

    //
    //
    //
    const ValueType& Value() const;

    //
    //
    //
    template<typename U>
    ValueType ValueOr(U&& value);

    //
    //
    //
    template<typename U>
    ValueType ValueOr(U&& value) const;

private:
    std::variant<ErrorType, ValueType> m_value;
};

//
//
//
static const Result<void> Success{ std::monostate{} };

}  // namespace Fusion

#define FUSION_IMPL_RESULT 1
#include <Fusion/Impl/Result.h>
