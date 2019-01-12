








#include "rom/cache.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"
#include "esp_wifi.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_event.h"
#include "esp_event_loop.h"

#include "esp_intr_alloc.h"

#include "driver/spi_master.h"



#include "include/anemoi_gpio.h"
#include "include/anemoi_clock.h"
#include "include/anemoi_spi.h"
#include "include/anemoi_tasks.h"


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

//Main application
void app_main()
{    
    //init_clock();
    //xTaskCreate(&trigger_task, "trigger_task", 512, NULL, 4, NULL);
    //xTaskCreate(&TDC1000_task, "TDC1000_task", 4096, NULL, 5, NULL);
    //xTaskCreate(&time_measurement_task, "time_measurement_task", 8192, NULL, 6, NULL);*/
    
    //init_gpio_vdd_enable();
    
    
    esp_err_t ret;
    spi_device_handle_t tdc1000_x_handle;
    spi_device_handle_t tdc1000_y_handle;
    spi_device_handle_t tdc7200_handle;
   
    
    ret=init_SPI(&tdc1000_x_handle,&tdc1000_y_handle,&tdc7200_handle);
    if(ret==ESP_OK)
    {
        printf("SPI initialized\n");
    }  
    ret=init_TDC1000(&tdc1000_x_handle);
    if(ret==ESP_OK)
    {
        printf("TDC1000 X initialized\n");
    }  
            
            
    while(1)
    {
        printf("\nTDC1000 X \n");
        read_TDC1000_registers(&tdc1000_x_handle);
        vTaskDelay(5000 / portTICK_PERIOD_MS); 
        printf("\nTDC1000 Y \n");
        read_TDC1000_registers(&tdc1000_y_handle);
        vTaskDelay(5000 / portTICK_PERIOD_MS); 
    }
    
}



