/*
 * uart.h
 *
 *  Created on: Mar 18, 2016
 *      Author: daguilera
 */

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_system.h"
#include "em_emu.h"
#include "em_usart.h"

#ifndef UART_H_
#define UART_H_

    /*
    // Check if RX buffer has overflowed
    if (rxBuf.overflow) {
      rxBuf.overflow = false;
      //uartPutData((uint8_t*) overflowString, ofsLen);
      LED1ON();
      while(1){};
    }
	*/


	/*
    if (rxBuf.data[(rxBuf.wrI - 1) % BUFFERSIZE] == TERMINATION_CHAR)
    {
      // Copy received data to UART transmit queue
      uint8_t tmpBuf[BUFFERSIZE];
      int     len = uartGetData(tmpBuf, 0);
      uartPutData(tmpBuf, len);
    }
    */




/* Declare a circular buffer structure to use for Rx and Tx queues */
#define BUFFERSIZE          256

typedef struct circularBuffer {
	  uint8_t  data[BUFFERSIZE];  /* data buffer */
	  uint32_t rdI;               /* read index */
	  uint32_t wrI;               /* write index */
	  uint32_t pendingBytes;      /* count of how many bytes are not yet handled */
	  bool     overflow;          /* buffer overflow indicator */
} CircularBuffer;

void initUart(uint16_t uartSpeed);

//Rx
void uartPutChar(uint8_t charPtr);
void uartPutData(uint8_t * dataPtr, uint32_t dataLen);

//Tx
uint8_t uartGetChar(void);
uint32_t uartGetData(uint8_t * dataPtr, uint32_t dataLen);

void UART0_RX_IRQHandler(void);
void UART0_TX_IRQHandler(void);

#endif /* UART_H_ */
