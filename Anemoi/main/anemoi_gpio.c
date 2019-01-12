


#include "driver/gpio.h"
#include "soc/gpio_reg.h"
#include "esp_intr_alloc.h"

#include "include/anemoi_gpio.h"
#include "include/anemoi_pinout.h"


void init_gpio_vdd_enable(void)
{
    gpio_pad_select_gpio(GPIO_VDD_EN_X1);
    gpio_set_direction(GPIO_VDD_EN_X1, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    
    gpio_pad_select_gpio(GPIO_VDD_EN_X2);
    gpio_set_direction(GPIO_VDD_EN_X2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    
    gpio_pad_select_gpio(GPIO_VDD_EN_Y1);
    gpio_set_direction(GPIO_VDD_EN_Y1, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    
    gpio_pad_select_gpio(GPIO_VDD_EN_Y2);
    gpio_set_direction(GPIO_VDD_EN_Y2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_VDD_EN_Y2, 0); 
}

void vdd_enable_X1(void)
{
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0); 
    
    gpio_set_level(GPIO_VDD_EN_X1, 1);
    
}
void vdd_enable_X2(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0);
    
    gpio_set_level(GPIO_VDD_EN_X2, 1);
}
void vdd_enable_Y1(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0); 
    
    gpio_set_level(GPIO_VDD_EN_Y1, 1);
}
void vdd_enable_Y2(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    
    gpio_set_level(GPIO_VDD_EN_Y2, 1); 
}
