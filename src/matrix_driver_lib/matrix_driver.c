#include "matrix_driver.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define r 0
#define g 1
#define b 2
#define r1 3
#define g1 4
#define b1 5
#define la 6
#define lb 7
#define lc 8
#define ld 13
#define le 9
#define lat 10
#define clk 11
#define oe 12

#define pin_dir_mask ((1<<r) | (1<<g) | (1<<b) | \
                        (1<<r1) | (1<<g1) | (1<<b1) | \
                        (1<<la) | (1<<lb) | (1<<lc) | \
                        (1<<ld) | (1<<le) | (1<<clk) | \
                        (1<<lat))



uint32_t pwm_set_freq_duty(uint slice_num,
                           uint chan, uint32_t f, int d)
{
    uint32_t clock = 125000000;
    uint32_t divider16 = clock / f / 4096 +
                         (clock % (f * 4096) != 0);
    if (divider16 / 16 == 0)
        divider16 = 16;
    uint32_t wrap = clock * 16 / divider16 / f - 1;
    pwm_set_clkdiv_int_frac(slice_num, divider16 / 16,
                            divider16 & 0xF);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, chan, wrap * d / 100);
    return wrap;
}

void matrix_clocker(void *arg)
{
    frame_line_t frame;
    frame_line_t frameg;
    frame_line_t frameb;
    for(uint8_t i = 0; i<64; i++) {
        frame.data[i] = (i % 2) ? 0 : 0xffff;
    }
    for(uint8_t i = 0; i<64; i++) {
        frameg.data[i] = (i % 2) ? 0 : 3;
    }
    for(uint8_t i = 0; i<64; i++) {
        frameb.data[i] = (i % 2) ? 0 : 0xff;
    }

    gpio_init_mask(pin_dir_mask);
    gpio_set_dir_out_masked(pin_dir_mask);
    gpio_put_masked(pin_dir_mask, 0);
    gpio_set_function(oe, GPIO_FUNC_PWM);
    uint32_t slice = pwm_gpio_to_slice_num(oe);
    uint32_t channel = pwm_gpio_to_channel(oe);
    pwm_set_enabled(slice, 1);
    pwm_set_freq_duty(slice, channel, 1000000, 93);

    sleep_us(100);
    gpio_put(lat, 0);
    sleep_us(100);
    gpio_put(lat, 1);
    sleep_us(100);
    uint32_t out_reg_val;
    while (1)
    {
        for (uint8_t plane = 0; plane < 24; plane++)
        {
        for(uint8_t line = 0; line < 32; line++) {


            for (uint8_t pixel = 0; pixel < 64; pixel++)
            {
                 out_reg_val= ((frame.data[pixel] >> plane) & 1) | ((frameg.data[pixel] >> plane) & 1)  << g | ((frameb.data[pixel] >> plane) & 1)  << b
                              | (((frame.data[pixel] >> plane) & 1) << r1) | (((frameg.data[pixel] >> plane) & 1)  << g1) | (((frameb.data[pixel] >> plane) & 1)  << b1)
                              | ((line & 1) << la) | ((line & 2) <<lb) >> 1 | ((line & 4) <<lc) >> 2 | ((line & 8) << ld) >> 3 | ((line & 16) <<le) >> 4;
                
                sio_hw->gpio_out = out_reg_val;
                sio_hw->gpio_set = (1 << clk);
                for(uint8_t i =0; i< 2; i++);
                sio_hw->gpio_clr = (1 << clk);
            }
            sio_hw->gpio_set = (1 << lat);
            for(uint8_t i =0; i< 2; i++);
            sio_hw->gpio_clr = (1 << lat);
        }
        }
        sleep_us(10);
    }

}

void matrix_init()
{
    int status = xTaskCreate(matrix_clocker, "matrix clocker", 2048, NULL, 6, NULL);
    if (status != pdPASS)
    {
        while (1)
            ;
    }
}