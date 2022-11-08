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

#include <Fusion/Result.h>

#include <functional>
#include <type_traits>

namespace Fusion
{
    //
    //
    //
    template<bool value> struct SequenceBase
    {
        static const bool IsSequence = value;
    };

    template<typename T, typename = void> struct ConvertArgument;
    template<typename T, typename = void> struct ConvertArgumentOption;

    class ArgumentCommand;
    class ArgumentParser;

    template<typename T> class Argument;
    template<typename T> class ArgumentImpl;
    template<typename T> class ArgumentOption;
    template<typename T> class ArgumentPositional;

    //
    //
    //
    template<typename T>
    using ArgumentType = std::enable_if_t<std::is_default_constructible_v<T>, T>;

    //
    //
    //
    template<typename T>
    using ArgumentTypePtr = ArgumentType<T>*;

    //
    //
    //
    template<typename T>
    using ArgumentAction = std::function<Result<void>(const ArgumentType<T>&)>;

    //
    //
    //
    using ArgumentCommandAction = std::function<Result<void>(const ArgumentCommand&)>;
}  // namepsace Fusion
