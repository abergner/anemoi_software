#ifndef _ANEMOI_SPI_H_
#define _ANEMOI_SPI_H_

//#include "include/anemoi_pinout.h"
#include "esp_err.h"
#include "driver/spi_master.h"
#include <string.h>

#include "anemoi_app.h"
#include "anemoi_pinout.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
LO SIGUIENTE VA EN TDC7200.H
typedef enum {TIME1_ADDRESS=0x10,CLOCK_COUNT1_ADDRESS ,TIME2_ADDRESS ,CLOCK_COUNT2_ADDRESS ,TIME3_ADDRESS
        ,CLOCK_COUNT3_ADDRESS, TIME4_ADDRESS, CLOCK_COUNT4_ADDRESS, TIME5_ADDRESS, CLOCK_COUNT5_ADDRESS,
        TIME6_ADDRESS,CALIBRATION1_ADDRESS , CALIBRATION2_ADDRESS  }tdc7200_reg_addr_t;

//Configuracion TDC7200
#define CONFIG_1_TDC7200           0x02  //sin comenzar medición;MEASUREMENT_MODE=2
#define CONFIG_2_TDC7200           0x44 //CALIBRATION2_PERIODS=10;NUM_STOPS=5
#define INT_STATUS                 0x00
#define INT_MASK                   0x00
#define COARSE_CNTR_OVF_H          0xFF
#define COARSE_CNTR_OVF_L          0xFF
#define CLOCK_CNTR_OVF_H           0xFF
#define CLOCK_CNTR_OVF_L           0xFF
#define CLOCK_CNTR_STOP_MASK_H     0x00
#define CLOCK_CNTR_STOP_MASK_L     0x01
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Máscaras
#define WRITE               0b01000000
#define READ               0b00000000


//Funciones

esp_err_t spi_init(void);
void spi_get_handles(anemoi_device_t device);
esp_err_t spi_config_bus(void);
esp_err_t spi_add_device(spi_device_handle_t * spi_device_handle, unsigned char esp_csb_pin);

esp_err_t spi_write_register(spi_device_handle_t * handle_ptr, unsigned char address, const unsigned char * data);
unsigned char* spi_read_register(spi_device_handle_t * handle_ptr, unsigned char address);
esp_err_t spi_write_all_registers(spi_device_handle_t * handle_ptr, unsigned char * buffer, unsigned char number_of_registers);


#endif



