#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "hal/hal_interface.hpp"
#include "app_logic.hpp"
#include "pico/cyw43_arch.h"

// Global HAL pointer used by the application logic
HALInterface* g_hal = nullptr;

// Mock HAL class for testing
class MockHAL : public HALInterface {
public:
    MOCK_METHOD(void, gpio_put, (uint32_t pin, bool value), (override));
};

// Test fixture for HAL tests
class HALTest : public ::testing::Test {
protected:
    MockHAL mock_hal;

    void SetUp() override {
        // Set the global HAL interface to our mock for each test
        g_hal = &mock_hal;
    }

    void TearDown() override {
        // Reset the global HAL interface after each test
        g_hal = nullptr;
    }
};

// Test case to verify that the LED is turned on at startup
TEST_F(HALTest, LedTurnsOnAtStartup) {
    // Expect that gpio_put is called once with the LED pin and value 1
    EXPECT_CALL(mock_hal, gpio_put(CYW43_WL_GPIO_LED_PIN, 1))
        .Times(1);

    // Call the function under test
    application_startup();
}
