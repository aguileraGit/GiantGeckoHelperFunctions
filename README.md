Helper functions for the Giant Gecko.

1. Delay function
  * Adds delayMs();
2. LEDs (Demo kit)
  * Uses LED0 and LED1. n - 0/1
  * LEDnON();
  * LEDnOFF();
  * LEDnTOGGLE();
3. UART - Currently prints out a bunch of ...... Lame.
  * Demo board and hydo board have different pin assignments and locations.
  * Must define UARTdemo or UARThydro when initializing debug port. Defined in userDefines
4. ADC - Currently only supports 1 ADC. Requires the following in main code:
  * #include "em_adc.h"
  * #include "adc/adc.h"
  * Create default instance: ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;
  * Update settings as needed: singleInit.input = adcSingleInpVrefDiv2;
5. userDefines
  * Adds TRUE/FALSE
  * Defines UART debug for demoboard and hydro board
