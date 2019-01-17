

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
    
    init_anemoi_gpio();
    init_clock();
    
    enable_tdc1000_x();
    enable_tdc1000_y();
    select_channel_1();
    enable_X1_vdd();
    //run_anemoi_tasks();
    disable_tdc7200();
    vTaskDelay(2000/ portTICK_PERIOD_MS); 
    enable_tdc7200();
    
    esp_err_t ret;
    spi_device_handle_t tdc1000_x_handle;
    spi_device_handle_t tdc1000_y_handle;
    spi_device_handle_t tdc7200_handle;
   
    
    ret=init_SPI(&tdc1000_x_handle,&tdc1000_y_handle,&tdc7200_handle);
    if(ret==ESP_OK)
    {
        printf("SPI initialized\n");
    }  
    ret=init_TDC1000_SPI(&tdc1000_x_handle);
    if(ret==ESP_OK)
    {
        printf("TDC1000 X initialized\n");
    }  
    ret=init_TDC1000_SPI(&tdc1000_y_handle);
    if(ret==ESP_OK)
    {
        printf("TDC1000 Y initialized\n");
    }  
    ret=init_TDC7200_SPI(&tdc7200_handle);
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
        
        
        vTaskDelay(2000 / portTICK_PERIOD_MS); 
        //read_TDC7200_config_registers(&tdc7200_handle);
        new_TDC7200_measurement(&tdc7200_handle);
        printf("\nTDC7200  \n");
        vTaskDelay(2000 / portTICK_PERIOD_MS); 
        read_TDC7200_meas_registers(&tdc7200_handle);
        /*vTaskDelay(1000/ portTICK_PERIOD_MS);  
        ret=init_TDC7200(&tdc7200_handle);
        if(ret==ESP_OK)
        {
           printf("TDC7200 initialized\n");
        }*/
        
              
    }
    
}



