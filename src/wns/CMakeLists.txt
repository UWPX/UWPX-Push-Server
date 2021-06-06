cmake_minimum_required(VERSION 3.16)

add_library(wns STATIC WnsClient.cpp
                       WnsClient.hpp
                       WnsToken.cpp
                       WnsToken.hpp)

target_link_libraries(wns PRIVATE logger cpr::cpr nlohmann_json::nlohmann_json)
