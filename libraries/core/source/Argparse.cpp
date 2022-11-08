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

#include <Fusion/Argparse.h>
#include <Fusion/Assert.h>
#include <Fusion/Platform.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace Fusion
{
using namespace std::string_view_literals;

ArgumentBase::ArgumentBase(
    Type type,
    ArgumentParser& parser,
    std::string_view name,
    char flag)
    : m_parser(&parser)
    , m_type(type)
    , m_name(name)
    , m_flag(flag)
{ }

ArgumentBase::~ArgumentBase()
{ }

std::string ArgumentBase::GetParameterStr(bool showMetaVar) const
{
    std::string param;
    switch (m_type)
    {
    case ArgumentBase::Type::Argument:
    case ArgumentBase::Type::Option:
        param.append("--"sv);
        break;
    case ArgumentBase::Type::Command:
    case ArgumentBase::Type::Positional:
        break;
    }
    param.append(m_name);
    if (m_flag)
    {
        param.append("/-"sv);
        param += m_flag;
    }
    if (showMetaVar && !m_metavar.empty())
    {
        param.append(" "sv);
        param.append(m_metavar);
    }
    return param;
}

bool ArgumentBase::IsFound() const
{
    return m_parser->IsArgumentFound(m_name);
}

bool ArgumentBase::IsRequired() const
{
    return m_required;
}

bool ArgumentBase::Match(std::string_view arg) const
{
    if (arg.empty() || arg.front() != '-')
    {
        return false;
    }
    if (m_flag && arg.size() == 2)
    {
        // Short-form argument
        if (arg[1] == m_flag) return true;
    }

    auto Matcher = [](std::string_view input, std::string_view param)
    {
        if (param.empty() || input[1] != '-')
        {
            return false;
        }
        // Long-form argument
        if (input = input.substr(2); param == input)
        {
            // Long form "--arg <thing>"
            return true;
        }
        else if (auto equal = input.find('=');
            equal != std::string_view::npos)
        {
            if (input = input.substr(0, equal); param == input)
            {
                return true;
            }
        }
        return false;
    };

    return Matcher(arg, m_name) || Matcher(arg, m_alias);
}

ArgumentCommand::ArgumentCommand(
    ArgumentParser& parser,
    std::string_view name,
    bool helpMenu)
    : Argument<ArgumentCommand>(ArgumentBase::Type::Command, parser, name, 0)
{
    ArgumentParser::Params params;
    params.program = name;
    params.noAutoHelp = !helpMenu;
    m_parser = std::make_unique<ArgumentParser>(std::move(params));
    m_parser->m_parent = &parser;
}

ArgumentCommand& ArgumentCommand::Action(
    ArgumentCommandAction action)
{
    m_action = std::move(action);

    return (*this);
}

ArgumentCommand& ArgumentCommand::Arguments(
    std::function<void(ArgumentCommand&)> fn)
{
    fn(*this);

    return (*this);
}

Result<void> ArgumentCommand::Execute()
{
    if (m_action)
    {
        if (auto result = m_action(*this); !result)
        {
            return result;
        }
    }
    return Success;
}

bool ArgumentCommand::Match(std::string_view arg) const
{
    return GetName() == arg;
}

Result<void> ArgumentCommand::Parse(
    std::span<std::string_view>& args)
{
    if (args.empty())
    {
        return Failure{ "no arguments to parse" };
    }

    FUSION_ASSERT(m_parser->m_parent);
    m_parser->m_parent->m_command = GetName();

    // Offset by 1 because args[0] is the matched sub-command string.
    auto arguments = args.subspan(1);
    if (auto result = m_parser->Process(arguments); !result)
    {
        return result;
    }

    args = arguments;
    return Success;
}

void ArgumentCommand::Reset()
{
    FUSION_ASSERT(m_parser);
    m_parser->Reset();
}

void ArgumentParser::Help::PrintUsage(
    const ArgumentParser& parser,
    bool showMetaVar)
{
    const ArgumentParser* parent = nullptr;

    if (parser.m_parent)
    {
        parent = parser.m_parent;
    }

    std::string program{ parser.m_params.program };

    if (parent)
    {
        program = parent->m_params.program;
    }
#if FUSION_PLATFORM_WINDOWS
    program += ".exe"sv;
#endif

    std::ostringstream out;
    out << "Usage: "sv << (!program.empty() ? program : "") << ' ';

    if (parent)
    {
        if (!parser.m_params.program.empty())
        {
            out << parser.m_params.program << ' ';
        }
    }

    std::vector<std::string> args, cmds, pos;

    for (const auto& arg : parser.m_args)
    {
        switch (arg->GetType())
        {
        case ArgumentBase::Type::Argument:
        case ArgumentBase::Type::Option:
            args.emplace_back(arg->GetParameterStr(showMetaVar));
            break;
        case ArgumentBase::Type::Command:
            cmds.emplace_back(arg->GetParameterStr());
            break;
        case ArgumentBase::Type::Positional:
            pos.emplace_back(arg->GetParameterStr());
            break;
        }
    }

    if (!args.empty())
    {
        for (auto& arg : args) out << '[' << std::move(arg) << "] ";
    }
    if (!pos.empty())
    {
        for (auto& p : pos) out << std::move(p) << ' ';
    }
    if (!cmds.empty())
    {
        out << '{';
        out << StringUtil::Join(cmds, ", "sv);
        out << "} ..."sv;
    }

    std::cerr << out.str() << '\n';
}

void ArgumentParser::Help::PrintMenu(const ArgumentParser& parser)
{
    PrintUsage(parser, true /* showMetaVar */);

    std::ostringstream out;
    out << '\n';

    std::vector<const ArgumentBase*> argsRequired, argsOptional, cmds, pos;
    size_t argMaxLength = 0;
    size_t cmdMaxLength = 0;
    size_t posMaxLength = 0;

    for (const auto& arg : parser.m_args)
    {
        switch (arg->GetType())
        {
        case ArgumentBase::Type::Argument:
        case ArgumentBase::Type::Option:
        {
            std::string param = arg->GetParameterStr(true /* showMetaVar */);
            argMaxLength = std::max(param.size(), argMaxLength);
            if (arg->IsRequired())
            {
                argsRequired.push_back(arg.get());
            }
            else
            {
                argsOptional.push_back(arg.get());
            }
            break;
        }
        case ArgumentBase::Type::Command:
        {
            std::string_view name = arg->GetName();
            cmdMaxLength = std::max(name.size(), cmdMaxLength);
            cmds.push_back(arg.get());
            break;
        }
        case ArgumentBase::Type::Positional:
        {
            std::string_view name = arg->GetName();
            posMaxLength = std::max(name.size(), posMaxLength);
            pos.push_back(arg.get());
            break;
        }
        }
    }

    // Adds an extra space for printing menu
    ++cmdMaxLength;
    ++argMaxLength;
    ++posMaxLength;

    if (!cmds.empty())
    {
        // Sets a minimum distance of spacing between the command
        // and the help text menu.
        cmdMaxLength = std::max<size_t>(cmdMaxLength, 16U);

        out << "Available Commands:\n"sv;
        for (const auto* arg : cmds)
        {
            std::string_view name = arg->GetName();
            out << "    "sv << name;

            if (std::string_view help = arg->GetHelp(); !help.empty())
            {
                for (size_t i = name.size(); i < cmdMaxLength; ++i)
                {
                    out << ' ';
                }
                out << help;
            }
            out << '\n';
        }
    }
    if (!pos.empty())
    {
        // Sets a minimum distance of spacing between the command
        // and the help text menu.
        posMaxLength = std::max<size_t>(posMaxLength, 16U);

        out << "Positional Arguments:\n";
        for (const auto* arg : pos)
        {
            std::string_view name = arg->GetName();
            out << "    "sv << name;

            if (std::string_view help = arg->GetHelp(); !help.empty())
            {
                for (size_t i = name.size(); i < posMaxLength; ++i)
                {
                    out << ' ';
                }
                out << help;
            }
            out << '\n';
        }
        out << '\n';
    }

    // Sets a minimum distance of spacing between the command
    // and the help text menu.
    argMaxLength = std::max<size_t>(argMaxLength, 16U);

    if (!argsRequired.empty())
    {
        if (!cmds.empty() || !pos.empty())
        {
            // Adds an extra space if we have menu items already
            out << '\n';
        }
        out << "Required Arguments:\n"sv;

        for (const auto* arg : argsRequired)
        {
            std::string param = arg->GetParameterStr(true /* showMetaVar */);
            out << "    "sv << param;

            if (std::string_view help = arg->GetHelp(); !help.empty())
            {
                for (size_t i = param.size(); i < argMaxLength; ++i)
                {
                    out << ' ';
                }
                out << help;
            }
            out << '\n';
        }
    }
    if (!argsOptional.empty())
    {
        if (!cmds.empty() || !pos.empty() || !argsRequired.empty())
        {
            // Adds an extra space if we have menu items already
            out << '\n';
        }

        out << "Optional Arguments:\n"sv;

        for (const auto* arg : argsOptional)
        {
            std::string param = arg->GetParameterStr(true /* showMetaVar */);
            out << "    "sv << param;

            if (std::string_view help = arg->GetHelp(); !help.empty())
            {
                for (size_t i = param.size(); i < argMaxLength; ++i)
                {
                    out << ' ';
                }
                out << help;
            }
            out << '\n';
        }
    }

    std::cerr << out.str() << '\n';
}

ArgumentParser::ArgumentParser(Params params)
    : m_params(std::move(params))
{
    SetupHelp();
}

ArgumentCommand&
ArgumentParser::AddCommand(std::string_view name, bool helpMenu)
{
    FUSION_ASSERT(CheckArgName(name, true));

    auto cmd = std::make_shared<ArgumentCommand>(*this, name, helpMenu);

    auto& parser = cmd->m_parser;
    auto& args = parser->m_args;

    for (auto& arg : m_args)
    {
        if (arg->GetType() == ArgumentBase::Type::Argument
            || arg->GetType() == ArgumentBase::Type::Option)
        {
            if (arg->GetName() == "help"sv)
            {
                FUSION_ASSERT(arg->GetType() == ArgumentBase::Type::Option);
                continue;
            }
            args.emplace_back(arg);
        }
    }

    m_cmds.push_back(cmd.get());

    parser->Sort();
    return AddArgument(std::move(cmd));
}

bool ArgumentParser::CheckArgName(std::string_view name, bool isCommand)
{
    if (name.empty())
    {
        return false;
    }

    if (!std::all_of(begin(name), end(name),
        [&](char c) -> bool { return std::isalnum(c) || c == '_' || c == '-'; }))
    {
        return false;
    }

    for (const auto& arg : m_args)
    {
        if (arg->GetName() == name || arg->GetAlias() == name)
        {
            if (isCommand)
            {
                if (arg->GetType() == ArgumentBase::Type::Command)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

bool ArgumentParser::CheckArgFlag(char flag)
{
    if (!flag)
    {
        return true;
    }

    if (!std::isalnum(flag))
    {
        return false;
    }

    return std::none_of(begin(m_args), end(m_args),
        [&](const auto& arg) { return arg->GetFlag() == flag; });
}

bool ArgumentParser::IsArgumentFound(std::string_view name) const
{
    return (m_parent)
        ? m_parent->IsArgumentFound(name)
        : m_foundArgs.contains(name);
}

void ArgumentParser::MarkArgumentFound(std::string_view name)
{
    if (m_parent)
    {
        m_parent->MarkArgumentFound(name);
        return;
    }
    if (!IsArgumentFound(name))
    {
        m_foundArgs.insert(name);
    }
}

Result<void> ArgumentParser::Parse(
    char** args,
    size_t count,
    size_t offset)
{
    if (offset > count)
    {
        return Failure{ "invalid offset" };
    }

    std::vector<std::string> arguments;
    arguments.reserve(count);

    for (size_t i = offset; i < count; ++i)
    {
        arguments.push_back(args[i]);
        StringUtil::Trim(arguments.back());
    }

    return Parse(arguments, offset);
}

Result<void> ArgumentParser::Parse(
    const std::vector<std::string>& args,
    size_t offset)
{
    if (args.empty())
    {
        return Failure{ "empty argument list" };
    }

    std::vector<std::string_view> arguments;
    arguments.reserve(args.size());

    const auto count = args.size();
    for (size_t i = offset; i < count; ++i)
    {
        arguments.emplace_back(args[i]);
    }

    std::span<std::string_view> input(
        begin(arguments),
        end(arguments));

    if (auto result = Process(input); !result)
    {
        Reset();
        return result;
    }

    return Success;
}

Result<void> ArgumentParser::Parse(
    std::span<std::string_view> args,
    size_t offset)
{
    if (offset > args.size())
    {
        return Failure{ "invalid offset" };
    }

    args = args.subspan(offset);
    if (auto result = Process(args); !result)
    {
        Reset();
        return result;
    }

    return Success;
}

void ArgumentParser::PrintHelp()
{
    Help::PrintMenu(*this);
}

Result<void> ArgumentParser::Process(
    std::span<std::string_view>& args)
{
    bool printHelp = false;

    if (!args.empty())
    {
        // Don't printHelp by default when no options are specified. The caller
        // should detect nothing is given and call PrintHelp directly.
        printHelp = std::any_of(begin(m_args), end(m_args),
            [](const auto& arg) {
                return arg->IsRequired();
            });
    }

    while (!args.empty())
    {
        bool match = false;

        for (const auto& arg : m_args)
        {
            if (arg->Match(args.front()))
            {
                if (IsArgumentFound(arg->GetName()) && !arg->IsSequence())
                {
                    return Failure{ "duplicate argument: {}", arg->GetName() };
                }

                if (auto result = arg->Parse(args); !result)
                {
                    return result.Error();
                }

                if (arg->GetType() == ArgumentBase::Type::Command)
                {
                    auto& command = static_cast<ArgumentCommand&>(*arg);
                    if (auto result = command.Execute(); !result)
                    {
                        return result.Error();
                    }
                }

                MarkArgumentFound(arg->GetName());

                match = true;
                break;
            }
        }

        if (!match)
        {
            break;
        }
        printHelp = false;
    }

    for (const auto& arg : m_args)
    {
        if (arg->IsRequired() && !IsArgumentFound(arg->GetName()))
        {
            return Failure{ "missing required option: {}", arg->GetParameterStr(false) };
        }
    }

    if (!args.empty())
    {
        Help::PrintUsage(*this, true);

        return Failure{ "unrecognized argument: {}", args.front() };
    }

    if (printHelp)
    {
        Help::PrintUsage(*this, true);
    }

    return Success;
}

void ArgumentParser::Reset()
{
    for (auto& arg : m_args) arg->Reset();
    m_foundArgs.clear();
    m_command = {};
}

void ArgumentParser::SetupHelp()
{
    if (!m_params.noAutoHelp)
    {
        AddOption(IgnoredArgument<bool>{}, "help"sv, 'h')
            .Action([&](auto) -> Result<void> {
                Help::PrintMenu(*this);
                return Failure{ 1 };
            })
            .Help("Print this helpful menu and exit."sv);
    }

}

void ArgumentParser::Sort()
{
    std::sort(begin(m_args), end(m_args),
        [=](const auto& left, const auto& right) -> bool {
            return left->GetType() < right->GetType();
        });
}
}  // namespace Fusion
