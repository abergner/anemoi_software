/*
 * NmeaAnemoi.c
 *
 *  Created on: May 28, 2019
 *      Author: alex
 */


/*

https://www.tronico.fi/OH6NT/docs/NMEA0183.pdf

1. What is the NMEA 0183 Standard?

The National Marine Electronics Association (NMEA) is a non-profit association of manufacturers,distributors,
dealers, educational institutions, and others interested in peripheral marine electronicsoccupations.
The NMEA 0183 standard defines an electrical interface and data protocol forcommunications between marine instrumentation.
NMEA 0183 is a voluntary industry standard, first released in March of 1983. Ithas been updated fromtime to time;
the latest release, currently (August 2001) Version 3.0, July2001, is available from theNMEA office
(Warning: the price for non-members is 250 US$).P O Box 3435New Bern NC 28564-3435USA
www.nmea.orgNMEA has also established a working group to develop a new standard for data communications amongshipboard
electronic devices. The new standard, NMEA 2000, is a bi-directional, multi-transmitter,multi-receiver serial data network.
It is multi-master and self-configuring, and there is no central controller.
The NMEA began a beta testing period in January 2000 with eleven manufacturers. A release version ofNMEA 2000 is expected in 2001.

2. Electrical Interface

NMEA 0183 devices are designated as either talkers or listeners (with some devices being both),employing an asynchronous
 serial interface with the following parameters:

 Baud rate:4800
 Number of data bits:8 (bit 7 is 0)
 Stop bits:1 (or more)
 Parity:none
 Handshake:none

 NMEA 0183 allows a single talker and several listeners on one circuit.
 The recommended interconnect wiring is a shielded twisted pair, with the shield grounded only at the talker.
 The standard dos not specifythe use of a particular connector.
 Note: The new 0183-HS standard (HS= high speed) introduced inversion 3.0 uses a 3-wire interface and a baud rate of 38400.
 This type of interface is not discussed here.
 Its is recommended that the talker output comply with EIA RS-422, a differential system with two signallines, "A" and "B".
 Differential drive signals have no reference to ground and are more immune to noise.However,
 a single-ended line at TTL level is accepted as well.
 The voltages on the A line correspond to those on the TTL single wire, while the B voltages are inverted
 (when output A is at +5 V, outputB is at0V, and vice versa. This is the unipolar RS-422 operation. In bipolar mode ±5V are used).
 In either case, the recommended receive circuit uses an opto-isolator with suitable protection circuitry.
 The input should be isolated from the receiver's ground.
 In practice, the single wire, or the RS-422 "A"wire may be directly connected to a computer's RS-232 input.
 In fact even many of the latest products,like hand-held GPS receivers, do not have a RS-422 differential output,
 but just a single line with TTL or5V CMOS compatible signal level.

3. General Sentence Format

All data is transmitted in the form of sentences. Only printable ASCII characters are allowed, plus CR(carriage return) and
LF (line feed). Each sentence starts with a "$" sign and ends with <CR><LF>.
There are three basic kinds of sentences: talker sentences, proprietary sentences and query sentences.Talker Sentences.

The general format for a talker sentence is: $ttsss,d1,d2,....<CR><LF>

The first two letters following the „$” are the talker identifier.
The next three characters (sss) are the sentence identifier, followed by a number of data fields separated by commas,
followed by an optional checksum, and terminated by carriage return/line feed.
The data fields are uniquely defined for each sentence type.
An example talker sentence is:$HCHDM,238,M<CR><LF>where "HC" specifies the talker as being a magnetic compass,
the "HDM" specifies the magnetic heading message follows. The "238" is the heading value, and "M" designates the heading value
as magnetic.A sentence may contain up to 80 characters plus "$" and CR/LF. If data for a field is not available,
the field is omitted, but the delimiting commas are still sent, with no space between them.
The checksum field consists of a "*" and two hex digits representing the exclusive OR of all characters between,
but not including, the "$" and "*".


	MWV Wind Speed and Angle
	$--MWV,x.x,a,x.x,a,A*hh
	|||||||1|||2|3|||4|5|6||
    1) Wind Angle, 0 to 360 degrees
	2) Reference: R = Relative, T = True
	3) Wind Speed
	4) Wind Speed Units: K/M/N
	5) Status: A= data valid, V=data invalid
	6) Checksum

 */

//DRIVERS//
#include "driver/uart.h"
#include "driver/gpio.h"
#include "soc/gpio_reg.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
//DRIVERS//

#include "include/Anemoi.h"
#include "include/NmeaAnemoi.h"
#include <string.h>


#define UART_NUM                UART_NUM_2
#define UART_TX_BUF_SIZE      	(1024)
#define UART_RX_BUF_SIZE      	(1024)

uint8_t checksum(uint8_t * string, int length);

void sendNmeaWindData(double angle, double speed, char unit)
{

	char nmeaSentence[30]="$55MWV,";
	char aString[20]=" ";
	sprintf(aString,"%.1f",angle);
	strcat(nmeaSentence,aString);
	strcat(nmeaSentence,",R,");
	sprintf(aString,"%.1f",speed);
	strcat(nmeaSentence,aString);
	switch(unit)
	{
		case 'K':
			strcat(nmeaSentence,",K");
			break;
		case 'M':
			strcat(nmeaSentence,",M");
				break;
		case 'N':
			strcat(nmeaSentence,",N");
				break;
		default:
				break;

	}
	strcat(nmeaSentence,",A");
	sprintf(aString,"*%02x",checksum((uint8_t *)&(nmeaSentence[1]),strlen(nmeaSentence)-1));
	strcat(nmeaSentence,aString);

	//#ifdef DEBUG_PRINTS
	//printf("\nNMEA sentence: \n\n");
	printf(nmeaSentence);
	printf("\n");
	//#endif

	uart_write_bytes(UART_NUM, nmeaSentence, strlen(nmeaSentence));
}

void initNmea(void)
{
	uart_config_t uart_config = {
	            .baud_rate = 4800,
	            .data_bits = UART_DATA_8_BITS,
	            .parity = UART_PARITY_DISABLE,
	            .stop_bits = UART_STOP_BITS_1,
	            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	    };
	uart_param_config(UART_NUM, &uart_config);
	uart_set_pin(UART_NUM, GPIO_UART_TX, GPIO_UART_RX,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	uart_driver_install(UART_NUM, UART_RX_BUF_SIZE , UART_TX_BUF_SIZE , 0, NULL, 0);

}


uint8_t checksum(uint8_t * string, int length)
{
	int i=0;
	uint8_t sum=string[0];
	for(i=1;i < length;i++)
	{
		sum=sum^string[i];
	}
	return sum;
}








