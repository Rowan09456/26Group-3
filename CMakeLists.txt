cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Target Board: Raspberry Pi Pico 2 W (RP2350)
set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(26Group3 C CXX ASM)
pico_sdk_init()

add_executable(26Group3
    Main/main.c
    Drivers/Ambient/ssd1306_i2c.c
    Drivers/Ambient/bme280_spi.c
    Drivers/Display/display.c
    Drivers/Reporting/reporting.c
    Drivers/Ultrasonic/timer.c
    Drivers/Ultrasonic/ultrasonic.c
    Drivers/Keypad/keypad.c
    Drivers/Servo/servo.c
    Drivers/ThroughBeam/through_beam.c
)

pico_set_program_name(26Group3 "26Group3_SecuritySystem")
pico_set_program_version(26Group3 "1.0")

# Enable USB stdio for serial console output
pico_enable_stdio_uart(26Group3 0)
pico_enable_stdio_usb(26Group3 1)

# Header Search Paths
target_include_directories(26Group3 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/Main
    ${CMAKE_CURRENT_LIST_DIR}/Drivers
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/Ambient
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/Display
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/Reporting
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/Ultrasonic
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/Keypad
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/Servo
    ${CMAKE_CURRENT_LIST_DIR}/Drivers/ThroughBeam
)

# Link RP2350 SDK Hardware Libraries
target_link_libraries(26Group3
    pico_stdlib
    hardware_gpio
    hardware_pwm
    hardware_clocks
    hardware_i2c
    hardware_spi
)

pico_add_extra_outputs(26Group3)