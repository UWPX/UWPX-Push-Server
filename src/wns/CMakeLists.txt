cmake_minimum_required(VERSION 3.16)

add_library(wns STATIC Wns.cpp
                       Wns.hpp)

target_link_libraries(wns PRIVATE logger)
