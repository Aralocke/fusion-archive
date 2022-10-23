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

cmake_minimum_required(VERSION 3.21)
project(Fusion
    LANGUAGES CXX C
    VERSION 0.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Prevent in-tree generation with CMake. This prevents us accidentally
# flooding the project with CMake files and making it a nightmare to
# clean up manually
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE)
    message(FATAL_ERROR, "In-Source builds are not permitted")
endif()

# Sets up the entry variable to the CMake build system for the Fusion
# project. If this value is not set then only the basic macros and utilities
# will be loaded by CMake.
set(FUSION_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR
        AND PROJECT_IS_TOP_LEVEL)
    set(FUSION_MAIN_PROJECT ON)
endif()

# Available options for the project
option(FUSION_BUILD_EXAMPLES "Build the example utilities" ON)
option(FUSION_BUILD_SHARED_LIBRARY "Build the entire fusion project as a shared library" ON)
option(FUSION_BUILD_TESTS "Build the unit test project" ON)

# Define global options which affect the libraries and what is generated.
option(FUSION_BUILD_BINDINGS "Build all library bindings if they are configured on this platform" ON)
option(FUSION_BUILD_PYTHON_BINDINGS "Build Python library bindings if supported on this platform" ON)

# Configure CMake to load the project build scripts. If this is a full
# build the system will pull in  the entire environment setup.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/CMake)
include(Fusion)

add_subdirectory(external/libraries)
if(FUSION_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
add_subdirectory(libraries)
if(FUSION_BUILD_TESTS)
    add_subdirectory(tests)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT FusionTests)
endif()
