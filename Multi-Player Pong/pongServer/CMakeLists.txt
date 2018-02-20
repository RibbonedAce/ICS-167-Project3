# cmake_minimum_required(VERSION <specify CMake version here>)
cmake_minimum_required(VERSION 3.9)
project(pongServer)

set(CMAKE_CXX_STANDARD 11)

link_libraries(ws2_32 wsock32)



add_executable(pongServer
        pongServer/cmake-build-debug/cmake_install.cmake
        pongServer/cmake-build-debug/CMakeCache.txt
        pongServer/cmake-build-debug/Makefile
        pongServer/cmake-build-debug/pongServer
        pongServer/cmake-build-debug/pongServer.cbp
        pongServer/client.cpp
        pongServer/client.h
        pongServer/CMakeLists.txt
        pongServer/main.cpp
        pongServer/pongServer.cpp
        pongServer/pongServer.h)

