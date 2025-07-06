#ifndef MOCK_PICO_MUTEX_H
#define MOCK_PICO_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int lock_count;
    bool locked;
} mutex_t;

void mutex_enter_blocking(mutex_t *mtx);
void mutex_exit(mutex_t *mtx);

#ifdef __cplusplus
}
#endif

#endif // MOCK_PICO_MUTEX_H
