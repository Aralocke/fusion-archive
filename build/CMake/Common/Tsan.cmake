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
    set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=thread")
    check_c_compiler_flag("-fsanitize=thread" HAVE_FLAG_THREAD_SANITIZER)
endif()

# Unset the CMAKE_REQUIRED_FLAGS value to remove the requirements
set(CMAKE_REQUIRED_FLAGS ${CMAKE_ORIGINAL_FLAGS})

if(HAVE_FLAG_THREAD_SANITIZER)
    set(THREAD_SANITIZER_COMPILER_FLAG "-fsanitize=thread")
endif()

if(NOT THREAD_SANITIZER_COMPILER_FLAG)
    set(FUSION_SUPPORTS_TSAN 0)
else()
    set(FUSION_SUPPORTS_TSAN 1)
endif()