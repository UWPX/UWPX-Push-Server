cmake_minimum_required(VERSION 3.16)

add_library(tcp_messages STATIC Parser.cpp
                                Parser.hpp
                                AbstractMessage.cpp
                                AbstractMessage.hpp
                                AbstractResponseMessage.cpp
                                AbstractResponseMessage.hpp
                                ErrorResponseMessage.cpp
                                ErrorResponseMessage.hpp
                                RequestTestPushMessage.cpp
                                RequestTestPushMessage.hpp
                                SetChannelUriMessage.cpp
                                SetChannelUriMessage.hpp
                                SetPushAccountsMessage.cpp
                                SetPushAccountsMessage.hpp
                                SuccessResponseMessage.cpp
                                SuccessResponseMessage.hpp
                                SuccessSetPushAccountsMessage.cpp
                                SuccessSetPushAccountsMessage.hpp
                                PushMessage.cpp
                                PushMessage.hpp
                                TestPushMessage.cpp
                                TestPushMessage.hpp)

target_link_libraries(tcp_messages PRIVATE logger nlohmann_json::nlohmann_json utils)
