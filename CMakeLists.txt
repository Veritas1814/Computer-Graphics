cmake_minimum_required(VERSION 3.16)
project(EmptyWindow)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

include_directories(include)

add_executable(empty_window
        main.cpp
        src/glad.c
)

target_link_libraries(empty_window
        OpenGL::GL
        glfw
)
