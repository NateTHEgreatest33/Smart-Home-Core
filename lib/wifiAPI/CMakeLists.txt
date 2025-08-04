cmake_minimum_required(VERSION 3.13)
project(wifiAPI CXX ASM)

set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_library( wifiAPI
    wifiAPI.hpp
    wifiAPI.cpp
    )

target_link_libraries( wifiAPI
  pico_stdlib 
  pico_cyw43_arch_lwip_threadsafe_background
  consoleAPI
)

target_include_directories(wifiAPI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
