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

## Generating HTML Test Reports

This project includes a Python script to generate an HTML report from the XML test results. This provides a more human-readable format for reviewing test outcomes.

1.  **Run the script:**

    ```bash
    python3 vnv/mod/mod_results.py
    ```

2.  **View the report:** The script will generate `mod_test_results.html` in the `results/mod` directory. You can open this file in your web browser to view the report.

## Generating Code Coverage

This build is configured to generate code coverage data. To create an HTML report, you'll need to have `lcov` installed. You can typically install it with Homebrew on macOS (`brew install lcov`).

1.  **Clean the build directory and coverage data:** This is important to ensure you're starting with a fresh set of coverage data.

    ```bash
    rm -rf *
    find . -name "*.gcda" -delete
    ```

2.  **Re-run CMake and make:**

    ```bash
    cmake ../vnv/mod
    make
    ```

3.  **Run the tests:** This will generate the initial coverage data.

    ```bash
    ./run_mod_tests
    ```

4.  **Generate the coverage report:**

    ```bash
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' '*/_deps/*' --output-file coverage.info
    genhtml coverage.info --output-directory coverage_report
    ```

5.  **View the report:** Open `coverage_report/index.html` in your web browser to see the detailed coverage results.

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
