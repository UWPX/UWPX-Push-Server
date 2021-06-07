cmake_minimum_required(VERSION 3.16)

set(MAIN_EXECUTABLE ${PROJECT_NAME})

add_subdirectory(logger)
add_subdirectory(storage)
add_subdirectory(io)
add_subdirectory(wns)
add_subdirectory(server)

add_executable(${MAIN_EXECUTABLE} WIN32 main.cpp)

target_link_libraries(${MAIN_EXECUTABLE} PRIVATE logger storage server io)
set_property(SOURCE main.cpp PROPERTY COMPILE_DEFINITIONS UWPX_PUSH_SERVER_VERSION="${PROJECT_VERSION}" UWPX_PUSH_SERVER_NAME="${VERSION_NAME}")
install(TARGETS ${MAIN_EXECUTABLE} RUNTIME DESTINATION)
