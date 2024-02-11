#ifndef MATRIX_DRIVER_H
#define MATRIX_DRIVER_H
#include <FreeRTOS.h>
#include <queue.h>

typedef struct {
uint32_t data[64];
}frame_line_t;

void matrix_init(QueueHandle_t frame_queue);

#endif /* MATRIX_DRIVER_H */