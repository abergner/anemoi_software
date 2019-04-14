/*
 * anemoi_TOF_measurement.c
 *
 *  Created on: Apr 11, 2019
 *      Author: alex
 */

//DRIVERS//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//DRIVERS//

#include <math.h>

#include "include/Anemoi.h"
#include "include/TimeMeasurementAnemoi.h"
#include "include/GpioAnemoi.h"

#define CYCLES_PER_SEC 240000000

#define UPPER_TIME_LIMIT 0.005
#define LOWER_TIME_LIMIT 0.0001

#define X_POSITIVE_CALIBRATION	0.0001
#define X_NEGATIVE_CALIBRATION	0.0001
#define Y_POSITIVE_CALIBRATION	0.0001
#define Y_NEGATIVE_CALIBRATION	0.0001

ErrorsAndWarnings calculateTimes(double * ptrTimes, unsigned int * ptrTimesCount);
ErrorsAndWarnings checkMissingStopPulses(double * ptrTimes, unsigned int * ptrTimesCount);
double calculateTimeOfFlight(double * ptrTimes, unsigned int timesCount,Axis axis, Direction direction);

void initTimeMeasurementHardware(void)
{
	initGpio();
}

ErrorsAndWarnings measureTimeOfFlight(Axis axis, Direction direction, double * ptrTimeOfFlight)
{
	ErrorsAndWarnings errorsAndWarnings=NO_ERRORS_NO_WARNINGS;
	double timeOfFlight=0;
	double times[STOP_QUEUE_LENGTH];
	unsigned int timesCount=0;
	selectChannel(axis);
	enableVdd(axis,direction);
	enableTdc1000(direction);
	//wait for everything to stabilize (just in case)
	vTaskDelay(20 / portTICK_PERIOD_MS);
	enableStartStopInterrupt(direction);

	sendTrigger();
	//wait for transmission and reception
	vTaskDelay(100 / portTICK_PERIOD_MS);

	errorsAndWarnings=calculateTimes(times,&timesCount);
	if(errorsAndWarnings == NO_ERRORS_NO_WARNINGS)
	{
		errorsAndWarnings=checkMissingStopPulses(times,&timesCount);
		timeOfFlight=calculateTimeOfFlight(times, timesCount, axis, direction);
		if( (timeOfFlight > UPPER_TIME_LIMIT)||(timeOfFlight < LOWER_TIME_LIMIT))
		{
			errorsAndWarnings=ERROR_TIME_OUT_OF_RANGE;
		}
		else
		{
			(* ptrTimeOfFlight)=timeOfFlight;
		}
	}
	disableStartStopInterrupt();
	disableTdc1000();
	disableVdd();

	return errorsAndWarnings;
}

double calculateTimeOfFlight(double * ptrTimes, unsigned int timesCount,Axis axis, Direction direction)
{
	double timeOfFlight=0;

	timeOfFlight=ptrTimes[timesCount-1];
	if(axis==X_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			timeOfFlight=timeOfFlight-X_POSITIVE_CALIBRATION;
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			timeOfFlight=timeOfFlight-X_NEGATIVE_CALIBRATION;
		}
	}
	else if(axis==Y_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			timeOfFlight=timeOfFlight-Y_POSITIVE_CALIBRATION;
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			timeOfFlight=timeOfFlight-Y_NEGATIVE_CALIBRATION;
		}
	}

	return timeOfFlight;
}


ErrorsAndWarnings calculateTimes(double * ptrTimes, unsigned int * ptrTimesCount)
{
	uint32_t start=0;
	uint32_t stops[STOP_QUEUE_LENGTH];
	unsigned int i=0;
	unsigned int arrivedStopPulses=0;

	if( getQueueCycles(START_QUEUE, &start) != true)
	{
		return ERROR_NO_START_RECEIVED;
	}

	for(i=0;i<STOP_QUEUE_LENGTH;i++)
	{
		if( getQueueCycles(STOP_QUEUE, &(stops[i])) )
		{
			arrivedStopPulses++;
		}
	}
	(*ptrTimesCount)=arrivedStopPulses;
	if(arrivedStopPulses==0)
	{
		return ERROR_NO_STOP_RECEIVED;
	}
	for(i=0;i<arrivedStopPulses;i++)
	{
		if(stops[i]>start)
		{
			ptrTimes[i]=(double)(stops[i]-start)/CYCLES_PER_SEC;
		}
		else
		{
			ptrTimes[i]=(double)(start-stops[i])/CYCLES_PER_SEC;
		}
	}
	return NO_ERRORS_NO_WARNINGS;
}


ErrorsAndWarnings checkMissingStopPulses(double * ptrTimes, unsigned int * ptrTimesCount)
{
	ErrorsAndWarnings errorsAndWarnings = NO_ERRORS_NO_WARNINGS;
	double newTimes[STOP_QUEUE_LENGTH];
	unsigned int timesCount=(*ptrTimesCount);
	unsigned int i,k;
	double firstTime=ptrTimes[0];
	double lastTime=ptrTimes[timesCount-1];
    double pulses=round((lastTime-firstTime)*TRANSDUCER_FREQUENCY_IN_HZ);
	unsigned int newTimesCount=(unsigned int) pulses+1;

	if(newTimesCount > timesCount)
	{
		for(i=0;i<newTimesCount;i++)
		{
			newTimes[i]=ptrTimes[0]+(double)i/TRANSDUCER_FREQUENCY_IN_HZ;
		}

		for(i=1;i<newTimesCount;i++)//first pulse cannot be missing
		{
			for(k=1;k<timesCount;k++)
			{
				if( (ptrTimes[k] < (newTimes[i] + 0.3/TRANSDUCER_FREQUENCY_IN_HZ)) && (ptrTimes[k] > (newTimes[i] - 0.3/TRANSDUCER_FREQUENCY_IN_HZ)))
				{
					newTimes[i]=ptrTimes[k];
				}
			}
		}
		for(i=0;i<newTimesCount;i++)
		{
			ptrTimes[i]=newTimes[i];
		}
		errorsAndWarnings = WARNING_MISSING_STOP_PULSE;
	}
	(*ptrTimesCount)=newTimesCount;
	return errorsAndWarnings;
}
