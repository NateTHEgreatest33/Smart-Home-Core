/*********************************************************************
*
*   HEADER:
*       mutex_lock_test.cpp
*
*   DESCRIPTION:
*       This file contains the unit tests for the utl::mutex_lock class.
*
*   Copyright 2025 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                              INCLUDES
--------------------------------------------------------------------*/
#include "gtest/gtest.h"
#include "lib/util/mutex_lock.hpp"
#include "pico/mutex.h" // This will now pick up our mock

/*--------------------------------------------------------------------
                               MOCKS
--------------------------------------------------------------------*/

// Extern declarations for the mock functions
extern "C" {
extern void mutex_enter_blocking(mutex_t *mtx);
extern void mutex_exit(mutex_t *mtx);
}

// Mock variables to track calls
mutex_t *mutex_enter_blocking_called_with = nullptr;
mutex_t *mutex_exit_called_with = nullptr;

// Implement mock functions
extern "C" {
void mutex_enter_blocking(mutex_t *mtx) {
    mutex_enter_blocking_called_with = mtx;
    mtx->locked = true;
    mtx->lock_count++;
}

void mutex_exit(mutex_t *mtx) {
    mutex_exit_called_with = mtx;
    mtx->locked = false;
}
}

/*--------------------------------------------------------------------
                               CLASSES
--------------------------------------------------------------------*/

namespace utl {
namespace test {

/*********************************************************************
*
*   CLASS NAME:
*       MutexLockTest
*
*   DESCRIPTION:
*       Test fixture for the utl::mutex_lock class.
*
*********************************************************************/
class MutexLockTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset mocks before each test
        mutex_enter_blocking_called_with = nullptr;
        mutex_exit_called_with = nullptr;
        // Initialize a dummy mutex for testing
        test_mutex.locked = false;
        test_mutex.lock_count = 0;
    }

    mutex_t test_mutex;
};

/*--------------------------------------------------------------------
                              TEST CASES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   TEST NAME:
*       MutexLockTest.ConstructorLocksMutex
*
*   DESCRIPTION:
*       Tests that the constructor locks the mutex.
*
*********************************************************************/
TEST_F(MutexLockTest, ConstructorLocksMutex)
{
    // Ensure the mutex is initially unlocked
    ASSERT_FALSE(test_mutex.locked);
    ASSERT_EQ(test_mutex.lock_count, 0);

    {
        utl::mutex_lock lock(test_mutex);
        // Verify that mutex_enter_blocking was called with the correct mutex
        ASSERT_EQ(mutex_enter_blocking_called_with, &test_mutex);
        // Verify that the mutex is now considered locked by our mock
        ASSERT_TRUE(test_mutex.locked);
        ASSERT_EQ(test_mutex.lock_count, 1);
    }
}

/*********************************************************************
*
*   TEST NAME:
*       MutexLockTest.DestructorUnlocksMutex
*
*   DESCRIPTION:
*       Tests that the destructor unlocks the mutex.
*
*********************************************************************/
TEST_F(MutexLockTest, DestructorUnlocksMutex)
{
    // Ensure the mutex is initially unlocked
    ASSERT_FALSE(test_mutex.locked);

    {
        utl::mutex_lock lock(test_mutex);
        // Mutex is locked by constructor
        ASSERT_TRUE(test_mutex.locked);
    } // mutex_lock object goes out of scope, destructor is called

    // Verify that mutex_exit was called with the correct mutex
    ASSERT_EQ(mutex_exit_called_with, &test_mutex);
    // Verify that the mutex is now considered unlocked by our mock
    ASSERT_FALSE(test_mutex.locked);
}

} // namespace test
} // namespace utl
