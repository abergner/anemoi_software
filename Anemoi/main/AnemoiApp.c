/*
 * anemoi.c
 *
 *  Created on: Mar 28, 2019
 *      Author: alex
 */


#include <math.h>

//DRIVERS//
#include "rom/cache.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_event_loop.h"
#include "freertos/task.h"
#include "esp_intr_alloc.h"
#include "driver/spi_master.h"
//DRIVERS//

#include "include/Anemoi.h"
#include "include/TimeMeasurementAnemoi.h"
#include "include/ClockAnemoi.h"
#include "include/GpioAnemoi.h"
#include "include/SpiAnemoi.h"

#define X_AXIS 'x'
#define Y_AXIS 'y'
#define POSITIVE_DIRECTION 'p'
#define NEGATIVE_DIRECTION 'n'

#define RADIANS2DEGREES	180.0 / M_PI



void initAnemoi(void);
Wind calculateWind(double xPositiveTime,double xNegativeTime, double yPositiveTime,double yNegativeTime);
bool measureTime(char axis,char direction, double * timeofFlight);



//void test(void);

void AnemoiApp(void)
{
	initAnemoi();

	Wind wind;

	double xPositiveTimeofFlight=0;
	double xNegativeTimeofFlight=0;
	double yPositiveTimeofFlight=0;
	double yNegativeTimeofFlight=0;

	bool xPositiveMeasurementOK=false;
	bool xNegativeMeasurementOK=false;
	bool yPositiveMeasurementOK=false;
	bool yNegativeMeasurementOK=false;

	while(true)
	{
		printf("\nLoop:\n");

		xPositiveMeasurementOK=measureTime(X_AXIS,POSITIVE_DIRECTION,&xPositiveTimeofFlight);
		if(xPositiveMeasurementOK==false)
		{
			printf(RED"X positive direction measurement error\n"RESET);
		}


		yPositiveMeasurementOK=measureTime(Y_AXIS,POSITIVE_DIRECTION,&yPositiveTimeofFlight);
		if(yPositiveMeasurementOK==false)
		{
			printf(RED"Y positive direction measurement error\n"RESET);
		}
/*
		xNegativeMeasurementOK=measureTime(X_AXIS,NEGATIVE_DIRECTION,&xNegativeTimeofFlight);
		if(xNegativeMeasurementOK==false)
		{
			printf(RED"X negative direction measurement error\n"RESET);
		}

		yNegativeMeasurementOK=measureTime(Y_AXIS,NEGATIVE_DIRECTION,&yNegativeTimeofFlight);
		if(yNegativeMeasurementOK==false)
		{
			printf(RED"Y negative direction measurement error\n"RESET);
		}
*/
		//test();

		if(xPositiveMeasurementOK&&xNegativeMeasurementOK&&yPositiveMeasurementOK&&yNegativeMeasurementOK)
		{
			wind=calculateWind(xPositiveTimeofFlight,xNegativeTimeofFlight,yPositiveTimeofFlight,yNegativeTimeofFlight);
			printf(GREEN"Wind speed:\t %f m/s "RESET,wind.speed);
			if(wind.speed>1)
			{
				printf(GREEN"\t \t Wind Direction:\t %f º"RESET,wind.direction);
			}
			printf("\n ");
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);

	}




}

Wind calculateWind(double xPositiveTime,double xNegativeTime,double yPositiveTime,double yNegativeTime)
{

	Wind wind={0};

	double xDistance=0.2;	//mts
	double yDistance=0.2;	//mts

	double xSpeed=0;
	double ySpeed=0;



	xSpeed=xDistance*(1/xPositiveTime-1/xNegativeTime)/2;
	printf("X speed:\t %f m/s\n",xSpeed );
	ySpeed=yDistance*(1/yPositiveTime-1/yNegativeTime)/2;
	printf("Y speed:\t %f m/s\n",ySpeed );

	wind.speed=sqrt(xSpeed*xSpeed+ySpeed*ySpeed);

	wind.direction=atan(xSpeed/ySpeed)*RADIANS2DEGREES;



	return wind;


}



bool measureTime(char axis,char direction, double * ptrTimeofFlight)
{
	bool measurementOK=false;
	if(axis==X_AXIS)
	{
		selectChannel1();
		if(direction==POSITIVE_DIRECTION)
		{
			printf("\tTDC1000 X Positive\t Pulse Y1 to X1\t");
			enableVddY1();//hardware correction, pulse going from Y1 to X1
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			printf("\tTDC1000 Y Negative \t Pulse X1 to Y1\t");
			enableVddX1();//hardware correction, pulse going from X1 to Y1
		}
	}
	else if(axis==Y_AXIS)
	{
		selectChannel2();
		if(direction==POSITIVE_DIRECTION)
		{
			printf("\tTDC1000 X Positive \t Pulse Y2 to X2\t");
			enableVddY2();//hardware correction, pulse going from Y2 to X2
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			printf("\tTDC1000 Y Negative\t Pulse X2 to Y2\t");

			enableVddX2();//hardware correction, pulse going from X2 to Y2
		}
	}
	//wait for everything to stabilize (just in case)

	if(direction==POSITIVE_DIRECTION)
	{
		enableX();
		vTaskDelay(20 / portTICK_PERIOD_MS);
		enableStartStopInterruptX();
		sendTrigger();
		//wait for transmission and reception
		vTaskDelay(100 / portTICK_PERIOD_MS);
		measurementOK=calculateTOF(ptrTimeofFlight, START_STOP_X);
		disableStartStopInterruptX();
		disableX();

	}
	else if(direction==NEGATIVE_DIRECTION)
	{
		enableY();
		vTaskDelay(20 / portTICK_PERIOD_MS);
		enableStartStopInterruptY();
		sendTrigger();
		//wait for transmission and reception
		vTaskDelay(100 / portTICK_PERIOD_MS);
		measurementOK=calculateTOF(ptrTimeofFlight, START_STOP_Y);
		disableStartStopInterruptY();
		disableY();
	}
	double TOF=*ptrTimeofFlight;
	printf("\t  TOF:%f ms\n",TOF*1000);
	return measurementOK;
}

void initAnemoi(void)
{
	//Init clock of ESP32. Frequency set to 1.28MHz
	initClock();
    //Init pins connected from ESP32 to TDC1000, TDC7200
    initGpio();

    initTimeOfFlightMeasurementHardware();

    //Create handles for SPI communication and add devices to SPI bus
    esp_err_t ret;
    spi_device_handle_t xHandleTDC1000;
    spi_device_handle_t yHandleTDC1000;
    spi_device_handle_t handleTDC7200;

    ret=initSpi(&xHandleTDC1000,&yHandleTDC1000,&handleTDC7200);

	if(ret==ESP_OK)
	{
		printf("SPI initialized\n");
	}

	ret=initRegistersTdc1000(&xHandleTDC1000,NORMAL_CONFIG);
	if(ret==ESP_OK)
	{
		printf("TDC1000 X initialized\n");
	}

	ret=initRegistersTdc1000(&yHandleTDC1000,NORMAL_CONFIG);
	if(ret==ESP_OK)
	{
		printf("TDC1000 Y initialized\n");
	}

	/*ret=init_TDC7200_SPI(&handleTDC7200);
	if(ret==ESP_OK)
	{
		printf("TDC7200 initialized\n");
	}*/
	printf("\nTDC1000 X \n");
	readRegistersTdc1000(&xHandleTDC1000);
	printf("\nTDC1000 Y \n");
	readRegistersTdc1000(&yHandleTDC1000);


}

/*
void test(void)
{
	bool measurementOK=false;
	double TOF;

	printf("\tTDC1000 X \t Pulse X2 to X1\t");
	enableX();
	//selectChannel2();
	enableX2Vdd();//hardware correction, pulse going from X2 to Y2


	vTaskDelay(20 / portTICK_PERIOD_MS);
	sendTrigger();
	enableStartStopInterrupt();
	vTaskDelay(100 / portTICK_PERIOD_MS);
	measurementOK=calculateTOF(&TOF);
	if(measurementOK==true)
	{
		printf("\t  TOF:%f ms\n",TOF*1000);
	}

	disableStartStopInterrupt();

}*/



