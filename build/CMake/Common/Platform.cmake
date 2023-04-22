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

if(NOT _FUSION_MAIN_PROJECT)
    return()
endif()

include(CheckCCompilerFlag)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(FUSION_BUILD_ARCH "X64")
    else()
        set(FUSION_BUILD_ARCH "X86")
    endif()
    set(FUSION_BUILD_PLATFORM "Windows")
elseif(UNIX)
    if(CMAKE_SYSTEM_NAME STREQUAL Darwin)

        set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;x86_64")
        check_c_compiler_flag("-arch x86_64" FUSION_SUPPORTS_X86_64)

        if(FUSION_SUPPORTS_X86_64)
            message(STATUS "Detected support for x86_64 architecture")
            set(FUSION_BUILD_ARCH "X64")
        endif()

        set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;arm64")
        check_c_compiler_flag("-arch arm64" FUSION_SUPPORTS_ARM64)

        if(FUSION_SUPPORTS_ARM64)
            message(STATUS "Detected support for ARM64 architecture")
            set(FUSION_BUILD_ARCH "ARM64")
        endif()

        if(NOT FUSION_BUILD_ARCH)
            message(FATAL_ERROR "Unable to detect compiler/platform architecture")
        endif()

        set(FUSION_BUILD_PLATFORM "OSX")
    elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(FUSION_BUILD_ARCH "X64")
        else()
            set(FUSION_BUILD_ARCH "X86")
        endif()
        set(FUSION_BUILD_PLATFORM "Linux")
    endif()
endif()
if(NOT FUSION_BUILD_ARCH OR NOT FUSION_BUILD_PLATFORM)
    message(FATAL_ERROR "Unrecognized platform")
endif()
