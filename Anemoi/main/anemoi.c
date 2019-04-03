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

typedef struct
{
	double Speed;
	double Direction;
}Wind_t;

void init_anemoi(void);
Wind_t calculateWind(double TimeXPositive,double TimeXNegative, double TimeYPositive,double TimeYNegative);
bool measure_time(char axis,char direction, double * TimeofFlight);


void anemoi(void)
{
	init_anemoi();

	Wind_t Wind={0};

	double TimeofFlightXPositive=0;
	double TimeofFlightXNegative=0;
	double TimeofFlightYPositive=0;
	double TimeofFlightYNegative=0;

	bool XPositiveMeasurementOK=false;
	bool XNegativeMeasurementOK=false;
	bool YPositiveMeasurementOK=false;
	bool YNegativeMeasurementOK=false;

	while(true)
	{
		XPositiveMeasurementOK=measure_time(X_AXIS,POSITIVE_DIRECTION,&TimeofFlightXPositive);
		if(XPositiveMeasurementOK==false)
		{
			printf("X positive direction measurement error\n");
		}
		YPositiveMeasurementOK=measure_time(Y_AXIS,POSITIVE_DIRECTION,&TimeofFlightYPositive);
		if(YPositiveMeasurementOK==false)
		{
			printf("Y positive direction measurement error\n");
		}/*
		XNegativeMeasurementOK=measure_time(X_AXIS,NEGATIVE_DIRECTION,&TimeofFlightXNegative);
		if(XNegativeMeasurementOK==false)
		{
			printf("X negative direction measurement error\n");
		}
		YNegativeMeasurementOK=measure_time(Y_AXIS,NEGATIVE_DIRECTION,&TimeofFlightYNegative);
		if(YNegativeMeasurementOK==false)
		{
			printf("Y negative direction measurement error\n");
		}*/


		if(XPositiveMeasurementOK&&XNegativeMeasurementOK&&YPositiveMeasurementOK&&YNegativeMeasurementOK)
		{
			Wind=calculateWind(TimeofFlightXPositive,TimeofFlightXNegative,TimeofFlightYPositive,TimeofFlightYNegative);
		}

		vTaskDelay(500 / portTICK_PERIOD_MS);

	}




}

Wind_t calculateWind(double TimeXPositive,double TimeXNegative,double TimeYPositive,double TimeYNegative)
{

	Wind_t Wind={0};

	double XDistance=0.2;	//mts
	double YDistance=0.2;	//mts

	double XSpeed=0;
	double YSpeed=0;

	double Radians2Degrees = 180.0 / M_PI;

	XSpeed=XDistance*(1/TimeXPositive-1/TimeXNegative)/2;
	printf("X Speed:\t %f m/s\n",XSpeed );
	YSpeed=YDistance*(1/TimeYPositive-1/TimeYNegative)/2;
	printf("Y Speed:\t %f m/s\n",YSpeed );

	Wind.Speed=sqrt(XSpeed*XSpeed+YSpeed*YSpeed);

	Wind.Direction=atan(XSpeed/YSpeed)*Radians2Degrees;

	printf("Wind Speed:\t %f m/s \n",Wind.Speed);
	printf("Wind Direction:\t %f º\n",Wind.Direction);

	return Wind;


}



bool measure_time(char axis,char direction, double * TimeofFlight)
{
	bool MeasurementOK=false;
	if(axis==X_AXIS)
	{

		if(direction==POSITIVE_DIRECTION)
		{

			enable_tdc1000_x();
			select_channel_1();
			enable_Y1_vdd();//hardware correction, pulse going from Y1 to X1
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			enable_tdc1000_y();
			select_channel_1();
			enable_X1_vdd();//hardware correction, pulse going from X1 to Y1
		}
	}
	else if(axis==Y_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{

			enable_tdc1000_x();
			select_channel_2();
			enable_Y2_vdd();//hardware correction, pulse going from Y2 to X2
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			enable_tdc1000_y();
			select_channel_2();
			enable_X2_vdd();//hardware correction, pulse going from X2 to Y2
		}
	}

	send_trigger();
	enable_start_stop_interrupt();
	vTaskDelay(500 / portTICK_PERIOD_MS);
	MeasurementOK=calculate_TOF(TimeofFlight);
	disable_start_stop_interrupt();
	//disable_tdc1000();
	return MeasurementOK;


}

void init_anemoi(void)
{
	//Init clock of ESP32. Frequency set to 1.28MHz
	init_clock();
    //Init pins connected from ESP32 to TDC1000, TDC7200
    init_anemoi_gpio();

    //Create handles for SPI communication and add devices to SPI bus
    esp_err_t ret;
    spi_device_handle_t tdc1000_x_handle;
    spi_device_handle_t tdc1000_y_handle;
    spi_device_handle_t tdc7200_handle;

    ret=init_SPI(&tdc1000_x_handle,&tdc1000_y_handle,&tdc7200_handle);

	if(ret==ESP_OK)
	{
		printf("SPI initialized\n");
	}

	ret=init_TDC1000_SPI(&tdc1000_x_handle,NORMAL_CONFIG);
	if(ret==ESP_OK)
	{
		printf("TDC1000 X initialized\n");
	}

	ret=init_TDC1000_SPI(&tdc1000_y_handle,NORMAL_CONFIG);
	if(ret==ESP_OK)
	{
		printf("TDC1000 Y initialized\n");
	}

	/*ret=init_TDC7200_SPI(&tdc7200_handle);
	if(ret==ESP_OK)
	{
		printf("TDC7200 initialized\n");
	}*/
	printf("\nTDC1000 X \n");
	read_TDC1000_registers(&tdc1000_x_handle);
	printf("\nTDC1000 Y \n");
	read_TDC1000_registers(&tdc1000_y_handle);


}


