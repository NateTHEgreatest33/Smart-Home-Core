add_library(hal INTERFACE)

target_sources(hal INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pico_hal.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_hal.hpp
    ${CMAKE_CURRENT_LIST_DIR}/hal_interface.hpp
)

target_include_directories(hal INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(hal INTERFACE pico_stdlib hardware_gpio)
