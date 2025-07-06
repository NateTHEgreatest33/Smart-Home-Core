#include "gtest/gtest.h"
#include "lib/util/mutex_lock.hpp"
#include "pico/mutex.h" // This will now pick up our mock

// Extern declarations for the mock functions
extern "C" {
extern void mutex_enter_blocking(mutex_t *mtx);
extern void mutex_exit(mutex_t *mtx);
}

// Mock variables to track calls
mutex_t *mutex_enter_blocking_called_with = nullptr;
mutex_t *mutex_exit_called_with = nullptr;

// Reset mocks before each test
void reset_mutex_mocks() {
    mutex_enter_blocking_called_with = nullptr;
    mutex_exit_called_with = nullptr;
}

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

namespace utl {
namespace test {

class MutexLockTest : public ::testing::Test {
protected:
    void SetUp() override {
        reset_mutex_mocks();
        // Initialize a dummy mutex for testing
        test_mutex.locked = false;
        test_mutex.lock_count = 0;
    }

    mutex_t test_mutex;
};

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
