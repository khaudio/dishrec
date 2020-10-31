#ifndef WDT_H
#define WDT_H

#include <Arduino.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "esp_task_wdt.h"
#include "FreeRTOS.h"

/* To feed wdt normally, use esp_task_wdt_reset() */

void disable_wdt();
void force_reset_wdt_0();
void force_reset_wdt_1();

#endif
