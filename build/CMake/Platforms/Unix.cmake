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

if(NOT _FUSION_MAIN_PROJECT)
    return()
endif()
if(NOT UNIX)
    message(FATAL_ERROR "Unix platform file included on a non-unix build")
endif()

if(FUSION_SUPPORTS_ASAN)
    FSN_CREATE_BUILD_CONFIG(Asan ${ADDRESS_SANITIZER_COMPILER_FLAG} "")
endif()
if(FUSION_SUPPORTS_TSAN)
    FSN_CREATE_BUILD_CONFIG(Tsan ${THREAD_SANITIZER_COMPILER_FLAG} "")
endif()
if(FUSION_SUPPORTS_UBSAN)
    FSN_CREATE_BUILD_CONFIG(Ubsan ${UNDEFINED_SANITIZER_COMPILER_FLAG} "")
endif()

FSN_ADD_COMPILER_FLAGS(RELEASE CXX -O3)
FSN_ADD_COMPILER_FLAGS(RELEASE C -O3)

# Force some basic warnings on all files to warn against
# any unused variables and parameters.
FSN_ADD_COMPILER_FLAGS(CXX
    -Wno-uninitialized
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-comment
    -Wall
)

FSN_ADD_COMPILER_FLAGS(C
    -Wno-uninitialized
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-comment
    -Wall
)

# Include the specific platform file.
if(APPLE)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        include(Platforms/Darwin)
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    include(Platforms/Linux)
elseif(CYGWIN)
    message(FATAL_ERROR "Cygwin is not supported")
endif()
