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

#include <Fusion/Json.h>

#include <iostream>

namespace Fusion
{
void PrettyPrint(
    std::ostream& out,
    const simdjson::dom::element& element,
    int32_t indentation)
{
    using namespace std::string_view_literals;

    const auto type = element.type();
    const std::string indent(indentation, ' ');

    constexpr std::string_view FALSE{ "false" };
    constexpr std::string_view TRUE{ "true" };
    constexpr std::string_view NULLSTR{ "null" };

    switch (type)
    {
    case simdjson::dom::element_type::ARRAY:
    {
        out << "[\n";
        const auto array = element.get_array();
        for (const simdjson::dom::element& e : array)
        {
            out << indent << "  "sv;
            PrettyPrint(out, e, indentation + 2);
            out << ",\n"sv;
        }
        out << indent << ']';
        break;
    }
    case simdjson::dom::element_type::OBJECT:
    {
        out << "{\n"sv;
        const auto object = element.get_object();
        for (const auto& [key, value] : object)
        {
            out << indent << "  \""sv << key << "\": "sv;
            PrettyPrint(out, value, indentation + 2);
            out << ",\n"sv;
        }
        out << indent << '}';
        break;
    }
    case simdjson::dom::element_type::STRING:
        out << '"' << element.get_string() << '"';
        break;
    case simdjson::dom::element_type::INT64:
        out << element.get_int64();
        break;
    case simdjson::dom::element_type::UINT64:
        out << element.get_uint64();
        break;
    case simdjson::dom::element_type::DOUBLE:
        out << element.get_double();
        break;
    case simdjson::dom::element_type::BOOL:
        out << (element.get_bool() ? TRUE : FALSE);
        break;
    case simdjson::dom::element_type::NULL_VALUE:
        out << NULLSTR;
        break;
    default:
        out << "Unknown element type"sv;
        break;
    }
}

std::ostream& operator<<(
    std::ostream& out,
    const simdjson::dom::element& json)
{
    PrettyPrint(out, json); 
    return out;
}
}  // namespace Fusion
