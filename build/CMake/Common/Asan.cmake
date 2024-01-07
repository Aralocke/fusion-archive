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

include(CheckCCompilerFlag)

# Store the original required flags if they were set before we checked for ASAN
set(CMAKE_ORIGINAL_FLAGS ${CMAKE_REQUIRED_FLAGS})

if(NOT MSVC)
    # Set -Werror to catch "argument unused during compilation" warnings
    set(CMAKE_REQUIRED_FLAGS "-Werror -faddress-sanitizer")
    check_c_compiler_flag("-faddress-sanitizer" HAVE_FLAG_ADDRESS_SANITIZER)

    set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=address")
    check_c_compiler_flag("-fsanitize=address" HAVE_FLAG_SANITIZE_ADDRESS)
endif()

# Check for ASAN on MSVC
if(MSVC)
    set(CMAKE_REQUIRED_FLAGS "/fsanitize=address /Zi")
    check_c_compiler_flag("/fsanitize=address" HAVE_FLAG_MSVC_SANITIZE_ADDRESS)
endif()

# Unset the CMAKE_REQUIRED_FLAGS value to remove the requirements
set(CMAKE_REQUIRED_FLAGS ${CMAKE_ORIGINAL_FLAGS})

if(HAVE_FLAG_SANITIZE_ADDRESS)
    set(ADDRESS_SANITIZER_COMPILER_FLAG "-fsanitize=address")
endif()
if(HAVE_FLAG_ADDRESS_SANITIZER)
    set(ADDRESS_SANITIZER_COMPILER_FLAG "-faddress-sanitizer")
endif()
if(HAVE_FLAG_MSVC_SANITIZE_ADDRESS)
    set(ADDRESS_SANITIZER_COMPILER_FLAG "/fsanitize=address /Zi")
endif()

if(NOT ADDRESS_SANITIZER_COMPILER_FLAG)
    set(FUSION_SUPPORTS_ASAN 0)
else()
    set(FUSION_SUPPORTS_ASAN 1)
endif()
