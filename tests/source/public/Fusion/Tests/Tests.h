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

#include <Fusion/Result.h>

#include <gtest/gtest.h>

using namespace std::string_view_literals;

namespace Fusion
{
template<typename T, typename F>
auto ResultCheck(const Result<T, F>& result) -> testing::AssertionResult
{
    using namespace testing;

    if (result.Failed())
    {
        return AssertionFailure() << result.Error().Summary();
    }
    else
    {
        return AssertionSuccess();
    }
}

template<typename T, typename F, typename Fn>
auto ResultHandler(
    Result<T, F> result,
    Fn handler)
{
    using namespace testing;

    static_assert(std::is_invocable_v<Fn, typename Result<T, F>::ValueType>,
        "callback handler must accept a Result<T,F>::ValueType input");

    if (result.Failed())
    {
        return AssertionFailure() << result.Error().Summary();
    }
    else
    {
        handler(std::move(*result));
        return AssertionSuccess();
    }
}

template<typename T, typename F>
auto FailureCheck(const Result<T, F>& result) -> testing::AssertionResult
{
    using namespace testing;

    if (result.Failed())
    {
        return AssertionSuccess();
    }
    else
    {
        return AssertionFailure();
    }
}

template<typename T, typename F, typename Fn>
auto FailureHandler(Result<T, F> result, Fn handler)
{
    using namespace testing;

    static_assert(std::is_invocable_v<Fn, const F&>,
        "callback handler must accept a Failure input");

    if (!result)
    {
        handler(result.Error());
        return AssertionSuccess();
    }

    return AssertionFailure()
        << "result successful, expected failure"sv;
}
}  // namespace Fusion

#define FUSION_GET_RESULT_FN(_1,_2,NAME,...) NAME

#define FUSION_ASSERT_FAILURE(...) \
    ASSERT_TRUE( FUSION_GET_RESULT_FN(__VA_ARGS__, \
        Fusion::FailureHandler, \
        Fusion::FailureCheck)(__VA_ARGS__)\
    );

#define FUSION_ASSERT_ERROR(result,error) \
    ASSERT_TRUE(Fusion::FailureHandler(result, error));

#define FUSION_ASSERT_RESULT(...) \
    ASSERT_TRUE( FUSION_GET_RESULT_FN(__VA_ARGS__, \
        Fusion::ResultHandler, \
        Fusion::ResultCheck)(__VA_ARGS__)\
    );

#define FUSION_EXPECT_RESULT(result) \
    EXPECT_TRUE(Fusion::ResultCheck(result));

using namespace Fusion;
