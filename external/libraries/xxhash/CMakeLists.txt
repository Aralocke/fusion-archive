cmake_minimum_required(VERSION 3.10)

project(xxhash
    LANGUAGES C
)

set(XXHASH_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(xxhash)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(xxhash PRIVATE -DXXH_DEBUGLEVEL=1)
endif()

target_include_directories(xxhash PUBLIC ${XXHASH_SRC_ROOT})

cmake_host_system_information(RESULT PLATFORM QUERY OS_PLATFORM)
message(STATUS "xxHash Detected architecture: ${PLATFORM}")

set(XXHASH_HEADERS
    ${XXHASH_SRC_ROOT}/xxhash.h
)
set(XXHASH_SOURCES
    ${XXHASH_SRC_ROOT}/xxhash.c
)

# See the follwoing for the value on windows
# https://learn.microsoft.com/en-us/windows/win32/winprog64/wow64-implementation-details?redirectedfrom=MSDN

if("${PLATFORM}" STREQUAL "AMD64")
    set(XXHASH_SYSTEM_X86_64 ON)
elseif("${PLATFORM}" STREQUAL "x86_64")
    set(XXHASH_SYSTEM_X86_64 ON)
else()
    set(XXHASH_SYSTEM_X86_64 OFF)
endif()

if(XXHASH_SYSTEM_X86_64)
    message(STATUS "Enabling xxhash dispatch mode")
    set(XXHASH_SOURCES
        ${XXHASH_SOURCES}
        ${XXHASH_SRC_ROOT}/xxh_x86dispatch.c
    )
    target_compile_definitions(xxhash PRIVATE -DXXHSUM_DISPATCH=1)
endif()

target_sources(xxhash
    PRIVATE
        ${XXHASH_HEADERS}
        ${XXHASH_SOURCES}
)
