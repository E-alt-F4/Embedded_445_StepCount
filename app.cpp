
//include accelerometer class
#include "ADXL345.h"

//include custom timer object
#include "PeriodicTimer.h"

//include vectors, math, string
#include <vector>
#include <cmath>
#include <string>

//define appropriate namespace
using namespace std;

//ADXL object
ADXL345 accel;

//Timer object
PeriodicTimer timer;


/**
 * Define all global variables and data arrays
 */
const int ARRAY_LENGTH = 16;
float magnitude[ARRAY_LENGTH];
float diffs[ARRAY_LENGTH];
float squares[ARRAY_LENGTH];
float averages[ARRAY_LENGTH];

/**
 * Enumerate some basic states the machine may wish to follow
 */
enum STATE {IDLE, DETECT, TIMEOUT};
STATE currentState;
STATE nextState;


/**
 * Place all operations here that will be run "once". These include initializing the
 * SPI, UART, and TIMER Interfaces
 */
void app_init(void)
{

  //enable the clock for the High-Frequency Peripherals
  CMU_ClockEnable(cmuClock_HFPER, true);

  //enable the specific clock for the TIMER0
  CMU_ClockEnable(cmuClock_TIMER0, true);

  /**
   * Initial all arrays defined above to known values. Really should be 0.
   * Will use FOR LOOP that is very inefficient but is guaranteed to work on all systems
   */

  for(int i=0;i<ARRAY_LENGTH;i++)
    {
      magnitude[i]=0;
      diffs[i]=0;
      squares[i]=0;
      averages[i]=0;
    }

  // Initialize the states of the machine to the initial one
  currentState = IDLE;
  nextState = IDLE;

  //initialize accelerometer
  accel.initialize();

  bool success = accel.selfTest();

  //infinite loop if self-test fails
  while(!success){}

  //initialize periodic timer
  timer.initialize();

  /* Turn on board LEDS for debugging purpose */
  GPIO_PinModeSet(gpioPortE,                /* GPIO port */
                  2,                        /* GPIO port number */
                  gpioModePushPull,         /* Pin mode is set to push pull */
                  0);                       /* High idle state */

}

int counter = 0;
void app_process_action(void)
{

  /**
   * Step 1: Acquire data from ADXL345
   */
  int16_t xAccel = accel.getXAcceleration();
  int16_t yAccel = accel.getYAcceleration();
  int16_t zAccel = accel.getZAcceleration();



  /**
   * Step 2: Put new acceleration data through data path
   */

  //YOUR DATA PATH CODE HERE. Example code below.

  int32_t mag = sqrt(xAccel*xAccel + yAccel*yAccel + zAccel*zAccel);

  float g_magnitude = mag / 256.0;

  /**
   * Step 3: Run your analysis pipeline
   */

  // YOUR DATAPATH CODE HERE....

  magnitude[counter%ARRAY_LENGTH] = g_magnitude;
  //diff
  float d = magnitude[counter%ARRAY_LENGTH]-magnitude[(counter-1)%ARRAY_LENGTH];
  diffs[counter%ARRAY_LENGTH] = d;
  //square
  squares[counter%ARRAY_LENGTH] = d*d;
  //sum squares
  float sum = 0;
  for (int i=0;i<ARRAY_LENGTH;i++) {
      sum += squares[i];
  }
  float averages_sum =sum/ARRAY_LENGTH;
  averages[counter%ARRAY_LENGTH] = averages_sum;


  /**
   * Step 4: Run your finite state machine
   */
  //YOUR FSM CODE HERE. Example code below.

  static int step_counter=0;
  static int time_out=0;

    switch(currentState){
       case(IDLE): {
           // do what is needed in the IDLE state. We'll just stay here as an example
           float value = magnitude[counter%ARRAY_LENGTH];
           //give me the current value to reference
           if (averages_sum > 0.02 && value > magnitude[(counter - 1 + ARRAY_LENGTH) % ARRAY_LENGTH] && value > magnitude[(counter +1)%ARRAY_LENGTH])
            //lowered the value to detect a step at and then only record the value if it is a peak meaning the previous and next value are lower than current value
             {
               nextState = DETECT;
           }
           else {
               nextState = IDLE;
           }
           break;
       }
       case(DETECT):
           // do what is needed in the DETECT state
           step_counter++;
           time_out=0;
           nextState=TIMEOUT;
           break;

       case(TIMEOUT):
           // do what is needed in the TIMEOUT state
           time_out++;
           if (time_out>7) { //same static number used on the clion code which was my personal walk data
               nextState = IDLE;
           }
           else {
               nextState = TIMEOUT;
           }
           break;

       default:
           //printf("Error! This should never happen!\n");
           break;
       }

   // update state machine for next iteration
   currentState = nextState;

   // increment our counter to know how many iterations this has been.
   counter++;
   //printf("Number of steps taken is %d\n", step_counter);
   // return true so the main() method will continue to call us...
   return;


  /**
   * Step 5: Sleep the processor for a fixed duration. This will be the
   * sample rate for the project.
   */
  // wait for 1s = 1000 ms
  timer.wait(500);
 //lowered so the number of data points sampled was less
  // toggle the on-board LED for debugging purposes
  GPIO_PinOutToggle(gpioPortE,2);
}
