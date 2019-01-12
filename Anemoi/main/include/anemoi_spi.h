#ifndef _ANEMOI_SPI_H_
#define _ANEMOI_SPI_H_


typedef enum {ADDRESS_CONFIG_0=0, ADDRESS_CONFIG_1, ADDRESS_CONFIG_2 ,ADDRESS_CONFIG_3 ,
ADDRESS_CONFIG_4 ,ADDRESS_TOF_1 ,ADDRESS_TOF_0 ,ADDRESS_ERRORS ,ADDRESS_TIMEOUT  ,ADDRESS_CLOCK_RATE  }TDC1000_Registers_t;

#define TDC1000_REGISTERS 10

#define WRITE       0b01000000
#define CONFIG_0    0x8C //Divide by 8, 16 pulses
#define CONFIG_1    0x40
#define CONFIG_2    0x25
#define CONFIG_3    0x03
#define CONFIG_4    0x1F
#define TOF_1       0xE7
#define TOF_0       0x00
#define ERRORS      0x00
#define TIMEOUT     0x43
#define CLOCK_RATE  0x00



esp_err_t init_SPI(spi_device_handle_t * tdc1000_x_handle_ptr,spi_device_handle_t * tdc1000_y_handle_ptr,spi_device_handle_t * tdc7200_handle_ptr);
esp_err_t init_TDC1000(spi_device_handle_t * handle_ptr);
esp_err_t read_TDC1000_registers(spi_device_handle_t * handle_ptr);


#endif