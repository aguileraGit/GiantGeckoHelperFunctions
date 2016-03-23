/*
 * led.c
 *
 *  Created on: Mar 18, 2016
 *      Author: daguilera
 */

#include "led.h"


void initDebugLEDs(void) {
	// Enable GPIO peripheral clock
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED0 pin as digital output (push-pull)
	GPIO_PinModeSet(LED_PORT, LED0, gpioModePushPullDrive, 0);

	// Configure LED1 pin as digital output (push-pull) with drive-strength to lowest setting
	GPIO_PinModeSet(LED_PORT, LED1, gpioModePushPullDrive, 0);

	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

	GPIO_PinOutClear(LED_PORT, LED1);
	GPIO_PinOutClear(LED_PORT, LED0);
}


void LED0ON(void) {
	GPIO_PinOutSet(LED_PORT, LED0);
}


void LED0OFF(void) {
	GPIO_PinOutClear(LED_PORT, LED0);
}


void LED0TOGGLE(void) {
	GPIO_PinOutToggle(LED_PORT, LED0);
}


void LED1ON(void) {
	GPIO_PinOutSet(LED_PORT, LED1);
}


void LED1OFF(void) {
	GPIO_PinOutClear(LED_PORT, LED1);
}


void LED1TOGGLE(void) {
	GPIO_PinOutToggle(LED_PORT, LED1);
}


