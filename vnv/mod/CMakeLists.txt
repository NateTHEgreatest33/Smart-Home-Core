
cmake_minimum_required(VERSION 3.14)
project(MOD_TESTS)

# Enable code coverage
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")

# Add the main project directory to the include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mocks)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG    release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find all test files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Add the gtest and gmock libraries to the include path
include_directories(${googletest_SOURCE_DIR}/googletest/include)
include_directories(${googletest_SOURCE_DIR}/googlemock/include)

# Add an executable for the tests
set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app_logic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/util/disable_interrupt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/util/mutex_lock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks/hardware/sync.cpp
)
add_executable(run_mod_tests ${TEST_SOURCES} ${PROJECT_SOURCES})

# Link the tests to GoogleTest
target_link_libraries(run_mod_tests gtest gtest_main)

# Add a test to CTest
include(GoogleTest)
gtest_discover_tests(run_mod_tests)
