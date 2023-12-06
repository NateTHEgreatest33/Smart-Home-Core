cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(PicoModule C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory( lib/messageAPI )
add_subdirectory( lib/LoRa )
add_subdirectory( lib/button )

pico_sdk_init()

add_executable( PicoModule
  src/main.cpp
)


target_link_libraries(PicoModule
  pico_stdlib
  hardware_spi
  # Using Pico W
  pico_cyw43_arch_none

  # libraries
  loraAPI
  messageAPI
  buttonComponent

)
