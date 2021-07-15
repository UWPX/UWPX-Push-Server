cmake_minimum_required(VERSION 3.16)

add_library(server STATIC PushServer.cpp
                          PushServer.hpp)

target_link_libraries(server PRIVATE logger storage wns tcp tcp_messages)
