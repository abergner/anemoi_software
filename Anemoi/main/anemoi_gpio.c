


#include "driver/gpio.h"
#include "soc/gpio_reg.h"
#include "esp_intr_alloc.h"

#include "include/anemoi_gpio.h"
#include "include/anemoi_pinout.h"

void init_gpio_tdc1000_enable(void)
{
    gpio_pad_select_gpio(GPIO_TDC1000_X_EN);
    gpio_set_direction(GPIO_TDC1000_X_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_TDC1000_X_EN, 0);
    
    gpio_pad_select_gpio(GPIO_TDC1000_Y_EN);
    gpio_set_direction(GPIO_TDC1000_Y_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_TDC1000_Y_EN, 0);
}

void enable_tdc1000_x(void)
{
    gpio_set_level(GPIO_TDC1000_X_EN, 1);
}
void enable_tdc1000_y(void)
{
    gpio_set_level(GPIO_TDC1000_Y_EN, 1);
}

void disable_tdc1000_x(void)
{
    gpio_set_level(GPIO_TDC1000_X_EN, 0);
}
void disable_tdc1000_y(void)
{
    gpio_set_level(GPIO_TDC1000_Y_EN, 0);
}


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

void enable_X1_vdd(void)
{
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0); 
    
    gpio_set_level(GPIO_VDD_EN_X1, 1);
    
}
void enable_X2_vdd(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0);
    
    gpio_set_level(GPIO_VDD_EN_X2, 1);
}
void enable_Y1_vdd(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0); 
    
    gpio_set_level(GPIO_VDD_EN_Y1, 1);
}
void enable_Y2_vdd(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    
    gpio_set_level(GPIO_VDD_EN_Y2, 1); 
}
