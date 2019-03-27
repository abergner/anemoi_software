#include "include/anemoi_app.h"

void app(void)
{
	//Init clock of ESP32. Frequency set to 1.28MHz
    clock_init();

    //Init pins connected from ESP32 to TDC1000, TDC7200
    gpio_myinit();

    spi_init();

    tdc1000_init();

    tdc7200_init();


    //Run trigger generation task, among others
    run_anemoi_tasks();

    //Send positive edge to TDC7200 for correct initialization (see datasheet)
    //disable_tdc7200();
    //vTaskDelay(2000/ portTICK_PERIOD_MS);
    enable_tdc7200();

    //Create handles for SPI communication and add devices to SPI bus


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
        printf("\nTDC1000 X \n");
        read_TDC1000_registers(&tdc1000_x_handle);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        printf("\nTDC1000 Y \n");
        read_TDC1000_registers(&tdc1000_y_handle);
        vTaskDelay(5000 / portTICK_PERIOD_MS);


        //vTaskDelay(2000 / portTICK_PERIOD_MS);
        //read_TDC7200_config_registers(&tdc7200_handle);
        //new_TDC7200_measurement(&tdc7200_handle);
        //printf("\nTDC7200  \n");
        //vTaskDelay(2000 / portTICK_PERIOD_MS);
        //read_TDC7200_meas_registers(&tdc7200_handle);
        /*vTaskDelay(1000/ portTICK_PERIOD_MS);
        ret=init_TDC7200(&tdc7200_handle);
        if(ret==ESP_OK)
        {
           printf("TDC7200 initialized\n");
        }*/


    }

}
