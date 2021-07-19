include(FetchContent)

# Disable linting for qxmpp
clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
FetchContent_Declare(qxmpp
                     GIT_REPOSITORY         https://github.com/qxmpp-project/qxmpp.git
                     GIT_TAG v1.4.0
                     PATCH_COMMAND  git reset --hard
                        COMMAND     git apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/qxmpp_1.4.0_add_conan.diff)
set(BUILD_EXAMPLES OFF)
set(BUILD_TESTS OFF)
FetchContent_MakeAvailable(qxmpp)

restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

#PATCH_COMMAND "/usr/bin/git apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/qxmpp_1.4.0_add_conan.diff"
# "if [ -e has_been_patched.txt ]; then echo 'Already patched'; else ${git_cmd} apply ${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/qxmpp_1.4.0_add_conan.diff; touch has_been_patched.txt; fi"