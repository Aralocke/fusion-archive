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

# Run the compiler check for all of the supported versions
FSN_CHECK_COMPILER_FLAG(20)
FSN_CHECK_COMPILER_FLAG(17)
FSN_CHECK_COMPILER_FLAG(14)

if(FUSION_SUPPORTS_CXX20)
    FSN_SET_COMPILER_VERSION(20)
elseif(FUSION_SUPPORTS_CXX17)
    FSN_SET_COMPILER_VERSION(17)
elseif(FUSION_SUPPORTS_CXX14)
    FSN_SET_COMPILER_VERSION(14)
else()
    message(FATAL_ERROR "Compiler does not support C++{14,17,20}")
endif()
