/*
 * rtc.h
 *
 *  Created on: Apr 29, 2021
 *      Author: grantweiss
 */

#ifndef RTC_H_
#define RTC_H_

void rtc_init();
void rtc_wakeup(uint32_t time);
void rtc_deinit();

#endif /* RTC_H_ */
