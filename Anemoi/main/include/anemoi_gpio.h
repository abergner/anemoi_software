#ifndef _ANEMOI_GPIO_H_
#define _ANEMOI_GPIO_H_

void initAnemoiGPIO(void);

void initStartStop(void);
void enableStartStopInterrupt(void);
void disableStartStopInterrupt(void);
bool calculateTOF(double * TimeofFlight);

void sendTrigger(void);
void initTrigger(void);

void initEnableTDC1000(void);
void enableX(void);
void enableY(void);
void disableTDC1000(void);
void disableX(void);
void disableY(void);

void initEnableTDC7200(void);
void enableTDC7200(void);
void disableTDC7200(void);

void initChannelSelect(void);
void selectChannel1(void);
void selectChannel2(void);


void initEnableVdd(void);
void enableX1Vdd(void);
void enableX2Vdd(void);
void enableY1Vdd(void);
void enableY2Vdd(void);
void disableAllVdd(void);



#endif
