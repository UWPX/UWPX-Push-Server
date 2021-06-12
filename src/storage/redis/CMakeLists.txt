cmake_minimum_required(VERSION 3.16)

add_library(storage_redis STATIC RedisClient.cpp
                                 RedisClient.hpp)

# Include redis++ and hiredis as system library headers to prevent it showing compiler warnings:
include_directories(SYSTEM ${redis++_INCLUDE_DIR} ${hiredis_INCLUDE_DIR})
target_link_libraries(storage_redis PRIVATE logger storage redis++ hiredis)
