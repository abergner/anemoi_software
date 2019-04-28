


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

#define GPIO_ON 1
#define GPIO_OFF 0

static	QueueHandle_t startQueue;
static	QueueHandle_t stopQueue;

static uint32_t startCycles=0;
static uint32_t stopCycles=0;


static void IRAM_ATTR gpioIsrHandlerStart(void* arg);
static void IRAM_ATTR gpioIsrHandlerStop(void* arg);


static void initTdc1000Enable(void);
static void initTrigger(void);
static void initChannelSelection(void);
static void initVddEnable(void);
static void initStartStop(void);

void initGpio(void)
{
    initTdc1000Enable();
    initChannelSelection();
    initVddEnable();
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


static void initStartStop(void)
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
    gpio_isr_handler_add(GPIO_START_X, gpioIsrHandlerStart, (void*) GPIO_START_X);
    gpio_isr_handler_add(GPI_STOP_X, gpioIsrHandlerStop, (void*) GPI_STOP_X);

    gpio_isr_handler_add(GPI_START_Y, gpioIsrHandlerStart, (void*) GPI_START_Y);
    gpio_isr_handler_add(GPI_STOP_Y, gpioIsrHandlerStop, (void*) GPI_STOP_Y);

    disableStartStopInterrupt();

}

void enableStartStopInterrupt(Direction direction)
{
	disableStartStopInterrupt();
	if(direction==POSITIVE_DIRECTION)
	{
		gpio_intr_enable(GPIO_START_X);
		gpio_intr_enable(GPI_STOP_X);
	}
	else if(direction==NEGATIVE_DIRECTION)
	{
		gpio_intr_enable(GPI_START_Y);
		gpio_intr_enable(GPI_STOP_Y);
	}

}
void disableStartStopInterrupt(void)
{
	gpio_intr_disable(GPI_START_Y);
	gpio_intr_disable(GPI_STOP_Y);
	gpio_intr_disable(GPIO_START_X);
	gpio_intr_disable(GPI_STOP_X);
}



static void IRAM_ATTR gpioIsrHandlerStart(void* arg)
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
static void IRAM_ATTR gpioIsrHandlerStop(void* arg)
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

void initTrigger(void)
{
	gpio_pad_select_gpio(GPIO_ESP_TRIGG);
	gpio_set_direction(GPIO_ESP_TRIGG, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_ESP_TRIGG, GPIO_OFF);
}
void sendTrigger(void)
{
	gpio_set_level(GPIO_ESP_TRIGG, GPIO_ON);
	vTaskDelay(1 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_ESP_TRIGG, GPIO_OFF);
}

static void initTdc1000Enable(void)
{
    gpio_pad_select_gpio(GPIO_TDC1000_X_EN);
    gpio_set_direction(GPIO_TDC1000_X_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_TDC1000_X_EN, GPIO_OFF);

    gpio_pad_select_gpio(GPIO_TDC1000_Y_EN);
    gpio_set_direction(GPIO_TDC1000_Y_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_TDC1000_Y_EN, GPIO_OFF);
}


void enableTdc1000(Direction direction)
{
	disableTdc1000();
	if(direction==POSITIVE_DIRECTION)
	{
		gpio_set_level(GPIO_TDC1000_X_EN, GPIO_ON);
	}
	else if(direction==NEGATIVE_DIRECTION)
	{
		gpio_set_level(GPIO_TDC1000_Y_EN, GPIO_ON);
	}
}


void disableTdc1000(void)
{
	gpio_set_level(GPIO_TDC1000_X_EN, GPIO_OFF);
	gpio_set_level(GPIO_TDC1000_Y_EN, GPIO_OFF);
}



static void initChannelSelection(void)
{
    gpio_pad_select_gpio(GPIO_CHSEL);
    gpio_set_direction(GPIO_CHSEL, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_CHSEL, GPIO_OFF);
}

void selectChannel(Axis axis)
{
	if(axis==X_AXIS)
	{
		gpio_set_level(GPIO_CHSEL, GPIO_OFF);
	}
	else if(axis==Y_AXIS)
	{
		gpio_set_level(GPIO_CHSEL, GPIO_ON);
	}
}

static void initVddEnable(void)
{
    gpio_pad_select_gpio(GPIO_VDD_EN_X1);
    gpio_set_direction(GPIO_VDD_EN_X1, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_VDD_EN_X1, GPIO_OFF);

    gpio_pad_select_gpio(GPIO_VDD_EN_X2);
    gpio_set_direction(GPIO_VDD_EN_X2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_VDD_EN_X2, GPIO_OFF);

    gpio_pad_select_gpio(GPIO_VDD_EN_Y1);
    gpio_set_direction(GPIO_VDD_EN_Y1, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_VDD_EN_Y1, GPIO_OFF);

    gpio_pad_select_gpio(GPIO_VDD_EN_Y2);
    gpio_set_direction(GPIO_VDD_EN_Y2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_VDD_EN_Y2, GPIO_OFF);
}

void enableVdd(Axis axis, Direction direction)
{
    disableVdd();
	if(axis==X_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			gpio_set_level(GPIO_VDD_EN_Y1, GPIO_ON);
			//hardware correction, pulse going from Y1 to X1
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			gpio_set_level(GPIO_VDD_EN_X1, GPIO_ON);
			//hardware correction, pulse going from X1 to Y1
		}
	}
	else if(axis==Y_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			gpio_set_level(GPIO_VDD_EN_Y2, GPIO_ON);
			//hardware correction, pulse going from Y2 to X2
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			gpio_set_level(GPIO_VDD_EN_X2, GPIO_ON);
			//hardware correction, pulse going from X2 to Y2
		}
	}
}


void disableVdd(void)
{
    gpio_set_level(GPIO_VDD_EN_X1, GPIO_OFF);
    gpio_set_level(GPIO_VDD_EN_X2, GPIO_OFF);
    gpio_set_level(GPIO_VDD_EN_Y1, GPIO_OFF);
    gpio_set_level(GPIO_VDD_EN_Y2, GPIO_OFF);
}


