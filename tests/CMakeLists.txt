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
project(FusionTests LANGUAGES CXX)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/public)

add_subdirectory(tools)

add_executable(FusionTests)
add_dependencies(FusionTests TestLibrary)

target_include_directories(FusionTests
    PUBLIC
        ${PROJECT_INCLUDE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/core/source/private
)

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(FusionTests "${FUSION_BUILD_ROOT}/tests")
endif()

file(GLOB_RECURSE PUBLIC_HEADERS ${PROJECT_INCLUDE_DIR} "*/public/*.h")

if(MSVC)
    source_group(
        TREE ${PROJECT_INCLUDE_DIR}/Fusion
        PREFIX "Header Files\\Public"
        FILES ${PUBLIC_HEADERS}
    )
endif()

# Create the directory for the Source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR} "source/*.cpp")

if(MSVC)
    source_group("Source Files" FILES ${SOURCES})
endif()

target_sources(FusionTests
    PRIVATE ${PUBLIC_HEADERS} ${SOURCES})

target_compile_definitions(FusionTests
    PUBLIC
        FUSION_BUILD_PLATFORM=${FUSION_BUILD_PLATFORM}
        FUSION_BUILD_ARCH=${FUSION_BUILD_ARCH}
        FUSION_BUILD_CONFIG=$<CONFIG>
)

target_link_libraries(FusionTests
    PRIVATE Fusion::Core gtest)

if(MSVC)
    # Set the folder for the executable
    set_property(TARGET FusionTests PROPERTY FOLDER "Tests")
endif()
