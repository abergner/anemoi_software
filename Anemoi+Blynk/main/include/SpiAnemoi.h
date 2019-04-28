#ifndef _ANEMOI_SPI_H_
#define _ANEMOI_SPI_H_


typedef enum {NORMAL_CONFIG,RESET_CONFIG}Tdc1000Config;

esp_err_t initSpi(spi_device_handle_t * tdc1000_x_handle_ptr,spi_device_handle_t * tdc1000_y_handle_ptr,spi_device_handle_t * tdc7200_handle_ptr);
esp_err_t initRegistersTdc1000(spi_device_handle_t * handle_ptr,Tdc1000Config config);
esp_err_t readRegistersTdc1000(spi_device_handle_t * handle_ptr);


#endif
