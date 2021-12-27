cmake_minimum_required(VERSION 3.16)

add_library(utils STATIC CryptoUtils.cpp
                         CryptoUtils.hpp
                         date.hpp)
target_link_libraries(utils PUBLIC libsodium::libsodium)