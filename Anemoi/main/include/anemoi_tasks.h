#ifndef _ANEMOI_TASKS_H_
#define _ANEMOI_TASKS_H_

void init_anemoi_tasks(void);

void time_measurement_task(void *pvParameter);
void TDC1000_task(void *pvParameter);
void trigger_task(void *pvParameter);


#endif
