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
#include "include/NmeaAnemoi.h"
}

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "2a794a4bb48f4d31acf3ef060b966e4a";//cc6ed5a1905d42d4b6d029ff51f91fac



char message[50];


extern "C" void initAnemoi(void);
extern "C" Wind calculateWind(double xPositiveTime,double xNegativeTime, double yPositiveTime,double yNegativeTime);

double AngleSpeedCorrection(double speed,double direction);

extern "C" void app_main()
{
	int i=0,k=0,j=0;
   	Wind wind[10],correctedWind[10];
   	for(i=0; i < 10; i++)
   	{
   		wind[i].speed=0;
   		wind[i].direction=0;
   		wind[i].xSpeed=0;
   		wind[i].ySpeed=0;
   		wind[i].temperature=0;
   		wind[i].soundSpeed=0;
   		correctedWind[i]=wind[i];
   	}
   	i=0;
   	double speedAverage=0;

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
		#ifdef DEBUG_PRINTS
		printf("X POS: Y1 => X1 \n");
		#endif
		errorsAndWarningsXpos=measureTimeOfFlight(X_AXIS,POSITIVE_DIRECTION,&xPositiveTimeofFlight);

		#ifdef DEBUG_PRINTS
		printf("Y POS: Y2 => X2 \n");
		#endif
		errorsAndWarningsYpos=measureTimeOfFlight(Y_AXIS,POSITIVE_DIRECTION,&yPositiveTimeofFlight);

		#ifdef DEBUG_PRINTS
		printf("X NEG: X1 => Y1 \n");
		#endif
		errorsAndWarningsXneg=measureTimeOfFlight(X_AXIS,NEGATIVE_DIRECTION,&xNegativeTimeofFlight);

		#ifdef DEBUG_PRINTS
		printf("Y NEG: X2 => Y2 \n");
		#endif
		errorsAndWarningsYneg=measureTimeOfFlight(Y_AXIS,NEGATIVE_DIRECTION,&yNegativeTimeofFlight);

		if(errorsAndWarningsYneg==ERROR_NO_STOP_RECEIVED||errorsAndWarningsYpos==ERROR_NO_STOP_RECEIVED)
		{
			Blynk.virtualWrite(V7,"BLOCKED PATH!!!");
			#ifdef DEBUG_PRINTS
			printf(RED "BLOCKED PATH!!!\n" RESET);
			#endif
		}
		else if(errorsAndWarningsXneg==ERROR_NO_STOP_RECEIVED||errorsAndWarningsXpos==ERROR_NO_STOP_RECEIVED)
		{
			Blynk.virtualWrite(V7,"BLOCKED PATH!!!");
			#ifdef DEBUG_PRINTS
			printf(RED "BLOCKED PATH!!!\n" RESET);
			#endif
		}
		else
		{
			Blynk.virtualWrite(V7," ");
		}

		#ifdef DEBUG_PRINTS
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",xPositiveTimeofFlight*1000000,xNegativeTimeofFlight*1000000,yPositiveTimeofFlight*1000000,yNegativeTimeofFlight*1000000);
		#endif

		if(errorsAndWarningsXpos!=ERROR_TIME_OUT_OF_RANGE && errorsAndWarningsXneg!=ERROR_TIME_OUT_OF_RANGE )
		{
			if(errorsAndWarningsYpos!=ERROR_TIME_OUT_OF_RANGE && errorsAndWarningsYneg!=ERROR_TIME_OUT_OF_RANGE )
			{
				SoftCalibration(&xPositiveTimeofFlight,&xNegativeTimeofFlight,&yPositiveTimeofFlight,&yNegativeTimeofFlight);
				i++;
				if(i==10)
				{
					i=0;
				}
				wind[i]=calculateWind(xPositiveTimeofFlight,xNegativeTimeofFlight,yPositiveTimeofFlight,yNegativeTimeofFlight);
				printf("Wind Direction:%.2f º\n",wind[i].direction);
				printf("Speed: %.2f kts \n" ,wind[i].speed * METERS_PER_SECOND_2_KNOTS);
				//wind[i].speed=AngleSpeedCorrection(wind[i].speed,wind[i].direction);
				//printf("Speed corrected: %.2f kts \n" ,wind[i].speed * METERS_PER_SECOND_2_KNOTS);
				for(k=i,j=0,speedAverage=0;j<AVERAGE_COUNT;j++)
				{
					speedAverage=speedAverage+wind[k].speed;
					k--;
					if(k<0)
					{
						k=9;
					}
				}
				speedAverage=speedAverage/(double)AVERAGE_COUNT;
				printf("Speed average: %.2f kts \n" ,speedAverage * METERS_PER_SECOND_2_KNOTS);

				correctedWind[i]=wind[i];
				correctedWind[i].speed=AngleSpeedCorrection(correctedWind[i].speed,correctedWind[i].direction);
				printf("Speed corrected: %.2f kts \n" ,correctedWind[i].speed * METERS_PER_SECOND_2_KNOTS);
				for(k=i,j=0,speedAverage=0;j<AVERAGE_COUNT;j++)
				{
					speedAverage=speedAverage+correctedWind[k].speed;
					k--;
					if(k<0)
					{
						k=9;
					}
				}
				speedAverage=speedAverage/(double)AVERAGE_COUNT;
				printf("Speed corrected average: %.2f kts \n" ,speedAverage * METERS_PER_SECOND_2_KNOTS);

			}
		}
		#ifdef DEBUG_PRINTS
		printf("\n");
		printf(GREEN "Current X speed: %.2f kn\t" RESET,wind[i].xSpeed * METERS_PER_SECOND_2_KNOTS);
		printf(GREEN "Current Y speed: %.2f kn\t" RESET,wind[i].ySpeed * METERS_PER_SECOND_2_KNOTS );
		printf(GREEN "Current Wind speed: %.2f kn \n " RESET,wind[i].speed * METERS_PER_SECOND_2_KNOTS);
		#endif
		for(k=0;k<10;k++)
		{
			#ifdef DEBUG_PRINTS
			printf(GREEN "X speed: %.2f kn\t" RESET,wind[k].xSpeed * METERS_PER_SECOND_2_KNOTS);
			printf(GREEN "Y speed: %.2f kn\t" RESET,wind[k].ySpeed * METERS_PER_SECOND_2_KNOTS );
			printf(GREEN "Wind speed: %.2f kn \n " RESET,wind[k].speed * METERS_PER_SECOND_2_KNOTS);
			#endif
		}


		//sendNmeaWindData(67.5,4.8,'N');

		Blynk.run();
		if(wind[i].speed > LOW_SPEED)
		{
			sendNmeaWindData(wind[i].direction,speedAverage,'M');

			//printf( "%.2f \t" ,wind[i].direction);

			sprintf(message, "X speed: %.2f kn     Y speed: %.2f kn", wind[i].xSpeed * METERS_PER_SECOND_2_KNOTS, wind[i].ySpeed * METERS_PER_SECOND_2_KNOTS );
			Blynk.virtualWrite(V1, message);
			sprintf(message, "Wind speed: %.2f m/s",speedAverage);
			Blynk.virtualWrite(V2, message);
			sprintf(message, "Wind speed: %.2f knots",speedAverage * METERS_PER_SECOND_2_KNOTS);
			Blynk.virtualWrite(V3, message);
			sprintf(message, "Wind speed: %.2f km/h",speedAverage * METERS_PER_SECOND_2_KILOMETERS_PER_HOUR);
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
		vTaskDelay(15 / portTICK_PERIOD_MS);
	}

}


Wind calculateWind(double xPositiveTime,double xNegativeTime,double yPositiveTime,double yNegativeTime)
{
	Wind wind;

	//printf("fabs pos: %.2f   neg: %.2f\n",1000000*fabs(yPositiveTime-ZERO_WIND_TIME),1000000*fabs(yNegativeTime-ZERO_WIND_TIME));
	//printf("0.5/freq: %.2f \n",  1000000*(0.5/(double)TRANSDUCER_FREQUENCY_IN_HZ) );


	wind.xSpeed=X_DISTANCE*(1.0/xPositiveTime-1.0/xNegativeTime)/2.0;
	wind.ySpeed=Y_DISTANCE*(1.0/yPositiveTime-1.0/yNegativeTime)/2.0;


	wind.direction=atan(wind.ySpeed/wind.xSpeed)*RADIANS_2_DEGREES+90.0;
	if(wind.xSpeed<0)
	{
		//to get full 360º instead of only 180º
		wind.direction=wind.direction+180.0;
	}
	wind.speed=sqrt(wind.xSpeed*wind.xSpeed+wind.ySpeed*wind.ySpeed);

	if(wind.xSpeed > wind.ySpeed)
	{
		wind.soundSpeed=X_DISTANCE*(1.0/xPositiveTime+1.0/xNegativeTime)/2.0;

	}
	else
	{
		wind.soundSpeed=Y_DISTANCE*(1.0/yPositiveTime+1.0/yNegativeTime)/2.0;
	}
	wind.temperature=(wind.soundSpeed-ZERO_CELSIUS_SPEED_OF_SOUND)/SPEED_OF_SOUND_TEMPERATURE_COEFFICIENT;
	#ifdef DEBUG_PRINTS
	printf("Sound speed: %.2f\n",wind.soundSpeed);
	printf("Temperature: %.2f\n",wind.temperature);
	#endif
	return wind;
}


double AngleSpeedCorrection(double speed,double direction)
{
	double correction=speed;

	correction=(correction/((sin((4.0*direction-90.0)/RADIANS_2_DEGREES)/9.7) + 1.0))*1.167;


	return correction;
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
	#ifdef DEBUG_PRINTS
		printf("SPI initialized\n");
	#endif
	}

	ret=initRegistersTdc1000(&xHandleTDC1000,NORMAL_CONFIG);
	if(ret==ESP_OK)
	{
	#ifdef DEBUG_PRINTS
		printf("TDC1000 X initialized\n");
	#endif
	}

	ret=initRegistersTdc1000(&yHandleTDC1000,NORMAL_CONFIG);
	if(ret==ESP_OK)
	{
		#ifdef DEBUG_PRINTS
		printf("TDC1000 Y initialized\n");
		#endif
	}
	#ifdef DEBUG_PRINTS
	printf("\nTDC1000 X \n");
	#endif
	readRegistersTdc1000(&xHandleTDC1000);
	#ifdef DEBUG_PRINTS
	printf("\nTDC1000 Y \n");
	#endif
	readRegistersTdc1000(&yHandleTDC1000);

	initNmea();

}






	
