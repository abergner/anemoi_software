


#include "driver/gpio.h"
#include "soc/gpio_reg.h"
#include "esp_intr_alloc.h"

#include "include/anemoi_gpio.h"
#include "include/anemoi_pinout.h"

int interrupt=0;

void init_anemoi_gpio(void)
{
    init_gpio_tdc1000_enable();
    init_gpio_tdc7200_enable();
    init_gpio_chsel();
    init_gpio_vdd_enable();
}

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

void init_gpio_tdc7200_enable(void)
{
    gpio_pad_select_gpio(GPIO_TDC7200_EN);
    gpio_set_direction(GPIO_TDC7200_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_TDC7200_EN, 0);
    
}
void enable_tdc7200(void)
{
    gpio_set_level(GPIO_TDC7200_EN, 1);
}
void disable_tdc7200(void)
{
    gpio_set_level(GPIO_TDC7200_EN, 0);
}

void init_gpio_chsel(void)
{
    gpio_pad_select_gpio(GPIO_CHSEL);
    gpio_set_direction(GPIO_CHSEL, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_CHSEL, 0); 
}

void select_channel_1(void)
{
    gpio_set_level(GPIO_CHSEL, 0);
}

void select_channel_2(void)
{
    gpio_set_level(GPIO_CHSEL, 1);
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

void disable_all_vdd(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, 0);
    gpio_set_level(GPIO_VDD_EN_X2, 0);
    gpio_set_level(GPIO_VDD_EN_Y1, 0);   
    gpio_set_level(GPIO_VDD_EN_Y2, 0);    
}


#define ESP_INTR_FLAG_DEFAULT 0
#define START_QUEUE_LENGTH 1
#define STOP_QUEUE_LENGTH 30

//QueueHandle_t START_queue;
//QueueHandle_t STOP_queue;

uint32_t START_time=0;
uint32_t STOP_time=0;



static void IRAM_ATTR gpio_isr_handler_START(void* arg)
{
	/*if(interrupt==0){
		interrupt=1;
	}else{
		interrupt=0;
	}*/
	//interrupt=1;

    //printf("Handler Start\n");

	/*
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
    */
}

static void IRAM_ATTR gpio_isr_handler_STOP(void* arg)
{       
    //printf("Handler Start\n");

	/*
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
    */
} 

void init_gpio_start_stop(void)
{
    gpio_pad_select_gpio(GPI_START);
    gpio_set_direction(GPI_START, GPIO_MODE_INPUT);
    //gpio_set_intr_type(GPI_START, GPIO_PIN_INTR_POSEDGE);
    gpio_pad_select_gpio(GPI_STOP);
    gpio_set_direction(GPI_STOP, GPIO_MODE_INPUT);
    //gpio_set_intr_type(GPI_STOP, GPIO_PIN_INTR_POSEDGE);
    
    print("Hello :)");

    //install gpio isr service
    //gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
   // gpio_isr_handler_add(GPI_START, gpio_isr_handler_START, (void*) GPI_START);
    //hook isr handler for specific gpio pin
    //gpio_isr_handler_add(GPI_STOP, gpio_isr_handler_STOP, (void*) GPI_STOP);
    
    //gpio_intr_enable(GPI_START);
    //gpio_intr_enable(GPI_STOP);
}
  
int get_interrupt(void){
	return interrupt;
}
