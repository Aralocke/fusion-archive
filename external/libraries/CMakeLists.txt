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

function(FUSION_INIT_GIT_SUBMODULE DIRECTORY)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/.git)
        find_package(Git QUIET REQUIRED)
        message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/.git does not exist. Initializing ${DIRECTORY} submodule ...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_EXIT_CODE
        )
        if(NOT GIT_EXIT_CODE EQUAL "0")
            message(FATAL_ERROR "${GIT_EXECUTABLE} submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY} failed with exit code ${GIT_EXIT_CODE}, please checkout submodules")
        endif()
    else()
        message(STATUS "Git submodule '${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}' is initialized")
    endif()
endfunction()

################################
# GoogleTest
################################

FUSION_INIT_GIT_SUBMODULE(googletest)

set(BUILD_GMOCK OFF CACHE INTERNAL "")
set(INSTALL_GTEST OFF CACHE INTERNAL "")
set(gtest_build_samples OFF CACHE INTERNAL "")

add_subdirectory(googletest EXCLUDE_FROM_ALL)

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

FUSION_INIT_GIT_SUBMODULE(fmtlib)

set(FMT_DOC OFF CACHE INTERNAL "")
set(FMT_INSTALL OFF CACHE INTERNAL "")
set(FMT_TEST OFF CACHE INTERNAL "")

add_subdirectory(fmtlib EXCLUDE_FROM_ALL)

# Ensure that the fmt target ends up in the correct folder in the solution.
set_property(TARGET fmt PROPERTY FOLDER "External")

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(fmt "${FUSION_BUILD_ROOT}/external")
endif()
