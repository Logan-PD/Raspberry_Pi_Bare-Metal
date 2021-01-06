/*
 * Logan Perry-Din
 * UCID: 30070661
 * CPSC 359 Fall 2019
 * Assignment 2
 *
 * This program initializes three pins to be output light pins
 * and two pins as interrupt pins coming from buttons.
 * On boot, the lights display one pattern, then after an Interrupt
 * is received a different pattern is displayed.
 * One button will put the program into state 1/pattern 1 and the
 * other button will go into state 2.
 */


// Include files
#include "uart.h"
#include "sysreg.h"
#include "gpio.h"
#include "irq.h"
#include "systimer.h"


// Function prototypes
void init_GPIOPIN(int pin, int io);
void set_GPIO(int pin);
void clear_GPIO(int pin);

// Shared value to communicate with handlers
unsigned int sharedValue;


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       main
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    Initializes pins 4, 17 and 27 as output pins; 22 and 23 as
//                  interrupt pins. Initializes the uart and IRQ then loops
//                  forever displaying one of two patterns, waiting for
//                  interrupts. The pattern that is displayed is decided by the
//                  value of the global shared value, which is changed by the
//                  interrupts.
//
////////////////////////////////////////////////////////////////////////////////

void main(){

  // Set up the UART serial port
  uart_init();

  // IRQ exceptions
  enableIRQ();

  //Initialize
  sharedValue = 0;

  // Set up GPIO pin #23 and 22 for rising edge interrupts
  init_GPIOPIN(22,0);
  init_GPIOPIN(23,0);

  // Set pin 22 and 23 to generate an interrupt on rising edge.
  // Do so by setting bit 22 and 23 by shifting bit pattern '11'
  *GPREN0 = (0x3 << 22);

  // Enable the GPIO IRQS for ALL the GPIO pins by setting IRQ 52
  // GPIO_int[3] in the Interrupt Enable Register 2 to a 1 value.
  // See p. 117 in the Broadcom Peripherals Manual.
  *IRQ_ENABLE_IRQS_2 = (0x1 << 20);

  // Set up pin 4, 17 and 27 (lights) for output
  init_GPIOPIN(4,1);
  init_GPIOPIN(17,1);
  init_GPIOPIN(27,1);

  // Print out a message to the console
  uart_puts("Program starting.\n");

  // Loop forever, waiting for interrupts...
  while (1) {

    if (sharedValue==0) { // Pattern 1

      uart_puts("Pattern 1");

      set_GPIO(4);  //turn light 1 on
      microsecond_delay(500000);  //wait 0.5 seconds
      clear_GPIO(4);  //turn light 1 off
      microsecond_delay(500000);  //wait 0.5 seconds

      set_GPIO(17);
      microsecond_delay(500000);
      clear_GPIO(17);
      microsecond_delay(500000);

      set_GPIO(27);
      microsecond_delay(500000);
      clear_GPIO(27);
      microsecond_delay(500000);

    } else if(sharedValue==1){ // Pattern 2

      uart_puts("Pattern 2");

      set_GPIO(27);
      microsecond_delay(250000);
      clear_GPIO(27);
      microsecond_delay(250000);

      set_GPIO(17);
      microsecond_delay(250000);
      clear_GPIO(17);
      microsecond_delay(250000);

      set_GPIO(4);
      microsecond_delay(250000);
      clear_GPIO(4);
      microsecond_delay(250000);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_GPIOPIN
//
//  Arguments:      int:pin pin number to initialize
//                  int:io number deciding whether pin is initialized to input
//                  output:
//                  input = 0
//                  ouput = 1
//
//  Returns:        void
//
//  Description:    Initializes the specified pin to be input or output.
//
//  Sources:        Numerous examples where this exact procedure is followed
//                  in tutorial and Dr. Boyds slides were used to write this
//                  method.
//
////////////////////////////////////////////////////////////////////////////////

void init_GPIOPIN(int pin, int io){

  register unsigned int r;

  // Get the current contents of the GPIO Function Select Register 0-5
  //depending on pin number

  if(pin<10){
    r = *GPFSEL0;
  }else if(pin<20){
    r = *GPFSEL1;
  }else if(pin<30){
    r = *GPFSEL2;
  }else if(pin<40){
    r = *GPFSEL3;
  }else if(pin<50){
    r = *GPFSEL4;
  }else{
    r = *GPFSEL5;
  }


  // Clear 3 bits corresponding to the pin.
  // Use least significant digit of pin number and times by three
  // We clear the bits by ANDing with a 000 bit pattern in the field.
  int lsd = pin % 10;
  r &= ~(0x7 << (3*lsd));

  // Set function select to 001 if necessary, by ORin with 001
  // if not, the pin is already set for output.
  if(io==1){
    r |= (0x1 << 3*lsd);
  }


  // Write the modified bit pattern back to the
  // corresponding GPIO Function Select Register
  if(pin<10){
    *GPFSEL0 = r;
  }else if(pin<20){
    *GPFSEL1 = r;
  }else if(pin<30){
    *GPFSEL2 = r;
  }else if(pin<40){
    *GPFSEL3 = r;
  }else if(pin<50){
    *GPFSEL4 = r;
  }else{
    *GPFSEL5 = r;
  }


  // Disable the pull-up/pull-down control line. We follow the
  // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. We
  // will pull down the pin using an external resistor connected to ground.

  // Disable internal pull-up/pull-down by setting bits 0:1
  // to 00 in the GPIO Pull-Up/Down Register
  *GPPUD = 0x0;

  // Wait 150 cycles to provide the required set-up time
  // for the control signal
  r = 150;
  while (r--) {
      asm volatile("nop");
  }

  // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit pin number
  // to clock in the control signal. Note that all other pins
  // will retain their previous state.
  *GPPUDCLK0 = (0x1 << pin);

  // Wait 150 cycles to provide the required hold time
  // for the control signal
  r = 150;
  while (r--) {
      asm volatile("nop");
  }

  // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
  // in order to remove the clock
  *GPPUDCLK0 = 0;

}


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       get_GPIO
//
//  Arguments:      Pin number to set
//
//  Returns:        none
//
//  Description:    This function sets the pin bit by putting a 1 in the
//                  GPSET0 register at the specified pin number.
//
//  Sources:        Dr. Boyds examples
//
////////////////////////////////////////////////////////////////////////////////
void set_GPIO(int pin){
  *GPSET0 = (0x1 << pin);
}


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       clear_GPIO
//
//  Arguments:      Pin number to clear
//
//  Returns:        none
//
//  Description:    This function clears the pin bit by putting a 1 in the
//                  GPCLR0 register at the specified pin numbe.
//
//  Sources:        Dr. Boyds examples
//
////////////////////////////////////////////////////////////////////////////////

void clear_GPIO(int pin){
  *GPCLR0 = (0x1 << pin);
}
