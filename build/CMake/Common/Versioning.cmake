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

function(FSN_VERSION_FILE TARGET BUILD_ROOT)
    message(STATUS "Generating Version files for: ${TARGET}")

    # Configure the associated paths that the generated locations
    # will output to and eventually set for the include and file
    # paths.
    set(BUILD_ROOT "${BUILD_ROOT}/${TARGET}")

    # Push the definitions for the Platform, Architecture, and Config to the
    # given target.
    target_compile_definitions(${TARGET}
        PUBLIC
            FUSION_BUILD_PLATFORM=${FUSION_BUILD_PLATFORM}
            FUSION_BUILD_ARCH=${FUSION_BUILD_ARCH}
            FUSION_BUILD_CONFIG=$<CONFIG>
    )

    # Set the paths that get exported to the target
    # These are used to configure the executable for the generated files
    # and their include/source paths.
    set(GENERATED_PATH_${TARGET}
        "${BUILD_ROOT}/Generated"
        CACHE STRING ""
        FORCE)
    set(GENERATED_PRIVATE_HEADER_PATH_${TARGET}
        "${GENERATED_PATH_${TARGET}}/Private"
        CACHE STRING ""
        FORCE)
    set(GENERATED_SOURCE_PATH_${TARGET}
        "${GENERATED_PATH_${TARGET}}/Sources"
        CACHE STRING ""
        FORCE)

    # Link the generated paths to the target
    mark_as_advanced(
        GENERATED_PATH_${TARGET}
        GENERATED_PRIVATE_HEADER_PATH_${TARGET}
        GENERATED_SOURCE_PATH_${TARGET}
    )

    # Short paths for this run
    set(HEADER_OUTPUT_PATH "${GENERATED_PRIVATE_HEADER_PATH_${TARGET}}/Fusion/Generated")
    set(SOURCE_OUTPUT_PATH "${GENERATED_SOURCE_PATH_${TARGET}}")

    # Generate a Private include path for the target to use.
    file(MAKE_DIRECTORY "${HEADER_OUTPUT_PATH}")
    # Generate a Source folder for generated files for the target to use.
    file(MAKE_DIRECTORY "${SOURCE_OUTPUT_PATH}")

    # Get the Git commit from the active repository
    execute_process(
        OUTPUT_VARIABLE GIT_COMMIT_LONG
        COMMAND git rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    string(STRIP ${GIT_COMMIT_LONG} GIT_COMMIT_LONG)

    execute_process(
        OUTPUT_VARIABLE GIT_COMMIT_SHORT
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    string(STRIP ${GIT_COMMIT_SHORT} GIT_COMMIT_SHORT)

    execute_process(
        OUTPUT_VARIABLE GIT_BRANCH
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    string(STRIP ${GIT_BRANCH} GIT_BRANCH)

    # Generate the configure files
    set(TEMPLATE_PATH "${CMAKE_SOURCE_DIR}/build/Templates/Versioning")

    configure_file(
        ${TEMPLATE_PATH}/Version.h.in
        ${HEADER_OUTPUT_PATH}/Version.h
        COPYONLY
    )

    configure_file(
        ${TEMPLATE_PATH}/Version.cpp.in
        ${SOURCE_OUTPUT_PATH}/Version.cpp
        NEWLINE_STYLE LF
        @ONLY
    )

    configure_file(
        ${TEMPLATE_PATH}/VersionCommand.h.in
        ${HEADER_OUTPUT_PATH}/VersionCommand.h
        COPYONLY
    )

    configure_file(
        ${TEMPLATE_PATH}/VersionCommand.cpp.in
        ${SOURCE_OUTPUT_PATH}/VersionCommand.cpp
        COPYONLY
    )

    # Set a variable to track the full-paths to the generated files that
    # need to be installed in the source/header lists.
    set(GENERATED_PRIVATE_HEADERS_${TARGET}
        ${HEADER_OUTPUT_PATH}/Version.h ${HEADER_OUTPUT_PATH}/VersionCommand.h
        CACHE STRING ""
        FORCE
    )
    set(GENERATED_SOURCES_${TARGET}
        ${SOURCE_OUTPUT_PATH}/Version.cpp ${SOURCE_OUTPUT_PATH}/VersionCommand.cpp
        CACHE STRING ""
        FORCE
    )

    # Output the generated variables
    mark_as_advanced(
        GENERATED_PRIVATE_HEADERS_${TARGET}
        GENERATED_SOURCES_${TARGET}
    )
endfunction()
