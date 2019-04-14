/*
 * anemoi.c
 *
 *  Created on: Mar 28, 2019
 *      Author: alex
 */


#include <math.h>

//DRIVERS//
#include "esp_err.h"
#include "esp_event_loop.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
//DRIVERS//

#include "include/Anemoi.h"
#include "include/TimeMeasurementAnemoi.h"
#include "include/ClockAnemoi.h"
#include "include/SpiAnemoi.h"

#define RADIANS2DEGREES	180.0 / M_PI


void initAnemoi(void);
Wind calculateWind(double xPositiveTime,double xNegativeTime, double yPositiveTime,double yNegativeTime);
void processErrorsAndWarnings(ErrorsAndWarnings errorsAndWarnings);

//void test(void);

void AnemoiApp(void)
{
	initAnemoi();

	Wind wind;

	double xPositiveTimeofFlight=0;
	double xNegativeTimeofFlight=0;
	double yPositiveTimeofFlight=0;
	double yNegativeTimeofFlight=0;

	ErrorsAndWarnings errorsAndWarnings;

	while(true)
	{
		printf("\nLoop:\n");

		errorsAndWarnings=measureTimeOfFlight(X_AXIS,POSITIVE_DIRECTION,&xPositiveTimeofFlight);
		processErrorsAndWarnings(errorsAndWarnings);


		errorsAndWarnings=measureTimeOfFlight(Y_AXIS,POSITIVE_DIRECTION,&yPositiveTimeofFlight);
		processErrorsAndWarnings(errorsAndWarnings);
/*
		errorsAndWarnings=measureTimeOfFlight(X_AXIS,NEGATIVE_DIRECTION,&xNegativeTimeofFlight);
		processErrorsAndWarnings(errorsAndWarnings);

		errorsAndWarnings=measureTimeOfFlight(Y_AXIS,NEGATIVE_DIRECTION,&yNegativeTimeofFlight);
		processErrorsAndWarnings(errorsAndWarnings);
*/
		//test();

		if(errorsAndWarnings==NO_ERRORS_NO_WARNINGS)
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
	Wind wind;
	double xSpeed=0;
	double ySpeed=0;

	xSpeed=X_DISTANCE*(1/xPositiveTime-1/xNegativeTime)/2;
	printf("X speed:\t %f m/s\n",xSpeed );
	ySpeed=Y_DISTANCE*(1/yPositiveTime-1/yNegativeTime)/2;
	printf("Y speed:\t %f m/s\n",ySpeed );
	wind.speed=sqrt(xSpeed*xSpeed+ySpeed*ySpeed);
	wind.direction=atan(xSpeed/ySpeed)*RADIANS2DEGREES;

	return wind;
}



void initAnemoi(void)
{
	 //Create handles for SPI communication and add devices to SPI bus
	esp_err_t ret;
	spi_device_handle_t xHandleTDC1000;
	spi_device_handle_t yHandleTDC1000;
	spi_device_handle_t handleTDC7200;

	//Init clock of ESP32. Frequency set to 1.28MHz
	initClock();

	initTimeMeasurementHardware();

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
	printf("\nTDC1000 X \n");
	readRegistersTdc1000(&xHandleTDC1000);
	printf("\nTDC1000 Y \n");
	readRegistersTdc1000(&yHandleTDC1000);

}

void processErrorsAndWarnings(ErrorsAndWarnings errorsAndWarnings)
{
	switch(errorsAndWarnings)
	{
		case NO_ERRORS_NO_WARNINGS:
			break;
		case ERROR_NO_STOP_RECEIVED:
			printf(ERROR"No stop received\n"RESET);
			break;
		case ERROR_NO_START_RECEIVED:
			printf(ERROR"No start received\n"RESET);
			break;
		case WARNING_MISSING_STOP_PULSE:
			printf(WARNING"Missing stop pulse\n"RESET);
			break;
		case ERROR_TIME_OUT_OF_RANGE:
			printf(ERROR"Time out of range\n"RESET);
			break;
		default:
			break;
	}
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



