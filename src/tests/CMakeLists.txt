cmake_minimum_required(VERSION 3.16)

# Documentation: https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md

find_package(Catch2 REQUIRED)
include(Catch)

add_executable(tests Tests.cpp
                     MessageTests.cpp
                     CryptoUtilsTests.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2 nlohmann_json::nlohmann_json tcp_messages utils)
target_compile_definitions(tests PRIVATE CATCH_CONFIG_FAST_COMPILE)
catch_discover_tests(tests)
