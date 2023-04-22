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

cmake_minimum_required(VERSION 3.21)
project(Fusion
    LANGUAGES CXX C
    VERSION 0.1.0)

add_library(Fusion)
add_library(Fusion::Core ALIAS Fusion)

if(FUSION_MAIN_PROJECT)
    FSN_SET_TARGET_OUTPUT(Fusion "${FUSION_BUILD_ROOT}/libraries")
endif()

#
# Define the CXX version and add the defines for specific CXX support
#
target_compile_features(Fusion
    PUBLIC
        cxx_std_${FUSION_GLOBAL_CXX_STANDARD}
)
target_compile_definitions(Fusion
    PUBLIC
        -DFUSION_SUPPORTS_CXX20=${FUSION_SUPPORTS_CXX20}
        -DFUSION_SUPPORTS_CXX17=${FUSION_SUPPORTS_CXX17}
        -DFUSION_SUPPORTS_CXX14=${FUSION_SUPPORTS_CXX14}
)

#
# Configure the library directories that are needed by the project setup
#
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/public")
set(PROJECT_INTERNAL_DIR "${PROJECT_SOURCE_DIR}/private")
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

#
# Scan the directories for the correct files to add to the given source groups
#
# Source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB APPLE_SOURCES "${PROJECT_SOURCE_DIR}/platforms/apple/*.cpp")
file(GLOB POSIX_SOURCES "${PROJECT_SOURCE_DIR}/platforms/posix/*.cpp")
file(GLOB WIN_SOURCES "${PROJECT_SOURCE_DIR}/platforms/windows/*.cpp")
# Header Files
file(GLOB HEADERS "${PROJECT_INCLUDE_DIR}/Fusion/*.h")
file(GLOB FWD_HEADERS "${PROJECT_INCLUDE_DIR}/Fusion/Fwd/*.h")
file(GLOB IMPL_HEADERS "${PROJECT_INCLUDE_DIR}/Fusion/Impl/*.h")
# Header files (Global)
file(GLOB PRIVATE_HEADERS "${PROJECT_INTERNAL_DIR}/Fusion/Internal/*.h")

if(MSVC)
    # Set the headers folder. We split up the internal from the
    # public headers to prevent added confusion.
    source_group(
        TREE ${PROJECT_INCLUDE_DIR}/Fusion
        PREFIX "Header Files\\Public"
        FILES ${HEADERS} ${FWD_HEADERS} ${IMPL_HEADERS}
    )
    source_group(
        TREE ${PROJECT_INTERNAL_DIR}/Fusion/Internal
        PREFIX "Header Files\\Private"
        FILES ${PRIVATE_HEADERS}
    )
    # The the sources folders. We need to split the pltaform
    # specific with the base sources.
    source_group(
        TREE ${PROJECT_SOURCE_DIR}
        PREFIX "Source Files"
        FILES ${SOURCES}
    )
    source_group(
        TREE ${PROJECT_SOURCE_DIR}/platforms
        PREFIX "Source Files"
        FILES ${APPLE_SOURCES} ${POSIX_SOURCES} ${WIN_SOURCES}
    )
    # Set the folder for the library
    set_property(TARGET Fusion PROPERTY FOLDER "Libraries")
endif()

#
# Configure the include directories
#
target_sources(Fusion
    PRIVATE
        ${HEADERS} ${FWD_HEADERS} ${IMPL_HEADERS}
        ${PRIVATE_HEADERS}
        ${SOURCES} ${APPLE_SOURCES} ${POSIX_SOURCES} ${WIN_SOURCES}
)
target_include_directories(Fusion
    PUBLIC
        ${PROJECT_INCLUDE_DIR}
    PRIVATE
        ${PROJECT_INTERNAL_DIR}
)
#
# Set the correct defines if we have debug symbols.
# This will affect how and what we are able to log.
#
target_compile_definitions(Fusion
    PUBLIC
        -DFUSION_DEBUG=$<CONFIG:Debug,Asan>)
if(WIN32)
    target_compile_definitions(Fusion
        PUBLIC
            -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN
    )
endif()
#
# Configure link targets
#
list(APPEND fusion_link_targets "")
if(FUSION_BUILD_RUST_EXTENSIONS)
    list(APPEND fusion_link_targets Fusion::RustCore)
endif()
if(WIN32)
    list(APPEND fusion_link_targets ws2_32)
endif()

list(APPEND fusion_link_targets
    fmt     # fmtlib for logging
)

target_link_libraries(Fusion PUBLIC ${fusion_link_targets})
