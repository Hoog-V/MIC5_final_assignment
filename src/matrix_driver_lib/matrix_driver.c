#include "matrix_driver.h"
#include <stdio.h>
#include "pico/stdlib.h"


void matrix_clocker(void *arg)
{
    QueueHandle_t handle = (QueueHandle_t)arg;
    int status;
    frame_line_t frame;
    const uint8_t r = 2;
    const uint8_t clk = 3;
    const uint8_t oe = 4;
    const uint8_t lat = 5;
    gpio_init(r);
    gpio_init(clk);
    gpio_init(oe);
    gpio_init(lat);
    gpio_set_dir(r, 1);
    gpio_set_dir(clk, 1);
    gpio_set_dir(oe, 1);
    gpio_set_dir(lat, 1);
    gpio_put(oe, 1);
    while (1)
    {
        status = xQueueReceive(handle, &frame, 1 / portTICK_PERIOD_MS);
        if (status == pdPASS)
        {
            for (uint8_t plane = 1; plane < 5; plane++)
                {
                    for(uint8_t pixel = 0; pixel < 64; pixel++) {
                        gpio_put(r, (frame.data[pixel] & (1 << plane)));
                        gpio_put(clk, 1);
                        gpio_put(clk, 0);
                    }
                    gpio_put(lat, 1);
                    gpio_put(lat, 0);
                    gpio_put(lat, 1);
                    vTaskDelay(1/portTICK_PERIOD_MS);
                }
        }
    }
}

void matrix_init(QueueHandle_t frame_queue)
{
    int status = xTaskCreate(matrix_clocker, "matrix clocker", 256, frame_queue, 6, NULL);
    if (status != pdPASS)
    {
        while (1)
            ;
    }
}