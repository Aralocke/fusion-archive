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
#include <Fusion/Fwd/Argparse.h>
#include <Fusion/Result.h>

#include <functional>
#include <set>
#include <span>
#include <string>
#include <vector>

namespace Fusion
{
//
//
//
template<typename T>
struct IgnoredArgument { };

//
//
//
class ArgumentBase : public std::enable_shared_from_this<ArgumentBase>
{
    template<typename T> friend class Argument;
    friend class ArgumentParser;

public:
    virtual ~ArgumentBase();

protected:
    //
    //
    //
    enum class Type
    {
        Argument = 0,
        Command,
        Option,
        Positional,
    };

    //
    //
    //
    ArgumentBase(
        Type type,
        ArgumentParser& parser,
        std::string_view name,
        char flag = 0);

protected:
    //
    //
    //
    std::string_view GetAlias() const { return m_alias; }

    //
    //
    //
    char GetFlag() const { return m_flag; }

    //
    //
    //
    std::string_view GetHelp() const { return m_help; }

    //
    //
    //
    std::string_view GetMetavar() const { return m_metavar; }

    //
    //
    //
    std::string_view GetName() const { return m_name; }

    //
    //
    //
    std::string GetParameterStr(bool showMetaVar = false) const;

    //
    //
    //
    const ArgumentParser* GetParser() const { return m_parser; }

    //
    //
    //
    size_t GetNargs() const { return m_nargs; }

    //
    //
    //
    Type GetType() const { return m_type; }

    //
    //
    //
    bool IsFound() const;

    //
    //
    //
    bool IsRequired() const;

protected:
    //
    //
    //
    virtual bool IsSequence() const { return false; }

    //
    //
    //
    virtual bool Match(std::string_view arg) const;

    //
    //
    //
    virtual Result<void> Parse(std::span<std::string_view>& args) = 0;

    //
    //
    //
    virtual void Reset() { }

private:
    const ArgumentParser* m_parser = nullptr;
    const Type m_type;
    std::string m_alias;
    std::string m_help;
    std::string m_metavar;
    std::string m_name;
    bool m_required = false;
    char m_flag = 0;
    size_t m_nargs = 1;
};

//
//
//
template<typename T>
class Argument : public ArgumentBase
{
public:
    //
    //
    //
    Argument(
        Type type,
        ArgumentParser& parser,
        std::string_view name,
        char flag = 0);

    ~Argument() override = default;

public:
    //
    //
    //
    T& Alias(std::string_view alias);

    //
    //
    //
    T& Help(std::string_view str);

    //
    //
    //
    T& Metavar(std::string_view meta);

    //
    //
    //
    T& Nargs(size_t count);

    //
    //
    //
    T& Required();
};

//
//
//
template<typename T>
class ArgumentImpl : public Argument<ArgumentImpl<T>>
{
    static_assert(std::is_default_constructible_v<T>,
        "argument type must be default-constructable");
public:
    //
    //
    //
    ArgumentImpl(
        IgnoredArgument<T> ignored,
        ArgumentParser& parser,
        std::string_view name,
        char flag = 0);

    //
    //
    //
    ArgumentImpl(
        T& arg,
        ArgumentParser& parser,
        std::string_view name,
        char flag = 0);

    ~ArgumentImpl() override = default;

    //
    //
    //
    ArgumentImpl<T>& Action(ArgumentAction<T> action);

    //
    //
    //
    ArgumentImpl<T>& Choices(std::initializer_list<T> choices);

    //
    //
    //
    ArgumentImpl<T>& Default(T value);

protected:

    //
    //
    //
    bool IsSequence() const override;

    //
    //
    //
    Result<void> Parse(std::span<std::string_view>& args) override;

private:
    ArgumentTypePtr<T> m_value = nullptr;
    std::vector<T> m_choices;
    ArgumentType<T> m_default = {};
    ArgumentAction<T> m_action;
};

//
//
//
template<typename T>
class ArgumentOption : public Argument<ArgumentOption<T>>
{
    static_assert(std::is_default_constructible_v<T>,
        "argument type must be default-constructable");

public:

    //
    //
    //
    ArgumentOption(
        IgnoredArgument<T> ignored,
        ArgumentParser& parser,
        std::string_view name,
        char flag = 0);

    //
    //
    //
    ArgumentOption(
        T& value,
        ArgumentParser& parser,
        std::string_view name,
        char flag = 0);

    ~ArgumentOption() override = default;

    //
    //
    //
    ArgumentOption<T>& Action(ArgumentAction<T> action);

    //
    //
    //
    ArgumentOption<T>& Default(T value);

protected:

    //
    //
    //
    bool IsSequence() const override;

    //
    //
    //
    Result<void> Parse(std::span<std::string_view>& arguments) override;

private:
    ArgumentTypePtr<T> m_value = nullptr;
    ArgumentType<T> m_default = {};
    ArgumentAction<T> m_action;
};

//
//
//
template<typename T>
class ArgumentPositional : public Argument<ArgumentPositional<T>>
{
    static_assert(std::is_default_constructible_v<T>,
        "argument type must be default-constructable");

public:

    //
    //
    //
    ArgumentPositional(
        IgnoredArgument<T> ignored,
        ArgumentParser& parser,
        std::string_view name);

    //
    //
    //
    ArgumentPositional(
        T& value,
        ArgumentParser& parser,
        std::string_view name);

    ~ArgumentPositional() override = default;

    //
    //
    //
    ArgumentPositional<T>& Action(ArgumentAction<T> action);

    //
    //
    //
    ArgumentPositional<T>& Default(T value);

protected:

    //
    //
    //
    bool IsSequence() const override;

    //
    //
    //
    bool Match(std::string_view arg) const override;

    //
    //
    //
    Result<void> Parse(std::span<std::string_view>& arguments) override;

private:
    ArgumentTypePtr<T> m_value = nullptr;
    ArgumentType<T> m_default = {};
    ArgumentAction<T> m_action;
};

//
//
//
class ArgumentCommand : public Argument<ArgumentCommand>
{
    friend class ArgumentParser;

public:

    //
    //
    //
    ArgumentCommand(
        ArgumentParser& parser,
        std::string_view name,
        bool helpMenu = true);

    ~ArgumentCommand() override = default;

    //
    //
    //
    ArgumentCommand& Action(ArgumentCommandAction action);

    //
    //
    //
    template<typename T, typename ...Params>
    ArgumentImpl<T>& AddArgument(T& argument, Params&& ...params);

    //
    //
    //
    template<typename T, typename ...Params>
    ArgumentImpl<T>& AddArgument(IgnoredArgument<T> ignored, Params&& ...params);

    //
    //
    //
    template<typename T, typename ...Params>
    ArgumentOption<T>& AddOption(T& argument, Params&& ...params);

    //
    //
    //
    template<typename T, typename ...Params>
    ArgumentOption<T>& AddOption(IgnoredArgument<T> ignored, Params&& ...params);

    //
    //
    //
    template<typename T, typename ...Params>
    ArgumentPositional<T>& AddPositional(T& argument, Params&& ...params);

    //
    //
    //
    template<typename T, typename ...Params>
    ArgumentPositional<T>& AddPositional(IgnoredArgument<T> ignored, Params&& ...params);

    //
    //
    //
    ArgumentCommand& Arguments(std::function<void(ArgumentCommand&)> fn);

protected:
    //
    //
    //
    Result<void> Execute();

    //
    //
    //
    bool Match(std::string_view arg) const override;

    //
    //
    //
    Result<void> Parse(std::span<std::string_view>& arguments) override;

    //
    //
    //
    void Reset() override;

private:
    ArgumentCommandAction m_action;
    std::unique_ptr<ArgumentParser> m_parser;
};

//
//
//
class ArgumentParser
{
    friend class ArgumentCommand;

public:

    //
    //
    //
    struct Params
    {
        //
        //
        //
        std::string_view program;

        //
        //
        //
        std::string_view description;

        //
        //
        //
        bool noAutoHelp = false;
    };

    //
    //
    //
    explicit ArgumentParser(Params params);

    ~ArgumentParser() = default;

    //
    //
    //
    template<typename T>
    ArgumentImpl<T>& AddArgument(T& argument, std::string_view name, char flag = 0);

    //
    //
    //
    template<typename T>
    ArgumentImpl<T>& AddArgument(IgnoredArgument<T> ignored, std::string_view name, char flag = 0);

    //
    //
    //
    ArgumentCommand& AddCommand(std::string_view name, bool helpMenu = true);

    //
    //
    //
    template<typename T>
    ArgumentOption<T>& AddOption(T& argument, std::string_view name, char flag = 0);

    //
    //
    //
    template<typename T>
    ArgumentOption<T>& AddOption(IgnoredArgument<T> ignored, std::string_view name, char flag = 0);

    //
    //
    //
    template<typename T>
    ArgumentPositional<T>& AddPositional(T& argument, std::string_view name);

    //
    //
    //
    template<typename T>
    ArgumentPositional<T>& AddPositional(IgnoredArgument<T> ignored, std::string_view name);

    //
    //
    //
    std::string_view Command() const { return m_command; }

    //
    //
    //
    bool IsArgumentFound(std::string_view name) const;

    //
    //
    //
    Result<void> Parse(char** args, size_t count, size_t offset = 1);

    //
    //
    //
    Result<void> Parse(const std::vector<std::string>& args, size_t offset = 1);

    //
    //
    //
    Result<void> Parse(std::span<std::string_view> args, size_t offset = 1);

    //
    //
    //
    void PrintHelp();

    //
    //
    //
    void Reset();

private:

    //
    //
    //
    struct Help
    {
        //
        //
        //
        static void PrintMenu(const ArgumentParser& parser);

        //
        //
        //
        static void PrintUsage(const ArgumentParser& parser, bool showMetaVar);
    };

    //
    //
    //
    template<typename T>
    T& AddArgument(std::shared_ptr<T> ptr);

    //
    //
    //
    bool CheckArgFlag(char flag);

    //
    //
    //
    bool CheckArgName(std::string_view name, bool command = false);

    //
    //
    //
    void MarkArgumentFound(std::string_view name);

    //
    //
    //
    Result<void> Process(std::span<std::string_view>& args);

    //
    //
    //
    void SetupHelp();

    //
    //
    //
    void Sort();

    ArgumentParser* m_parent = nullptr;
    Params m_params;

    std::vector<std::shared_ptr<ArgumentBase>> m_args;
    std::vector<ArgumentCommand*> m_cmds;
    std::set<std::string_view> m_foundArgs;
    std::string_view m_command;
};

}  // namespace Fusion

#define FUSION_IMPL_ARGPARSE 1
#include <fusion/impl/Argparse.h>
