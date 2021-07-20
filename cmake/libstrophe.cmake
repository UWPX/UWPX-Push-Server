include(ExternalProject)

# Based on: https://github.com/iotivity/iotivity-xmpp/blob/master/CMakeLists.txt

# Disable linting for libstrophe
clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
ExternalProject_Add(libstrophe
    URL https://github.com/strophe/libstrophe/releases/download/0.10.1/libstrophe-0.10.1.tar.gz
    URL_HASH SHA256=4918c47029ecdea2deab4b0f9336ca4a8bb12c28b72b2cec397d98664b94c771
    SOURCE_DIR libstrophe-prefix/src/libstrophe-master
    BUILD_IN_SOURCE ON
    CONFIGURE_COMMAND pwd
        COMMAND ./bootstrap.sh
        COMMAND ./configure
    BUILD_COMMAND make
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
ExternalProject_Get_Property(libstrophe source_dir)
include_directories(
    ${source_dir}/
)
ExternalProject_Get_Property(libstrophe binary_dir)
add_library(strophe UNKNOWN IMPORTED)
set_property(TARGET strophe PROPERTY IMPORTED_LOCATION ${binary_dir}/.libs/${CMAKE_FIND_LIBRARY_PREFIXES}strophe.so)
add_dependencies(strophe libstrophe)

restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)