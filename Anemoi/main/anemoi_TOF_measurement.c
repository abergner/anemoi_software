/*
 * anemoi_TOF_measurement.c
 *
 *  Created on: Apr 11, 2019
 *      Author: alex
 */
#include "driver/gpio.h"
#include "soc/gpio_reg.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "include/anemoi_pinout.h"
#include "include/anemoi_TOF_measurement.h"
#include "include/anemoi_clock.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define START_QUEUE_LENGTH 1
#define STOP_QUEUE_LENGTH 30


static QueueHandle_t startQueueX;
static QueueHandle_t stopQueueX;
static QueueHandle_t startQueueY;
static QueueHandle_t stopQueueY;


static uint32_t startTimeX=0;
static uint32_t stopTimeX=0;
static uint32_t startTimeY=0;
static uint32_t stopTimeY=0;


static void IRAM_ATTR gpioIsrHandlerStartX(void* arg);
static void IRAM_ATTR gpioIsrHandlerStopX(void* arg);

static void IRAM_ATTR gpioIsrHandlerStartY(void* arg);
static void IRAM_ATTR gpioIsrHandlerStopY(void* arg);


bool calculateTOF(double * ptrTimeofFlight, StartStop startStop)
{
	uint32_t start=0;
	uint32_t stops[STOP_QUEUE_LENGTH]={0};
	unsigned int i=0;
	//unsigned int k=0;
	unsigned int arrived_pulses=0;

	double times[STOP_QUEUE_LENGTH]={0};
	//double compare_stops[STOP_QUEUE_LENGTH]={0};
	//double er_temp=0;
	//double mean_error=0;
	//double er=100000000;
	//unsigned int pulse_number[STOP_QUEUE_LENGTH]={0};
	double TOF=0;

	TOF=0;
	arrived_pulses=0;
	//mean_error=0;
	if(startStop==START_STOP_X)
	{
		if( startQueueX != 0 )
		{
			if(xQueueReceive(startQueueX, &start, (1 / portTICK_PERIOD_MS)))
			{
				//printf("Start received\n");
			}
			else
			{

				return false;
			}
		}
		if( stopQueueX != 0 )
		{
			vTaskDelay(20 / portTICK_PERIOD_MS);
			for(i=0;i<STOP_QUEUE_LENGTH;i++)
			{
				if(xQueueReceive(stopQueueX, &(stops[i]), (1 / portTICK_PERIOD_MS)))
				{
					arrived_pulses++;
				}
			}
		}
		if(arrived_pulses==0)
		{
			return false;
		}
	}
	else if(startStop==START_STOP_Y)
	{
		if( startQueueY != 0 )
		{
			if(xQueueReceive(startQueueY, &start, (1 / portTICK_PERIOD_MS)))
			{
				//printf("Start received\n");
			}
			else
			{
				return false;
			}
		}
		if( stopQueueY != 0 )
		{
			vTaskDelay(20 / portTICK_PERIOD_MS);
			for(i=0;i<STOP_QUEUE_LENGTH;i++)
			{
				if(xQueueReceive(stopQueueY, &(stops[i]), (1 / portTICK_PERIOD_MS)))
				{
					arrived_pulses++;
				}
			}
		}
		if(arrived_pulses==0)
		{
			return false;
		}
	}

	for(i=0;i<arrived_pulses;i++)
	{

		if(stops[i]>start)
		{
			times[i]=(double)(stops[i]-start)/240000000;
		}
		else
		{
			times[i]=(double)(start-stops[i])/240000000;
		}
	}
	for(i=0;i<arrived_pulses;i++)
	{
		TOF=TOF+times[arrived_pulses-1-i]-((double) (26-i))/((double)TRANSDUCER_FREQUENCY_IN_HZ );
	}
	TOF=TOF/arrived_pulses;

	//TOF=times[arrived_pulses-1-i]-((double) 26)/((double)Transducer_Freq_Hz);
	*ptrTimeofFlight=TOF;


	if((TOF>0.0004)&&(TOF<0.001))
	{
		return true;
	}
	else
	{
		return false;
	}

}




void initTimeOfFlightMeasurementHardware(void)
{
	startQueueX=xQueueCreate( START_QUEUE_LENGTH, sizeof( uint32_t ) );
	if( startQueueX == 0 )
	{
	   printf("Failed to create the START X queue.\n");
	}
	stopQueueX=xQueueCreate( STOP_QUEUE_LENGTH, sizeof( uint32_t ) );
	if( stopQueueX == 0 )
	{
	   printf("Failed to create the STOP X queue.\n");
	}

	startQueueY=xQueueCreate( START_QUEUE_LENGTH, sizeof( uint32_t ) );
	if( startQueueY == 0 )
	{
	   printf("Failed to create the START Y queue.\n");
	}
	stopQueueY=xQueueCreate( STOP_QUEUE_LENGTH, sizeof( uint32_t ) );
	if( stopQueueY == 0 )
	{
	   printf("Failed to create the STOP Y queue.\n");
	}


	gpio_pad_select_gpio(GPIO_START_X);
	gpio_set_direction(GPIO_START_X, GPIO_MODE_INPUT);
	gpio_set_intr_type(GPIO_START_X, GPIO_PIN_INTR_POSEDGE);

	gpio_pad_select_gpio(GPI_STOP_X);
	gpio_set_direction(GPI_STOP_X, GPIO_MODE_INPUT);
	gpio_set_intr_type(GPI_STOP_X, GPIO_PIN_INTR_POSEDGE);

	gpio_pad_select_gpio(GPI_START_Y);
	gpio_set_direction(GPI_START_Y, GPIO_MODE_INPUT);
	gpio_set_intr_type(GPI_START_Y, GPIO_PIN_INTR_POSEDGE);

	gpio_pad_select_gpio(GPI_STOP_Y);
	gpio_set_direction(GPI_STOP_Y, GPIO_MODE_INPUT);
	gpio_set_intr_type(GPI_STOP_Y, GPIO_PIN_INTR_POSEDGE);


    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_START_X, gpioIsrHandlerStartX, (void*) GPIO_START_X);
    gpio_isr_handler_add(GPI_STOP_X, gpioIsrHandlerStopX, (void*) GPI_STOP_X);
    gpio_isr_handler_add(GPI_START_Y, gpioIsrHandlerStartY, (void*) GPI_START_Y);
    gpio_isr_handler_add(GPI_STOP_Y, gpioIsrHandlerStopY, (void*) GPI_STOP_Y);

}

void enableStartStopInterruptX(void)
{
	gpio_intr_enable(GPIO_START_X);
	gpio_intr_enable(GPI_STOP_X);
}
void enableStartStopInterruptY(void)
{
	gpio_intr_enable(GPI_START_Y);
	gpio_intr_enable(GPI_STOP_Y);
}

void disableStartStopInterruptX(void)
{
	gpio_intr_disable(GPIO_START_X);
	gpio_intr_disable(GPI_STOP_X);
}

void disableStartStopInterruptY(void)
{
	gpio_intr_disable(GPI_START_Y);
	gpio_intr_disable(GPI_STOP_Y);
}



static void IRAM_ATTR gpioIsrHandlerStartX(void* arg)
{
    startTimeX=xthal_get_ccount();
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( startQueueX != 0 )
    {
        xQueueSendFromISR( startQueueX, &startTimeX, &xHigherPriorityTaskWoken);
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
}
static void IRAM_ATTR gpioIsrHandlerStartY(void* arg)
{
    startTimeY=xthal_get_ccount();
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( startQueueY != 0 )
    {
        xQueueSendFromISR( startQueueY, &startTimeY, &xHigherPriorityTaskWoken);
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
}

static void IRAM_ATTR gpioIsrHandlerStopX(void* arg)
{
    stopTimeX=xthal_get_ccount();

    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( stopQueueX != 0 )
    {
        xQueueSendFromISR( stopQueueX, &stopTimeX, &xHigherPriorityTaskWoken );
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
}

static void IRAM_ATTR gpioIsrHandlerStopY(void* arg)
{
    stopTimeY=xthal_get_ccount();

    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( stopQueueY != 0 )
    {
        xQueueSendFromISR( stopQueueY, &stopTimeY, &xHigherPriorityTaskWoken );
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
}
