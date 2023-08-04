#include <Fusion/Tests/Tests.h>

#include <Fusion/Library.h>

#include <sstream>

typedef int32_t LibraryAddFn(int32_t, int32_t);

constexpr std::string_view buildPlatform = FUSION_STRINGIZE(FUSION_BUILD_PLATFORM);
constexpr std::string_view buildArch = FUSION_STRINGIZE(FUSION_BUILD_ARCH);
constexpr std::string_view buildConfig = FUSION_STRINGIZE(FUSION_BUILD_CONFIG);

TEST(LibraryTests, LoadUnload)
{
    // This will probably break if the UnitTests are run from something
    // other than the repository root. Look into making this more dynamic
    // later or maybe using filesystem recursive finds or something.

    std::string_view suffix;
    if (buildConfig == "Debug")
    {
        suffix = "-d"sv;
    }

    std::string path;
    {
        std::stringstream o;
        o << "tmp/tests/TestLibrary/"sv;
        o << buildPlatform;
        o << "-"sv;
        o << buildArch;
        o << "-"sv;
        o << buildConfig;
        o << "/bin/"sv;
        o << Library::FormatLibraryName("TestLibrary", suffix);
        path = o.str();
    }

    Library lib;
    FUSION_ASSERT_RESULT(lib.Load(path));
    ASSERT_TRUE(lib.IsLoaded());

    LibraryAddFn* fn = lib.GetEntrypoint<LibraryAddFn*>("Add");
    EXPECT_TRUE(fn);
    EXPECT_EQ(fn(1, 1), 2);

    FUSION_ASSERT_RESULT(lib.Unload());
}
