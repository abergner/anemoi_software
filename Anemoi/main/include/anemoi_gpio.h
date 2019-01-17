#ifndef _ANEMOI_GPIO_H_
#define _ANEMOI_GPIO_H_

void init_anemoi_gpio(void);

void init_gpio_tdc1000_enable(void);
void enable_tdc1000_x(void);
void enable_tdc1000_y(void);
void disable_tdc1000_x(void);
void disable_tdc1000_y(void);

void init_gpio_tdc7200_enable(void);
void enable_tdc7200(void);
void disable_tdc7200(void);

void init_gpio_chsel(void);
void select_channel_1(void);
void select_channel_2(void);


void init_gpio_vdd_enable(void);
void enable_X1_vdd(void);
void enable_X2_vdd(void);
void enable_Y1_vdd(void);
void enable_Y2_vdd(void);
void disable_all_vdd(void);



#endif
