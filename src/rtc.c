/*
 * rtc.c
 *
 *  Created on: Apr 29, 2021
 *      Author: grantweiss
 */

#include "stm32l0xx.h"
#include "main.h"
#include "stdbool.h"

bool rtc_isSetup = false;

void rtc_init()
{
    // Enable the power interface
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);

    // Enable access to the backup domain
    SET_BIT(PWR->CR, PWR_CR_DBP);

    // Enable LSI
    SET_BIT(RCC->CSR, RCC_CSR_LSION);

    // Wait until LSI is ready
    while(READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY);

    // Select LSI as clock source for the RTC
    MODIFY_REG(RCC->CSR, RCC_CSR_RTCSEL, RCC_CSR_RTCSEL_LSI);

    // Enable the RTC
    SET_BIT(RCC->CSR, RCC_CSR_RTCEN);

	// enable LSI clock in stop mode
	SET_BIT(RCC->CSR, RCC_CSR_LSION);

	rtc_isSetup = true;
}

void rtc_wakeup(uint32_t time)
{
	if(rtc_isSetup == false) {	// not enabled
		rtc_init();
	}

    // Unlock the write protection
    WRITE_REG(RTC->WPR, 0xCA);
    WRITE_REG(RTC->WPR, 0x53);

    // Stop the wakeup timer to allow configuration update
    CLEAR_BIT(RTC->CR, RTC_CR_WUTE);

    // Wait until the wakeup timer is ready for configuration update
    while (READ_BIT(RTC->ISR, RTC_ISR_WUTWF) != RTC_ISR_WUTWF);

    SET_BIT(RTC->ISR, RTC_ISR_INIT);
    while(READ_BIT(RTC->ISR, RTC_ISR_INITF) != RTC_ISR_INITF);

    // set prescalers for 37khz LSI clock, resulting in 1Hz RTC counter
    MODIFY_REG(RTC->PRER, (RTC_PRER_PREDIV_A | RTC_PRER_PREDIV_S), ((74-1) << RTC_PRER_PREDIV_A_Pos) | ((500-1) << RTC_PRER_PREDIV_S_Pos));

    CLEAR_BIT(RTC->ISR, RTC_ISR_INIT);

    // The wakeup period is 0+1 clock pulses
    WRITE_REG(RTC->WUTR, time);

    // Clock source of the wakeup timer is 1 Hz
    MODIFY_REG(RTC->CR, RTC_CR_WUCKSEL, RTC_CR_WUCKSEL_2);

    // Enable the wakeup timer with interrupts
    SET_BIT(RTC->CR, RTC_CR_WUTE | RTC_CR_WUTIE);

    // Switch the write protection back on
    WRITE_REG(RTC->WPR, 0xFE);
    WRITE_REG(RTC->WPR, 0x64);

    // Enable EXTI20 interrupt on rising edge
    SET_BIT(EXTI->IMR, EXTI_IMR_IM20);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR20);
	NVIC_SetPriority(RTC_IRQn,0);
    NVIC_EnableIRQ(RTC_IRQn);

    // Clear (old) pending interrupt flag
    CLEAR_BIT(RTC->ISR, RTC_ISR_WUTF);  // Clear in RTC
    SET_BIT(EXTI->PR, EXTI_PR_PR20);    // Clear in NVIC
}

void rtc_deinit() {
    // Unlock the write protection
    WRITE_REG(RTC->WPR, 0xCA);
    WRITE_REG(RTC->WPR, 0x53);

    // Stop the wakeup timer to allow configuration update
    CLEAR_BIT(RTC->CR, RTC_CR_WUTE);

    // Wait until the wakeup timer is ready for configuration update
    while (READ_BIT(RTC->ISR, RTC_ISR_WUTWF) != RTC_ISR_WUTWF);

    // Disable the wakeup timer with interrupts
    CLEAR_BIT(RTC->CR, RTC_CR_WUTE | RTC_CR_WUTIE);

    // Switch the write protection back on
    WRITE_REG(RTC->WPR, 0xFE);
    WRITE_REG(RTC->WPR, 0x64);

    // Disable EXTI20 interrupt on rising edge
    CLEAR_BIT(EXTI->IMR, EXTI_IMR_IM20);
    CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR20);
    NVIC_DisableIRQ(RTC_IRQn);

    // Clear (old) pending interrupt flag
    CLEAR_BIT(RTC->ISR, RTC_ISR_WUTF);  // Clear in RTC
    SET_BIT(EXTI->PR, EXTI_PR_PR20);    // Clear in NVIC

    // Disable the RTC
    CLEAR_BIT(RCC->CSR, RCC_CSR_RTCEN);

	// disable LSI clock in stop mode
	CLEAR_BIT(RCC->CSR, RCC_CSR_LSION);

	rtc_isSetup = false;
}

void RTC_IRQHandler(void) {
    // Clear interrupt flag
    CLEAR_BIT(RTC->ISR, RTC_ISR_WUTF);  // Clear in RTC
    SET_BIT(EXTI->PR, EXTI_PR_PR20);    // Clear in NVIC

    hat_flag = 1;
}
