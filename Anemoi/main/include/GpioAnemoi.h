#ifndef _ANEMOI_GPIO_H_
#define _ANEMOI_GPIO_H_


#define START_QUEUE_LENGTH 1
#define STOP_QUEUE_LENGTH 30

typedef enum {START_QUEUE, STOP_QUEUE} TypeQueue;

bool getQueueCycles(TypeQueue typeQueue, uint32_t * ptrCycles);

void initStartStop(void);

void enableStartStopInterruptX(void);
void disableStartStopInterruptX(void);
void enableStartStopInterruptY(void);
void disableStartStopInterruptY(void);


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
