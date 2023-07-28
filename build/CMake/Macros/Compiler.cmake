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

if(NOT _FUSION_CONFIG_PROJECT)
    return()
endif()

include(CheckCXXCompilerFlag)

#
# Ensure that there are no duplicates in a "space separated" variable being used as a list.
#
macro(FSN_REMOVE_DUPES ARG_STR OUTPUT)
    set(ARG_LIST ${ARG_STR})
    separate_arguments(ARG_LIST)
    list(REMOVE_DUPLICATES ARG_LIST)
    string(REGEX REPLACE "([^\\]|^);" "\\1 " _TMP_STR "${ARG_LIST}")
    string(REGEX REPLACE "[\\](.)" "\\1" _TMP_STR "${_TMP_STR}") #fixes escaping
    set(${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endmacro()

#
# Add global linker flags
#
function(FSN_ADD_LINKER_FLAGS)
    set(flags_list "")
    FSN_PARSE_ARGUMENTS(ARG "" "SHARED;MODULE;EXECUTABLE;DEBUG;RELEASE" ${ARGN})

    if(NOT ARG_DEBUG AND NOT ARG_RELEASE)
        foreach(flag ${ARG_DEFAULT_ARGS})
            set(flags_list "${flags_list} ${flag}")
        endforeach()
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_SHARED)
            FSN_REMOVE_DUPES("${CMAKE_SHARED_LINKER_FLAGS} ${flags_list}" CMAKE_SHARED_LINKER_FLAGS)
        endif()
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_MODULE)
            FSN_REMOVE_DUPES("${CMAKE_MODULE_LINKER_FLAGS} ${flags_list}" CMAKE_MODULE_LINKER_FLAGS)
        endif()
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_EXECUTABLE)
            FSN_REMOVE_DUPES("${CMAKE_SHARED_LINKER_FLAGS} ${flags_list}" CMAKE_SHARED_LINKER_FLAGS)
        endif()
    elseif(ARG_DEBUG)
        foreach(flag ${ARG_DEFAULT_ARGS})
            set(flags_list "${flags_list} ${flag}")
        endforeach()
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_SHARED)
            foreach(CFG IN LISTS FUSION_DEBUG_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_SHARED_LINKER_FLAGS_${CONFIG}} ${flags_list}" CMAKE_SHARED_LINKER_FLAGS_${CONFIG})
            endforeach()
        endif()
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_MODULE)
            foreach(CFG IN LISTS FUSION_DEBUG_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_MODULE_LINKER_FLAGS_${CONFIG}} ${flags_list}" CMAKE_MODULE_LINKER_FLAGS_${CONFIG})
            endforeach()
        endif()
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_EXECUTABLE)
            foreach(CFG IN LISTS FUSION_DEBUG_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_SHARED_LINKER_FLAGS_${CONFIG}} ${flags_list}" CMAKE_SHARED_LINKER_FLAGS_${CONFIG})
            endforeach()
        endif()
    elseif(ARG_RELEASE)
        foreach(flag ${ARG_DEFAULT_ARGS})
            set(flags_list "${flags_list} ${flag}")
        endforeach()
        message(STATUS "(RELEASE) flags_list=${flags_list}")
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_SHARED)
            foreach(CFG IN LISTS FUSION_RELEASE_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_SHARED_LINKER_FLAGS_${CFG}} ${flags_list}" CMAKE_SHARED_LINKER_FLAGS_${CFG})
            endforeach()
        endif()
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_MODULE)
            foreach(CFG IN LISTS FUSION_RELEASE_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_MODULE_LINKER_FLAGS_${CFG}} ${flags_list}" CMAKE_MODULE_LINKER_FLAGS_${CFG})
            endforeach()
        endif()
        if(NOT ARG_SHARED AND NOT ARG_MODULE AND NOT ARG_EXECUTABLE OR ARG_EXECUTABLE)
            foreach(CFG IN LISTS FUSION_RELEASE_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_SHARED_LINKER_FLAGS_${CFG}} ${flags_list}" CMAKE_SHARED_LINKER_FLAGS_${CFG})
            endforeach()
        endif()
    endif()
endfunction()

#
# Add compiler flags for C / CXX configurations globally.
# You can also target the DEBUG or RELEASE configuration groups and assign flags
# to each group.
#
function(FSN_ADD_COMPILER_FLAGS)
    set(flags_list "")
    FSN_PARSE_ARGUMENTS(ARG "" "C;CXX;DEBUG;RELEASE" ${ARGN})

    if(NOT ARG_DEBUG AND NOT ARG_RELEASE)
        foreach(flag ${ARG_DEFAULT_ARGS})
            set(flags_list "${flags_list} ${flag}")
        endforeach()
        if(NOT ARG_C AND NOT ARG_CXX OR ARG_C)
            FSN_REMOVE_DUPES("${CMAKE_C_FLAGS} ${flags_list}" CMAKE_C_FLAGS)
        endif()
        if(NOT ARG_C AND NOT ARG_CXX OR ARG_CXX)
            FSN_REMOVE_DUPES("${CMAKE_CXX_FLAGS} ${flags_list}" CMAKE_CXX_FLAGS)
        endif()
    elseif(ARG_DEBUG)
        foreach(flag ${ARG_DEFAULT_ARGS})
            set(flags_list "${flags_list} ${flag}")
        endforeach()
        if(NOT ARG_C AND NOT ARG_CXX OR ARG_C)
            foreach(CFG IN LISTS FUSION_DEBUG_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_C_FLAGS_${CFG}} ${flags_list}" CMAKE_C_FLAGS_${CFG})
            endforeach()
        endif()
        if(NOT ARG_C AND NOT ARG_CXX OR ARG_CXX)
            foreach(CFG IN LISTS FUSION_DEBUG_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_CXX_FLAGS_${CFG}} ${flags_list}" CMAKE_CXX_FLAGS_${CFG})
            endforeach()
        endif()
    elseif(ARG_RELEASE)
        foreach(flag ${ARG_DEFAULT_ARGS})
            set(flags_list "${flags_list} ${flag}")
        endforeach()
        if(NOT ARG_C AND NOT ARG_CXX OR ARG_C)
            foreach(CFG IN LISTS FUSION_RELEASE_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_C_FLAGS_${CFG}} ${flags_list}" CMAKE_C_FLAGS_${CFG})
            endforeach()
        endif()
        if(NOT ARG_C AND NOT ARG_CXX OR ARG_CXX)
            foreach(CFG IN LISTS FUSION_RELEASE_CONFIGS)
                FSN_REMOVE_DUPES("${CMAKE_CXX_FLAGS_${CFG}} ${flags_list}" CMAKE_CXX_FLAGS_${CFG})
            endforeach()
        endif()
    endif()
endfunction()

#
# http://www.itk.org/Wiki/CMakeMacroParseArguments
#
macro(FSN_PARSE_ARGUMENTS prefix arg_names option_names)
    set(DEFAULT_ARGS)
    foreach(arg_name ${arg_names})
        set(${prefix}_${arg_name})
    endforeach(arg_name)
    foreach(option ${option_names})
        set(${prefix}_${option} FALSE)
    endforeach(option)

    set(current_arg_name DEFAULT_ARGS)
    set(current_arg_list)
    foreach(arg ${ARGN})
        set(larg_names ${arg_names})
        list(FIND larg_names "${arg}" is_arg_name)
        if(is_arg_name GREATER -1)
            set(${prefix}_${current_arg_name} ${current_arg_list})
            set(current_arg_name ${arg})
            set(current_arg_list)
        else(is_arg_name GREATER -1)
            set(loption_names ${option_names})
            list(FIND loption_names "${arg}" is_option)
            if(is_option GREATER -1)
                set(${prefix}_${arg} TRUE)
            else(is_option GREATER -1)
                set(current_arg_list ${current_arg_list} ${arg})
            endif()
        endif()
    endforeach()
    set(${prefix}_${current_arg_name} ${current_arg_list})
endmacro()

#
# FSN_CHECK_COMPILER_FLAG
#
# Check if a compiler flag of a given VERSION is available foiir this compiler.
# Sets a global 'FUSION_SUPPORTS_CXX{VERSION}' variable based on the detected
# support within the compiler.
#
function(FSN_CHECK_COMPILER_FLAG VERSION)
    if(MSVC)
        CHECK_CXX_COMPILER_FLAG(/std:c++${VERSION} FUSION_SUPPORTS_CXX${VERSION})
    elseif(UNIX)
        CHECK_CXX_COMPILER_FLAG(-std=c++${VERSION} FUSION_SUPPORTS_CXX${VERSION})
    endif()
    mark_as_advanced(FUSION_SUPPORTS_CXX${VERSION})
endfunction()

#
# FSN_SET_COMPILER_VERSION
#
# Set the global CXX standard to the given VERSION.
#
function(FSN_SET_COMPILER_VERSION VERSION)
    if(DEFINED FUSION_GLOBAL_CXX_STANDARD)
        return()
    endif()
    if(FUSION_MAIN_PROJECT)
        message(STATUS "Fusion setting required CXX version to: ${VERSION}")
        if(MSVC)
            FSN_ADD_COMPILER_FLAGS(CXX /std:c++${VERSION})
        else()
            FSN_ADD_COMPILER_FLAGS(CXX -std=c++${VERSION})
        endif()
        set(CMAKE_CXX_STANDARD ${VERSION})
        set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
    endif()
    set(FUSION_GLOBAL_CXX_STANDARD
        ${VERSION}
        CACHE STRING "Supported CXX version for the fusion library"
        FORCE
    )
    mark_as_advanced(
        FUSION_GLOBAL_CXX_STANDARD
        CMAKE_CXX_STANDARD
        CMAKE_CXX_STANDARD_REQUIRED
    )
endfunction()
