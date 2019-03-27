
#include "driver/ledc.h"

#include "include/anemoi_clock.h"
#include "include/anemoi_pinout.h"



ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .timer_num  = LEDC_TIMER_0,
    .bit_num    = 2,
    .freq_hz    = TDC_Clock_Hz
};

ledc_channel_config_t ledc_channel = {
    .channel    = LEDC_CHANNEL_0,
    .gpio_num   = GPIO_CLK,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = 2
};

void init_clock(void)
{
    //Init of clock output  
    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);
    //Init of clock output 
 
 }
