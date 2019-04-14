#ifndef _ANEMOI_GPIO_H_
#define _ANEMOI_GPIO_H_


#define START_QUEUE_LENGTH 1
#define STOP_QUEUE_LENGTH 30

typedef enum {START_QUEUE, STOP_QUEUE} TypeQueue;

void initGpio(void);

bool getQueueCycles(TypeQueue typeQueue, uint32_t * ptrCycles);

void enableStartStopInterrupt(Direction direction);
void disableStartStopInterrupt(void);

void sendTrigger(void);

void enableTdc1000(Direction direction);
void disableTdc1000(void);

void selectChannel(Axis axis);

void enableVdd(Axis axis, Direction direction);
void disableVdd(void);


#endif
