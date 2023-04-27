#include <Fusion/Compiler.h>
#include <Fusion/Types.h>

#if FUSION_COMPILER_MSVC
#define TEST_LIBRARY __declspec(dllexport)
#else
#define TEST_LIBRARY __attribute__((visibility("default")))
#endif

extern "C"
{
TEST_LIBRARY int32_t Add(int32_t num, int32_t adder)
{
    return num + adder;
}
}
