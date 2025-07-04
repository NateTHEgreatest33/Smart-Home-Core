# Module Testing with Google Test

This directory contains the module tests for the Smart Home Core project, built using the Google Test framework. These tests are designed to be run on a host machine (like macOS or Linux) and are compiled with a native compiler, separate from the embedded build for the Raspberry Pi Pico.

## Running Tests

To run the tests, follow these steps:

1.  **Create a build directory:**

    ```bash
    mkdir -p build_mod_tests
    ```

2.  **Navigate into the build directory:**

    ```bash
    cd build_mod_tests
    ```

3.  **Configure the build using CMake:**

    ```bash
    cmake ../vnv/mod
    ```

4.  **Compile the tests:**

    ```bash
    make
    ```

5.  **Run the test executable:**

    ```bash
    ./run_mod_tests
    ```

## Adding New Tests

To add a new test for a module:

1.  Create a new `_test.cpp` file in the `vnv/mod` directory (e.g., `my_feature_test.cpp`).
2.  The CMake build system will automatically discover and include any new `.cpp` files.
3.  If your test requires including code from the main project, you can include the headers directly (e.g., `#include "lib/util/queue.hpp"`).

### Testing Non-Header-Only Libraries

If the code you want to test is not header-only (i.e., it has a corresponding `.cpp` file), you will need to add the source file to the `CMakeLists.txt` in this directory.

1.  Open `vnv/mod/CMakeLists.txt`.
2.  Add the path to the `.cpp` file to the `add_executable` command. For example, to test the `button` library, you would modify the file like this:

```cmake
# ... (other cmake content)

# Find all test files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Add the source files you want to test
set(PROJECT_SOURCES
    ${CMAKE_SOURCE_DIR}/lib/button/button.cpp
)

# Add an executable for the tests
add_executable(run_mod_tests ${TEST_SOURCES} ${PROJECT_SOURCES})

# ... (rest of the file)
```
