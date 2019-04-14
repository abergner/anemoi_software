


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

#define ESP_INTR_FLAG_DEFAULT 0



static	QueueHandle_t startQueue;
static	QueueHandle_t stopQueue;

static uint32_t startCycles=0;
static uint32_t stopCycles=0;


static void IRAM_ATTR gpioIsrHandlerStartX(void* arg);
static void IRAM_ATTR gpioIsrHandlerStopX(void* arg);

static void IRAM_ATTR gpioIsrHandlerStartY(void* arg);
static void IRAM_ATTR gpioIsrHandlerStopY(void* arg);

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
    initStartStop();
}


bool getQueueCycles(TypeQueue typeQueue, uint32_t * ptrCycles)
{

	if(typeQueue==START_QUEUE)
	{
		if( startQueue != 0 )
		{
			if(xQueueReceive(startQueue, ptrCycles, (1 / portTICK_PERIOD_MS)))
			{
				//printf("Start received\n");
			}
			else
			{
				//printf("Start not received\n");
				return false;
			}
		}
	}
	else if(typeQueue==STOP_QUEUE)
	{
		if( stopQueue != 0 )
		{

			if(xQueueReceive(stopQueue, ptrCycles, (1 / portTICK_PERIOD_MS)))
			{
				//printf("Stop received\n");
			}
			else
			{
				//printf("Stop not received\n");
				return false;
			}
		}
	}
	return true;
}


void initStartStop(void)
{
	startQueue=xQueueCreate( START_QUEUE_LENGTH, sizeof( uint32_t ) );
	if( startQueue == 0 )
	{
	   printf("Failed to create the START X queue.\n");
	}
	stopQueue=xQueueCreate( STOP_QUEUE_LENGTH, sizeof( uint32_t ) );
	if( stopQueue == 0 )
	{
	   printf("Failed to create the STOP X queue.\n");
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

    disableStartStopInterruptX();
    disableStartStopInterruptY();

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
    startCycles=xthal_get_ccount();
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( startQueue != 0 )
    {
        xQueueSendFromISR( startQueue, &startCycles, &xHigherPriorityTaskWoken);
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
}
static void IRAM_ATTR gpioIsrHandlerStartY(void* arg)
{
    startCycles=xthal_get_ccount();
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( startQueue != 0 )
    {
        xQueueSendFromISR(startQueue, &startCycles, &xHigherPriorityTaskWoken);
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
}

static void IRAM_ATTR gpioIsrHandlerStopX(void* arg)
{
    stopCycles=xthal_get_ccount();

    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( stopQueue != 0 )
    {
        xQueueSendFromISR(stopQueue, &stopCycles, &xHigherPriorityTaskWoken );
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
}

static void IRAM_ATTR gpioIsrHandlerStopY(void* arg)
{
    stopCycles=xthal_get_ccount();

    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    if( stopQueue != 0 )
    {
        xQueueSendFromISR( stopQueue, &stopCycles, &xHigherPriorityTaskWoken );
    }
    if( xHigherPriorityTaskWoken )
    {
		// Actual macro used here is port specific.
        portYIELD_FROM_ISR ();
    }
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


