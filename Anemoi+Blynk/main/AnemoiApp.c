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

	ErrorsAndWarnings errorsAndWarningsXpos=NO_ERRORS_NO_WARNINGS;
	ErrorsAndWarnings errorsAndWarningsXneg=NO_ERRORS_NO_WARNINGS;
	ErrorsAndWarnings errorsAndWarningsYpos=NO_ERRORS_NO_WARNINGS;
	ErrorsAndWarnings errorsAndWarningsYneg=NO_ERRORS_NO_WARNINGS;

	while(true)
	{
//		printf("\nLoop:\n");

		printf("X POS: Y1 => X1 \n");
		errorsAndWarningsXpos=measureTimeOfFlight(X_AXIS,POSITIVE_DIRECTION,&xPositiveTimeofFlight);


		printf("Y POS: Y2 => X2 \n");
		errorsAndWarningsYpos=measureTimeOfFlight(Y_AXIS,POSITIVE_DIRECTION,&yPositiveTimeofFlight);


		printf("X NEG: X1 => Y1 \n");
		errorsAndWarningsXneg=measureTimeOfFlight(X_AXIS,NEGATIVE_DIRECTION,&xNegativeTimeofFlight);


		printf("Y NEG: X2 => Y2 \n");
		errorsAndWarningsYneg=measureTimeOfFlight(Y_AXIS,NEGATIVE_DIRECTION,&yNegativeTimeofFlight);


		//test();

		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",xPositiveTimeofFlight*1000000,xNegativeTimeofFlight*1000000,yPositiveTimeofFlight*1000000,yNegativeTimeofFlight*1000000);
		if(errorsAndWarningsXpos!=ERROR_TIME_OUT_OF_RANGE && errorsAndWarningsXneg!=ERROR_TIME_OUT_OF_RANGE )
		{
			if(errorsAndWarningsYpos!=ERROR_TIME_OUT_OF_RANGE && errorsAndWarningsYneg!=ERROR_TIME_OUT_OF_RANGE )
			{
				wind=calculateWind(xPositiveTimeofFlight,xNegativeTimeofFlight,yPositiveTimeofFlight,yNegativeTimeofFlight);
			}
		}
		printf("\n");


		vTaskDelay(10 / portTICK_PERIOD_MS);

	}
}

Wind calculateWind(double xPositiveTime,double xNegativeTime,double yPositiveTime,double yNegativeTime)
{
	Wind wind;
	double xSpeed=0;
	double ySpeed=0;

	xSpeed=X_DISTANCE*(1.0/xPositiveTime-1.0/xNegativeTime)/2.0;
	printf(GREEN"X speed:\t %.2f kn\t"RESET,xSpeed * METERS_PER_SECOND_2_KNOTS);

	ySpeed=Y_DISTANCE*(1.0/yPositiveTime-1.0/yNegativeTime)/2.0;
	printf(GREEN"Y speed:\t %.2f kn\n"RESET,ySpeed * METERS_PER_SECOND_2_KNOTS );

	wind.speed=sqrt(xSpeed*xSpeed+ySpeed*ySpeed);
	wind.direction=atan(xSpeed/ySpeed)*RADIANS_2_DEGREES;

	printf(GREEN"Wind speed:\t %.2f kn "RESET,wind.speed * METERS_PER_SECOND_2_KNOTS);
	if(wind.speed>1)
	{
		printf(GREEN"\t \t Wind Direction:\t %.2f º"RESET,wind.direction);
	}
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
		//	printf(WARNING"Missing stop pulse\n"RESET);
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



