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

function(FSN_CREATE_BUILD_CONFIG CFG_NAME COMPILER_FLAGS LINKER_FLAGS)
    message(STATUS "Fusion configuring new build configuration: ${CFG_NAME}")
    string(TOUPPER ${CFG_NAME} CONFIG)

    set(CMAKE_CXX_FLAGS_${CONFIG}
        "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILER_FLAGS}"
        CACHE STRING "Flags used by the C++ compiler during ${CFG_NAME} builds."
        FORCE
    )
    set(CMAKE_C_FLAGS_${CONFIG}
        "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILER_FLAGS}"
        CACHE STRING "Flags used by the C compiler during ${CFG_NAME} builds."
        FORCE
    )
    set(CMAKE_EXE_LINKER_FLAGS_${CONFIG}
        "${LINKER_FLAGS}"
        CACHE STRING "Flags used for linking binaries during ${CFG_NAME} builds."
        FORCE
    )
    set(CMAKE_SHARED_LINKER_FLAGS_${CONFIG}
        "${LINKER_FLAGS}"
        CACHE STRING "Flags used by the shared libraries linker during ${CFG_NAME} builds."
        FORCE
    )
    mark_as_advanced(
        CMAKE_CXX_FLAGS_${CONFIG}
        CMAKE_C_FLAGS_${CONFIG}
        CMAKE_EXE_LINKER_FLAGS_${CONFIG}
        CMAKE_SHARED_LINKER_FLAGS_${CONFIG}
    )
    if(CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES ${CFG_NAME})
        list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
            "Add the configurations that we need"
            FORCE
        )
    endif()
endfunction()
