#ifndef MOCK_HARDWARE_SYNC_H
#define MOCK_HARDWARE_SYNC_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t save_and_disable_interrupts();
void restore_interrupts(uint32_t status);

#ifdef __cplusplus
}
#endif

#endif // MOCK_HARDWARE_SYNC_H
