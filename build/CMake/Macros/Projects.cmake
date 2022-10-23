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

#
# FSN_SET_TARGET_OUTPUT_PROPERTY
#
function(FSN_SET_TARGET_OUTPUT_PROPERTY TARGET CONFIG CFG DIRECTORY)
    if(NOT FUSION_MAIN_PROJECT)
        return()
    endif()
    set(TARGET_DIR "${DIRECTORY}/${TARGET}/${FUSION_BUILD_PLATFORM}-${FUSION_BUILD_ARCH}-${CFG}")
    if(FUSION_COMPILER_NAME)
        set(TARGET_DIR "${TARGET_DIR}-${FUSION_COMPILER_NAME}")
    endif()
    set_target_properties(${TARGET}
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${TARGET_DIR}"
            LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${TARGET_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${TARGET_DIR}/bin"
    )
    if(MSVC)
        set_target_properties(${TARGET}
            PROPERTIES
                CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG} "${TARGET_DIR}"
        )
    endif()
endfunction()

#
# FSN_SET_TARGET_OUTPUT
#
function(FSN_SET_TARGET_OUTPUT TARGET DIRECTORY)
    if(NOT FUSION_MAIN_PROJECT)
        return()
    endif()
    if(FUSION_SUPPORTS_ASAN)
        FSN_SET_TARGET_OUTPUT_PROPERTY(${TARGET} ASAN "Asan" ${DIRECTORY})
    endif()
    if(FUSION_SUPPORTS_UBSAN)
        FSN_SET_TARGET_OUTPUT_PROPERTY(${TARGET} UBSAN "Ubsan" ${DIRECTORY})
    endif()
    FSN_SET_TARGET_OUTPUT_PROPERTY(${TARGET} DEBUG "Debug" ${DIRECTORY})
    FSN_SET_TARGET_OUTPUT_PROPERTY(${TARGET} RELEASE "Release" ${DIRECTORY})
    FSN_SET_TARGET_OUTPUT_PROPERTY(${TARGET} MINSIZEREL "Public" ${DIRECTORY})
    FSN_SET_TARGET_OUTPUT_PROPERTY(${TARGET} RELWITHDEBINFO "ReleaseDbg" ${DIRECTORY})
endfunction()
