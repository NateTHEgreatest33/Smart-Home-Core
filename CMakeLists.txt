cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(PicoModule C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory( lib/messageAPI )
add_subdirectory( lib/LoRa )
add_subdirectory( lib/button )
add_subdirectory( lib/util )
add_subdirectory( lib/console )

pico_sdk_init()

add_executable( PicoModule
  src/main.cpp

  src/background_task.cpp
  src/background_task.hpp
)

# Debugger Attached
pico_enable_stdio_usb( PicoModule 0)
pico_enable_stdio_uart( PicoModule 1)

# # No Debugger
# pico_enable_stdio_usb( {PROJECTNAME} 1)

target_link_libraries(PicoModule
  pico_stdlib
  hardware_spi
  pico_multicore
  # Using Pico W
  pico_cyw43_arch_none

  # libraries
  loraAPI
  messageAPI
  buttonComponent
  utilLib
  consoleAPI

)
