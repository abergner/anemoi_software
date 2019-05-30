/*
 * anemoi_TOF_measurement.h
 *
 *  Created on: Apr 11, 2019
 *      Author: alex
 */

#ifndef MAIN_INCLUDE_TIMEMEASUREMENTANEMOI_H_
#define MAIN_INCLUDE_TIMEMEASUREMENTANEMOI_H_

ErrorsAndWarnings measureTimeOfFlight(Axis axis, Direction direction, double * ptrTimeOfFlight);
void initTimeMeasurementHardware(void);




#endif /* MAIN_INCLUDE_TIMEMEASUREMENTANEMOI_H_ */