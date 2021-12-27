cmake_minimum_required(VERSION 3.16)

set(PROJECT_DESCRIPTION "UWPX XMPP push server.")

project("UWPX_Push_Server" 
            VERSION 0.0.0
            DESCRIPTION "${PROJECT_DESCRIPTION}"
            HOMEPAGE_URL "https://github.com/UWPX/UWPX-Push-Server")
set(VERSION_NAME "dev")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

macro(uwpx_push_server_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

message(STATUS "C++ UWPX Push Server Options")
message(STATUS "=======================================================")
uwpx_push_server_option(UWPX_PUSH_SERVER_STATIC_ANALYZE "Set to ON to enable the GCC 10 static analysis. If enabled, UWPX_PUSH_SERVER_ENABLE_LINTING has to be disabled." OFF)
uwpx_push_server_option(UWPX_PUSH_SERVER_ENABLE_LINTING "Set to ON to enable clang linting. If enabled, UWPX_PUSH_SERVER_STATIC_ANALYZE has to be disabled." ON)
uwpx_push_server_option(UWPX_PUSH_SERVER_BUILD_TESTS "Set to ON to enable building tests." ON)
message(STATUS "=======================================================")

option(BUILD_TESTING "Build tests" ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(sanitizer)
include(gcc_analyze)
include(clear_variable)

if (UWPX_PUSH_SERVER_ENABLE_LINTING)
    message(STATUS "Enabling linting")
    include(clang-tidy)
else()
    message(STATUS "Linting is disabled")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT isMultiConfig)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${ALLOWED_BUILD_TYPES}")
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST ALLOWED_BUILD_TYPES)
        message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
    endif()
else ()
    unset(CMAKE_BUILD_TYPE)
    foreach(TYPE ${ALLOWED_BUILD_TYPES})
    if (NOT ${TYPE} IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES ${TYPE})
    endif()  
    endforeach()  
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(CONAN_CONFIGS "Release;Debug;RelWithDebInfo")
if(NOT CMAKE_BUILD_TYPE IN_LIST CONAN_CONFIGS)
    set(CONAN_BUILD_TYPE "Debug")
else()
    set(CONAN_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()

conan_cmake_run(REQUIRES nlohmann_json/3.9.1
                         glog/0.4.0
                         cpr/1.6.2
                         redis-plus-plus/1.2.3
                         catch2/2.13.6
                         libsodium/1.0.18
                         cppserver/1.0.1.0
                         ctre/3.4.1
                         openssl/1.1.1l # Override: 'cppserver/1.0.1.0' requires 'openssl/1.1.1k' while 'libcurl/7.79.0' requires 'openssl/1.1.1l'
                GENERATORS cmake_find_package
                BUILD missing)
conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD redis-plus-plus
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS "${settings};redis-plus-plus:compiler.cppstd=17") # Workaround for now: https://github.com/conan-io/conan-center-index/pull/6349

find_package(nlohmann_json REQUIRED)
find_package(glog REQUIRED)
find_package(cpr REQUIRED)
find_package(redis++ REQUIRED)
find_package(libsodium REQUIRED)
find_package(cppserver REQUIRED)
find_package(ctre REQUIRED)

include(libstrophe)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)
