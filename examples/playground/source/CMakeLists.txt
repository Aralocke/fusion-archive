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

if(NOT FUSION_BUILD_EXAMPLES)
    message(FATAL_ERROR "Playground example incorrectly included")
endif()

cmake_minimum_required(VERSION 3.21)
project(Playground
    LANGUAGES CXX
    VERSION 0.1.0
)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/public)

add_executable(Playground)

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(Playground "${FUSION_BUILD_ROOT}/examples")
    FSN_VERSION_FILE(Playground "${FUSION_BUILD_ROOT}/examples")
endif()

target_include_directories(Playground
    PUBLIC
        ${PROJECT_INCLUDE_DIR}
    PRIVATE
        ${GENERATED_PRIVATE_HEADER_PATH_Playground}
)

file(GLOB_RECURSE PUBLIC_HEADERS ${PROJECT_INCLUDE_DIR} "*.h")

if(MSVC)
    source_group("Header Files\\Public" FILES ${PUBLIC_HEADERS})
    if(GENERATED_PRIVATE_HEADERS_Playground)
        source_group("Header Files\\Generated"
            FILES ${GENERATED_PRIVATE_HEADERS_Playground})
    endif()
endif()

# Create the directory for the Source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR} "*.cpp")

if(MSVC)
    source_group("Source Files" FILES ${SOURCES})
    if(GENERATED_SOURCES_Playground)
        source_group("Source Files\\Generated"
            FILES ${GENERATED_SOURCES_Playground})
    endif()
endif()

list(APPEND playground_link_targets Fusion::Core RustPlayground)

target_sources(Playground
    PRIVATE ${PUBLIC_HEADERS} ${SOURCES}
        ${GENERATED_SOURCES_Playground}
        ${GENERATED_PRIVATE_HEADERS_Playground}
)
target_link_libraries(Playground PUBLIC ${playground_link_targets})

if(MSVC)
    # Set the folder for the executable
    set_property(TARGET Playground PROPERTY FOLDER "Examples")
endif()

if(NOT FUSION_BUILD_TESTS AND MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT Playground)
endif()
