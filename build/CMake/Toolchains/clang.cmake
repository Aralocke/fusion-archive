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

if(DEFINED FUSION_COMPILER_CLANG_CMAKE_)
    return()
else()
    set(FUSION_COMPILER_CLANG_CMAKE_ 1)
endif()

find_program(CMAKE_C_COMPILER NAMES clang)
find_program(CMAKE_CXX_COMPILER NAMES clang++)

if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "clang (system default) not found")
endif()

if(NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "clang++ (system default) not found")
endif()

set(CMAKE_C_COMPILER
    "${CMAKE_C_COMPILER}"
    CACHE
    STRING
    "C compiler"
    FORCE
)

set(CMAKE_CXX_COMPILER
    "${CMAKE_CXX_COMPILER}"
    CACHE
    STRING
    "C++ compiler"
    FORCE
)

set(FUSION_COMPILER_NAME "clang")
