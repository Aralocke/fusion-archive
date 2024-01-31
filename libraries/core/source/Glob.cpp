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

#include <Fusion/Glob.h>

#include <Fusion/Ascii.h>

namespace Fusion
{
using CompareFn = bool(char, char);

static bool _Compare(
    std::string_view pattern,
    std::string_view name,
    CompareFn compare)
{
    const char* p = pattern.data();
    const char* s = name.data();

    const char* pend = p + pattern.size();
    const char* send = s + name.size();

    const char* pnext = nullptr;
    const char* snext = nullptr;
    bool atStart = true;

    while (p < pend || s < send)
    {
        if (p < pend)
        {
            char c = *p;
            switch (c)
            {
            case '*':
            {
                // zero or more character wildcard
                if (s < send)
                {
                    char b = *s;
                    if (b == '/')
                    {
                        // Stop matching wildcards at '/'
                        atStart = true;
                        p++;
                        snext = nullptr;
                        continue;
                    }
                    else if (b == '.' && atStart)
                    {
                        // Dont match '.' at directory start
                        return false;
                    }
                    else
                    {
                        // if we can't match the character restart at s + 1
                        atStart = false;
                        pnext = p;
                        snext = s + 1;
                        p++;
                        continue;
                    }
                }
                else
                {
                    // No more characters but we can still match the zero or more
                    snext = nullptr;
                    p++;
                    continue;
                }
                break;
            }
            case '?':
            {
                // single character wildcard
                if (s < send)
                {
                    char a = *s;
                    if (a == '/')
                    {
                        // Stop matching wildcards at '/'
                        atStart = true;
                    }
                    else if (a == '.' && atStart)
                    {
                        // Dont match '.' at directory start
                        return false;
                    }
                    else
                    {
                        atStart = false;
                        p++;
                        s++;
                        continue;
                    }
                }
                break;
            }
            default:
            {
                // Normal character
                if (s < send && compare(*s, c))
                {
                    atStart = (c == '/');
                    p++;
                    s++;
                    continue;
                }
                break;
            }
            }
        }

        if (snext)
        {
            p = pnext;
            s = snext;
            continue;
        }

        return false;
    }

    return true;
}

bool Glob::Match(
    std::string_view pattern,
    std::string_view name,
    bool sensitive)
{
    Glob glob(pattern);
    return glob.Compare(name, sensitive);
}

Glob::Glob(std::string_view pattern)
    : m_pattern(pattern)
{ }

bool Glob::Compare(
    std::string_view name,
    bool sensitive)
{
    if (sensitive)
    {
        return _Compare(m_pattern, name,
            [](char a, char b) -> bool
            {
                return a == b;
            });
    }
    else
    {
        return _Compare(m_pattern, name,
            [](char a, char b) -> bool
            {
                return Ascii::ToLower(a) == Ascii::ToLower(b);
            });
    }
}
}  // namespace Fusion
