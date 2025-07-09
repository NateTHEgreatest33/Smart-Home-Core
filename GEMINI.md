# Gemini Project Context

This document provides context about the Smart-Home-Core project for the Gemini AI assistant.

## Project Overview

Smart-Home-Core is a modular C++ project for embedded systems, primarily targeting the Raspberry Pi Pico. It provides a suite of utilities and messaging formats for building smart home applications. The project is structured in a style similar to DO-178 projects, with requirements, tests, and code tracing to each other.

## Build System

The project uses CMake. The main `CMakeLists.txt` is in the root directory.

- **To configure the project:** `cmake -S . -B build`
- **To build the main application:** `make -C build`

The module tests have a separate build configuration. See the Testing section.

## Project Structure

- `src`: Contains the main application source code.
- `lib`: Contains various library modules.
- `vnv`: Contains verification and validation (testing) code.
  - `mod`: C++ module tests using Google Test.
  - `system`: Python-based system tests.
- `reqs`: Contains project requirements documentation.
- `tools`: Contains helper tools and scripts.

## Key Technologies

- **C++20**: The primary programming language.
- **CMake**: The build system.
- **Google Test**: For C++ module and unit testing.
- **Raspberry Pi Pico SDK**: The underlying hardware abstraction layer.
- **Python**: Used for system-level testing.

## Testing

The project has two types of tests:

1.  **Module Tests (C++/Google Test):**
    - **Location:** `vnv/mod/tests`
    - **Purpose:** To test individual C++ modules in isolation (unit tests). This is where we mock hardware dependencies.
    - **To Run:**
        1. `cmake -S vnv/mod -B build_mod_tests`
        2. `make -C build_mod_tests`
        3. `./build_mod_tests/run_mod_tests`

2.  **System Tests (Python):**
    - **Location:** `vnv/system`
    - **Purpose:** To test the fully built application running on the hardware.
    - **To Run:** See the READMEs and scripts in the `vnv/system` directory.

## Important Files

- `CMakeLists.txt`: The main CMake build script.
- `vnv/mod/CMakeLists.txt`: The build script for module tests.
- `pico_sdk_import.cmake`: Imports the Raspberry Pi Pico SDK.
- `src/main.cpp`: The main application entry point.