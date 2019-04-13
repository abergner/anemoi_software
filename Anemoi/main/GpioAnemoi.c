


//DRIVERS//
#include "driver/gpio.h"
#include "soc/gpio_reg.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
//DRIVERS//

#include "include/Anemoi.h"
#include "include/GpioAnemoi.h"




void initEnableTdc1000(void);
void initTrigger(void);
void initChannelSelect(void);
void initEnableVdd(void);

void initGpio(void)
{
    initEnableTdc1000();
    initChannelSelect();
    initEnableVdd();
    initTrigger();
}

void sendTrigger(void)
{
	gpio_set_level(GPIO_ESP_TRIGG, 1);
	vTaskDelay(1 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_ESP_TRIGG, 0);
}
void initTrigger(void)
{
	gpio_pad_select_gpio(GPIO_ESP_TRIGG);
	gpio_set_direction(GPIO_ESP_TRIGG, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_ESP_TRIGG, 0);
}


void initEnableTdc1000(void)
{
    gpio_pad_select_gpio(GPIO_TDC1000_X_EN);
    gpio_set_direction(GPIO_TDC1000_X_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_TDC1000_X_EN, 0);
    
    gpio_pad_select_gpio(GPIO_TDC1000_Y_EN);
    gpio_set_direction(GPIO_TDC1000_Y_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_TDC1000_Y_EN, 0);
}

void enableX(void)
{
	//disable_tdc1000_y();
	disableY();
    gpio_set_level(GPIO_TDC1000_X_EN, 1);


}
void enableY(void)
{
	//disable_tdc1000_x();
	disableX();
    gpio_set_level(GPIO_TDC1000_Y_EN, 1);

}

void disableTdc1000(void)
{
	gpio_set_level(GPIO_TDC1000_X_EN, 0);
	gpio_set_level(GPIO_TDC1000_Y_EN, 0);
}

void disableX(void)
{
    gpio_set_level(GPIO_TDC1000_X_EN, 0);
}
void disableY(void)
{
    gpio_set_level(GPIO_TDC1000_Y_EN, 0);
}


void initChannelSelect(void)
{
    gpio_pad_select_gpio(GPIO_CHSEL);
    gpio_set_direction(GPIO_CHSEL, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_CHSEL, 0); 
}

void selectChannel1(void)
{
    gpio_set_level(GPIO_CHSEL, 0);
}

void selectChannel2(void)
{
    gpio_set_level(GPIO_CHSEL, 1);
}

void initEnableVdd(void)
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

void enableVddX1(void)
{
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0); 
    
    gpio_set_level(GPIO_VDD_EN_X1, 1);
    
}
void enableVddX2(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0);
    
    gpio_set_level(GPIO_VDD_EN_X2, 1);
}
void enableVddY1(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y2, 0); 
    
    gpio_set_level(GPIO_VDD_EN_Y1, 1);
}
void enableVddY2(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);
    
    gpio_set_level(GPIO_VDD_EN_Y2, 1); 
}

void disableAllVdd(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);   
    gpio_set_level(GPIO_VDD_EN_Y2, 0);    
}





