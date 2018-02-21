cmake_minimum_required(VERSION 3.6)
project(pongServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp pongServer.cpp pongServer.h client.cpp client.h)
add_executable(pongServer ${SOURCE_FILES})

if(WIN32)
    target_link_libraries(pongServer wsock32 ws2_32)
endif()