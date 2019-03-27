#ifndef MAIN_INCLUDE_ANEMOI_APP_H_
#define MAIN_INCLUDE_ANEMOI_APP_H_

///////////////////
//Includes


#include "anemoi_tasks.h"
#include "anemoi_tdc1000.h"
//#include "include/anemoi_tdc7200.h"
#include "anemoi_spi.h"
#include "anemoi_gpio.h"
#include "anemoi_clock.h"

///////////////////
//Tipos de datos
typedef enum {TDC1000_X_DEVICE, TDC1000_Y_DEVICE, TDC7200_DEVICE} anemoi_device_t;



///////////////////
//Funciones
void app(void);


#endif /* MAIN_INCLUDE_ANEMOI_APP_H_ */
