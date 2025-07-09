#include "app_logic.hpp"
#include "hal/hal_interface.hpp"
#include "pico/cyw43_arch.h"

// This is a global variable defined in main.cpp. We need to declare it here.
extern HALInterface* g_hal;

void application_startup()
{
    // This is the part of main() that we want to test.
    g_hal->gpio_put( CYW43_WL_GPIO_LED_PIN, 1 );
}
