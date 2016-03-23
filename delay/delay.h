/*
 * delay.h
 *
 *  Created on: Mar 18, 2016
 *      Author: daguilera
 */

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_system.h"
#include "em_chip.h"

#ifndef DELAY_H_
#define DELAY_H_

volatile uint32_t msTicks; /* counts 1ms timeTicks */

void delayMs(uint32_t dlyTicks);

#endif /* DELAY_H_ */
