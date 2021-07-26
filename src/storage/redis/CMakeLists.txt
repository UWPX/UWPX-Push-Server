cmake_minimum_required(VERSION 3.16)

add_library(storage_redis STATIC RedisClient.cpp
                                 RedisClient.hpp)

target_link_libraries(storage_redis PRIVATE logger storage redis++::redis++ tcp_messages utils)
