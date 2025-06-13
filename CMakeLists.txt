cmake_minimum_required(VERSION 3.13)

# This option will control whether we are building for the target or for the host tests.
# To build tests, configure with: cmake -DBUILD_TESTING=ON ..
option(BUILD_TESTING "Build the unit tests" OFF)

if(BUILD_TESTING)
    # --- HOST TEST BUILD CONFIGURATION ---
    message(STATUS "Configuring for HOST build (tests enabled)")
    project(SmartHomeCore_Test C CXX) # Project for the host build

    set(CMAKE_CXX_STANDARD 20)

    # Add libraries that need to be tested.
    # Note: Libraries with hardware-specific code may need their own modifications
    # to compile on the host (e.g., using mocks or stubs).
    # add_subdirectory(lib/messageAPI)
    # add_subdirectory(lib/button)
    # add_subdirectory(lib/util)
    # add_subdirectory(lib/console)
    # add_subdirectory(lib/LoRa) # Likely has hardware dependencies, excluded for now

    # Fetch GoogleTest using FetchContent
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    # Enable CTest
    enable_testing()

    # Add the test executable
    add_executable(
        unit_tests
        vnv/mod/example_test.cpp
    )

    # Link the test executable against GoogleTest and the libraries under test.
    target_link_libraries(unit_tests PRIVATE
        GTest::gtest_main
        # messageAPI
        # buttonComponent
        # utilLib
        # consoleAPI
    )

    # Automatically discover tests from the executable
    include(GoogleTest)
    gtest_discover_tests(unit_tests)

else()
    # --- EMBEDDED TARGET BUILD CONFIGURATION ---
    message(STATUS "Configuring for EMBEDDED build (tests disabled)")

    include(pico_sdk_import.cmake)
    project(SmartHomeCore C CXX ASM)

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 20)

    add_subdirectory(lib/messageAPI)
    add_subdirectory(lib/LoRa)
    add_subdirectory(lib/button)
    add_subdirectory(lib/util)
    add_subdirectory(lib/console)

    pico_sdk_init()

    add_executable(SmartHomeCore
        src/main.cpp
        src/background_task.cpp
        src/background_task.hpp
        src/test_mode.hpp
        src/test_mode.cpp
    )

    # Debugger Attached
    pico_enable_stdio_usb(SmartHomeCore 0)
    pico_enable_stdio_uart(SmartHomeCore 1)

    target_link_libraries(SmartHomeCore
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

    #make uf2
    pico_add_extra_outputs(SmartHomeCore)

endif()