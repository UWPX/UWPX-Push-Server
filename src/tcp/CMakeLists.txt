cmake_minimum_required(VERSION 3.16)

add_subdirectory(messages)

add_library(tcp STATIC TcpServer.cpp
                       TcpServer.hpp
                       ClientSslSession.cpp
                       ClientSslSession.hpp
                       SslServer.cpp
                       SslServer.hpp)

target_link_libraries(tcp PRIVATE logger storage
                          PUBLIC cppserver::cppserver)
