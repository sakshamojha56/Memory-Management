cmake_minimum_required(VERSION 3.10)
project(memory_manager)

set(CMAKE_CXX_STANDARD 17)

add_executable(memory_manager
    src/main.cpp
    src/MemoryManager.cpp
)
