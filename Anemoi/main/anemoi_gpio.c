#include "include/anemoi_gpio.h"


esp_err_t gpio_myinit(void)
{
	esp_err_t ret=0;

	//INIT PINOUT
	ret=gpio_padselect_and_setdirection(GPIO_TDC1000_X_EN, GPIO_MODE_OUTPUT);
	ret=gpio_padselect_and_setdirection(GPIO_TDC1000_Y_EN, GPIO_MODE_OUTPUT);
	ret=gpio_padselect_and_setdirection(GPIO_TDC7200_EN, GPIO_MODE_OUTPUT);
	ret=gpio_padselect_and_setdirection(GPIO_CHSEL, GPIO_MODE_OUTPUT);
	ret=gpio_padselect_and_setdirection(GPIO_VDD_EN_X1, GPIO_MODE_OUTPUT);
	ret=gpio_padselect_and_setdirection(GPIO_VDD_EN_X2, GPIO_MODE_OUTPUT);
	ret=gpio_padselect_and_setdirection(GPIO_VDD_EN_Y1, GPIO_MODE_OUTPUT);
	ret=gpio_padselect_and_setdirection(GPIO_VDD_EN_Y2, GPIO_MODE_OUTPUT);
    
    return ret;
}

esp_err_t gpio_padselect_and_setdirection(unsigned char pin, gpio_mode_t direction)
{
	esp_err_t ret=0;

    gpio_pad_select_gpio(pin);
    ret=gpio_set_direction(pin, direction);

    return ret;
}

esp_err_t gpio_set_pin_high(unsigned char pin)
{
	esp_err_t ret=0;

	ret=gpio_set_level(GPIO_TDC1000_Y_EN, 1);

    return ret;
}

esp_err_t gpio_set_pin_low(unsigned char pin)
{
	esp_err_t ret=0;

	ret=gpio_set_level(GPIO_TDC1000_Y_EN, 0);

    return ret;
}

unsigned char gpio_get_pin_level(unsigned char pin)
{
	esp_err_t ret=0;

	ret=gpio_get_level(pin);
    
    return ret;
}



/*
#define ESP_INTR_FLAG_DEFAULT 0
#define START_QUEUE_LENGTH 1
#define STOP_QUEUE_LENGTH 30

QueueHandle_t START_queue;
QueueHandle_t STOP_queue;

uint32_t START_time=0;
uint32_t STOP_time=0;



static void IRAM_ATTR gpio_isr_handler_START(void* arg)
{
    START_time=xthal_get_ccount(); 
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( START_queue != 0 )
    {
        xQueueSendFromISR( START_queue, &START_time, &xHigherPriorityTaskWoken);
    } 
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
}

static void IRAM_ATTR gpio_isr_handler_STOP(void* arg)
{       
    STOP_time=xthal_get_ccount();
    
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( STOP_queue != 0 )
    {
        xQueueSendFromISR( STOP_queue, &STOP_time, &xHigherPriorityTaskWoken );
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
} 

void init_gpio_start_stop(void)
{
    gpio_pad_select_gpio(GPI_START);
    gpio_set_direction(GPI_START, GPIO_MODE_INPUT);
    gpio_set_intr_type(GPI_START, GPIO_PIN_INTR_POSEDGE);
    gpio_pad_select_gpio(GPI_STOP);
    gpio_set_direction(GPI_STOP, GPIO_MODE_INPUT);
    gpio_set_intr_type(GPI_STOP, GPIO_PIN_INTR_POSEDGE);
    

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPI_START, gpio_isr_handler_START, (void*) GPI_START);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPI_STOP, gpio_isr_handler_STOP, (void*) GPI_STOP);
    
    gpio_intr_enable(GPI_START);
    gpio_intr_enable(GPI_STOP);
}
  
  */
