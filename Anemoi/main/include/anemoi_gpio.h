#ifndef _ANEMOI_GPIO_H_
#define _ANEMOI_GPIO_H_


#include "driver/gpio.h"
#include "soc/gpio_reg.h"
#include "esp_intr_alloc.h"

#include "anemoi_pinout.h"

esp_err_t gpio_myinit(void);
esp_err_t gpio_padselect_and_setdirection(unsigned char pin, gpio_mode_t direction);

esp_err_t gpio_set_pin_high(unsigned char pin);
esp_err_t gpio_set_pin_low(unsigned char pin);
unsigned char gpio_get_pin_level(unsigned char pin);

#endif
