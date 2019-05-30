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
char auth[] = "2a794a4bb48f4d31acf3ef060b966e4a";//cc6ed5a1905d42d4b6d029ff51f91fac



char message[50];


extern "C" void initAnemoi(void);
extern "C" Wind calculateWind(double xPositiveTime,double xNegativeTime, double yPositiveTime,double yNegativeTime);



extern "C" void app_main()
{
	int i=0,k=0;
   	Wind wind[10];
   	for(i=0; i < 10; i++)
   	{
   		wind[i].speed=0;
   		wind[i].direction=0;
   		wind[i].xSpeed=0;
   		wind[i].ySpeed=0;
   		wind[i].temperature=0;
   		wind[i].soundSpeed=0;
   	}
	i=0;

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
				wind[i]=calculateWind(xPositiveTimeofFlight,xNegativeTimeofFlight,yPositiveTimeofFlight,yNegativeTimeofFlight);
			}
		}
		printf("\n");


		for(k=0;k<10;k++)
		{
			printf(GREEN "X speed: %.2f kn\t" RESET,wind[k].xSpeed * METERS_PER_SECOND_2_KNOTS);
			printf(GREEN "Y speed: %.2f kn\t" RESET,wind[k].ySpeed * METERS_PER_SECOND_2_KNOTS );
			printf(GREEN "Wind speed: %.2f kn \n " RESET,wind[k].speed * METERS_PER_SECOND_2_KNOTS);
		}



		Blynk.run();
		if(wind[i].speed > LOW_SPEED)
		{
			printf(GREEN "\t \t Wind Direction:\t %.2f º\n" RESET,wind[i].direction);

			sprintf(message, "X speed: %.2f kn     Y speed: %.2f kn", wind[i].xSpeed * METERS_PER_SECOND_2_KNOTS, wind[i].ySpeed * METERS_PER_SECOND_2_KNOTS );
			Blynk.virtualWrite(V1, message);
			sprintf(message, "Wind speed: %.2f m/s",wind[i].speed);
			Blynk.virtualWrite(V2, message);
			sprintf(message, "Wind speed: %.2f knots",wind[i].speed * METERS_PER_SECOND_2_KNOTS);
			Blynk.virtualWrite(V3, message);
			sprintf(message, "Wind speed: %.2f km/h",wind[i].speed * METERS_PER_SECOND_2_KILOMETERS_PER_HOUR);
			Blynk.virtualWrite(V4, message);
			sprintf(message, "Wind direction: %.2f º",wind[i].direction);
			Blynk.virtualWrite(V5, message);
			sprintf(message, "Temperature: %.1f ºC",wind[i].temperature);
			Blynk.virtualWrite(V6, message);
		}
		else
		{
			sprintf(message, "Low speed  " );
			Blynk.virtualWrite(V1, message);
			sprintf(message, "Wind speed: < 0.5 m/s");
			Blynk.virtualWrite(V2, message);
			sprintf(message, "Low speed");
			Blynk.virtualWrite(V3, message);
			sprintf(message, "  ");
			Blynk.virtualWrite(V4, message);
			sprintf(message, "Wind direction: low speed");
			Blynk.virtualWrite(V5, message);
			sprintf(message, "Temperature: %.1f ºC",wind[i].temperature);
			Blynk.virtualWrite(V6, message);
		}

		i++;
		if(i==10)
		{
			i=0;
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

}

#define NEG_TRESH (0.95/(double)TRANSDUCER_FREQUENCY_IN_HZ)
#define POS_TRESH (1.05/(double)TRANSDUCER_FREQUENCY_IN_HZ)
#define ZERO_WIND_TIME 0.000425



Wind calculateWind(double xPositiveTime,double xNegativeTime,double yPositiveTime,double yNegativeTime)
{
	Wind wind;

	printf("fabs pos: %.2f   neg: %.2f\n",1000000*fabs(yPositiveTime-ZERO_WIND_TIME),1000000*fabs(yNegativeTime-ZERO_WIND_TIME));
	printf("0.5/freq: %.2f \n",  1000000*(0.5/(double)TRANSDUCER_FREQUENCY_IN_HZ) );
	if(fabs(fabs(yPositiveTime-ZERO_WIND_TIME)-fabs(yNegativeTime-ZERO_WIND_TIME)) > (0.75/(double)TRANSDUCER_FREQUENCY_IN_HZ))
	{
		printf("Correction\n");
		if(fabs(yPositiveTime-ZERO_WIND_TIME)>fabs(yNegativeTime-ZERO_WIND_TIME))
		{
			yPositiveTime=yPositiveTime-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		}
		else
		{
			yNegativeTime=yNegativeTime-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		}
	}

	if(fabs(fabs(xPositiveTime-ZERO_WIND_TIME)-fabs(xNegativeTime-ZERO_WIND_TIME)) > (0.75/(double)TRANSDUCER_FREQUENCY_IN_HZ))
	{
		printf("Correction\n");
		if(fabs(xPositiveTime-ZERO_WIND_TIME)>fabs(xNegativeTime-ZERO_WIND_TIME))
		{
			xPositiveTime=xPositiveTime-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		}
		else
		{
			xNegativeTime=xNegativeTime-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		}
	}

	wind.xSpeed=X_DISTANCE*(1.0/xPositiveTime-1.0/xNegativeTime)/2.0;
	wind.ySpeed=Y_DISTANCE*(1.0/yPositiveTime-1.0/yNegativeTime)/2.0;
	

	wind.speed=sqrt(wind.xSpeed*wind.xSpeed+wind.ySpeed*wind.ySpeed);
	wind.direction=atan(wind.ySpeed/wind.xSpeed)*RADIANS_2_DEGREES;
	if(wind.xSpeed<0)
	{
		//to get full 360º instead of only 180º
		wind.direction=wind.direction+180.0;


	}

	if(wind.xSpeed > wind.ySpeed)
	{
		wind.soundSpeed=X_DISTANCE*(1.0/xPositiveTime+1.0/xNegativeTime)/2.0;

	}
	else
	{
		wind.soundSpeed=Y_DISTANCE*(1.0/yPositiveTime+1.0/yNegativeTime)/2.0;
	}
	wind.temperature=(wind.soundSpeed-ZERO_CELSIUS_SPEED_OF_SOUND)/SPEED_OF_SOUND_TEMPERATURE_COEFFICIENT;
	printf("Temperature: %.2f\n",wind.temperature);
	
	
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


	
