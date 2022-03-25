cmake_minimum_required(VERSION 3.5)

project(shttp LANGUAGES C)

set(CMAKE_C_STANDARD 11)

add_executable(shttp main.c http.c)

target_link_libraries(shttp -lpthread)