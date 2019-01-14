

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


//Main application
void app_main()
{    
    init_clock();
    //xTaskCreate(&trigger_task, "trigger_task", 512, NULL, 4, NULL);
    //xTaskCreate(&TDC1000_task, "TDC1000_task", 4096, NULL, 5, NULL);
    //xTaskCreate(&time_measurement_task, "time_measurement_task", 8192, NULL, 6, NULL);*/
    
    //init_gpio_vdd_enable();
    init_gpio_vdd_enable();
    //enable_Y2_vdd();
    init_clock();
    init_gpio_tdc1000_enable();
    //enable_tdc1000_y();
    //run_anemoi_tasks();
    
    esp_err_t ret;
    spi_device_handle_t tdc1000_x_handle;
    spi_device_handle_t tdc1000_y_handle;
    spi_device_handle_t tdc7200_handle;
   
    
    ret=init_SPI(&tdc1000_x_handle,&tdc1000_y_handle,&tdc7200_handle);
    if(ret==ESP_OK)
    {
        printf("SPI initialized\n");
    }  
    //ret=init_TDC1000(&tdc1000_x_handle);
    if(ret==ESP_OK)
    {
        printf("TDC1000 X initialized\n");
    }  
    //ret=init_TDC1000(&tdc1000_y_handle);
    if(ret==ESP_OK)
    {
        printf("TDC1000 Y initialized\n");
    }  
    ret=init_TDC7200(&tdc7200_handle);
    if(ret==ESP_OK)
    {
        printf("TDC7200 initialized\n");
    }      
   
    while(1)
    {
        //printf("\nTDC1000 X \n");
        //read_TDC1000_registers(&tdc1000_x_handle);
        //vTaskDelay(5000 / portTICK_PERIOD_MS); 
        //printf("\nTDC1000 Y \n");
        //read_TDC1000_registers(&tdc1000_y_handle);
        //vTaskDelay(5000 / portTICK_PERIOD_MS); 
        
        read_TDC7200_registers(&tdc7200_handle);
        vTaskDelay(2000 / portTICK_PERIOD_MS); 
        ret=init_TDC7200(&tdc7200_handle);
        printf("\nTDC7200  \n");
        if(ret==ESP_OK)
        {
           printf("TDC7200 initialized\n");
        }
        
              
    }
    
}



