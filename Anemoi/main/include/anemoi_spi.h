#ifndef _ANEMOI_SPI_H_
#define _ANEMOI_SPI_H_

#include "anemoi_tdc_registers.h"

typedef enum {CONFIG_0_ADDRESS=0, CONFIG_1_ADDRESS, CONFIG_2_ADDRESS ,CONFIG_3_ADDRESS ,
CONFIG_4_ADDRESS ,TOF_1_ADDRESS ,TOF_0_ADDRESS ,ERRORS_ADDRESS ,TIMEOUT_ADDRESS  ,CLOCK_RATE_ADDRESS  }TDC1000_Registers_t;

#define TDC1000_REGISTERS 10

//Máscaras
#define WRITE               0b01000000
#define AUTOINCREMENT       0b10000000
#define NEW_MEASUREMENT     0b00000001

//Configuracion TDC1000
//#define CONFIG_0    0x8B //Divide by 32, 12 Pulses
//#define CONFIG_1    0x40 //1 Measurment cycle; Do not count events
//#define CONFIG_2    0x28 //External Channel select enabled;Damping enabled; TOF_MEAS_MODE = 0
//#define CONFIG_3    0x03 //Threshold -125mV
//#define CONFIG_4    0x1F //No phase shift
//#define TOF_1       0xE7 //21 Db PGA Gain; Resistive feedback;TIMING_REG[9:8]
//#define TOF_0       0x00 //TIMING_REG[7:0]
//#define TIMEOUT     0x1F

//#define ERRORS      0x00 //Default
//#define CLOCK_RATE  0x00 //Default

#define TDC7200_CONFIG_REGISTERS 10

typedef enum {TIME1_ADDRESS=0x10,CLOCK_COUNT1_ADDRESS ,TIME2_ADDRESS ,CLOCK_COUNT2_ADDRESS ,TIME3_ADDRESS
        ,CLOCK_COUNT3_ADDRESS, TIME4_ADDRESS, CLOCK_COUNT4_ADDRESS, TIME5_ADDRESS, CLOCK_COUNT5_ADDRESS,
        TIME6_ADDRESS,CALIBRATION1_ADDRESS , CALIBRATION2_ADDRESS  }TDC7200_Meas_Registers_t;

//Configuraciçon TDC7200
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

typedef enum {NORMAL_CONFIG,RESET_CONFIG}config_t;



esp_err_t initSPI(spi_device_handle_t * tdc1000_x_handle_ptr,spi_device_handle_t * tdc1000_y_handle_ptr,spi_device_handle_t * tdc7200_handle_ptr);
esp_err_t initTDC1000SPI(spi_device_handle_t * handle_ptr,config_t config);
esp_err_t read_TDC1000_registers(spi_device_handle_t * handle_ptr);

esp_err_t init_TDC7200_SPI(spi_device_handle_t * handle_ptr);
esp_err_t read_TDC7200_config_registers(spi_device_handle_t * handle_ptr);
esp_err_t read_TDC7200_meas_registers(spi_device_handle_t * handle_ptr);
esp_err_t new_TDC7200_measurement(spi_device_handle_t * handle_ptr);

#endif
