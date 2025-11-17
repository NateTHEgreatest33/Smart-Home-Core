# cmake_minimum_required(VERSION 3.5)
set(TEST_TCP_SERVER_IP "10.0.0.187") 
set(WIFI_PASSWORD "Ranger2023") 
set(WIFI_SSID "PartyPlanningCommittee") 


project(wifiAPI CXX ASM)

set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_library( wifiAPI
    wifi.hpp
    wifi.tpp
    lwipopts.h
    
    )

target_compile_definitions(wifiAPI PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        TEST_TCP_SERVER_IP=\"${TEST_TCP_SERVER_IP}\"
        )

target_link_libraries( wifiAPI
  pico_stdlib 
#   messageAPI
#   utilLib
#   consoleAPI

)

target_include_directories( wifiAPI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" )
