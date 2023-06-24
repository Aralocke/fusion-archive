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

#if !defined(FUSION_IMPL_RESULT)
#error "Result impl included before main header"
#endif

#include <Fusion/Assert.h>

namespace Fusion
{
template<typename T, typename E>
Result<T, E>::Result()
{ }

template<typename T, typename E>
Result<T, E>::Result(const ErrorType& error)
    : m_value{std::in_place_type<ErrorType>, error}
{ }

template<typename T, typename E>
Result<T, E>::Result(ErrorType&& error)
    : m_value{ std::in_place_type<ErrorType>, std::move(error) }
{ }

template<typename T, typename E>
Result<T, E>::Result(ErrorType& error)
    : m_value{ std::in_place_type<ErrorType>, std::move(error) }
{ }

template<typename T, typename E>
Result<T, E>::~Result()
{ }

template<typename T, typename E>
Result<T, E>::operator bool() const
{
    return Succeeded();
}

template<typename T, typename E>
typename Result<T, E>::ValueType& Result<T, E>::operator*()
{
    FUSION_ASSERT(Succeeded());
    return std::get<ValueType>(m_value);
}

template<typename T, typename E>
const typename Result<T, E>::ValueType& Result<T, E>::operator*() const
{
    FUSION_ASSERT(Succeeded());
    return std::get<ValueType>(m_value);
}

template<typename T, typename E>
typename Result<T, E>::ValueType* Result<T, E>::operator->()
{
    FUSION_ASSERT(Succeeded());
    return &std::get<ValueType>(m_value);
}

template<typename T, typename E>
const typename Result<T, E>::ValueType* Result<T, E>::operator->() const
{
    FUSION_ASSERT(Succeeded());
    return &std::get<ValueType>(m_value);
}

template<typename T, typename E>
typename Result<T, E>::ValueType Result<T, E>::Default()
{
    if (Succeeded())
    {
        return std::move(std::get<ValueType>(m_value));
    }
    else
    {
        return T{};
    }
}

template<typename T, typename E>
typename Result<T, E>::ValueType Result<T, E>::Default() const
{
    if (Succeeded())
    {
        return std::get<ValueType>(m_value);
    }
    else
    {
        return T{};
    }
}

template<typename T, typename E>
bool Result<T, E>::Empty() const
{
    return !Failed() && !Failure();
}

template<typename T, typename E>
bool Result<T, E>::Failed() const
{
    return m_value.index() == 0;
}

template<typename T, typename E>
bool Result<T, E>::Succeeded() const
{
    return m_value.index() == 1;
}

template<typename T, typename E>
typename Result<T, E>::ErrorType& Result<T, E>::Error()
{
    FUSION_ASSERT(Failed());
    return std::get<ErrorType>(m_value);
}

template<typename T, typename E>
const typename Result<T, E>::ErrorType& Result<T, E>::Error() const
{
    FUSION_ASSERT(Failed());
    return std::get<ErrorType>(m_value);
}

template<typename T, typename E>
typename Result<T, E>::ValueType& Result<T, E>::Value()
{
    FUSION_ASSERT(Succeeded());
    return std::get<ValueType>(m_value);
}

template<typename T, typename E>
const typename Result<T, E>::ValueType& Result<T, E>::Value() const
{
    FUSION_ASSERT(Succeeded());
    return std::get<ValueType>(m_value);
}

template<typename T, typename E>
template<typename U>
typename Result<T, E>::ValueType Result<T, E>::ValueOr(U&& value)
{
    static_assert(std::is_convertible_v<U, ValueType>, "The default value must be convertible to T");

    if (Succeeded())
    {
        return std::move(std::get<ValueType>(m_value));
    }
    else
    {
        return T{ std::forward<U>(value) };
    }
}

template<typename T, typename E>
template<typename U>
typename Result<T, E>::ValueType Result<T, E>::ValueOr(U&& value) const
{
    static_assert(std::is_convertible_v<U, ValueType>, "The default value must be convertible to T");

    if (Succeeded())
    {
        return std::get<ValueType>(m_value);
    }
    else
    {
        return T{ std::forward<U>(value) };
    }
}

}  // namespace Fusion

template<typename T>
struct fmt::formatter<Fusion::Result<T>> : fmt::formatter<Fusion::Failure>
{
    template <typename FormatContext>
    auto format(
        const Fusion::Result<T>& result,
        FormatContext& ctx) -> decltype(ctx.out())
    {
        return formatter<Fusion::Failure>::format(result.Error(), ctx);
    }
};
