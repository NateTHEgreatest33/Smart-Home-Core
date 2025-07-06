# Gemini Project Context

This document provides context about the Smart-Home-Core project for the Gemini AI assistant.

## Project Overview

Smart-Home-Core is a modular C++ project for embedded systems, primarily targeting the Raspberry Pi Pico. It provides a suite of utilities and messaging formats for building smart home applications. The project is structured in a style similar to DO-178 projects, with requirements, tests, and code tracing to each other.

## Build System

The project uses CMake for building. The main `CMakeLists.txt` file is in the root directory. To build the project, you will likely need to use the `cmake` and `make` commands in the `build` directory.

## Project Structure

The project is organized into the following main directories:

- `src`: Contains the main application source code.
- `lib`: Contains various library modules, such as LoRa, messageAPI, and console.
- `vnv`: Contains verification and validation (testing) code, including Python-based system tests.
- `reqs`: Contains project requirements documentation.
- `tools`: Contains helper tools and scripts.

## Key Technologies

- **C++20**: The primary programming language.
- **CMake**: The build system.
- **Raspberry Pi Pico SDK**: The underlying hardware abstraction layer.
- **Python**: Used for system-level testing.

## Development Workflow

1.  **Modify code**: Make changes to the C++ source files in `src` or `lib`.
2.  **Build**: Run `cmake` and `make` in the `build` directory to compile the code.
3.  **Test**: Run the Python system tests in the `vnv/system` directory to verify functionality.

## Important Files

- `CMakeLists.txt`: The main CMake build script.
- `pico_sdk_import.cmake`: Imports the Raspberry Pi Pico SDK.
- `src/main.cpp`: The main application entry point.
- `vnv/system/`: Directory containing the Python system tests.
