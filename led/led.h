/*
 * led.h
 *
 *  Created on: Mar 18, 2016
 *      Author: daguilera
 */

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_system.h"

#define LED_PORT gpioPortE
#define LED0 2
#define LED1 3

#ifndef LED_H_
#define LED_H_

void initDebugLEDs(void);

void LED0ON(void);
void LED0OFF(void);
void LED0TOGGLE(void);

void LED1ON(void);
void LED1OFF(void);
void LED1TOGGLE(void);

#endif /* LED_H_ */
