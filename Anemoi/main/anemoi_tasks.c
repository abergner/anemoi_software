

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "include/anemoi_tasks.h"



void time_measurement_task(void *pvParameter)
{
    
    START_queue=xQueueCreate( START_QUEUE_LENGTH, sizeof( uint32_t ) );
    if( START_queue == 0 )
    {
       printf("Failed to create the START queue.\n");
    }
    STOP_queue=xQueueCreate( STOP_QUEUE_LENGTH, sizeof( uint32_t ) );
    if( STOP_queue == 0 )
    {
       printf("Failed to create the STOP queue.\n");
    }
    
    init_gpio_start_stop();
    
    uint32_t start=0;
    uint32_t stops[STOP_QUEUE_LENGTH]={0};  
    unsigned int i=0;
    unsigned int k=0;
    unsigned int arrived_pulses=0;
    double time=0;
    double stop_times[STOP_QUEUE_LENGTH]={0}; 
    double compare_stops[STOP_QUEUE_LENGTH]={0}; 
    double er_temp=0; 
    double mean_error=0; 
    double er=100000000; 
    unsigned int pulse_number[STOP_QUEUE_LENGTH]={0};
    double TOF=0;
    
    while(1)
    {
        TOF=0;
        arrived_pulses=0;
        mean_error=0;
        if( START_queue != 0 )
        {
            if(xQueueReceive(START_queue, &start, (1000 / portTICK_PERIOD_MS)))
            {
                printf("startttt\n");
            }
        }
        if( STOP_queue != 0 )
        {
            vTaskDelay(20 / portTICK_PERIOD_MS);
            for(i=0;i<STOP_QUEUE_LENGTH;i++)
            {
                if(xQueueReceive(STOP_queue, &(stops[i]), (2 / portTICK_PERIOD_MS)))
                {
                    arrived_pulses++;
                }
            }
        }
       
        for(i=0;i<arrived_pulses;i++)
        {
           
            if(stops[i]>start)
            {
                time=(double)(stops[i]-start)/240000000;
            }
            else
            {
                time=(double)(start-stops[i])/240000000;                      
            }*/
            /*if(i==0)
            {
                
                printf("%.12fcentimetros\n",100*343*(time/1000));
                printf("%.12fcentimetros\n",(100*343*(time/1000-((double)7/(double)40000))));
            }*/
            stop_times[i]=time;
           
        }
        
        for(i=0;i<STOP_QUEUE_LENGTH;i++)
        {
            compare_stops[i]=stop_times[0]+((double)i)/((double)Transducer_Freq_Hz);
           // printf("%.12fmseg\n",compare_stops[i]*1000);
        }
        for(i=0;i<arrived_pulses;i++)
        {
            er=10000000;          
            for(k=0;k<STOP_QUEUE_LENGTH;k++)
            {
                er_temp=(compare_stops[k]-stop_times[i])*(compare_stops[k]-stop_times[i]);
               
                if(er_temp<er)
                {
                    er=er_temp;
                    pulse_number[i]=k;
                   
                }
            }  
            
            TOF=TOF+(stop_times[i]-((double)pulse_number[i])/((double)Transducer_Freq_Hz));
            //printf("%.12fmseg\n",(stop_times[i]-((double)pulse_number[i])/((double)Transducer_Freq_Hz))*1000);
            if((stop_times[i]-stop_times[0]-((double)pulse_number[i])/((double)Transducer_Freq_Hz))>0)
            {
                printf(" %.12fmseg\n",(stop_times[i]-stop_times[0]-((double)pulse_number[i])/((double)Transducer_Freq_Hz))*1000);
            }
            else
            {
                 printf("%.12fmseg\n",(stop_times[i]-stop_times[0]-((double)pulse_number[i])/((double)Transducer_Freq_Hz))*1000);
            }
            mean_error=mean_error+(stop_times[i]-stop_times[0]-((double)pulse_number[i])/((double)Transducer_Freq_Hz));
        }
        printf("Mean error:\t%.12fmseg\n",mean_error*1000);
        TOF=TOF/((double)arrived_pulses);
        printf("Time of flight:\t%.12fmseg\n",TOF*1000);
        vTaskDelay(50 / portTICK_PERIOD_MS); 
        
    }
       
}
           
void TDC1000_task(void *pvParameter)
{
    esp_err_t ret;
    spi_device_handle_t handle;
    
    
    ret=init_SPI(&handle);
    if(ret==ESP_OK)
    {
        printf("SPI initialized\n");
    }      
   
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ret=init_TDC1000(&handle);
    if(ret==ESP_OK)
    {
        printf("TDC1000 register initialization sent\n");
    } 
    
    while(1)
    {       
        printf("----------------------\n");
        ret=init_TDC1000(&handle);
        if(ret==ESP_OK)
        {
            printf("TDC1000 register initialization sent\n");
        } 
        vTaskDelay(200 / portTICK_PERIOD_MS);
      
        read_TDC1000_registers(&handle);
        
        vTaskDelay(20000 / portTICK_PERIOD_MS);
       
    }
    //Never reached.
    ret=spi_bus_remove_device(handle);
    assert(ret==ESP_OK);
    
}

            
void trigger_task(void *pvParameter)
{
    gpio_pad_select_gpio(GPIO_ESP_TRIGG);
    gpio_set_direction(GPIO_ESP_TRIGG, GPIO_MODE_OUTPUT);
    while (1)
    {
        gpio_set_level(GPIO_ESP_TRIGG, 1);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_ESP_TRIGG, 0);      
        vTaskDelay(1000 / portTICK_PERIOD_MS);   
        
    }
}

             
