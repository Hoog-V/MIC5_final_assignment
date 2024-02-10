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
#include "lwip/altcp_tls.h"
#include "lwip/apps/mqtt.h"

#include "lwip/apps/mqtt_priv.h"

#include "tusb.h"
#include "FreeRTOS.h"
#include <task.h>
#include "board_definitions.hpp"

void error_handler()
{
    cyw43_arch_disable_sta_mode();
    cyw43_arch_deinit();
    while (1)
    {
        ;
    }
}

void led_task(void *pvArg)
{
    while (true)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        vTaskDelay(250/portTICK_PERIOD_MS);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        vTaskDelay(250/portTICK_PERIOD_MS);
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
    xTaskCreate(led_task, "Led blinky", 128, NULL, 1, NULL);

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
