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

################################
# GoogleTest
################################

set(BUILD_GMOCK OFF CACHE INTERNAL "")
set(INSTALL_GTEST OFF CACHE INTERNAL "")
set(gtest_build_samples OFF CACHE INTERNAL "")

add_subdirectory(googletest)

# GoogleTest comes with 4 solutions and we want to group
# them into their own folder.
set_property(TARGET gtest PROPERTY FOLDER "External")
set_property(TARGET gtest_main PROPERTY FOLDER "External")

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(gtest "${FUSION_BUILD_ROOT}/external")
    FSN_SET_TARGET_OUTPUT(gtest_main "${FUSION_BUILD_ROOT}/external")
endif()

################################
# Fmtlib
################################

set(FMT_DOC OFF CACHE INTERNAL "")
set(FMT_INSTALL OFF CACHE INTERNAL "")
set(FMT_TEST OFF CACHE INTERNAL "")

add_subdirectory(fmtlib)

# Ensure that the fmt target ends up in the correct folder in the solution.
set_property(TARGET fmt PROPERTY FOLDER "External")

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(fmt "${FUSION_BUILD_ROOT}/external")
endif()
