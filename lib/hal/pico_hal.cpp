#include "pico_hal.hpp"

// This is the C library we are wrapping
extern "C" {
#include "pico/cyw43_arch.h"
}

void PicoHAL::gpio_put(uint32_t pin, bool value) {
    cyw43_arch_gpio_put(pin, value);
}
