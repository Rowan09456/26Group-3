cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w CACHE STRING "Pico board")
include(pico_sdk_import.cmake)

project(group3 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_executable(group3 Main/main.c)
target_link_libraries(group3 PRIVATE pico_stdlib pico_cyw43_arch_none)
pico_add_extra_outputs(group3)
