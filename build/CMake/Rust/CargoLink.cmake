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

function(cargo_print)
execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${ARGN}")
endfunction()

function(cargo_link)
cmake_parse_arguments(CARGO_LINK "" "NAME" "TARGETS;EXCLUDE" ${ARGN})

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cargo-link.c" "void cargo_link() {}")
add_library(${CARGO_LINK_NAME} "${CMAKE_CURRENT_BINARY_DIR}/cargo-link.c")
target_link_libraries(${CARGO_LINK_NAME} ${CARGO_LINK_TARGETS})

get_target_property(LINK_LIBRARIES ${CARGO_LINK_NAME} LINK_LIBRARIES)

foreach(LINK_LIBRARY ${LINK_LIBRARIES})
    get_target_property(_INTERFACE_LINK_LIBRARIES ${LINK_LIBRARY} INTERFACE_LINK_LIBRARIES)
    list(APPEND LINK_LIBRARIES ${_INTERFACE_LINK_LIBRARIES})
endforeach()
list(REMOVE_DUPLICATES LINK_LIBRARIES)

if(CARGO_LINK_EXCLUDE)
    list(REMOVE_ITEM LINK_LIBRARIES ${CARGO_LINK_EXCLUDE})
endif()

set(LINK_DIRECTORIES "")
foreach(LINK_LIBRARY ${LINK_LIBRARIES})
    if(TARGET ${LINK_LIBRARY})
        get_target_property(_IMPORTED_CONFIGURATIONS ${LINK_LIBRARY} IMPORTED_CONFIGURATIONS)
        list(FIND _IMPORTED_CONFIGURATIONS "RELEASE" _IMPORTED_CONFIGURATION_INDEX)
        if (NOT (${_IMPORTED_CONFIGURATION_INDEX} GREATER -1))
            set(_IMPORTED_CONFIGURATION_INDEX 0)
        endif()
        list(GET _IMPORTED_CONFIGURATIONS ${_IMPORTED_CONFIGURATION_INDEX} _IMPORTED_CONFIGURATION)
        get_target_property(_IMPORTED_LOCATION ${LINK_LIBRARY} "IMPORTED_LOCATION_${_IMPORTED_CONFIGURATION}")
        get_filename_component(_IMPORTED_DIR ${_IMPORTED_LOCATION} DIRECTORY)
        get_filename_component(_IMPORTED_NAME ${_IMPORTED_LOCATION} NAME_WE)
        if(NOT WIN32)
            string(REGEX REPLACE "^lib" "" _IMPORTED_NAME ${_IMPORTED_NAME})
        endif()
        list(APPEND LINK_DIRECTORIES ${_IMPORTED_DIR})
        cargo_print("cargo:rustc-link-lib=static=${_IMPORTED_NAME}")
    else()
        if("${LINK_LIBRARY}" MATCHES "^.*/(.+)\\.framework$")
            set(FRAMEWORK_NAME ${CMAKE_MATCH_1})
            cargo_print("cargo:rustc-link-lib=framework=${FRAMEWORK_NAME}")
        elseif("${LINK_LIBRARY}" MATCHES "^(.*)/(.+)\\.so$")
            set(LIBRARY_DIR ${CMAKE_MATCH_1})
            set(LIBRARY_NAME ${CMAKE_MATCH_2})
            if(NOT WIN32)
                string(REGEX REPLACE "^lib" "" LIBRARY_NAME ${LIBRARY_NAME})
            endif()
            list(APPEND LINK_DIRECTORIES ${LIBRARY_DIR})
            cargo_print("cargo:rustc-link-lib=${LIBRARY_NAME}")
        else()
            cargo_print("cargo:rustc-link-lib=${LINK_LIBRARY}")
        endif()
    endif()
endforeach()
list(REMOVE_DUPLICATES LINK_DIRECTORIES)

foreach(LINK_DIRECTORY ${LINK_DIRECTORIES})
    cargo_print("cargo:rustc-link-search=native=${LINK_DIRECTORY}")
endforeach()
endfunction()
