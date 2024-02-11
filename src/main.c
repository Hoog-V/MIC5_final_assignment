#include "hardware/structs/rosc.h"
#include <stdio.h>

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

#include "lwip/altcp_tcp.h"
#include "lwip/apps/mqtt.h"

#include "lwip/apps/mqtt_priv.h"

#include "tusb.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include <task.h>
#include "board_definitions.hpp"
#include "matrix_driver.h"

void error_handler()
{
    cyw43_arch_disable_sta_mode();
    cyw43_arch_deinit();
    while (1)
    {
        ;
    }
}

void led_task(void *arg)
{
    QueueHandle_t frame_queue = (QueueHandle_t) arg;
    frame_line_t frame;
    int status;
    for(uint8_t i = 0; i<64; i++) {
        frame.data[i] = (i % 2) ? 0 : 0x06;
    }
    while (true)
    {
        status = xQueueSend(frame_queue, &frame, 0);
        if(status != pdPASS) {
            printf("Can not send message!\n");
        }
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        vTaskDelay(250/portTICK_PERIOD_MS);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        vTaskDelay(250/portTICK_PERIOD_MS);
    }
}


void matrix_frame_feeder(void *arg) {
    QueueHandle_t queue = (QueueHandle_t) arg;
    frame_line_t frame;
    int status;
    for(uint8_t i = 0; i<64; i++) {
        frame.data[i] = (i % 2) ? 0 : 0xFF;
    }
    while (true) 
    {
        status = xQueueSend(queue, &frame, 1/portTICK_PERIOD_MS);
        if(status == pdPASS) {
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
    }
}

int main()
{
    stdio_init_all();

    // printf("waiting for usb host");
    // while (!tud_cdc_connected()) {
    //     printf(".");
    //     sleep_ms(500);
    // }
    if (cyw43_arch_init())
    {
        //   printf("failed to initialise\n");
        return 1;
    }
    QueueHandle_t frame_queue = xQueueCreate(5, sizeof(frame_line_t));
    if(frame_queue == NULL) {
        while(1);
    }
    int status = xTaskCreate(led_task, "Led blinky", 512, frame_queue, 1, NULL);
    if(status != pdPASS) {
        while(1);
    }


    matrix_init(frame_queue);
    // status = xTaskCreate(matrix_frame_feeder, "frame feeder", 512, frame_queue, 3, NULL);
    // if(status != pdPASS) {
    //     while(1);
    // }

    // cyw43_arch_enable_sta_mode();
    // printf("Connecting to WiFi...\n");
    // do {
    //    printf(".");
    //} while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 3000));

    vTaskStartScheduler();


    while (true)
    {

    }
    cyw43_arch_deinit();

    return 0;
}
