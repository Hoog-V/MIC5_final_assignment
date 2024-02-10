cmake_minimum_required(VERSION 3.6)
set(PICO_BOARD pico_w)
set(PICO_SDK_FETCH_FROM_GIT ON)
# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(MIC5_Final_assignment C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif ()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)


if (PICO_CYW43_SUPPORTED) # set by BOARD=pico-w
    if (NOT TARGET pico_cyw43_arch)
        message("Skipping build as support is not available")
    else ()
        add_executable(led_matrix src/main.c)
        # enable usb output, disable uart output
        pico_enable_stdio_usb(led_matrix 1)
        pico_enable_stdio_uart(led_matrix 0)

        target_include_directories(led_matrix PUBLIC
                ${CMAKE_CURRENT_LIST_DIR}/src
        )
        
        target_link_libraries(led_matrix  pico_cyw43_arch_lwip_poll
        pico_stdlib
        pico_lwip_mbedtls
        pico_mbedtls)

        pico_add_extra_outputs(led_matrix)


    endif ()
endif()
