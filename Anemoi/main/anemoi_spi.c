#include "include/anemoi_spi.h"


//unsigned int TDC1000_init_registers[10]={CONFIG_0,CONFIG_1,CONFIG_2,CONFIG_3,CONFIG_4,TOF_1,TOF_0,ERRORS ,TIMEOUT,CLOCK_RATE};
//unsigned int TDC7200_init_registers[10]={CONFIG_1_TDC7200,CONFIG_2_TDC7200,INT_STATUS,INT_MASK,COARSE_CNTR_OVF_H,COARSE_CNTR_OVF_L, CLOCK_CNTR_OVF_H,CLOCK_CNTR_OVF_L,CLOCK_CNTR_STOP_MASK_H,CLOCK_CNTR_STOP_MASK_L};


esp_err_t spi_init(void)
{
    esp_err_t ret=0;


    spi_device_handle_t spi_tdc1000_x_handle;
    spi_device_handle_t spi_tdc1000_y_handle;
    spi_device_handle_t spi_tdc7200_handle;

    ret=spi_config_bus();

    ret=spi_add_device(&spi_tdc1000_x_handle, GPIO_TDC1000_X_CSB);
    ret=spi_add_device(&spi_tdc1000_y_handle, GPIO_TDC1000_Y_CSB);
    ret=spi_add_device(&spi_tdc7200_handle, GPIO_TDC7200_CSB);

    return ret;
}


//DEVUELVE VOID, CAMBIARLO. TIENE QUE DEVOLVER spi_device_handle_t
void spi_get_handles(anemoi_device_t device)
{
	switch(device){
		case TDC1000_X_DEVICE:
			return;
		case TDC1000_Y_DEVICE:
			return;
		case TDC7200_DEVICE:
			return;
	}

}


esp_err_t spi_config_bus(void)
{
    esp_err_t ret=0;

    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_SPI_MOSI,
        .miso_io_num=GPIO_SPI_MISO,
        .sclk_io_num=GPIO_SPI_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };

   ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);

   return ret;
}

esp_err_t spi_add_device(spi_device_handle_t * spi_device_handle, unsigned char esp_csb_pin)
{
    esp_err_t ret=0;

    //Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg={
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .clock_speed_hz=1000000,
        .duty_cycle_pos=128,        //50% duty cycle
        .mode=0,
        .spics_io_num=esp_csb_pin,
        .cs_ena_posttrans=3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size=2
    };

    ret=spi_bus_add_device(HSPI_HOST, &devcfg, spi_device_handle);

    return ret;
}


esp_err_t spi_write_register(spi_device_handle_t * handle_ptr, unsigned char address, const unsigned char * data)
{
    esp_err_t ret=0;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    char sendbuf[2] = {0};

    sendbuf[0]=(WRITE|address);
    sendbuf[1]=*data;
    t.length=sizeof(sendbuf)*8;

    t.tx_buffer=sendbuf;

    ret=spi_device_polling_transmit(*handle_ptr, &t);

    return ret;
}

unsigned char* spi_read_register(spi_device_handle_t * handle_ptr, unsigned char address)
{

    esp_err_t ret=0;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    unsigned char sendbuf[2] = {0};
    unsigned char rcvbuf[2] = {0};

    sendbuf[0]=(READ|address);
    sendbuf[1]=0;
    t.length=sizeof(sendbuf)*8;

    t.tx_buffer=sendbuf;
    t.rx_buffer=rcvbuf;

    ret=spi_device_transmit(*handle_ptr, &t);

    //OJO, QUE DEVUELVE?
    return rcvbuf;
}

esp_err_t spi_write_all_registers(spi_device_handle_t * handle_ptr, unsigned char * buffer, unsigned char number_of_registers)
{
    esp_err_t ret=0;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    unsigned int i;
    unsigned char sendbuf[number_of_registers*2];

    for(i=0;i<number_of_registers;i++)
    {
        sendbuf[2*i]=WRITE|((char)i);//dirección registro
        sendbuf[2*i+1]=buffer[i];//configuración de registro
    }

    t.length=sizeof(sendbuf)*8;

    t.tx_buffer=sendbuf;

    ret=spi_device_transmit(*handle_ptr, &t);

    return ret;
}

