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
project(RustPlayground
    LANGUAGES CXX C
    VERSION 0.1.0)

add_library(RustPlayground INTERFACE)

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(RustPlayground "${FUSION_BUILD_ROOT}/libraries")
endif()

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Public)

target_include_directories(RustPlayground INTERFACE ${PROJECT_INCLUDE_DIR})

# Create the directory for the Header Files
FILE(GLOB_RECURSE HEADERS ${PROJECT_INCLUDE_DIR} "*/public/*.h")
FILE(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR} "*/*.cpp")

if(MSVC)
    source_group("Header Files" FILES ${HEADERS})
    source_group("Source Files" FILES ${SOURCES})
    # Set the folder for the library
    set_property(TARGET RustPlayground PROPERTY FOLDER "Libraries")
endif()

target_sources(RustPlayground PRIVATE ${HEADERS} ${SOURCES})
target_compile_definitions(RustPlayground
    INTERFACE
        -DFUSION_RUST_ENABLED=1
)

cargo_build(
    NAME rust_playground
    RS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
)

list(APPEND rustplayground_link_targets rust_playground)
if(WIN32)
    list(APPEND rustplayground_link_targets bcrypt userenv ws2_32)
endif()

target_link_libraries(RustPlayground INTERFACE ${rustplayground_link_targets})
