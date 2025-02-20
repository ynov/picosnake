# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Set the closest board type (e.g., pico, pico2, etc...)
set(PICO_BOARD pico CACHE STRING "Board type")

set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/../../shared_lib/FreeRTOS-Kernel)

include_directories(include)
include_directories(${FREERTOS_KERNEL_PATH}/include)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Pull in FreeRTOS Kernel
set(PICO_BOARDS_RP2040 "pico;pico_w")
set(PICO_BOARDS_RP2350 "pico2;pico2_w")

if(${PICO_BOARD} IN_LIST PICO_BOARDS_RP2040)
    # For RP2040 (ARM Cortex-M0+)
    include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
elseif(${PICO_BOARD} IN_LIST PICO_BOARDS_RP2350)
    # For RP2350 (ARM Cortex-M33)
    include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2350_ARM_NTZ/FreeRTOS_Kernel_import.cmake)
endif()

project(picosnake C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(picosnake
    main.c
    serial.c
    app.c
    ledmatrix.c
    framebuffer.c
    snake.c
)

pico_enable_stdio_usb(picosnake 1)
pico_enable_stdio_uart(picosnake 0)

target_include_directories(picosnake PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# pull in common dependencies
target_link_libraries(picosnake pico_stdlib hardware_spi hardware_adc FreeRTOS-Kernel FreeRTOS-Kernel-Heap4)


# create map/bin/hex file etc.
pico_add_extra_outputs(picosnake)

# add url via pico_set_program_url
