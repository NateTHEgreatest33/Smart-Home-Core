#ifndef PICO_HAL_HPP
#define PICO_HAL_HPP

#include "hal_interface.hpp"

class PicoHAL : public HALInterface {
public:
    void gpio_put(uint32_t pin, bool value) override;
};

#endif // PICO_HAL_HPP
