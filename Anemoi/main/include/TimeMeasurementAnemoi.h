/*
 * anemoi_TOF_measurement.h
 *
 *  Created on: Apr 11, 2019
 *      Author: alex
 */

#ifndef MAIN_INCLUDE_TIMEMEASUREMENTANEMOI_H_
#define MAIN_INCLUDE_TIMEMEASUREMENTANEMOI_H_

typedef enum {START_STOP_X, START_STOP_Y} StartStop;

void initTimeOfFlightMeasurementHardware(void);


void enableStartStopInterruptX(void);
void disableStartStopInterruptX(void);
void enableStartStopInterruptY(void);
void disableStartStopInterruptY(void);
bool calculateTOF(double * ptrTimeofFlight, StartStop startStop);




#endif /* MAIN_INCLUDE_TIMEMEASUREMENTANEMOI_H_ */
