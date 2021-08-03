cmake_minimum_required(VERSION 3.16)

add_library(server STATIC PushServer.cpp
                          PushServer.hpp)

target_link_libraries(server PRIVATE logger storage storage_redis wns tcp_messages ctre::ctre
                             PUBLIC tcp xmpp)
