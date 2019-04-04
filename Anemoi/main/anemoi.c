/*
 * anemoi.c
 *
 *  Created on: Mar 28, 2019
 *      Author: alex
 */


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


#include "include/anemoi_gpio.h"
#include "include/anemoi_clock.h"
#include "include/anemoi_spi.h"
#include "include/anemoi_tasks.h"

#include <math.h>

#define X_AXIS 'x'
#define Y_AXIS 'y'
#define POSITIVE_DIRECTION 'p'
#define NEGATIVE_DIRECTION 'n'

#define RADIANS2DEGREES	180.0 / M_PI

typedef struct
{
	double speed;
	double direction;
}Wind;

void initAnemoi(void);
Wind calculateWind(double xPositiveTime,double xNegativeTime, double yPositiveTime,double yNegativeTime);
bool measureTime(char axis,char direction, double * timeofFlight);

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

void anemoi(void)
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
		printf("\nStart Loop\n");

		printf("\tX Positive \n");
		xPositiveMeasurementOK=measureTime(X_AXIS,POSITIVE_DIRECTION,&xPositiveTimeofFlight);
		if(xPositiveMeasurementOK==false)
		{
			printf(RED"X positive direction measurement error\n"RESET);
		}

		printf("\tY Positive \n");
		yPositiveMeasurementOK=measureTime(Y_AXIS,POSITIVE_DIRECTION,&yPositiveTimeofFlight);
		if(yPositiveMeasurementOK==false)
		{
			printf(RED"Y positive direction measurement error\n"RESET);
		}

		printf("\tX Negative \n");
		xNegativeMeasurementOK=measureTime(X_AXIS,NEGATIVE_DIRECTION,&xNegativeTimeofFlight);
		if(xNegativeMeasurementOK==false)
		{
			printf(RED"X negative direction measurement error\n"RESET);
		}

		printf("\tY Negative \n");
		yNegativeMeasurementOK=measureTime(Y_AXIS,NEGATIVE_DIRECTION,&yNegativeTimeofFlight);
		if(yNegativeMeasurementOK==false)
		{
			printf(RED"Y negative direction measurement error\n"RESET);
		}


		if(xPositiveMeasurementOK&&xNegativeMeasurementOK&&yPositiveMeasurementOK&&yNegativeMeasurementOK)
		{
			wind=calculateWind(xPositiveTimeofFlight,xNegativeTimeofFlight,yPositiveTimeofFlight,yNegativeTimeofFlight);
			printf("Wind speed:\t %f m/s \n",wind.speed);
			if(wind.speed>1)
			{
				printf("Wind Direction:\t %f º\n",wind.direction);
			}
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



bool measureTime(char axis,char direction, double * timeofFlight)
{
	bool measurementOK=false;
	if(axis==X_AXIS)
	{

		if(direction==POSITIVE_DIRECTION)
		{
			printf("\tTDC1000 X\t Pulse Y1 to X1\n");
			enableX();
			selectChannel1();
			enableY1Vdd();//hardware correction, pulse going from Y1 to X1
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			printf("\tTDC1000 Y\t Pulse X1 to Y1\n");
			enableY();
			selectChannel1();
			enableX1Vdd();//hardware correction, pulse going from X1 to Y1
		}
	}
	else if(axis==Y_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			printf("\tTDC1000 X\t Pulse Y2 to X2\n");
			enableX();
			selectChannel2();
			enableY2Vdd();//hardware correction, pulse going from Y2 to X2
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			printf("\tTDC1000 Y\t Pulse X2 to Y2\n");
			enableY();
			selectChannel2();
			enableX2Vdd();//hardware correction, pulse going from X2 to Y2
		}
	}
	vTaskDelay(20 / portTICK_PERIOD_MS);
	sendTrigger();
	enableStartStopInterrupt();
	vTaskDelay(500 / portTICK_PERIOD_MS);
	measurementOK=calculateTOF(timeofFlight);
	double TOF=*timeofFlight;
	printf("\t \t TOF:%f ms\n",TOF*1000);

	disableStartStopInterrupt();


	return measurementOK;


}

void initAnemoi(void)
{
	//Init clock of ESP32. Frequency set to 1.28MHz
	initClock();
    //Init pins connected from ESP32 to TDC1000, TDC7200
    initAnemoiGPIO();

    //Create handles for SPI communication and add devices to SPI bus
    esp_err_t ret;
    spi_device_handle_t xHandleTDC1000;
    spi_device_handle_t yHandleTDC1000;
    spi_device_handle_t handleTDC7200;

    ret=initSPI(&xHandleTDC1000,&yHandleTDC1000,&handleTDC7200);

	if(ret==ESP_OK)
	{
		printf("SPI initialized\n");
	}

	ret=initTDC1000SPI(&xHandleTDC1000,NORMAL_CONFIG);
	if(ret==ESP_OK)
	{
		printf("TDC1000 X initialized\n");
	}

	ret=initTDC1000SPI(&yHandleTDC1000,NORMAL_CONFIG);
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
	read_TDC1000_registers(&xHandleTDC1000);
	printf("\nTDC1000 Y \n");
	read_TDC1000_registers(&yHandleTDC1000);


}


