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

if(DEFINED _FUSION_MAIN_PROJECT)
    message(FATAL_ERROR "Fusion project included multiple times")
endif()
if(FUSION_MAIN_PROJECT)
    set(_FUSION_MAIN_PROJECT 1)
endif()
set(_FUSION_CONFIG_PROJECT 1)

if(CMAKE_BUILD_TYPE STREQUAL "")
    message(FATAL_ERROR "Setting CMAKE_BUILD_TYPE is required")
endif()

if(FUSION_MAIN_PROJECT)
    if(NOT FUSION_BUILD_ROOT)
        message(FATAL_ERROR "The 'FUSION_BUILD_ROOT' is not specified")
    endif()
    message(STATUS "Configuring Fusion as the main project builder")
else()
    message(STATUS "Configuring Fusion for inclusion as a secondary library")
endif()
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    if(WIN32)
        message(WARNING "Compile commands database will not be automatically generaated on Windows")
    endif()
endif()

# Include macros which are used for setting up the project.
include(Macros/Compiler)
include(Macros/Configs)
include(Macros/Projects)

# Include components which are required for the libraries to properly configure for the build.
# Nothing at this stage will modify the project in a global way if the library
# is being embedded into another.
include(Common/Platform)
include(Common/Compiler)
include(Bindings/Python/PythonBuild)
if(FUSION_BUILD_RUST_EXTENSIONS)
    include(Rust/RustBuild)
endif()

if(NOT FUSION_MAIN_PROJECT)
    return()
endif()

# At this point everything included assumes that the Fusion library is being built
# as a main project and it should include everything to configure itself.
include(Common/Asan)
include(Common/Tsan)
include(Common/Ubsan)
include(Common/Versioning)

list(APPEND FUSION_DEBUG_CONFIGS DEBUG RELWITHDEBINFO)
list(APPEND FUSION_RELEASE_CONFIGS RELEASE MINSIZEREL)

if(FUSION_SUPPORTS_ASAN)
    list(APPEND FUSION_DEBUG_CONFIGS ASAN)
endif()
if(FUSION_SUPPORTS_TSAN)
    list(APPEND FUSION_DEBUG_CONFIGS TSAN)
endif()
if(FUSION_SUPPORTS_UBSAN)
    list(APPEND FUSION_DEBUG_CONFIGS UBSAN)
endif()

if(WIN32)
    include(Platforms/Windows REQUIRED)
elseif(UNIX)
    include(Platforms/Unix REQUIRED)
else()
    message(FATAL_ERROR "Unsupported platform \'${CMAKE_SYSTEM_NAME}\'")
endif()
