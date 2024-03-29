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

if(NOT _FUSION_MAIN_PROJECT)
    return()
endif()
if(NOT WIN32)
    message(FATAL_ERROR "Windows platform file included on a non-WIN32 system")
endif()

if(FUSION_SUPPORTS_ASAN)
    FSN_CREATE_BUILD_CONFIG(Asan ${ADDRESS_SANITIZER_COMPILER_FLAG} ${ADDRESS_SANITIZER_LINKER_FLAG})
endif()
if(FUSION_SUPPORTS_UBSAN)
    FSN_CREATE_BUILD_CONFIG(Ubsan ${UNDEFINED_SANITIZER_COMPILER_FLAG} "")
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Asan>,$<CONFIG:RelWithDebInfo>>:Debug>")
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")

# Set Debug specific flags
# /Z7 - produce .obj files for debugging
# /FC - use full pathnames in debugging
# /RTC1 - enable runtime-error checking
# /Od - disables most optimizations
# /Oy- disables frame pointer ommissions
FSN_ADD_COMPILER_FLAGS(DEBUG CXX /Z7 /FC /Od /Oy-)
FSN_ADD_COMPILER_FLAGS(DEBUG C /Z7 /FC /Od /Oy-)

# Do not set runtime-error checking if we are buidling for ASAN
# This is a Windows specific requirement.
# FSN_ADD_COMPILER_FLAGS(DEBUG CXX /RTC1- /GS-)
# FSN_ADD_COMPILER_FLAGS(DEBUG C /RTC1- /GS-)

# Disable some Windows compilation warnings by default
if(MSVC)
    set(MSVC_DISABLE_WARNINGS
        /wd4005  # Disables warning for macros that are defined multiple times
    )
    FSN_ADD_COMPILER_FLAGS(CXX ${MSVC_DISABLE_WARNINGS})
    FSN_ADD_COMPILER_FLAGS(C ${MSVC_DISABLE_WARNINGS})
endif()

# Set global compiler flags. These will affect all configurations
# /MP - Enable multi-processor builds
# /GS - enable buffer overflow checking
# /GR- - Disable Runtime-Type-Checking (RTTI)
# /DEBUG - Generate .pdb files for debugging
# /LARGEADDRESSAWARE - mark executable for using >2GB addresses
FSN_ADD_COMPILER_FLAGS(CXX DEBUG /GS)
FSN_ADD_COMPILER_FLAGS(CXX /MP /GR-)
FSN_ADD_LINKER_FLAGS(/DEBUG /LARGEADDRESSAWARE)

# Global Definitions that effect every project built.
add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING=1)
add_definitions(-DUNICODE=1 -D_UNICODE=1)
add_definitions(-DWIN32_LEAN_AND_MEAN=1)
add_definitions(-DNOMINMAX=1)
