cmake_minimum_required(VERSION 3.7.2)
project(PrakBS23 C)

set(CMAKE_C_STANDARD 99)

add_executable(PrakBS23 src/main.c src/main.h src/keyValStore.c src/keyValStore.h src/sub.c src/pub.c src/common.h src/inputHandler.c src/inputHandler.h src/Server.c src/Server.h)
