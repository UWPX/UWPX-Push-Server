cmake_minimum_required(VERSION 3.16)

add_library(logger SHARED Logger.cpp)
target_link_libraries(logger PUBLIC glog::glog)
install(TARGETS logger)
