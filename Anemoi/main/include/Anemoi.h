/*
 * Anemoi.h
 *
 *  Created on: Apr 13, 2019
 *      Author: alex
 */

#ifndef MAIN_INCLUDE_ANEMOI_H_
#define MAIN_INCLUDE_ANEMOI_H_


///////DEFINES/////////DEFINES/////////DEFINES////////DEFINES/////////////

//PINOUT//
#define GPIO_CHSEL          23
#define GPIO_ESP_TRIGG      22
#define GPIO_CLK            21
#define GPIO_UART_RX        19
#define GPIO_UART_TX        18
#define GPIO_VDD_EN_X1      5
#define GPIO_VDD_EN_X2      17
#define GPIO_VDD_EN_Y1      16
#define GPIO_VDD_EN_Y2      4
#define GPIO_TDC7200_EN     15
#define GPIO_SPI_MOSI       13
#define GPIO_SPI_MISO       12
#define GPIO_SPI_CLK        14
#define GPIO_TDC1000_X_CSB  27
#define GPIO_TDC1000_Y_CSB  26
#define GPIO_START_X	    25 //antes era GPIO_TDC7200_CSB
#define GPIO_TDC1000_Y_EN   33
#define GPIO_TDC1000_X_EN   32
#define GPI_START_Y		    35
#define GPI_STOP_Y          34
#define GPI_STOP_X  		39 //antes era GPI_TDC7200_INTB
#define GPI_TDC1000_ERRB    36
//PINOUT//

//Printf colors
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
//Printf colors

//Printf colors meaning
#define INFO GREEN
#define WARNING YELLOW
#define ERROR RED
#define DEBUG BLUE
//Printf colors meaning

#define TRANSDUCER_FREQUENCY_IN_HZ 40000

//Distance between sensors in meters
#define X_DISTANCE 0.15
#define Y_DISTANCE 0.15
//Distance between sensors in meters

////CUSTOM VARIABLES//////CUSTOM VARIABLES//////CUSTOM VARIABLES///////


typedef enum {NO_ERRORS_NO_WARNINGS, ERROR_NO_STOP_RECEIVED, ERROR_NO_START_RECEIVED, WARNING_MISSING_STOP_PULSE,
	ERROR_TIME_OUT_OF_RANGE} ErrorsAndWarnings;

typedef	enum {X_AXIS,Y_AXIS} Axis;
typedef enum {POSITIVE_DIRECTION, NEGATIVE_DIRECTION} Direction;

typedef struct
{
	double speed;
	double direction;
} Wind;





#endif /* MAIN_INCLUDE_ANEMOI_H_ */
