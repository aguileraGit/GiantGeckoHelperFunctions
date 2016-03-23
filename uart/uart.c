/*
 * uart.c
 *
 *  Created on: Mar 18, 2016
 *      Author: daguilera
 */

#include "uart.h"

/* Setup USART1 in async mode for RS232*/
static USART_TypeDef           * uart   = UART0;
static USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

volatile CircularBuffer UglyRxBuff = { {0}, 0, 0, 0, false };
volatile CircularBuffer UglyTxBuff = { {0}, 0, 0, 0, false };

/******************************************************************************
* @brief  uartSetup function
*
******************************************************************************/
void initUart(uint16_t uartSpeed){
	/* Enable clock for HF peripherals */
	CMU_ClockEnable(cmuClock_HFPER, true);

	/* Enable clock for USART module */
	CMU_ClockEnable(cmuClock_UART0, true);

	/* Enable clock for GPIO module (required for pin configuration) */
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Configure GPIO pins */
	GPIO_PinModeSet(gpioPortE, 0, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortE, 1, gpioModeInput, 0);

	/* Prepare struct for initializing UART in asynchronous mode*/
	uartInit.enable       = usartDisable;   /* Don't enable UART upon intialization */
	uartInit.refFreq      = 0;              /* Provide information on reference frequency. When set to 0, the reference frequency is */
	uartInit.baudrate     = uartSpeed;      /* Baud rate */
	uartInit.oversampling = usartOVS16;     /* Oversampling. Range is 4x, 6x, 8x or 16x */
	uartInit.databits     = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
	uartInit.parity       = usartNoParity;  /* Parity mode */
	uartInit.stopbits     = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
	uartInit.mvdis        = false;          /* Disable majority voting */
	uartInit.prsRxEnable  = false;          /* Enable USART Rx via Peripheral Reflex System */
	uartInit.prsRxCh      = usartPrsRxCh0;  /* Select PRS channel if enabled */

	/* Initialize USART with uartInit struct */
	USART_InitAsync(uart, &uartInit);

	/* Prepare UART Rx and Tx interrupts */
	USART_IntClear(uart, _UART_IF_MASK);
	USART_IntEnable(uart, UART_IF_RXDATAV);
	NVIC_ClearPendingIRQ(UART0_RX_IRQn);
	NVIC_ClearPendingIRQ(UART0_TX_IRQn);
	NVIC_EnableIRQ(UART0_RX_IRQn);
	NVIC_EnableIRQ(UART0_TX_IRQn);

	/* Enable I/O pins at USART1 location #2 */
	uart->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC1;

	/* Enable UART */
	USART_Enable(uart, usartEnable);
}


/******************************************************************************
 * @brief  uartPutChar function
 *
 *****************************************************************************/
void uartPutChar(uint8_t ch) {



	/* Check if Tx queue has room for new data */
	if ((UglyTxBuff.pendingBytes + 1) > BUFFERSIZE) {
		/* Wait until there is room in queue */
	while ((UglyTxBuff.pendingBytes + 1) > BUFFERSIZE) ;
	}

	/* Copy ch into UglyTxBuffer */
	UglyTxBuff.data[UglyTxBuff.wrI] = ch;
	UglyTxBuff.wrI             = (UglyTxBuff.wrI + 1) % BUFFERSIZE;

	/* Increment pending byte counter */
	UglyTxBuff.pendingBytes++;

	/* Enable interrupt on USART TX Buffer*/
	USART_IntEnable(uart, UART_IF_TXBL);
}

/**************************************************************************//**
 * @brief USART1 RX IRQ Handler
 *
 * Set up the interrupt prior to use
 *
 * Note that this function handles overflows in a very simple way.
 *
 *****************************************************************************/
void UART0_RX_IRQHandler(void) {
	// Check for RX data valid interrupt
	if (uart->STATUS & UART_STATUS_RXDATAV)  {
		// Copy data into RX Buffer
		uint8_t rxData = USART_Rx(uart);
		UglyRxBuff.data[UglyRxBuff.wrI] = rxData;
		UglyRxBuff.wrI             = (UglyRxBuff.wrI + 1) % BUFFERSIZE;
		UglyRxBuff.pendingBytes++;

		// Flag Rx overflow
		if (UglyRxBuff.pendingBytes > BUFFERSIZE) {
			UglyRxBuff.overflow = true;
		}

	// Clear RXDATAV interrupt
	USART_IntClear(UART0, UART_IF_RXDATAV);
	}
}



/**************************************************************************//**
 * @brief USART1 TX IRQ Handler
 *
 * Set up the interrupt prior to use
 *
 *****************************************************************************/
void UART0_TX_IRQHandler(void) {
  // Clear interrupt flags by reading them.
  USART_IntGet(UART0);

  // Check TX buffer level status
  if (uart->STATUS & UART_STATUS_TXBL) {
    if (UglyTxBuff.pendingBytes > 0) {
      // Transmit pending character
      USART_Tx(uart, UglyTxBuff.data[UglyTxBuff.rdI]);
      UglyTxBuff.rdI = (UglyTxBuff.rdI + 1) % BUFFERSIZE;
      UglyTxBuff.pendingBytes--;
    }

    // Disable Tx interrupt if no more bytes in queue
    if (UglyTxBuff.pendingBytes == 0) {
      USART_IntDisable(uart, UART_IF_TXBL);
    }
  }
}



/******************************************************************************
 * @brief  uartGetChar function
 *
 *  Note that if there are no pending characters in the receive buffer, this
 *  function will hang until a character is received.
 *
 *****************************************************************************/
uint8_t uartGetChar( ) {
  uint8_t ch;

  // Check if there is a byte that is ready to be fetched. If no byte is ready, wait for incoming data
  if (UglyRxBuff.pendingBytes < 1) {
    while (UglyRxBuff.pendingBytes < 1) ;
  }

  // Copy data from buffer
  ch = UglyRxBuff.data[UglyRxBuff.rdI];
  UglyRxBuff.rdI = (UglyRxBuff.rdI + 1) % BUFFERSIZE;

  // Decrement pending byte counter
  UglyRxBuff.pendingBytes--;

  return ch;
}


/******************************************************************************
 * @brief  uartGetData function
 *
 *****************************************************************************/
uint32_t uartGetData(uint8_t * dataPtr, uint32_t dataLen) {
  uint32_t i = 0;

  // Wait until the requested number of bytes are available
  if (UglyRxBuff.pendingBytes < dataLen) {
    while (UglyRxBuff.pendingBytes < dataLen) ;
  }

  if (dataLen == 0) {
    dataLen = UglyRxBuff.pendingBytes;
  }

  // Copy data from Rx buffer to dataPtr
  while (i < dataLen) {
    *(dataPtr + i) = UglyRxBuff.data[UglyRxBuff.rdI];
    UglyRxBuff.rdI      = (UglyRxBuff.rdI + 1) % BUFFERSIZE;
    i++;
  }

  // Decrement pending byte counter
  UglyRxBuff.pendingBytes -= dataLen;

  return i;
}


/******************************************************************************
 * @brief  uartPutData function
 *
 *****************************************************************************/
void uartPutData(uint8_t * dataPtr, uint32_t dataLen) {
  uint32_t i = 0;

  // Check if buffer is large enough for data
  if (dataLen > BUFFERSIZE) {
    // Buffer can never fit the requested amount of data
    return;
  }

  // Check if buffer has room for new data
  if ((UglyTxBuff.pendingBytes + dataLen) > BUFFERSIZE) {
    // Wait until room //
    while ((UglyTxBuff.pendingBytes + dataLen) > BUFFERSIZE) ;
  }

  // Fill dataPtr[0:dataLen-1] into txBuffer
  while (i < dataLen) {

	  if (dataPtr != '\0') {
		  UglyTxBuff.data[UglyTxBuff.wrI] = *(dataPtr + i);
		  UglyTxBuff.wrI             = (UglyTxBuff.wrI + 1) % BUFFERSIZE;
		  i++;
	  }
  }

  // Increment pending byte counter
  UglyTxBuff.pendingBytes += dataLen;

  // Enable interrupt on USART TX Buffer
  USART_IntEnable(uart, UART_IF_TXBL);
}


