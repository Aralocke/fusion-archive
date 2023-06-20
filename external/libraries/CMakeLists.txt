# Copyright 2015-2022 Daniel Weiner
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

function(FUSION_INIT_GIT_SUBMODULE DIRECTORY)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/.git)
        find_package(Git QUIET REQUIRED)
        message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/.git does not exist. Initializing ${DIRECTORY} submodule ...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_EXIT_CODE
        )
        if(NOT GIT_EXIT_CODE EQUAL "0")
            message(FATAL_ERROR "${GIT_EXECUTABLE} submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY} failed with exit code ${GIT_EXIT_CODE}, please checkout submodules")
        endif()
    else()
        message(STATUS "Git submodule '${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}' is initialized")
    endif()
endfunction()

################################
# BoringSSL
################################

FUSION_INIT_GIT_SUBMODULE(boringssl/src)

set(BUILD_BORINGSSL_TOOL OFF CACHE INTERNAL "")

if(WIN32)
    # Visual Studio projects currently do not support building ASM files so
    # we have to disable them. Yes this is a performance hit, but apparently
    # works fine on Linux/OSX.
    #
    # Known Limitations on Windows
    # https://boringssl.googlesource.com/boringssl/+/HEAD/BUILDING.md
    add_definitions(-DOPENSSL_NO_ASM=1)
endif()

add_subdirectory(boringssl EXCLUDE_FROM_ALL)

if(BUILD_BORINGSSL_TOOL)
    set_property(TARGET bssl PROPERTY FOLDER "External")
endif()
set_property(TARGET crypto PROPERTY FOLDER "External")
set_property(TARGET ssl PROPERTY FOLDER "External")

if(WIN32)
    target_compile_options(crypto PRIVATE /wd4100 /wd4244 /wd4267 /wd4706)
    target_compile_options(ssl PRIVATE /wd4100 /wd4244 /wd4267 /wd4706)
endif()

target_include_directories(ssl PUBLIC boringssl/src/include)

if(FUSION_MASTER_PROJECT)
    if(BUILD_BORINGSSL_TOOL)
        FSN_SET_TARGET_OUTPUT(bssl "${FUSION_BUILD_ROOT}/external")
    endif()
    FSN_SET_TARGET_OUTPUT(crypto "${FUSION_BUILD_ROOT}/external")
    FSN_SET_TARGET_OUTPUT(ssl "${FUSION_BUILD_ROOT}/external")
endif()

################################
# C-Ares
################################

FUSION_INIT_GIT_SUBMODULE(c-ares)

set(CARES_STATIC ON CACHE INTERNAL "")
set(CARES_SHARED OFF CACHE INTERNAL "")
set(CARES_INSTALL OFF CACHE INTERNAL "")
set(CARES_STATIC_PIC ON CACHE INTERNAL "")
set(CARES_BUILD_TOOLS OFF CACHE INTERNAL "")

add_subdirectory(c-ares EXCLUDE_FROM_ALL)

# GoogleTest comes with 4 solutions and we want to group
# them into their own folder.
set_property(TARGET c-ares PROPERTY FOLDER "External")

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(c-ares "${FUSION_BUILD_ROOT}/external")
endif()

################################
# GoogleTest
################################

FUSION_INIT_GIT_SUBMODULE(googletest)

set(BUILD_GMOCK OFF CACHE INTERNAL "")
set(INSTALL_GTEST OFF CACHE INTERNAL "")
set(gtest_build_samples OFF CACHE INTERNAL "")

add_subdirectory(googletest EXCLUDE_FROM_ALL)

# GoogleTest comes with 4 solutions and we want to group
# them into their own folder.
set_property(TARGET gtest PROPERTY FOLDER "External")
set_property(TARGET gtest_main PROPERTY FOLDER "External")

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(gtest "${FUSION_BUILD_ROOT}/external")
    FSN_SET_TARGET_OUTPUT(gtest_main "${FUSION_BUILD_ROOT}/external")
endif()

################################
# Fmtlib
################################

FUSION_INIT_GIT_SUBMODULE(fmtlib)

set(FMT_DOC OFF CACHE INTERNAL "")
set(FMT_INSTALL OFF CACHE INTERNAL "")
set(FMT_TEST OFF CACHE INTERNAL "")

add_subdirectory(fmtlib EXCLUDE_FROM_ALL)

# Ensure that the fmt target ends up in the correct folder in the solution.
set_property(TARGET fmt PROPERTY FOLDER "External")

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(fmt "${FUSION_BUILD_ROOT}/external")
endif()

################################
# ZLib (Next Generation)
################################

FUSION_INIT_GIT_SUBMODULE(zlib)

set(ZLIB_COMPAT ON CACHE INTERNAL "")
set(ZLIB_ENABLE_TESTS OFF CACHE INTERNAL "")
set(ZLIBNG_ENABLE_TESTS OFF CACHE INTERNAL "")
set(WITH_GTEST OFF CACHE INTERNAL "")

if(NOT MSVC)
    set(WITH_NATIVE_INSTRUCTIONS ON CACHE INTERNAL "")
endif()

add_subdirectory(zlib EXCLUDE_FROM_ALL)

set_property(TARGET zlib PROPERTY FOLDER "External")
set_property(TARGET zlibstatic PROPERTY FOLDER "External")

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(zlib "${FUSION_BUILD_ROOT}/external")
    FSN_SET_TARGET_OUTPUT(zlibstatic "${FUSION_BUILD_ROOT}/external")
endif()

################################
# ZStandard
################################

FUSION_INIT_GIT_SUBMODULE(zstd)

set(ZSTD_BUILD_TESTS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_PROGRAMS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_CONTRIB OFF CACHE INTERNAL "")
set(ZSTD_LEGACY_SUPPORT OFF CACHE INTERNAL "")

if(BUILD_SHARED_LIBS)
    set(ZSTD_BUILD_SHARED ON CACHE INTERNAL "")
    set(ZSTD_BUILD_STATIC OFF CACHE INTERNAL "")
else()
    set(ZSTD_BUILD_SHARED OFF CACHE INTERNAL "")
    set(ZSTD_BUILD_STATIC ON CACHE INTERNAL "")
endif()

add_subdirectory(zstd/build/cmake EXCLUDE_FROM_ALL)

if(BUILD_SHARED_LIBS)
    add_library(libzstd ALIAS libzstd_shared)
    set_property(TARGET libzstd_shared PROPERTY FOLDER "External")
    if(FUSION_MAIN_PROJECT)
        FSN_SET_TARGET_OUTPUT(libzstd_shared "${FUSION_BUILD_ROOT}/external")
    endif()
    target_include_directories(libzstd_shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zstd/lib>
        $<INSTALL_INTERFACE:zstd/lib>  # <prefix>/include/mylib
    )
else()
    add_library(libzstd ALIAS libzstd_static)
    set_property(TARGET libzstd_static PROPERTY FOLDER "External")
    if(FUSION_MAIN_PROJECT)
        FSN_SET_TARGET_OUTPUT(libzstd_static "${FUSION_BUILD_ROOT}/external")
    endif()
    target_include_directories(libzstd_static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zstd/lib>
        $<INSTALL_INTERFACE:zstd/lib>  # <prefix>/include/mylib
    )
endif()
