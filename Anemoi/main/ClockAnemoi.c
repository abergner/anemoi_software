



//DRIVERS//
#include "driver/ledc.h"
//DRIVERS//

#include "include/Anemoi.h"
#include "include/ClockAnemoi.h"

#define CLOCK_FRECUENCY_IN_HZ TRANSDUCER_FREQUENCY_IN_HZ*32


ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .timer_num  = LEDC_TIMER_0,
    .bit_num    = 2,
    .freq_hz    = CLOCK_FRECUENCY_IN_HZ
};

ledc_channel_config_t ledc_channel = {
    .channel    = LEDC_CHANNEL_0,
    .gpio_num   = GPIO_CLK,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = 2
};

void initClock(void)
{
    //Init of clock output  
    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);
    //Init of clock output 
 
 }
