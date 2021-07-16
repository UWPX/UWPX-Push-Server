cmake_minimum_required(VERSION 3.16)

add_library(utils STATIC Utils.cpp
                         Utils.hpp)
# target_link_libraries(utils PRIVATE)