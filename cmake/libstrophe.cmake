include(ExternalProject)

# Based on: https://github.com/iotivity/iotivity-xmpp/blob/master/CMakeLists.txt

# Disable linting for libstrophe
clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
set(LIBSTROPHE_LIBRARY_INSTALL_PATH ${CMAKE_BINARY_DIR}/strophe/lib/${CMAKE_FIND_LIBRARY_PREFIXES}strophe.so)
ExternalProject_Add(libstrophe
    URL https://github.com/strophe/libstrophe/releases/download/0.10.1/libstrophe-0.10.1.tar.gz
    URL_HASH SHA256=4918c47029ecdea2deab4b0f9336ca4a8bb12c28b72b2cec397d98664b94c771
    SOURCE_DIR libstrophe-prefix/src/libstrophe-master
    BUILD_IN_SOURCE ON
    CONFIGURE_COMMAND ./bootstrap.sh
              COMMAND ./configure --prefix ${CMAKE_BINARY_DIR}/strophe
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    BUILD_BYPRODUCTS ${LIBSTROPHE_LIBRARY_INSTALL_PATH}) # This line fixes ninja not being able to build libstrophe
ExternalProject_Get_Property(libstrophe source_dir)
add_library(strophe UNKNOWN IMPORTED)
target_include_directories(strophe INTERFACE ${CMAKE_BINARY_DIR}/strophe/include)
set_property(TARGET strophe PROPERTY IMPORTED_LOCATION ${LIBSTROPHE_LIBRARY_INSTALL_PATH})
add_dependencies(strophe libstrophe)

restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)