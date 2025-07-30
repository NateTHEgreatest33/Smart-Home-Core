cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(SmartHomeCore C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# disable all compiler omptimzations for easier debugging
add_compile_options(-O0)

add_subdirectory( lib/messageAPI )
add_subdirectory( lib/wifiAPI )
add_subdirectory( lib/button )
add_subdirectory( lib/util )
add_subdirectory( lib/console )
add_subdirectory( lib/mailbox )

pico_sdk_init()

add_executable( SmartHomeCore
  src/main.cpp

  src/background_task.cpp
  src/background_task.hpp

  src/test_mode.hpp
  src/test_mode.cpp

  src/mailbox_map.hpp
)

# Debugger Attached
pico_enable_stdio_usb( SmartHomeCore 0)
pico_enable_stdio_uart( SmartHomeCore 1)

# # No Debugger
# pico_enable_stdio_usb( {PROJECTNAME} 1)

target_link_libraries(SmartHomeCore
  pico_stdlib
  pico_multicore
  # Using Pico W
  pico_cyw43_arch_lwip_threadsafe_background

  # libraries
  wifiAPI
  messageAPI
  buttonComponent
  utilLib
  consoleAPI
  mailboxAPI

)

# add_library(set-app-flags INTERFACE)
# target_compile_options(set-app-flags INTERFACE -O0)
# target_link_libraries(SmartHomeCore PRIVATE set-app-flags)

# target_compile_options(SmartHomeCore INTERFACE -O0)
# set(PICO_DEOPTIMIZED_DEBUG ON)      # Disable optimizations.


#make uf2
pico_add_extra_outputs(SmartHomeCore)

# add_library(set-app-flags INTERFACE)
# target_compile_options(set-app-flags INTERFACE -O0)
# target_link_libraries(app PRIVATE set-app-flags)