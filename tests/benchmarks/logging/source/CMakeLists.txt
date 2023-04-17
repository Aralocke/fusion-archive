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

project(LoggingBenchmark
    LANGUAGES CXX C
    VERSION 0.0.1
)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/public)

add_executable(LoggingBenchmark)

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(LoggingBenchmark "${FUSION_BUILD_ROOT}/tests")
    FSN_VERSION_FILE(LoggingBenchmark "${FUSION_BUILD_ROOT}/examples")
endif()

target_include_directories(LoggingBenchmark
    PUBLIC
        ${PROJECT_INCLUDE_DIR}
    PRIVATE
        ${GENERATED_PRIVATE_HEADER_PATH_LoggingBenchmark}
)

file(GLOB_RECURSE PUBLIC_HEADERS ${PROJECT_INCLUDE_DIR} "*.h")

if(MSVC)
    source_group("Header Files\\Public" FILES ${PUBLIC_HEADERS})
    if(GENERATED_PRIVATE_HEADERS_LoggingBenchmark)
        source_group("Header Files\\Generated"
            FILES ${GENERATED_PRIVATE_HEADERS_LoggingBenchmark}
        )
    endif()
endif()

# Create the directory for the Source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

if(MSVC)
    source_group("Source Files" FILES ${SOURCES})
    if(GENERATED_SOURCES_LoggingBenchmark)
        source_group("Source Files\\Generated"
            FILES ${GENERATED_SOURCES_LoggingBenchmark}
        )
    endif()
endif()


target_sources(LoggingBenchmark
    PRIVATE ${PUBLIC_HEADERS} ${SOURCES}
        ${GENERATED_PRIVATE_HEADERS_LoggingBenchmark}
        ${GENERATED_SOURCES_LoggingBenchmark}
)

target_link_libraries(LoggingBenchmark
    PRIVATE Fusion::Core benchmark::benchmark
)

if(MSVC)
    # Set the folder for the executable
    set_property(TARGET LoggingBenchmark PROPERTY FOLDER "Tests")
endif()
