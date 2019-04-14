/*
 * anemoi_TOF_measurement.c
 *
 *  Created on: Apr 11, 2019
 *      Author: alex
 */

//DRIVERS//
#include "freertos/FreeRTOS.h"
//DRIVERS//

#include <math.h>

#include "include/Anemoi.h"
#include "include/TimeMeasurementAnemoi.h"
#include "include/GpioAnemoi.h"

#define CYCLES_PER_SEC 240000000

ErrorsAndWarnings calculateTimes(double * ptrTimes, unsigned int * ptrTimesCount);
ErrorsAndWarnings checkMissingStopPulses(double * ptrTimes, unsigned int * ptrNewTimesCount, unsigned int timesCount);

ErrorsAndWarnings calculateTOF(double * ptrTimeofFlight)
{
	double TOF=0;
	double times[STOP_QUEUE_LENGTH];
	unsigned int timesCount=0;
	ErrorsAndWarnings errorAndWarnings=calculateTimes(times,&timesCount);
	if(errorAndWarnings != NO_ERRORS_NO_WARNINGS)
	{
		return errorAndWarnings;
	}

	TOF=times[0];
	*ptrTimeofFlight=TOF;


	if((TOF>0.0004)&&(TOF<0.001))
	{
		return true;
	}
	else
	{
		return ERROR_TIME_OUT_OF_RANGE;
	}
	return NO_ERRORS_NO_WARNINGS;
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


ErrorsAndWarnings checkMissingStopPulses(double * ptrTimes, unsigned int * ptrNewTimesCount, unsigned int timesCount)
{
	ErrorsAndWarnings errorsAndWarnings = NO_ERRORS_NO_WARNINGS;
	double newTimes[STOP_QUEUE_LENGTH];
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
	(*ptrNewTimesCount)=newTimesCount;
	return errorsAndWarnings;
}
