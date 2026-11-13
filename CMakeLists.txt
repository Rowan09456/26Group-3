# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w CACHE STRING "Pico board")
include(pico_sdk_import.cmake)

project(group3 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_executable(group3 Main/main.c)

target_sources(group3 PRIVATE 
    Drivers/Ambient/ssd1306_i2c.c
    Drivers/Ambient/bme280_spi.c
)

target_include_directories(group3 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers"
)

target_link_libraries(group3 PRIVATE 
    pico_stdlib 
    pico_cyw43_arch_none 
    hardware_i2c 
    hardware_spi
)


pico_add_extra_outputs(group3)




