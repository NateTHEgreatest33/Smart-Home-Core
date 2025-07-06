#include "hardware/sync.h"

uint32_t save_and_disable_interrupts_mock_return_value = 0;
uint32_t save_and_disable_interrupts() {
    return save_and_disable_interrupts_mock_return_value;
}

uint32_t restore_interrupts_called_value = 0;
void restore_interrupts(uint32_t status) {
    restore_interrupts_called_value = status;
}
