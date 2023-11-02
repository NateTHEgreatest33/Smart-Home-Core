cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(PicoModule C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory( messageAPI/LoRa )


pico_sdk_init()

# add_subdirectory( LoRa )

add_executable( PicoModule
  main.cpp
)

# target_include_directories(PicoModule PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
# )

# Uncomment if you want to flash your board via picotool.
# pico_add_extra_outputs(PicoModule)

target_link_libraries(PicoModule
  pico_stdlib
  hardware_spi
  # Using Pico W
  pico_cyw43_arch_none
  loraAPI


)

# target_link_libraries(pico_stdlib)