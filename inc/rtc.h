#include "stm32l0xx.h"
#include "stdbool.h"
#include "main.h"
#include "delay.h"

void rtc_init(void);
void rtc_setTimeout(int);


#define RTC_waitTime (0x2)
