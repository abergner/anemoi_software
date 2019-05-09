/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

//DRIVERS//
#include "esp_err.h"
#include "esp_event_loop.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
//DRIVERS//

#define BLYNK_USE_DIRECT_CONNECT

#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <string.h>

#include "Arduino.h"

#include <math.h>

extern "C"{ 
#include "include/Anemoi.h"
#include "include/TimeMeasurementAnemoi.h"
#include "include/ClockAnemoi.h"
#include "include/SpiAnemoi.h"
}

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "cc6ed5a1905d42d4b6d029ff51f91fac";//cc6ed5a1905d42d4b6d029ff51f91fac


double i=10.0;
char message[50];


extern "C" void initAnemoi(void);
extern "C" Wind calculateWind(double xPositiveTime,double xNegativeTime, double yPositiveTime,double yNegativeTime);


extern "C" void app_main()
{
   	Wind wind;
	wind.speed=0;
	wind.direction=0;
	wind.xSpeed=0;
	wind.ySpeed=0;

	double xPositiveTimeofFlight=0;
	double xNegativeTimeofFlight=0;
	double yPositiveTimeofFlight=0;
	double yNegativeTimeofFlight=0;

	ErrorsAndWarnings errorsAndWarningsXpos=NO_ERRORS_NO_WARNINGS;
	ErrorsAndWarnings errorsAndWarningsXneg=NO_ERRORS_NO_WARNINGS;
	ErrorsAndWarnings errorsAndWarningsYpos=NO_ERRORS_NO_WARNINGS;
	ErrorsAndWarnings errorsAndWarningsYneg=NO_ERRORS_NO_WARNINGS;

	initAnemoi();

	//initArduino();
	Blynk.setDeviceName("Anemoi Hardware");
  	Blynk.begin(auth);

  	
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

		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",xPositiveTimeofFlight*1000000,xNegativeTimeofFlight*1000000,yPositiveTimeofFlight*1000000,yNegativeTimeofFlight*1000000);

		if(errorsAndWarningsXpos!=ERROR_TIME_OUT_OF_RANGE && errorsAndWarningsXneg!=ERROR_TIME_OUT_OF_RANGE )
		{
			if(errorsAndWarningsYpos!=ERROR_TIME_OUT_OF_RANGE && errorsAndWarningsYneg!=ERROR_TIME_OUT_OF_RANGE )
			{
				wind=calculateWind(xPositiveTimeofFlight,xNegativeTimeofFlight,yPositiveTimeofFlight,yNegativeTimeofFlight);
			}
		}
		printf("\n");


		

		Blynk.run();
		sprintf(message, "X speed: %.2f kn     Y speed: %.2f kn", wind.xSpeed * METERS_PER_SECOND_2_KNOTS, wind.ySpeed * METERS_PER_SECOND_2_KNOTS );
		Blynk.virtualWrite(V1, message);

		sprintf(message, "Wind speed: %.2f m/s",wind.speed);
		Blynk.virtualWrite(V2, message);
		sprintf(message, "Wind speed: %.2f knots",wind.speed * METERS_PER_SECOND_2_KNOTS);
		Blynk.virtualWrite(V3, message);
		sprintf(message, "Wind speed: %.2f km/h",wind.speed * METERS_PER_SECOND_2_KILOMETERS_PER_HOUR);
		Blynk.virtualWrite(V4, message);
		if(wind.speed>1)
		{
			printf(GREEN "\t \t Wind Direction:\t %.2f º\n" RESET,wind.direction);
			sprintf(message, "Wind direction: %.2fº",wind.direction);
			Blynk.virtualWrite(V5, message);
		}
		else
		{
			sprintf(message, "Wind direction: low speed");
			Blynk.virtualWrite(V5, message);
		}

 		
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}

}


Wind calculateWind(double xPositiveTime,double xNegativeTime,double yPositiveTime,double yNegativeTime)
{
	Wind wind;

	wind.xSpeed=X_DISTANCE*(1.0/xPositiveTime-1.0/xNegativeTime)/2.0;
	printf(GREEN "X speed:\t %.2f kn\t" RESET,wind.xSpeed * METERS_PER_SECOND_2_KNOTS);

	wind.ySpeed=Y_DISTANCE*(1.0/yPositiveTime-1.0/yNegativeTime)/2.0;
	printf(GREEN "Y speed:\t %.2f kn\n" RESET,wind.ySpeed * METERS_PER_SECOND_2_KNOTS );

	

	wind.speed=sqrt(wind.xSpeed*wind.xSpeed+wind.ySpeed*wind.ySpeed);
	wind.direction=atan(wind.xSpeed/wind.ySpeed)*RADIANS_2_DEGREES+90.0;
	if(wind.xSpeed<0)
	{
		//to get full 360º instead of only 180º
		wind.direction=wind.direction+180.0;
	}

	printf(GREEN "Wind speed:\t %.2f kn " RESET,wind.speed * METERS_PER_SECOND_2_KNOTS);
	

	
	
	
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


	
