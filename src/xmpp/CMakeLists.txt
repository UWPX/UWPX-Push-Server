cmake_minimum_required(VERSION 3.16)

add_library(xmpp STATIC XmppClient.cpp
                        XmppClient.hpp)

target_link_libraries(xmpp PRIVATE logger storage storage_redis
                           PUBLIC strophe)
