

#include "driver/spi_master.h"
#include <string.h>

#include "include/anemoi_spi.h"
#include "include/anemoi_pinout.h"
#include "esp_err.h"

unsigned int normalConfigTDC1000[10]={CONFIG_0,CONFIG_1,CONFIG_2,CONFIG_3,CONFIG_4,TOF_1,TOF_0,ERRORS ,TIMEOUT,CLOCK_RATE};
unsigned int resetConfigTDC1000[10]={0x45,0x40,0x0,0x3,0x1F,0x0,0x0,0x0,0x19,0x0};
unsigned int config_TDC7200[10]={CONFIG_1_TDC7200,CONFIG_2_TDC7200,INT_STATUS,INT_MASK,COARSE_CNTR_OVF_H,COARSE_CNTR_OVF_L,
CLOCK_CNTR_OVF_H,CLOCK_CNTR_OVF_L,CLOCK_CNTR_STOP_MASK_H,CLOCK_CNTR_STOP_MASK_L};


esp_err_t initSPI(spi_device_handle_t * tdc1000_x_handle_ptr,spi_device_handle_t * tdc1000_y_handle_ptr,spi_device_handle_t * tdc7200_handle_ptr)
{
    esp_err_t ret;
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_SPI_MOSI,
        .miso_io_num=GPIO_SPI_MISO,
        .sclk_io_num=GPIO_SPI_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };

    //Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg={
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .clock_speed_hz=1000000,
        .duty_cycle_pos=128,        //50% duty cycle
        .mode=0,
        .spics_io_num=GPIO_TDC1000_X_CSB,
        .cs_ena_posttrans=3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size=2
    };
    
    
    //Initialize the SPI bus and add the device we want to send stuff to.
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    if(ret!=ESP_OK)
    {
        printf("SPI bus could not be initialized\n");        
    }
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, tdc1000_x_handle_ptr);
    if(ret!=ESP_OK)
    {
        printf("SPI device could not be added to bus\n");    
    }
    
    devcfg.spics_io_num=GPIO_TDC1000_Y_CSB;
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, tdc1000_y_handle_ptr);
    if(ret!=ESP_OK)
    {
        printf("SPI device could not be added to bus\n");    
    }
    
    /*devcfg.spics_io_num=GPIO_TDC7200_CSB;
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, tdc7200_handle_ptr);
    if(ret!=ESP_OK)
    {
        printf("SPI device could not be added to bus\n");    
    }*/
    return ret;
}


esp_err_t initTDC1000SPI(spi_device_handle_t * handle_ptr, config_t config)
{
    esp_err_t ret;
    unsigned int i=0;
    char initializationBuffer[20] = {0};
    char recvbuf[20] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
        
    
    for(i=CONFIG_0_ADDRESS;i<=CLOCK_RATE_ADDRESS;i++)
    {
        initializationBuffer[2*i]=WRITE|((char)i);//dirección Register
        if(config==NORMAL_CONFIG)
        {
        	initializationBuffer[2*i+1]=normalConfigTDC1000[i];//configuración de Register
        }
        else if(config==RESET_CONFIG)
        {
        	initializationBuffer[2*i+1]=resetConfigTDC1000[i];//configuración de Register
        }
    }
    t.length=sizeof(initializationBuffer)*8;
    t.tx_buffer=initializationBuffer;
    t.rx_buffer=recvbuf;
    ret=spi_device_transmit(*handle_ptr, &t);
    if(ret!=ESP_OK)
    {
        printf("TDC1000 initialization was not successful \n");    
    }
    return ret;
}


esp_err_t readTDC1000Registers(spi_device_handle_t * handle_ptr)
{
    esp_err_t ret=0;
    unsigned int i=0;
    char initializationBuffer[20] = {0};
    char recvbuf[20] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
        
    
    for(i=0;i<TDC1000_REGISTERS;i++)
    {
        initializationBuffer[2*i]=(char)i;//dirección Register
        initializationBuffer[2*i+1]=0;
    }
    t.length=sizeof(initializationBuffer)*8;
    t.tx_buffer=initializationBuffer;
    t.rx_buffer=recvbuf;
    ret=spi_device_transmit(*handle_ptr, &t);
    if(ret!=ESP_OK)
    {
        printf("TDC1000 read was not successful \n");    
    }
    for(i=0;i<TDC1000_REGISTERS;i++)
    {
        printf("Register %d:%0x\n",i,recvbuf[2*i+1]);
    }
    return ret;
}


esp_err_t initTDC7200SPI(spi_device_handle_t * handle_ptr)
{
    esp_err_t ret;
    unsigned int i=0;
    char initializationBuffer[2] = {0};
    char recvbuf[2] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=sizeof(initializationBuffer)*8;
    t.tx_buffer=initializationBuffer;
    t.rx_buffer=recvbuf;
    
        
    //initializationBuffer[1]=WRITE|((char)i);//dirección Register
    //initializationBuffer[1]=config_TDC7200[1];//configuración de Register
    for(i=0;i<TDC7200_CONFIG_REGISTERS;i++)
    {
        initializationBuffer[0]=WRITE|((char)i);//dirección Register
        initializationBuffer[1]=config_TDC7200[i];//configuración de Register
        ret=spi_device_transmit(*handle_ptr, &t);
        if(ret!=ESP_OK)
        {
            printf("TDC7200 initialization was not successful \n");    
        } 
    }
   
    return ret;
}

esp_err_t readTDC7200ConfigRegisters(spi_device_handle_t * handle_ptr)
{
    esp_err_t ret=0;
    unsigned int i=0;
    char initializationBuffer[2] = {0};
    char recvbuf[2] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=sizeof(initializationBuffer)*8;
    t.tx_buffer=initializationBuffer;
    t.rx_buffer=recvbuf; 
    
    for(i=0;i<TDC7200_CONFIG_REGISTERS;i++)
    {
        initializationBuffer[0]=(char)i;//dirección Register
        initializationBuffer[1]=0;
        ret=spi_device_transmit(*handle_ptr, &t);
        if(ret!=ESP_OK)
        {
            printf("TDC7200 read was not successful \n");    
        }
        printf("Register %d:%0x\n",i,recvbuf[1]);
    }
   
    
    return ret;
}

esp_err_t readTDC7200MeasRegisters(spi_device_handle_t * handle_ptr)
{
    esp_err_t ret=0;
    unsigned int i=0;
    char initializationBuffer[2] = {0};
    char recvbuf[5] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=sizeof(initializationBuffer)*8;
    t.tx_buffer=initializationBuffer;
    t.rx_buffer=recvbuf; 
    
    for(i=TIME1_ADDRESS;i<=CALIBRATION2_ADDRESS ;i++)
    {
        initializationBuffer[0]=(char)i;//dirección Register
        initializationBuffer[1]=0;
        ret=spi_device_transmit(*handle_ptr, &t);
        if(ret!=ESP_OK)
        {
            printf("TDC7200 read was not successful \n");    
        }
        printf("Register %d:%0x\n",i,recvbuf[1]);
        printf("Register %d:%0x\n",i,recvbuf[2]);
        printf("Register %d:%0x\n",i,recvbuf[3]);
        printf("Register %d:%0x\n",i,recvbuf[4]);
        
    }
   
    
    return ret;
    
}

esp_err_t newTDC7200Measurement(spi_device_handle_t * handle_ptr)
{
    esp_err_t ret=0;
   
    char initializationBuffer[2] = {0};
    char recvbuf[2] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=sizeof(initializationBuffer)*8;
    t.tx_buffer=initializationBuffer;
    t.rx_buffer=recvbuf; 
    
    
    initializationBuffer[0]=WRITE|((char)0);//dirección Register
    initializationBuffer[1]=config_TDC7200[0]|NEW_MEASUREMENT;
    ret=spi_device_transmit(*handle_ptr, &t);
    if(ret!=ESP_OK)
    {
        printf("TDC7200 write was not successful \n");    
    }
    
    
    return ret;
}
