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

#include <Fusion/Tests/Tests.h>

#include <Fusion/Argparse.h>

namespace
{
class ArgumentParserTests : public ::testing::Test
{
public:
    ArgumentParserTests() : parser({ .program = "test", }) { }

public:
    ArgumentParser parser;
};

template<typename T>
auto ConvertHelper(T& t, std::string_view value)
{
    return ConvertArgument<T>::Convert(t, value);
}
}


TEST_F(ArgumentParserTests, Parse)
{
    std::string arg1, arg2;

    parser.AddArgument(arg1, "alpha"sv);
    parser.AddArgument(arg2, "beta"sv, 'b');

    FUSION_ASSERT_RESULT(parser.Parse({
        "program",
        "--alpha=abc123",
        "-b",
        "def456"
    }));
    ASSERT_EQ(arg1, "abc123"sv);
    ASSERT_EQ(arg2, "def456"sv);
}

TEST_F(ArgumentParserTests, ParseOption)
{
    bool verbose = false;
    unsigned debug = 0;

    parser.AddOption(verbose, "verbosity"sv, 'v');
    parser.AddOption(debug, "debug"sv, 'd');

    FUSION_ASSERT_RESULT(parser.Parse({
        "program",
        "-v",
        "-d",
        "-d"
    }));

    ASSERT_TRUE(verbose);
    ASSERT_EQ(debug, 2U);

    ASSERT_TRUE(parser.IsArgumentFound("verbosity"sv));
    ASSERT_TRUE(parser.IsArgumentFound("debug"sv));
}

TEST_F(ArgumentParserTests, ParseSubCommand)
{
    bool arg1 = false;
    std::string arg2, arg3, arg4;

    parser.AddArgument(arg1, "charlie"sv, 'c');
    parser.AddCommand("abc"sv)
        .Arguments([&](auto& cmd) {
        cmd.AddArgument(arg2, "alpha"sv);
        cmd.AddArgument(arg3, "beta"sv, 'b');
        cmd.AddPositional(arg4, "delta"sv);
    });

    FUSION_ASSERT_RESULT(parser.Parse({
        "program",
        "-c", "true",
        "abc",  // command
        "--alpha=123",
        "-b", "456",
        "delta",
    }));

    ASSERT_TRUE(arg1);
    ASSERT_EQ(arg2, "123"sv);
    ASSERT_EQ(arg3, "456"sv);
    ASSERT_EQ(arg4, "delta"sv);

    ASSERT_TRUE(parser.IsArgumentFound("abc"sv));
    ASSERT_TRUE(parser.IsArgumentFound("charlie"sv));
    ASSERT_TRUE(parser.IsArgumentFound("alpha"sv));
    ASSERT_TRUE(parser.IsArgumentFound("beta"sv));
    ASSERT_TRUE(parser.IsArgumentFound("delta"sv));
}

TEST_F(ArgumentParserTests, ParsePositional)
{
    std::string alpha, charlie, delta;

    parser.AddArgument(alpha, "alpha"sv, 'a');
    parser.AddPositional(charlie, "charlie"sv);
    parser.AddArgument(delta, "delta"sv);

    FUSION_ASSERT_RESULT(parser.Parse({
        "program",
        "--alpha",
        "beta",
        "charlie",
        "--delta",
        "echo",
    }));

    ASSERT_EQ(alpha, "beta"sv);
    ASSERT_EQ(charlie, "charlie"sv);
    ASSERT_EQ(delta, "echo"sv);

    ASSERT_TRUE(parser.IsArgumentFound("alpha"sv));
    ASSERT_TRUE(parser.IsArgumentFound("charlie"sv));
    ASSERT_TRUE(parser.IsArgumentFound("delta"sv));
}

TEST(ArgparseConverter, BoolConversion)
{
    bool value = false;
    FUSION_ASSERT_RESULT(ConvertHelper(value, "true"sv));
    ASSERT_TRUE(value);

    FUSION_ASSERT_RESULT(ConvertHelper(value, "false"sv));
    ASSERT_FALSE(value);
}

TEST(ArgparseConverter, SignedConversions)
{
    int value = 0;
    FUSION_ASSERT_RESULT(ConvertHelper(value, "9000"sv));
    ASSERT_EQ(value, 9000);

    FUSION_ASSERT_RESULT(ConvertHelper(value, "-90000"sv));
    ASSERT_EQ(value, -90000);

    ASSERT_FALSE(ConvertHelper(value, "9876_"sv));
}

TEST(ArgparseConverter, UnsignedConversions)
{
    unsigned value = 0;
    FUSION_ASSERT_RESULT(ConvertHelper(value, "9000"sv));
    ASSERT_EQ(value, 9000U);

    ASSERT_FALSE(ConvertHelper(value, "987654321_"sv));
}

TEST(ArgparseConverter, FloatingPointConversions)
{
    double value = 0.0;
    FUSION_ASSERT_RESULT(ConvertHelper(value, "3.14"sv));
    ASSERT_EQ(value, 3.14);

    FUSION_ASSERT_RESULT(ConvertHelper(value, "3.1478349569384652034856"sv));
    ASSERT_EQ(value, 3.1478349569384654);

    ASSERT_FALSE(ConvertHelper(value, "3.14_"sv));
}

TEST(ArgparseConverter, StringConversions)
{
    std::string value;
    FUSION_ASSERT_RESULT(ConvertHelper(value, "   ABC123   "sv));
    ASSERT_EQ(value, "ABC123"sv);
}

TEST(ArgparseConverter, KeyMappedConversions)
{
    std::map<std::string, std::string> values;
    FUSION_ASSERT_RESULT(ConvertHelper(values, "key=value"sv));
    ASSERT_EQ(values.size(), 1U);
    values.clear();

    FUSION_ASSERT_RESULT(ConvertHelper(values, "a=b,c=d,e=f"sv));
    ASSERT_EQ(values.size(), 3U);

    ASSERT_NE(values.find("a"), values.end());
    ASSERT_EQ(values["c"], "d"sv);
    values.clear();

    ASSERT_FALSE(ConvertHelper(values, "a=b,c,e=f"sv));
}

TEST(ArgparseConverter, SequenceConversions)
{
    std::vector<std::string> vector;
    FUSION_ASSERT_RESULT(ConvertHelper(vector, "a,b,c,d,e"sv));
    ASSERT_EQ(vector.size(), 5U);

    std::list<std::string> list;
    FUSION_ASSERT_RESULT(ConvertHelper(list, "a,b,c,d,e"sv));
    ASSERT_EQ(list.size(), 5U);
}

TEST(ArgparseConverter, TupleConversions)
{
    std::tuple<unsigned, float, std::string> values;
    FUSION_ASSERT_RESULT(ConvertHelper(values, "100:9.001:something"sv));
    ASSERT_EQ(std::get<0>(values), 100U);
    ASSERT_EQ(std::get<1>(values), 9.001F);
    ASSERT_EQ(std::get<2>(values), "something"sv);

    FUSION_ASSERT_RESULT(ConvertHelper(values, "9000:3.14:abc123"sv));
    ASSERT_EQ(std::get<0>(values), 9000U);
    ASSERT_EQ(std::get<1>(values), 3.14F);
    ASSERT_EQ(std::get<2>(values), "abc123"sv);
}

TEST(ArgparseConverter, OptionalConversions)
{
    std::optional<uint64_t> value;
    FUSION_ASSERT_RESULT(ConvertHelper(value, "9001"sv));
    ASSERT_TRUE(value);
    ASSERT_EQ(*value, 9001);
}
