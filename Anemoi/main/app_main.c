/* SPI Slave example, sender (uses SPI master driver)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 * 
 *
*/

/*
SPI sender (master) example.

This example is supposed to work together with the SPI receiver. It uses the standard SPI pins (MISO, MOSI, SCLK, CS) to 
transmit data over in a full-duplex fashion, that is, while the master puts data on the MOSI pin, the slave puts its own
data on the MISO pin.

This example uses one extra pin: GPIO_HANDSHAKE is used as a handshake pin. The slave makes this pin high as soon as it is
ready to receive/send data. This code connects this line to a GPIO interrupt which gives the rdySem semaphore. The main 
task waits for this semaphore to be given before queueing a transmission.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "rom/cache.h"

#include "esp_log.h"
#include "esp_spi_flash.h"


#include "esp_intr_alloc.h"





#include "include/anemoi_gpio.h"
#include "include/anemoi_clock.h"



/*

#define ESP_INTR_FLAG_DEFAULT 0

typedef enum {ADDRESS_CONFIG_0=0, ADDRESS_CONFIG_1, ADDRESS_CONFIG_2 ,ADDRESS_CONFIG_3 ,
ADDRESS_CONFIG_4 ,ADDRESS_TOF_1 ,ADDRESS_TOF_0 ,ADDRESS_ERRORS ,ADDRESS_TIMEOUT  ,ADDRESS_CLOCK_RATE  }TDC1000_Registers_t;

#define TDC1000_REGISTERS 10

#define WRITE       0b01000000
#define CONFIG_0    0x8C //Divide by 8, 16 pulses
#define CONFIG_1    0x40
#define CONFIG_2    0x25
#define CONFIG_3    0x03
#define CONFIG_4    0x1F
#define TOF_1       0xE7
#define TOF_0       0x00
#define ERRORS      0x00
#define TIMEOUT     0x43
#define CLOCK_RATE  0x00

unsigned int init_config[10]={CONFIG_0,CONFIG_1,CONFIG_2,CONFIG_3,CONFIG_4,TOF_1,TOF_0,ERRORS ,TIMEOUT,CLOCK_RATE};



#define START_QUEUE_LENGTH 1
#define STOP_QUEUE_LENGTH 30

QueueHandle_t START_queue;
QueueHandle_t STOP_queue;

uint32_t START_time=0;
uint32_t STOP_time=0;



esp_err_t init_SPI(spi_device_handle_t *handle_ptr)
{
    esp_err_t ret;
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_SPI_MOSI,
        .miso_io_num=GPIO_SPI_MISO,
        .sclk_io_num=GPIO_SPI_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };

    //Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg={
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .clock_speed_hz=1000000,
        .duty_cycle_pos=128,        //50% duty cycle
        .mode=0,
        .spics_io_num=GPIO_TDC1000_X_CSB,
        .cs_ena_posttrans=3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size=2
    };
    
    
    //Initialize the SPI bus and add the device we want to send stuff to.
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    if(ret!=ESP_OK)
    {
        printf("SPI bus could not be initialized\n");        
    }
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, handle_ptr);
    if(ret!=ESP_OK)
    {
        printf("SPI device could not be added to bus\n");    
    }
    return ret;
}

esp_err_t init_TDC1000(spi_device_handle_t * handle_ptr)
{
    esp_err_t ret;
    unsigned int i=0;
    char init_sendbuf[20] = {0};
    char recvbuf[20] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
        
    
    for(i=0;i<TDC1000_REGISTERS;i++)
    {
        init_sendbuf[2*i]=WRITE|((char)i);//dirección registro
        init_sendbuf[2*i+1]=init_config[i];//configuración de registro
    }
    t.length=sizeof(init_sendbuf)*8;
    t.tx_buffer=init_sendbuf;
    t.rx_buffer=recvbuf;
    ret=spi_device_transmit(*handle_ptr, &t);
    if(ret!=ESP_OK)
    {
        printf("TDC1000 initialization was not successful not \n");    
    }
    return ret;
}
esp_err_t read_TDC1000_registers(spi_device_handle_t * handle_ptr)
{
    esp_err_t ret=0;
    unsigned int i=0;
    char init_sendbuf[20] = {0};
    char recvbuf[20] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
        
    
    for(i=0;i<TDC1000_REGISTERS;i++)
    {
        init_sendbuf[2*i]=(char)i;//dirección registro
        init_sendbuf[2*i+1]=0;
    }
    t.length=sizeof(init_sendbuf)*8;
    t.tx_buffer=init_sendbuf;
    t.rx_buffer=recvbuf;
    ret=spi_device_transmit(*handle_ptr, &t);
    if(ret!=ESP_OK)
    {
        printf("TDC1000 read was not successful \n");    
    }
    for(i=0;i<TDC1000_REGISTERS;i++)
    {
        printf("Register %d:%0x\n",i,recvbuf[2*i+1]);
    }
    return ret;
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

/*
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
            /*stop_times[i]=time;
           
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


*/
//Main application
void app_main()
{    
    //init_clock();
    //xTaskCreate(&trigger_task, "trigger_task", 512, NULL, 4, NULL);
    //xTaskCreate(&TDC1000_task, "TDC1000_task", 4096, NULL, 5, NULL);
    //xTaskCreate(&time_measurement_task, "time_measurement_task", 8192, NULL, 6, NULL);*/
    
    //init_gpio_vdd_enable();
    
    while(1)
    {
        
        //printf("Start:%d\n",START_time);
        vTaskDelay(15000 / portTICK_PERIOD_MS); 
    }
    
}



