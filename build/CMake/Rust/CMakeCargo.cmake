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

function(cargo_build)
    set(oneValueArgs NAME RS_SOURCE_DIR)
    cmake_parse_arguments(CARGO "" "${oneValueArgs}" "" ${ARGN})
    string(REPLACE "-" "_" LIB_NAME ${CARGO_NAME})

    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(LIB_TARGET "x86_64-pc-windows-msvc")
        else()
            set(LIB_TARGET "i686-pc-windows-msvc")
        endif()
	elseif(ANDROID)
        if(ANDROID_SYSROOT_ABI STREQUAL "x86")
            set(LIB_TARGET "i686-linux-android")
        elseif(ANDROID_SYSROOT_ABI STREQUAL "x86_64")
            set(LIB_TARGET "x86_64-linux-android")
        elseif(ANDROID_SYSROOT_ABI STREQUAL "arm")
            set(LIB_TARGET "arm-linux-androideabi")
        elseif(ANDROID_SYSROOT_ABI STREQUAL "arm64")
            set(LIB_TARGET "aarch64-linux-android")
        endif()
    elseif(IOS)
		set(LIB_TARGET "universal")
    elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
        set(LIB_TARGET "x86_64-apple-darwin")
	else()
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(LIB_TARGET "x86_64-unknown-linux-gnu")
        else()
            set(LIB_TARGET "i686-unknown-linux-gnu")
        endif()
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug"
            OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(LIB_BUILD_TYPE "Debug")
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"
            OR CMAKE_BUILD_TYPE STREQUAL "Release")
        set(LIB_BUILD_TYPE "Release")
    else()
        set(LIB_BUILD_TYPE "Debug")
    endif()

    set(BUILD_ROOT "${FUSION_BUILD_ROOT}/Rust/${CARGO_NAME}")
    set(FILE_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}${LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(LIB_FILE "${BUILD_ROOT}/${LIB_TARGET}/${LIB_BUILD_TYPE}/${CMAKE_STATIC_LIBRARY_PREFIX}${LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")

	if(IOS)
		set(CARGO_ARGS "lipo")
	else()
    	set(CARGO_ARGS "build")
		list(APPEND CARGO_ARGS "--target" ${LIB_TARGET})
	endif()

    if(${LIB_BUILD_TYPE} STREQUAL "Release")
        list(APPEND CARGO_ARGS "--release")
    endif()

    if(CARGO_RS_SOURCE_DIR)
        file(GLOB_RECURSE RUST_SOURCES ${CARGO_RS_SOURCE_DIR} "*.rs")
    else()
        file(GLOB_RECURSE RUST_SOURCES "*.rs")
    endif()

    set(CARGO_ENV_COMMAND ${CMAKE_COMMAND} -E env "CARGO_TARGET_DIR=${BUILD_ROOT}")

    add_custom_command(
        OUTPUT ${LIB_FILE}
        COMMAND ${CARGO_ENV_COMMAND} ${CARGO_EXECUTABLE} ARGS ${CARGO_ARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${RUST_SOURCES}
        COMMENT "Running cargo for '${CARGO_NAME}'"
    )
    add_custom_target(${CARGO_NAME}_target DEPENDS ${LIB_FILE})

    add_library(${CARGO_NAME} STATIC IMPORTED GLOBAL)
    add_dependencies(${CARGO_NAME} ${CARGO_NAME}_target)
    set_target_properties(${CARGO_NAME} PROPERTIES IMPORTED_LOCATION ${LIB_FILE})

    if(MSVC)
        set_property(TARGET ${CARGO_NAME}_target PROPERTY FOLDER "RustDefinedTargets")
    endif()
endfunction()
