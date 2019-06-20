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
#include <stdlib.h>

#include "include/Anemoi.h"
#include "include/TimeMeasurementAnemoi.h"
#include "include/GpioAnemoi.h"

#define CYCLES_PER_SEC 240000000

#define UPPER_TIME_LIMIT 0.01
#define LOWER_TIME_LIMIT 0.00001

//CALIBRATION
#define GENERAL_CALIBRATION 	 ( 0.0000000)

#define POSITIVE_CALIBRATION	  0.00000100
#define NEGATIVE_CALIBRATION	  0.00000000

#define X_CALIBRATION	  		 (-0.00000300)
#define Y_CALIBRATION	 		 ( 0.00000300)

#define X_POSITIVE_CALIBRATION	(  0.00000000  + GENERAL_CALIBRATION + X_CALIBRATION + POSITIVE_CALIBRATION )
#define X_NEGATIVE_CALIBRATION	(  0.00000010  + GENERAL_CALIBRATION + X_CALIBRATION + NEGATIVE_CALIBRATION )
#define Y_POSITIVE_CALIBRATION	(  0.00000010  + GENERAL_CALIBRATION + Y_CALIBRATION + POSITIVE_CALIBRATION )
#define Y_NEGATIVE_CALIBRATION	(  0.00000000  + GENERAL_CALIBRATION + Y_CALIBRATION + NEGATIVE_CALIBRATION )

#define GENERAL_PHASE_SHIFT 7.0/TRANSDUCER_FREQUENCY_IN_HZ

#define POSITIVE_PHASE_SHIFT 0.0/TRANSDUCER_FREQUENCY_IN_HZ
#define NEGATIVE_PHASE_SHIFT 0.0/TRANSDUCER_FREQUENCY_IN_HZ

#define X_PHASE_SHIFT 1.0/TRANSDUCER_FREQUENCY_IN_HZ
#define Y_PHASE_SHIFT 0.0/TRANSDUCER_FREQUENCY_IN_HZ

#define X_POSITIVE_PHASE_SHIFT	( 1.0/TRANSDUCER_FREQUENCY_IN_HZ + GENERAL_PHASE_SHIFT + X_PHASE_SHIFT + POSITIVE_PHASE_SHIFT )
#define X_NEGATIVE_PHASE_SHIFT	( 1.0/TRANSDUCER_FREQUENCY_IN_HZ + GENERAL_PHASE_SHIFT + X_PHASE_SHIFT + NEGATIVE_PHASE_SHIFT )
#define Y_POSITIVE_PHASE_SHIFT	( 1.0/TRANSDUCER_FREQUENCY_IN_HZ + GENERAL_PHASE_SHIFT + Y_PHASE_SHIFT + POSITIVE_PHASE_SHIFT )
#define Y_NEGATIVE_PHASE_SHIFT	( 1.0/TRANSDUCER_FREQUENCY_IN_HZ + GENERAL_PHASE_SHIFT + Y_PHASE_SHIFT + NEGATIVE_PHASE_SHIFT )

#define X_POSITIVE_SIGNAL_WIDTH 	20	//measured in pulses
#define X_NEGATIVE_SIGNAL_WIDTH		20
#define Y_POSITIVE_SIGNAL_WIDTH		21
#define Y_NEGATIVE_SIGNAL_WIDTH		18

#define ZERO_WIND_TIME 0.000425
//CALIBRATION

int compareFunction(const void *a, const void *b);
ErrorsAndWarnings calculateTimes(double * ptrTimes, unsigned int * ptrTimesCount);
ErrorsAndWarnings checkMissingStopPulses(double * ptrTimes, unsigned int * ptrTimesCount);
double calculateTimeOfFlight(double * ptrTimes, unsigned int * ptrTimesCount,Axis axis, Direction direction);

void initTimeMeasurementHardware(void)
{
	initGpio();
}

#define N 20

ErrorsAndWarnings measureTimeOfFlight(Axis axis, Direction direction, double * ptrTimeOfFlight)
{
	ErrorsAndWarnings errorsAndWarnings=NO_ERRORS_NO_WARNINGS;
	double timeOfFlight=0;
	double times[N][STOP_QUEUE_LENGTH];
	unsigned int timesCount[N];
	unsigned int i=0,k=0;

	selectChannel(axis);
	enableVdd(axis,direction);
	enableTdc1000(direction);
	vTaskDelay(5 / portTICK_PERIOD_MS);//wait for everything to stabilize (just in case)
	enableStartStopInterrupt(direction);
	for(i=0;i<N;i++)
	{
		for(k=0;k<STOP_QUEUE_LENGTH;k++)
		{
			times[i][k]=0;
		}
	}
	for(i=0;i<N;i++)
	{
		sendTrigger();
		//wait for transmission and reception
		vTaskDelay(2 / portTICK_PERIOD_MS);
		errorsAndWarnings=calculateTimes(&times[i][0], &timesCount[i]);
		if(errorsAndWarnings != NO_ERRORS_NO_WARNINGS)//if some error ocurres this batch doesnt count
		{
			break;
			//errorsAndWarnings=checkMissingStopPulses(&times[i][0], &timesCount[i]);
		}
		//printf("i:%d\t time: %f ms\n",i,times[0]*1000);
		/*if(errorsAndWarnings != NO_ERRORS_NO_WARNINGS)//if some error ocurres this batch doesnt count
		{
			for(k=0;k<STOP_QUEUE_LENGTH;k++)
			{
				times[i][k]=0;
			}
		}*/

	}

	if(errorsAndWarnings == NO_ERRORS_NO_WARNINGS)
	{
		timeOfFlight=calculateTimeOfFlight(&times[0][0], &timesCount[0] , axis, direction);
		if( (timeOfFlight > UPPER_TIME_LIMIT)||(timeOfFlight < LOWER_TIME_LIMIT))
		{
			errorsAndWarnings=ERROR_TIME_OUT_OF_RANGE;
		}
		else
		{
			(*ptrTimeOfFlight)=timeOfFlight;
		}
	}

	disableStartStopInterrupt();
	disableTdc1000();
	disableVdd();

	return errorsAndWarnings;
}

#define RELATIVE_TIMES_COUNT ( STOP_QUEUE_LENGTH + 10 )

typedef struct
{
	double time;
	double timeAccumulated;
	int count;
	int index;

}RelativeTime;

double calculateTimeOfFlight(double * ptrTimes, unsigned int * ptrTimesCount, Axis axis, Direction direction)
{
	double timeOfFlight=0;
	RelativeTime relativeTimes[RELATIVE_TIMES_COUNT];

	int k=0,i=0,j=0;
	for(i=0;i<RELATIVE_TIMES_COUNT;i++)
	{
			relativeTimes[i].count=0;
			relativeTimes[i].index=i;
			relativeTimes[i].time=0;
			relativeTimes[i].timeAccumulated=0;

	}
	for(j=0;j<N;j++)
	{
		if(ptrTimes[0]!=0)
		{
			for(i=0;i<RELATIVE_TIMES_COUNT;i++)
			{
					relativeTimes[i].time=ptrTimes[0]+ ((double)(i-10))/TRANSDUCER_FREQUENCY_IN_HZ;
			}
			break;
		}
	}
	if(relativeTimes[0].time==0)
	{
		return 0;
	}
	for(j=0;j<N;j++)
	{
		for(i=0;i<ptrTimesCount[j];i++)
		{
			for(k=0;k<RELATIVE_TIMES_COUNT;k++)
			{
				if(ptrTimes[j*STOP_QUEUE_LENGTH+i]!=0)
				{
					if( ((relativeTimes[k].time - 0.5/TRANSDUCER_FREQUENCY_IN_HZ) < ptrTimes[j*STOP_QUEUE_LENGTH+i] )&& (ptrTimes[j*STOP_QUEUE_LENGTH+i] < (relativeTimes[k].time + 0.5/TRANSDUCER_FREQUENCY_IN_HZ)) )
					{
						relativeTimes[k].timeAccumulated=relativeTimes[k].timeAccumulated+ptrTimes[j*STOP_QUEUE_LENGTH+i];
						relativeTimes[k].count++;
						break;
					}
				}
			}
		}
	}
	int index=0;
	for (i = 0; i < RELATIVE_TIMES_COUNT; i++)
	{
		if(relativeTimes[i].count>=(int)N*0.9)
		{
			index=i;
			break;
		}
	}

	int count=0;
	for(k=0;k<RELATIVE_TIMES_COUNT;k++)
	{
		if(relativeTimes[k].count>0)
		{
			count++;
			#ifdef DEBUG_PRINTS
			printf("index: %d count: %d \t",relativeTimes[k].index,relativeTimes[k].count);
			printf("time: %.2f us\n",relativeTimes[k].timeAccumulated*1000000/(double)relativeTimes[k].count);
			#endif
		}
	}
	#ifdef DEBUG_PRINTS
	printf("count: %d\n",count);
	#endif

	for(i=index,k=0; i < RELATIVE_TIMES_COUNT; i++)
	{
		if(relativeTimes[i].count>0)
		{
			#ifdef DEBUG_PRINTS
			printf("time %d: %.2f us\n",i,1000000*((relativeTimes[i].timeAccumulated / (double) relativeTimes[i].count) - ((double)(i-index))/TRANSDUCER_FREQUENCY_IN_HZ));
			#endif
		}
		if(relativeTimes[i].count == N)
		{
			timeOfFlight=timeOfFlight + (relativeTimes[i].timeAccumulated / (double) relativeTimes[i].count) - ((double)(i-index))/TRANSDUCER_FREQUENCY_IN_HZ;
			k++;
			if(k==5)
			{
				//break;
			}
		}
	}

	timeOfFlight=(relativeTimes[index+4].timeAccumulated / (double) relativeTimes[index+4].count) - ((double)(4.0))/TRANSDUCER_FREQUENCY_IN_HZ;
	timeOfFlight=timeOfFlight+(relativeTimes[index+5].timeAccumulated / (double) relativeTimes[index+5].count) - ((double)(5.0))/TRANSDUCER_FREQUENCY_IN_HZ;
	timeOfFlight=timeOfFlight+(relativeTimes[index+6].timeAccumulated / (double) relativeTimes[index+6].count) - ((double)(6.0))/TRANSDUCER_FREQUENCY_IN_HZ;
	timeOfFlight=timeOfFlight/(double) 3.0;

	//CALIBRATION
	if(axis==X_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			timeOfFlight=timeOfFlight-X_POSITIVE_PHASE_SHIFT-X_POSITIVE_CALIBRATION;
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			timeOfFlight=timeOfFlight-X_NEGATIVE_PHASE_SHIFT-X_NEGATIVE_CALIBRATION;
		}
	}
	else if(axis==Y_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			timeOfFlight=timeOfFlight-Y_POSITIVE_PHASE_SHIFT-Y_POSITIVE_CALIBRATION;
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			timeOfFlight=timeOfFlight-Y_NEGATIVE_PHASE_SHIFT-Y_NEGATIVE_CALIBRATION;
		}
	}
	//CALIBRATION
	return timeOfFlight;
}

int compareFunction(const void *a, const void *b)
{
    RelativeTime *aPrima = (RelativeTime *) a;
    RelativeTime *bPrima = (RelativeTime *) b;
    if ((*aPrima).count > (*bPrima).count)
        return -1;
    else if ((*aPrima).count < (*bPrima).count)
        return 1;
    else
        return 0;
}

ErrorsAndWarnings calculateTimes(double * ptrTimes, unsigned int * ptrTimesCount)
{
	uint32_t start=0;
	uint32_t stops[STOP_QUEUE_LENGTH];
	unsigned int i=0;
	unsigned int arrivedStopPulses=0;

	if( getQueueCycles(START_QUEUE, &start) != true)
	{
		#ifdef DEBUG_PRINTS
		printf(ERROR"No start received\n"RESET);
		#endif
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
		#ifdef DEBUG_PRINTS
		printf(ERROR"No stop received\n"RESET);
		#endif
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
	//double newTimes[STOP_QUEUE_LENGTH];
	unsigned int timesCount=(*ptrTimesCount);
	//unsigned int i,k;
	double firstTime=ptrTimes[0];
	double lastTime=ptrTimes[timesCount-1];
    double pulses=round((lastTime-firstTime)*TRANSDUCER_FREQUENCY_IN_HZ);
	unsigned int newTimesCount=(unsigned int) pulses+1;

	if(newTimesCount > timesCount)
	{
		/*for(i=0;i<newTimesCount;i++)
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
		}*/
		//printf(WARNING"Missing pulse stops: %d\n"RESET,newTimesCount-timesCount);
		errorsAndWarnings = WARNING_MISSING_STOP_PULSE;
	}
	(*ptrTimesCount)=newTimesCount;
	return errorsAndWarnings;
}



void SoftCalibration(double *ptrXPositiveTime,double * ptrXNegativeTime,double * ptrYPositiveTime,double * ptrYNegativeTime)
{
	while((*ptrYPositiveTime)-ZERO_WIND_TIME >(0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ)&&(ZERO_WIND_TIME - (*ptrYNegativeTime)>(0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ)))
	{
		(*ptrYPositiveTime)=(*ptrYPositiveTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		(*ptrYNegativeTime)=(*ptrYNegativeTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		#ifdef DEBUG_PRINTS
		printf("Correction 11\n");
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",(*ptrXPositiveTime)*1000000,(*ptrXNegativeTime)*1000000,(*ptrYPositiveTime)*1000000,(*ptrYNegativeTime)*1000000);
		#endif
	}
	while(ZERO_WIND_TIME-(*ptrYPositiveTime) >(0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ)&&((*ptrYNegativeTime)-ZERO_WIND_TIME >(0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ)))
	{
		(*ptrYPositiveTime)=(*ptrYPositiveTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		(*ptrYNegativeTime)=(*ptrYNegativeTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		#ifdef DEBUG_PRINTS
		printf("Correction 22\n");
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",(*ptrXPositiveTime)*1000000,(*ptrXNegativeTime)*1000000,(*ptrYPositiveTime)*1000000,(*ptrYNegativeTime)*1000000);
		#endif
	}
	while((*ptrXPositiveTime)-ZERO_WIND_TIME >(0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ)&&(ZERO_WIND_TIME-(*ptrXNegativeTime) >(0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ)))
	{
		(*ptrXPositiveTime)=(*ptrXPositiveTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		(*ptrXNegativeTime)=(*ptrXNegativeTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		#ifdef DEBUG_PRINTS
		printf("Correction 33\n");
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",(*ptrXPositiveTime)*1000000,(*ptrXNegativeTime)*1000000,(*ptrYPositiveTime)*1000000,(*ptrYNegativeTime)*1000000);
		#endif
	}
	while(ZERO_WIND_TIME-(*ptrXPositiveTime) >(0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ)&&((*ptrXNegativeTime)-ZERO_WIND_TIME >(0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ)))
	{
		(*ptrXPositiveTime)=(*ptrXPositiveTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		(*ptrXNegativeTime)=(*ptrXNegativeTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		#ifdef DEBUG_PRINTS
		printf("Correction 44\n");
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",(*ptrXPositiveTime)*1000000,(*ptrXNegativeTime)*1000000,(*ptrYPositiveTime)*1000000,(*ptrYNegativeTime)*1000000);
		#endif
	}

	bool conditionOne=fabs(fabs((*ptrYPositiveTime)-ZERO_WIND_TIME)+fabs((*ptrYNegativeTime)-ZERO_WIND_TIME)) > (0.85/(double)TRANSDUCER_FREQUENCY_IN_HZ);
	bool conditionTwo=fabs(((*ptrYPositiveTime)+(*ptrYNegativeTime))/2.0-ZERO_WIND_TIME) > (0.4/(double)TRANSDUCER_FREQUENCY_IN_HZ);
	while(conditionOne&&conditionTwo)
	{
		if(fabs((*ptrYPositiveTime)-ZERO_WIND_TIME)>fabs((*ptrYNegativeTime)-ZERO_WIND_TIME))
		{
			if((*ptrYPositiveTime)-ZERO_WIND_TIME>0)
			{
				(*ptrYPositiveTime)=(*ptrYPositiveTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			}
			else
			{
				(*ptrYPositiveTime)=(*ptrYPositiveTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			}

		}
		else
		{
			if((*ptrYNegativeTime)-ZERO_WIND_TIME>0)
			{
				(*ptrYNegativeTime)=(*ptrYNegativeTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			}
			else
			{
				(*ptrYNegativeTime)=(*ptrYNegativeTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			}
		}
		conditionOne=fabs(fabs((*ptrYPositiveTime)-ZERO_WIND_TIME)+fabs((*ptrYNegativeTime)-ZERO_WIND_TIME)) > (0.85/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		conditionTwo=fabs(((*ptrYPositiveTime)+(*ptrYNegativeTime))/2.0-ZERO_WIND_TIME) > (0.4/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		#ifdef DEBUG_PRINTS
		printf("Correction 01\n");
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",(*ptrXPositiveTime)*1000000,(*ptrXNegativeTime)*1000000,(*ptrYPositiveTime)*1000000,(*ptrYNegativeTime)*1000000);
		#endif
	}

	conditionOne=fabs(fabs((*ptrXPositiveTime)-ZERO_WIND_TIME)+fabs((*ptrXNegativeTime)-ZERO_WIND_TIME)) > (0.85/(double)TRANSDUCER_FREQUENCY_IN_HZ);
	conditionTwo=fabs(((*ptrXPositiveTime)+(*ptrXNegativeTime))/2.0-ZERO_WIND_TIME) > (0.4/(double)TRANSDUCER_FREQUENCY_IN_HZ);
	while(conditionOne&&conditionTwo)
	{

		if(fabs((*ptrXPositiveTime)-ZERO_WIND_TIME)>fabs((*ptrXNegativeTime)-ZERO_WIND_TIME))
		{
			if((*ptrXPositiveTime)-ZERO_WIND_TIME>0)
			{
				(*ptrXPositiveTime)=(*ptrXPositiveTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			}
			else
			{
				(*ptrXPositiveTime)=(*ptrXPositiveTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			}

		}
		else
		{
			if((*ptrXNegativeTime)-ZERO_WIND_TIME>0)
			{
				(*ptrXNegativeTime)=(*ptrXNegativeTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			}
			else
			{
				(*ptrXNegativeTime)=(*ptrXNegativeTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			}
		}
		conditionOne=fabs(fabs((*ptrXPositiveTime)-ZERO_WIND_TIME)+fabs((*ptrXNegativeTime)-ZERO_WIND_TIME)) > (0.85/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		conditionTwo=fabs(((*ptrXPositiveTime)+(*ptrXNegativeTime))/2.0-ZERO_WIND_TIME) > (0.4/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		#ifdef DEBUG_PRINTS
		printf("Correction 02\n");
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",(*ptrXPositiveTime)*1000000,(*ptrXNegativeTime)*1000000,(*ptrYPositiveTime)*1000000,(*ptrYNegativeTime)*1000000);
		#endif
	}
	/*while(fabs(((*ptrYPositiveTime)+(*ptrYNegativeTime))/2.0-ZERO_WIND_TIME) > (0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ))
	{
		if(((*ptrYPositiveTime)+(*ptrYNegativeTime))/2.0-ZERO_WIND_TIME > 0)
		{
			(*ptrYPositiveTime)=(*ptrYPositiveTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			(*ptrYNegativeTime)=(*ptrYNegativeTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		}
		else
		{
			(*ptrYPositiveTime)=(*ptrYPositiveTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			(*ptrYNegativeTime)=(*ptrYNegativeTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		}
		#ifdef DEBUG_PRINTS
		printf("Correction s\n");
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",(*ptrXPositiveTime)*1000000,(*ptrXNegativeTime)*1000000,(*ptrYPositiveTime)*1000000,(*ptrYNegativeTime)*1000000);
		#endif
	}

	while(fabs(((*ptrXPositiveTime)+(*ptrXNegativeTime))/2.0-ZERO_WIND_TIME) > (0.8/(double)TRANSDUCER_FREQUENCY_IN_HZ))
	{
		if(((*ptrXPositiveTime)+(*ptrXNegativeTime))/2.0-ZERO_WIND_TIME > 0)
		{
			(*ptrXPositiveTime)=(*ptrXPositiveTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			(*ptrXNegativeTime)=(*ptrXNegativeTime)-(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		}
		else
		{
			(*ptrXPositiveTime)=(*ptrXPositiveTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
			(*ptrXNegativeTime)=(*ptrXNegativeTime)+(1.0/(double)TRANSDUCER_FREQUENCY_IN_HZ);
		}
		#ifdef DEBUG_PRINTS
		printf("Correction s\n");
		printf("X pos time: %.2f us\t X neg time: %.2f us\tY pos time: %.2f us\tY neg time: %.2f us\n",(*ptrXPositiveTime)*1000000,(*ptrXNegativeTime)*1000000,(*ptrYPositiveTime)*1000000,(*ptrYNegativeTime)*1000000);
		#endif
	}*/

}





/*int maxIndex = 0;
	int max = relativeTimesCount[maxIndex];

	for (i = 0; i < RELATIVE_TIMES_COUNT; i++)
	{
	    if (relativeTimesCount[i] > max)
	    {
	        maxIndex = i;
	        max = relativeTimesCount[i];
	        break;
	    }
	}
	for (i = 0; i < RELATIVE_TIMES_COUNT; i++)
	{
		if(relativeTimesCount[i]<(int)maxIndex*0.3)
		{
			relativeTimesCount[i]=0;
			relativeTimesAccumulated[i]=0;
		}
	}
	*/

/*double indexMassCenter=0;
	double massCenter=0;
	int totalCount=0;
	for (i = 0; i < RELATIVE_TIMES_COUNT; i++)
	{
		indexMassCenter=indexMassCenter+(double)(relativeTimesCount[i]*i);
		totalCount=totalCount+relativeTimesCount[i];
		massCenter=massCenter+relativeTimesAccumulated[i]*relativeTimesCount[i];
	}
	indexMassCenter=indexMassCenter/(double)totalCount;
	massCenter=massCenter/(double)totalCount;*/


	//printf("Index mass center: %f\n",indexMassCenter);
	//printf("Index mass center: %d\n",(int)round(indexMassCenter));
	//printf("Index mass center: %.2f\n",relativeTimesAccumulated[(int)round(indexMassCenter)]*1000000/(double)relativeTimesCount[(int)round(indexMassCenter)]);

	//printf("Mass center: %f\n",massCenter*1000000);*/




//qsort(relativeTimes,RELATIVE_TIMES_COUNT, sizeof(RelativeTime), compareFunction);

	/*for(k=0;k<RELATIVE_TIMES_COUNT;k++)
	{
		if(relativeTimes[k].count>0)
		{
			printf("index: %d count: %d \t",relativeTimes[k].index,relativeTimes[k].count);
			printf("time: %.2f us\n",relativeTimes[k].timeAccumulated*1000000/(double)relativeTimes[k].count);
		}
	}*/
	//CALIBRATION
	/*int signalWidth=0;
	if(axis==X_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			signalWidth=X_POSITIVE_SIGNAL_WIDTH;
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			signalWidth=X_NEGATIVE_SIGNAL_WIDTH;
		}
	}
	else if(axis==Y_AXIS)
	{
		if(direction==POSITIVE_DIRECTION)
		{
			signalWidth=Y_POSITIVE_SIGNAL_WIDTH;
		}
		else if(direction==NEGATIVE_DIRECTION)
		{
			signalWidth=Y_NEGATIVE_SIGNAL_WIDTH;
		}
	}
	//CALIBRATION
	int index=0;
	double minTime=relativeTimes[0].time;
	for(k=1;k<signalWidth;k++)
	{
		if(relativeTimes[k].time < minTime)
		{
			index=k;
			minTime=relativeTimes[k].time;
		}

	}*/
	//printf("time: %.2f us\n",relativeTimes[index].timeAccumulated*1000000/(double)relativeTimes[index].count);
