#include <em_device.h>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_emu.h>
#include <em_gpio.h>
#include <em_timer.h>
#include <em_usart.h>

#include <stdint.h>
#include <stdbool.h>

//#include "GPIO.h"
//#include "Helper.h"

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/

/**
 * Handle to my USART. Enables to be used with any USART
 * in the system. Will be set to USART1 for now. But will be a
 * place holder for future expansion
 */
USART_TypeDef* usart = USART1;

void delay(int num)
{
  int counter=0;
  for(int i=0;i<num*1000;i++)
  {
    counter++;
  }
}

void setup_clocks()
{
  //enable the clock for the High-Frequency Peripherals
  CMU_ClockEnable(cmuClock_HFPER, true);

  //enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  //enable the USART1 clock
  CMU_ClockEnable(cmuClock_USART1, true);
}

void setup_gpio_pins()
{
  /**
   * PD0 - MOSI - SDA
   * PD1 - MISO - SDO
   * PD2 - CLK - SCLK
   * PD3 - CS
   */
  GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 1);
  GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 1);
}

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
  //set frame size to 8, no parity, no stop..etc.

  //clear parity bits[9:8] to 0. Default is 0x0. No action needed.
  //clear stop bits[13:12] to 0. Default is 0x0. No action needed.
  // set DATABITS[3:0] to 0x5 for 8-bit mode. Default is 0x5. No action needed.

  //enable synchronous operations
  //set SYNC[0] in CTRL
  usart->CTRL |= 0x1;

  //set format to MSB first
  //set MSBF[10] in CTRL
  usart->CTRL |= 0x1 << 10;

  /**
   * Calculate desired clock speed (USARTn_CLKDIV) based upon formula below.
   * ADXL345 max speed is 5 MHz. Recommend selecting 1 MHz.
   *
   * USARTn_CLKDIV = 256 * (f_hfperclk/(2*desired_br)-1)
   */
  //query the USART clock to confirm value
  int usart1Clock = CMU_ClockFreqGet(cmuClock_USART1);
  unsigned int desired_BR = 1000000;
  unsigned int CLKDIV = 256 * (usart1Clock / (2*desired_BR)-1);

  // set CLKDIV register based upon calculation results
  usart->CLKDIV = CLKDIV;

  //ADXL345 expects SPI MODE 3.
  //Configure CPOL as Idle High. Set CPOL[8] to HIGH;
  //Configure CPHA as Trailing Edge. Set CPHA[9] to HIGH;
  usart->CTRL |= (0x1 << 8);
  usart->CTRL |= (0x1 << 9);

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
  usart->CMD |= 0x1 << 4;

  //enable transmissions and reception on the SPI interface
  //set transmit[2] and receive[0] in CMD to HIGH
  usart->CMD |= 0x1 << 2;
  usart->CMD |= 0x1;
}

/**
 * Conduct a single 8-bit transfer on the SPI interface
 * @param data Bits to be sent on SPI interface
 * @return Bits returned from transaction
 */
uint8_t SPI_Transfer(uint8_t data)
{

  //wait for transmit buffer to be empty (TXBL flag)
  //wait for space in the TX buffer
  while(!(usart->STATUS & USART_STATUS_TXBL)){};

  //put data in the TX buffer (TXDATA)
  usart->TXDATA=(uint32_t)data;

  //wait for transactions to complete (TXC flag)
  while(!(usart->STATUS & USART_STATUS_TXC)){};

  //read “received” data back from RX buffer
  return (uint8_t)usart->RXDATA;

}

/**
 * Read from an internal register of the ADXL 345.
 * @param address Internal ADXL345 register address to read
 * @return Result of read operation
 */
uint8_t readRegister(uint8_t address)
{

  //construct a read packet based upon the address
  //provided in the function parameter
  //...your code here...
  uint8_t readPacket = address |= 0x1<<7;
  //set CS (PD3) pin low
  GPIO_PinOutClear(gpioPortD,3);

  //send out the read packet using SPI_Transfer()
  //...your code here...
  SPI_Transfer(address);
  //send the second packet with dummy
  //data (0x0) to continue the transfer. Read the result into a
  //local uint8_t variable.
  //...your code here...
  uint8_t result = SPI_Transfer(0x0);
  //set CS (PD3) pin HIGH
  GPIO_PinOutSet(gpioPortD,3);

  //return the result of the second transfer
  //...your code here...
  return result;
}

/**
 * Write to an internal register of the ADXL345
 * @param address Internal register to be written to
 * @param data Bits that will be written to the register
 */
void writeRegister(uint8_t address, uint8_t data)
{
  //construct a write packet based upon the address and value
  //provided in the function parameter
  //...your code here...
  uint8_t writeAddress = address |= 0x0<<7;
  uint8_t writeData = data |= 0x13;
  //set CS (PD3) pin low
  GPIO_PinOutClear(gpioPortD,3);

  //send out the write packet using SPI_Transfer()
  //...your code here...
  SPI_Transfer(address);
  //send the second packet with the data to be written to the ADXL345
  //...your code here...
  SPI_Transfer(data);
  //set CS (PD3) pin HIGH
  GPIO_PinOutSet(gpioPortD,3);

  //return nothing
  return;

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

  /**
   * Task 1: Complete the SPI Transfer and the Read Register functions. Once done
   * the following lines will attempt to read from the DEVID register at
   * memory address 0x0 in the ADXL. The result should be 0xE5 if performed
   * correctly.
   */

  uint8_t register_to_read = 0x0;
  uint8_t result = readRegister(register_to_read);

  if(result==0xE5){
      //success!!!
      int success=0;
  }
  else{
      int failure=0;
  }

  /**
   * Task 2: Complete Write Register. Write a random set of data to a register
   * in the ADXL345 that will not be used (TAP_THRESH?). Read that data back, if the
   * same result is returned, then the Write function is correct.
   */

  uint8_t data_to_write = 0x1F;
  uint8_t register_to_write = 0x1D;

  writeRegister(register_to_write, data_to_write);
  uint8_t write_result = readRegister(register_to_write);

  if (write_result == data_to_write)
    {
      int succes=0;
    }
  else
    {
      int failure=0;
    }

  // wait a short time to not spam the interface
  delay(10);

}
