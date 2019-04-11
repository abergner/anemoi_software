#ifndef _ANEMOI_GPIO_H_
#define _ANEMOI_GPIO_H_



void initAnemoiGPIO(void);

void sendTrigger(void);
void initTrigger(void);

void initEnableTDC1000(void);
void enableX(void);
void enableY(void);
void disableTDC1000(void);
void disableX(void);
void disableY(void);

void initChannelSelect(void);
void selectChannel1(void);
void selectChannel2(void);


void initEnableVdd(void);
void enableVddX1(void);
void enableVddX2(void);
void enableVddY1(void);
void enableVddY2(void);
void disableAllVdd(void);



#endif
