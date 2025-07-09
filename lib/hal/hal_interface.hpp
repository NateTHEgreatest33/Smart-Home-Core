#ifndef HAL_INTERFACE_HPP
#define HAL_INTERFACE_HPP

#include <cstdint>

class HALInterface {
public:
    virtual ~HALInterface() = default;
    virtual void gpio_put(uint32_t pin, bool value) = 0;
};

#endif // HAL_INTERFACE_HPP
