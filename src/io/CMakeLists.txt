cmake_minimum_required(VERSION 3.16)

add_library(io STATIC IoThread.cpp
                      IoThread.hpp)

target_link_libraries(io PRIVATE logger server nlohmann_json::nlohmann_json
                         PUBLIC cli)
