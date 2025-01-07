#include <em_device.h>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_emu.h>
#include <em_gpio.h>
#include <em_timer.h>
#include <em_usart.h>

#include <stdint.h>
#include <stdbool.h>

#include "GPIO.h"
#include "Helper.h"

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/

/**
 * Handle to my USART. Enables to be used with any USART
 * in the system. Will be set to USART1 for now. But will be a
 * place holder for future expansion
 */
USART_TypeDef* usart = USART1;

/**
 * Turn on all the system clocks that will be needed:
 * High Frequency Peripherals
 * GPIO
 * USART1
 */
void setup_clocks()
{
  //enable the clock for the High-Frequency Peripherals
  CMU_ClockEnable(cmuClock_HFPER, true);

  //enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  //enable the USART1 clock
  CMU_ClockEnable(cmuClock_USART1, true);
}

/**
 * Configure all the GPIO pins that will be used. Use
 * the built-in EMLIB calls as a short cut
 */
void setup_gpio_pins()
{
  /**
   * PD0 - MOSI - Output
   * PD1 - MISO - Input
   * PD2 - CLK - Output
   * PD3 - CS - Output
   */
  GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 1);
  GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 1);
}

/**
 * Configure the USART1 interface to operate in
 * SPI mode
 */
void setup_SPI()
{
  /**
   * Setup USART for Synchronous operations
   */

  /**
   * Configure the size of the data packet to be sent.
   * It should be 8-bits with no stop, or parity bits.
   * Enable synchronous operation and Most Significant Bit First operation.
   */

  //clear parity bits[9:8] to 0. Default is 0x0. No action needed.
  //clear stop bits[13:12] to 0. Default is 0x0. No action needed.
  // set DATABITS[3:0] to 0x5 for 8-bit mode. Default is 0x5. No action needed.

  //enable synchronous operations
  //set SYNC[0] in CTRL

  //YOUR CODE HERE
  usart->CTRL |= 0x1;

  //set format to MSB first
  //set MSBF[10] in CTRL

  //YOUR CODE HERE
  usart->CTRL |= 0x1 << 10;

  /**
   * Calculate desired clock speed (USARTn_CLKDIV) based upon formula below.
   * ADXL345 max speed is 5 MHz. Recommend selecting 1 MHz.
   *
   * USARTn_CLKDIV = 256 * (usart1Clock/(2*desired_br)-1)
   */
  //query the USART clock to confirm value
  int usart1Clock = CMU_ClockFreqGet(cmuClock_USART1);

  //set variable to desired SPI CLK rate for calculation
  unsigned int desired_BR = 1000000;

  //complete USARTn_CLKDIV equation to find the correct value
  unsigned int CLKDIV = 256 * (usart1Clock/((2*desired_BR)-1));

  // set CLKDIV register based upon calculation results
  //YOUR CODE HERE
  usart->CLKDIV = CLKDIV;

  //ADXL345 expects SPI MODE 3.
  //Configure CPOL as Idle High. Set CPOL[8] to HIGH;
  //YOUR CODE HERE
  usart->CTRL |= (0x1 << 8);

  //Configure CPHA as Trailing Edge. Set CPHA[9] to HIGH;
  //YOUR CODE HERE
  usart->CTRL |= (0X1 << 9);

  // Enable transmission buffer level. Unsure why we have this but it works :)
  //set TXBIL to 1 [12] in control
  usart->CTRL |= (0x1 << 12);

  /**
   * Setup all pins for Route #1
   * Dr. Forsyth has completed this.
   */
  usart->ROUTE = (0x1) << 8;

  /**
   * Enable all pins in ROUTE[3-0]
   * Dr. Forsyth has completed this.
   */
  //re-route clock pin, set CLKPEN[3]
  usart->ROUTE |= (0x1) << 3;

  //re-route TX pin, set TXPEN[1]
  usart->ROUTE |= (0x1) << 1;

  //re-route RX pin, set RXPEN[0]
  usart->ROUTE |= 0x1;

  /**
   * Enable Master, TX, and RX
   */
  //set to master mode
  //set MASTEREN[4] in CMD to HIGH
  //YOUR CODE HERE
  usart->CMD |= (0x1 << 4);

  //enable transmissions and reception on the SPI interface
  //set transmit[2] in CMD to HIGH
  //YOUR CODE HERE
  usart->CMD |= (0x1 << 2);

  //set receive[0] in CMD to HIGH
  //YOUR CODE HERE
  usart->CMD |= (0x1);
}
void app_init(void)
{

  //configure the required clocks
  setup_clocks();

  //configure the required GPIO
  setup_gpio_pins();

  //configure the USART
  setup_SPI();


}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{

  // Collision Detection Enabled. Why did I turn this on?
  USART1->CTRL|=0x1<<2;

  /**
   * Step 1: Select a "random" 8-bit value to transmit.
   * Represent the number in hex.
   */
  uint8_t txNum = 0x37;

  /**
   * Step 2: Write the value to be sent to the TXDARA
   */

  //write your variable into the TXDATA register of the USART
  usart->TXDATA=txNum;


  /**
   * Step 3: Perpetually poll/query the Received Data bit in STATUS to see
   * if data has been received by the RX Buffer. When HIGH, break out of the loop
   * because data has been received.
   */
  bool dataAvailable = false;
  while(!dataAvailable){
      dataAvailable = readBit(&(usart->STATUS),7);
  }

  /**
   * Step 4: Read the transmission result out of the RXDATA buffer. It should
   * (hopefully) be the value that was sent above. We will text in the later
   * IF loop.
   */
  uint8_t rxNum = usart->RXDATA;

  bool task1Success=false;
  if(rxNum==txNum)
    {
      task1Success=true;
    }

  // wait a short time to not spam the interface
  delay(10);

}
