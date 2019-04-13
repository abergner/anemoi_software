#ifndef _ANEMOI_GPIO_H_
#define _ANEMOI_GPIO_H_


void initGpio(void);

void sendTrigger(void);

void enableX(void);
void enableY(void);

void disableX(void);
void disableY(void);

void selectChannel1(void);
void selectChannel2(void);


void enableVddX1(void);
void enableVddX2(void);
void enableVddY1(void);
void enableVddY2(void);

void disableTdc1000(void);
void disableAllVdd(void);



#endif
