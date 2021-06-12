cmake_minimum_required(VERSION 3.16)

add_subdirectory(redis)

add_library(storage STATIC Serializer.cpp
                           Serializer.hpp
                           ConfigurationStorage.cpp
                           ConfigurationStorage.hpp)

target_link_libraries(storage PUBLIC nlohmann_json::nlohmann_json
                              PRIVATE logger)
                              
target_compile_features(storage PRIVATE cxx_std_17
                                        cxx_constexpr)
