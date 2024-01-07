# Copyright 2015-2024 Daniel Weiner
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

project(TestLibrary)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/public)

add_library(TestLibrary SHARED)

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(TestLibrary "${FUSION_BUILD_ROOT}/tests")
endif()

file(GLOB_RECURSE HEADERS ${PROJECT_INCLUDE_DIR} "*/public/*.h")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

target_include_directories(TestLibrary
    PUBLIC ${PROJECT_INCLUDE_DIR}
)
target_sources(TestLibrary
    PRIVATE
        ${HEADERS} ${SOURCES}
)
target_link_libraries(TestLibrary
    PRIVATE Fusion::Core
)

if(MSVC)
    # Set the folder for the executable
    set_property(TARGET TestLibrary PROPERTY FOLDER "Tests")
endif()
