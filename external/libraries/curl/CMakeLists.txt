project(curl
    LANGUAGES C
    VERSION 8.1.2
)

option(CURL_BUILD_TOOL "Build the curl command line tool" ON)
option(CURL_BUILD_DEBUG "Build debug tools into libcurl for Debug builds" ON)

set(CURL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CURL_PUBLIC_INCLUDE ${CURL_ROOT}/include)
set(CURL_LIB ${CURL_ROOT}/lib)
set(CURL_SRC ${CURL_ROOT}/src)

file(STRINGS ${CURL_PUBLIC_INCLUDE}/curl/curlver.h CURL_VERSION_H_CONTENTS REGEX "#define LIBCURL_VERSION( |_NUM )")
string(REGEX MATCH "#define LIBCURL_VERSION \"[^\"]*"
  CURL_VERSION ${CURL_VERSION_H_CONTENTS})
string(REGEX REPLACE "[^\"]+\"" "" CURL_VERSION ${CURL_VERSION})
string(REGEX MATCH "#define LIBCURL_VERSION_NUM 0x[0-9a-fA-F]+"
  CURL_VERSION_NUM ${CURL_VERSION_H_CONTENTS})
string(REGEX REPLACE "[^0]+0x" "" CURL_VERSION_NUM ${CURL_VERSION_NUM})

message(STATUS "curl version=[${CURL_VERSION}]")

add_library(libcurl_static STATIC)
add_library(libcurl ALIAS libcurl_static)

target_include_directories(libcurl_static
    PUBLIC ${CURL_PUBLIC_INCLUDE}
    PRIVATE ${CURL_LIB}
)

list(APPEND LIBCURL_LINK_TARGETS
    c-ares
    crypto
    ssl
    zlibstatic
    libzstd_static
)

if(WIN32)
    target_compile_definitions(libcurl_static
        PUBLIC
            -DUNICODE
            -D_UNICODE
    )
    list(APPEND LIBCURL_LINK_TARGETS
        ws2_32
        advapi32
        crypt32
    )
endif()

# Setup the HTTP_ONLY values
target_compile_definitions(libcurl_static
    PUBLIC
        # Enable/Disable specific features
        -DHTTP_ONLY=1
        -DCURL_DISABLE_ALTSVC=1
        -DCURL_DISABLE_FILE=1
        -DCURL_DISABLE_DICT=1
        -DCURL_DISABLE_DOH=1
        -DCURL_DISABLE_NETRC=1
        -DCURL_DISABLE_SMB=1
        -DCURL_DISABLE_FTP=1
        -DCURL_DISABLE_LDAP=1
        -DCURL_DISABLE_LDAPS=1
        -DCURL_DISABLE_TELNET=1
        -DCURL_DISABLE_DICT=1
        -DCURL_DISABLE_TFTP=1
        -DCURL_DISABLE_RTSP=1
        -DCURL_DISABLE_POP3=1
        -DCURL_DISABLE_IMAP=1
        -DCURL_DISABLE_SMTP=1
        -DCURL_DISABLE_GOPHER=1
        -DCURL_DISABLE_PROXY=1
        # Enable/Disable Specific Flags
        -DBUILDING_LIBCURL=1
        -DCURL_STATICLIB=1
        -DHAVE_BORINGSSL=1
        -DHAVE_CONFIG_H=1
        -DHAVE_ZLIB=1
        -DHAVE_ZSTD=1
)

# The following sets up the platform specific configurations.
# These need to be loaded per-platform but the HAVE_CONFIG_H define
# should be global on every platform.
if(WIN32)
    target_include_directories(libcurl_static
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/config/win32
    )
elseif(UNIX)
    if(FUSION_PLATFORM_LINUX)
        target_include_directories(libcurl_static
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/config/linux
        )
    elseif(FUSION_PLATFORM_OSX)
        target_include_directories(libcurl_static
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/config/apple
        )
    endif()
    target_compile_definitions(libcurl_static
        PUBLIC
            -DCURL_HIDDEN_SYMBOLS
    )
endif()

# Glob the files and build the folder structure
file(GLOB_RECURSE PUBLIC_HEADERS ${CURL_PUBLIC_INCLUDE} "*/include/*.h")
file(GLOB_RECURSE LIB_HEADERS ${CURL_LIB} "*/lib/*.h")
file(GLOB_RECURSE LIB_SOURCES ${CURL_LIB} "*/lib/*.c")

if(WIN32)
    source_group("Header Files\\Public" FILES ${PUBLIC_HEADERS})
    source_group("Header Files\\Internal" FILES ${LIB_HEADERS})
    source_group("Source Files" FILES ${LIB_SOURCES})
endif()

target_sources(libcurl_static
    PRIVATE
        ${PUBLIC_HEADERS}
        ${LIB_HEADERS}
        ${LIB_SOURCES}
)

target_link_libraries(libcurl_static PUBLIC ${LIBCURL_LINK_TARGETS})
