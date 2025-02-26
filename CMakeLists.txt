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
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
project(blink C CXX ASM)
pico_sdk_init()

# Primeiro, criar o diretório para os arquivos gerados
file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

# Segundo, criar o executável
add_executable(blink 
    blink.c
    inc/ssd1306.c
)

# Terceiro, gerar o cabeçalho PIO para WS2812 (depois de criar o target)
pico_generate_pio_header(blink ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

# Resto das configurações
target_link_libraries(blink 
    pico_stdlib 
    hardware_i2c
    hardware_pio
    hardware_adc
    hardware_pwm
)

pico_enable_stdio_usb(blink 1)
pico_enable_stdio_uart(blink 0)

target_include_directories(blink PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/generated
)

pico_set_program_name(blink "blink")
pico_set_program_version(blink "0.1")

pico_add_extra_outputs(blink)