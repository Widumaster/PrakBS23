cmake_minimum_required(VERSION 3.7.2)
project(PrakBS23 C)

set(CMAKE_C_STANDARD 99)

add_executable(PrakBS23 main.c main.h keyValStore/keyValStore.c keyValStore/keyValStore.h requests.h requests.c socket.c commandStruct.c commandStruct.h keyValueStore.c threads.c threads.h )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(PrakBS23 PRIVATE Threads::Threads)