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

#if !defined(FUSION_IMPL_ARGPARSE)
#error "Argparse impl included before main header"
#endif

#include <Fusion/Ascii.h>
#include <Fusion/Assert.h>
#include <Fusion/StringUtil.h>

#include <algorithm>
#include <list>
#include <map>
#include <optional>
#include <tuple>
#include <unordered_map>

namespace Fusion
{
template<typename T>
struct ConvertArgument<T, std::enable_if_t<std::is_integral_v<T>>>
    : SequenceBase<false>
{
    static Result<void> Convert(T& out, std::string_view in)
    {
        in = StringUtil::Trim(in);
        auto result = StringUtil::ParseNumber<T>(in);

        if (result)
        {
            out = static_cast<T>(*result);
            return Success;
        }

        return result.Error();
    }
};

template<typename T>
struct ConvertArgument<T, std::enable_if_t<std::is_floating_point_v<T>>>
    : SequenceBase<false>
{
    static Result<void> Convert(T& out, std::string_view in)
    {
        in = StringUtil::Trim(in);
        auto result = StringUtil::ToFloatingPoint(in);

        if (result)
        {
            out = static_cast<T>(*result);
            return Success;
        }

        return result.Error();
    }
};

template<>
struct ConvertArgument<bool> : SequenceBase<false>
{
    static Result<void> Convert(bool& storage, std::string_view in)
    {
        using namespace std::string_view_literals;

        std::string tmp = StringUtil::ToLowerCopy(in);
        if (Ascii::IsTrueString(tmp))
        {
            storage = true;
            return Success;
        }
        if (Ascii::IsFalseString(tmp))
        {
            storage = false;
            return Success;
        }
        return Failure{ "unknown boolean string: {}", in };
    }
};

template<>
struct ConvertArgument<std::string> : SequenceBase<false>
{
    static Result<void> Convert(std::string& storage, std::string_view in)
    {
        in = StringUtil::Trim(in);
        storage.assign(in);

        return Success;
    }
};

namespace Impl
{
    template<typename T, typename ContainerType>
    struct SequenceConverter : SequenceBase<true>
    {
        static Result<void> Convert(ContainerType& container, std::string_view in)
        {
            using namespace std::string_view_literals;

            auto views = StringUtil::SplitViews(in, ","sv);

            for (auto view : views)
            {
                T value = {};
                if (auto result = ConvertArgument<T>::Convert(value, view); !result)
                {
                    return result;
                }
                container.emplace_back(std::move(value));
            }

            return Success;
        }
    };
}  // namespace Impl

template<typename T>
struct ConvertArgument<std::list<T>>
    : Impl::SequenceConverter<T, std::list<T>>
{};

template<typename T>
struct ConvertArgument<std::vector<T>>
    : Impl::SequenceConverter<T, std::vector<T>>
{};

namespace Impl
{
    template<typename K, typename V, typename Container>
    struct KeyValueConverter : SequenceBase<true>
    {
        static Result<void> Convert(Container& c, std::string_view in)
        {
            using namespace std::string_view_literals;

            auto views = StringUtil::SplitViews(in, ","sv);

            for (auto view : views)
            {
                if (auto result = Parse(c, view); !result)
                {
                    return result;
                }
            }

            return Success;
        }

        static Result<void> Parse(Container& c, std::string_view pair)
        {
            using namespace std::string_view_literals;

            if (auto eq = pair.find('='); eq != std::string_view::npos)
            {
                std::string_view key = pair.substr(0, eq);
                std::string_view value = pair.substr(eq + 1);

                K k = {};
                V v = {};

                if (auto result = ConvertArgument<K>::Convert(k, key); !result)
                {
                    return result;
                }
                if (auto result = ConvertArgument<V>::Convert(v, value); !result)
                {
                    return result;
                }

                c.emplace(std::move(k), std::move(v));
                return Success;
            }
            return Failure{ "unexpected key-value pair: {}", pair };
        }
    };
}  // namespace impl

template<typename K, typename V>
struct ConvertArgument<std::map<K, V>>
    : Impl::KeyValueConverter<K, V, std::map<K, V>>
{};

template<typename K, typename V>
struct ConvertArgument<std::unordered_map<K, V>>
    : Impl::KeyValueConverter<K, V, std::unordered_map<K, V>>
{};

namespace Impl
{
    template<typename ...T>
    struct TupleConverter : SequenceBase<false>
    {
        using Tuple = std::tuple<T...>;

        static Result<void> Convert(Tuple& tup, std::string_view in)
        {
            using namespace std::string_view_literals;

            constexpr size_t elementCount = sizeof...(T);
            auto elements = StringUtil::SplitViews(in, ":"sv);

            if (elements.size() != elementCount)
            {
                return Failure{ "invalid input, expected '{}' elements", elementCount };
            }

            return Parse(tup, elements, std::make_index_sequence<elementCount>{});
        }

        template<size_t ...Indexes>
        static Result<void> Parse(
            Tuple& tup,
            std::span<std::string_view> views,
            std::index_sequence<Indexes...>)
        {
            Result<void> result = Success;
            (Convert(std::get<Indexes>(tup), views[Indexes], result), ...);
            return result;
        }

        template<typename U>
        static void Convert(U& arg, std::string_view in, Result<void>& result)
        {
            if (!result)
            {
                return;
            }
            result = ConvertArgument<U>::Convert(arg, in);
        }
    };
}  // namespace impl

template<typename ...T>
struct ConvertArgument<std::tuple<T...>>
    : Impl::TupleConverter<T...>
{};

template<typename T>
struct ConvertArgument<std::optional<T>>
{
    static const bool IsSequence = ConvertArgument<T>::IsSequence;

    static Result<void> Convert(std::optional<T>& storage, std::string_view in)
    {
        T value = {};
        if (auto result = ConvertArgument<T>::Convert(value, in); !result)
        {
            return result.Error();
        }

        storage.emplace(std::move(value));
        return Success;
    }
};

template<>
struct ConvertArgumentOption<bool> : SequenceBase<false>
{
    static Result<void> Convert(bool& out)
    {
        out = true;
        return Success;
    }
};

template<typename T>
struct ConvertArgumentOption<T,
    std::enable_if_t<std::is_integral_v<T>>>
    : SequenceBase<true>
{
    static Result<void> Convert(T& out)
    {
        ++out;

        return Success;
    }
};

// --- Argument - START ---

template<typename T>
Argument<T>::Argument(
    Type type,
    ArgumentParser& parser,
    std::string_view name,
    char flag)
    : ArgumentBase(type, parser, name, flag)
{ }

template<typename T>
T& Argument<T>::Alias(std::string_view alias)
{
    ArgumentBase::m_alias = alias;

    return *static_cast<T*>(this);
}

template<typename T>
T& Argument<T>::Help(std::string_view str)
{
    ArgumentBase::m_help = str;

    return *static_cast<T*>(this);
}

template<typename T>
T& Argument<T>::Metavar(std::string_view metavar)
{
    ArgumentBase::m_metavar = metavar;

    return *static_cast<T*>(this);
}

template<typename T>
T& Argument<T>::Nargs(size_t count)
{
    ArgumentBase::m_nargs = count;

    return *static_cast<T*>(this);
}

template<typename T>
T& Argument<T>::Required()
{
    ArgumentBase::m_required = true;

    return *static_cast<T*>(this);
}

// --- Argument - END ---
// --- ArgumentImpl - START ---
template<typename T>
ArgumentImpl<T>::ArgumentImpl(
    IgnoredArgument<T> ignored,
    ArgumentParser& parser,
    std::string_view name,
    char flag)
    : Argument<ArgumentImpl<T>>(ArgumentBase::Type::Argument, parser, name, flag)
    , m_value(nullptr)
{
    (void)ignored;
}

template<typename T>
ArgumentImpl<T>::ArgumentImpl(
    T& arg,
    ArgumentParser& parser,
    std::string_view name,
    char flag)
    : Argument<ArgumentImpl<T>>(ArgumentBase::Type::Argument, parser, name, flag)
    , m_value(&arg)
{ }

template<typename T>
ArgumentImpl<T>& ArgumentImpl<T>::Action(ArgumentAction<T> action)
{
    m_action = std::move(action);
    return *this;
}

template<typename T>
ArgumentImpl<T>& ArgumentImpl<T>::Choices(std::initializer_list<T> choices)
{
    m_choices.assign(begin(choices), end(choices));
    return *this;
}

template<typename T>
ArgumentImpl<T>& ArgumentImpl<T>::Default(T value)
{
    m_default = value;
    if (m_value) *m_value = value;
    return *this;
}

template<typename T>
bool ArgumentImpl<T>::IsSequence() const
{
    return ConvertArgument<T>::IsSequence;
}

template<typename T>
Result<void> ArgumentImpl<T>::Parse(
    std::span<std::string_view>& args)
{
    using namespace std::string_view_literals;

    if (args.empty())
    {
        return Failure{ "invalid argument list" };
    }

    std::string_view arg = args.front();
    size_t nargs = 0;

    // This is a double-dash argument parameter
    if (arg[1] == '-')
    {
        if (auto equal = arg.find('='); equal != std::string_view::npos)
        {
            arg = arg.substr(equal + 1);
            nargs = 1;
        }
        else
        {
            if (args.size() == 1)
            {
                return Failure{ "no remaining arguments to parse" };
            }

            arg = args[1];
            nargs = 2;
        }
    }
    else
    {
        // This is a short parameter
        if (arg.size() != 2)
        {
            arg = arg.substr(2);
            nargs = 1;
        }
        else
        {
            if (args.size() == 1)
            {
                return Failure{ "no remaining arguments to parse" };
            }

            arg = args[1];
            nargs = 2;
        }
    }

    auto Handler = [&](T& value) -> Result<void>
    {
        if (auto result = ConvertArgument<T>::Convert(value, arg); !result)
        {
            return result;
        }
        if (!m_choices.empty())
        {
            bool match = std::any_of(begin(m_choices), end(m_choices),
                [&](const auto& choice) -> bool {
                    return value == choice;
                });

            if (!match)
            {
                return Failure{ "'{}' did not match any valid choices", value };
            }
        }
        if (m_action)
        {
            if (auto result = m_action(value); !result)
            {
                return result;
            }
        }
        return Success;
    };

    if (m_value)
    {
        if (auto result = Handler(*m_value); !result)
        {
            return result;
        }
    }
    else
    {
        T value = {};
        if (auto result = Handler(value); !result)
        {
            return result;
        }
    }

    args = args.subspan(nargs);
    return Success;
}

// --- ArgumentImpl - END ---
// --- ArgumentOption - START ---
template<typename T>
ArgumentOption<T>::ArgumentOption(
    IgnoredArgument<T> ignored,
    ArgumentParser& parser,
    std::string_view name,
    char flag)
    : Argument<ArgumentOption<T>>(ArgumentBase::Type::Option, parser, name, flag)
    , m_value(nullptr)
{
    (void)ignored;
}

template<typename T>
ArgumentOption<T>::ArgumentOption(
    T& value,
    ArgumentParser& parser,
    std::string_view name,
    char flag)
    : Argument<ArgumentOption<T>>(ArgumentBase::Type::Option, parser, name, flag)
    , m_value(&value)
{ }

template<typename T>
ArgumentOption<T>& ArgumentOption<T>::Action(ArgumentAction<T> action)
{
    m_action = std::move(action);
    return *this;
}

template<typename T>
ArgumentOption<T>& ArgumentOption<T>::Default(T value)
{
    m_default = value;
    if (m_value) *m_value = value;
    return *this;
}

template<typename T>
bool ArgumentOption<T>::IsSequence() const
{
    return ConvertArgumentOption<T>::IsSequence;
}

template<typename T>
Result<void> ArgumentOption<T>::Parse(
    std::span<std::string_view>& args)
{
    using namespace std::string_view_literals;

    if (args.empty())
    {
        return Failure{ "invalid argument list" };
    }

    auto Handler = [&](T& value) -> Result<void>
    {
        if (auto result = ConvertArgumentOption<T>::Convert(value); !result)
        {
            return result;
        }
        if (m_action)
        {
            if (auto result = m_action(value); !result)
            {
                return result;
            }
        }
        return Success;
    };

    if (m_value)
    {
        if (auto result = Handler(*m_value); !result)
        {
            return result;
        }
    }
    else
    {
        T value = {};
        if (auto result = Handler(value); !result)
        {
            return result;
        }
    }

    args = args.subspan(1);
    return Success;
}
// --- ArgumentOption - END ---
// --- ArgumentPositional - START ---
template<typename T>
ArgumentPositional<T>::ArgumentPositional(
    IgnoredArgument<T> ignored,
    ArgumentParser& parser,
    std::string_view name)
    : Argument<ArgumentPositional<T>>(ArgumentBase::Type::Positional, parser, name, 0)
    , m_value(nullptr)
{
    FUSION_UNUSED(ignored);
    Argument<ArgumentPositional<T>>::Required();
}

template<typename T>
ArgumentPositional<T>::ArgumentPositional(
    T& value,
    ArgumentParser& parser,
    std::string_view name)
    : Argument<ArgumentPositional<T>>(ArgumentBase::Type::Positional, parser, name, 0)
    , m_value(&value)
{
    Argument<ArgumentPositional<T>>::Required();
}

template<typename T>
ArgumentPositional<T>& ArgumentPositional<T>::Action(ArgumentAction<T> action)
{
    m_action = std::move(action);
    return *this;
}

template<typename T>
ArgumentPositional<T>& ArgumentPositional<T>::Default(T value)
{
    m_default = value;
    if (m_value) *m_value = value;
    return *this;
}

template<typename T>
bool ArgumentPositional<T>::IsSequence() const
{
    return ConvertArgument<T>::IsSequence;
}

template<typename T>
bool ArgumentPositional<T>::Match(std::string_view arg) const
{
    (void)arg;

    // We check for the found value in Positional because it will
    // continue to consume parameters unless we stop it at this point.
    if (!IsSequence() && ArgumentBase::IsFound())
    {
        return false;
    }

    return true;
}

template<typename T>
Result<void> ArgumentPositional<T>::Parse(
    std::span<std::string_view>& args)
{
    using namespace std::string_view_literals;

    if (args.empty())
    {
        return Failure{ "no arguments to parse" };
    }

    std::string_view arg = args.front();

    auto Handler = [&](T& value) -> Result<void>
    {
        if (auto result = ConvertArgument<T>::Convert(value, arg); !result)
        {
            return result;
        }
        if (m_action)
        {
            if (auto result = m_action(value); !result)
            {
                return result;
            }
        }
        return Success;
    };

    if (m_value)
    {
        if (auto result = Handler(*m_value); !result)
        {
            return result;
        }
    }
    else
    {
        T value = {};
        if (auto result = Handler(value); !result)
        {
            return result;
        }
    }

    args = args.subspan(1);
    return Success;
}

// --- ArgumentPositional - END ---
// --- ArgumentCommand - START ---
template<typename T, typename ...Params>
ArgumentImpl<T>& ArgumentCommand::AddArgument(
    T& argument,
    Params&& ...params)
{
    return m_parser->AddArgument(argument, std::forward<Params>(params)...);
}

template<typename T, typename ...Params>
ArgumentImpl<T>& ArgumentCommand::AddArgument(
    IgnoredArgument<T> ignored,
    Params&& ...params)
{
    return m_parser->AddArgument(ignored, std::forward<Params>(params)...);
}

template<typename T, typename ...Params>
ArgumentOption<T>& ArgumentCommand::AddOption(
    T& argument,
    Params&& ...params)
{
    return m_parser->AddOption(argument, std::forward<Params>(params)...);
}

template<typename T, typename ...Params>
ArgumentOption<T>& ArgumentCommand::AddOption(
    IgnoredArgument<T> ignored,
    Params&& ...params)
{
    return m_parser->AddOption(ignored, std::forward<Params>(params)...);
}

template<typename T, typename ...Params>
ArgumentPositional<T>& ArgumentCommand::AddPositional(
    T& argument,
    Params&& ...params)
{
    return m_parser->AddPositional(argument, std::forward<Params>(params)...);
}

template<typename T, typename ...Params>
ArgumentPositional<T>& ArgumentCommand::AddPositional(
    IgnoredArgument<T> ignored,
    Params&& ...params)
{
    return m_parser->AddPositional(ignored, std::forward<Params>(params)...);
}
// --- ArgumentCommand - END ---
// --- ArgumentParser - START ---
template<typename T>
ArgumentImpl<T>& ArgumentParser::AddArgument(
    T& argument,
    std::string_view name,
    char flag)
{
    FUSION_ASSERT(CheckArgName(name));
    FUSION_ASSERT(CheckArgFlag(flag));

    return AddArgument(std::make_shared<ArgumentImpl<T>>(
        argument,
        *this,
        name,
        flag));
}

template<typename T>
ArgumentImpl<T>& ArgumentParser::AddArgument(
    IgnoredArgument<T> ignored,
    std::string_view name,
    char flag)
{
    FUSION_ASSERT(CheckArgName(name));
    FUSION_ASSERT(CheckArgFlag(flag));

    return AddArgument(std::make_shared<ArgumentImpl<T>>(
        ignored,
        *this,
        name,
        flag));
}

template<typename T>
T& ArgumentParser::AddArgument(std::shared_ptr<T> ptr)
{
    using namespace std::string_view_literals;

    T* arg = ptr.get();

    if (arg->GetType() == ArgumentBase::Type::Argument
        || arg->GetType() == ArgumentBase::Type::Option)
    {
        if (arg->GetName() != "help"sv)
        {
            for (auto* cmd : m_cmds)
            {
                cmd->m_parser->m_args.emplace_back(ptr);
                cmd->m_parser->Sort();
            }
        }
    }

    m_args.emplace_back(std::move(ptr));

    if (m_args.size() > 1)
    {
        Sort();
    }

    return *arg;
}

template<typename T>
ArgumentOption<T>& ArgumentParser::AddOption(
    T& argument,
    std::string_view name,
    char flag)
{
    FUSION_ASSERT(CheckArgName(name));
    FUSION_ASSERT(CheckArgFlag(flag));

    return AddArgument(std::make_shared<ArgumentOption<T>>(
        argument,
        *this,
        name,
        flag));
}

template<typename T>
ArgumentOption<T>& ArgumentParser::AddOption(
    IgnoredArgument<T> ignored,
    std::string_view name,
    char flag)
{
    FUSION_ASSERT(CheckArgName(name));
    FUSION_ASSERT(CheckArgFlag(flag));

    return AddArgument(std::make_shared<ArgumentOption<T>>(
        ignored,
        *this,
        name,
        flag));
}

template<typename T>
ArgumentPositional<T>& ArgumentParser::AddPositional(
    T& argument,
    std::string_view name)
{
    FUSION_ASSERT(CheckArgName(name));

    return AddArgument(std::make_shared<ArgumentPositional<T>>(argument, *this, name));
}

template<typename T>
ArgumentPositional<T>& ArgumentParser::AddPositional(
    IgnoredArgument<T> ignored,
    std::string_view name)
{
    FUSION_ASSERT(CheckArgName(name));

    return AddArgument(std::make_shared<ArgumentPositional<T>>(ignored, *this, name));
}

// --- ArgumentParser - END ---
}  // namespace Fusion
