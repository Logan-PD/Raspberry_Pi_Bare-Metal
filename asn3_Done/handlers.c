// This file contains C functions to handle particular kinds of exceptions.
// Only a function to handle IRQ exceptions is currently implemented.

// Header files
#include "uart.h"
#include "gpio.h"
#include "irq.h"
#include "sysreg.h"

// Reference to the global shared value
extern unsigned int sharedValue;



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       IRQ_handler
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    Handles interrupts from IRQ exceptions, specifically GPIO
//                  interrupts. First checks if interupts is a GPIO interrupt
//                  Then checks if it is cause by pin 23 or 22, then Handles
//                  the interrupt by changing the sharedValue accordingly.
//
//  Sources:        The example code 'MinimalInterruptExample' was used
//                  to write this method.
//
//
////////////////////////////////////////////////////////////////////////////////

void IRQ_handler(){

  uart_puts("\nInside IRQ exception handler:\n");

  // Handle GPIO interrupts in general
  //check if interrupt is a gpio interrupts. i.e. pin 20
  if (*IRQ_PENDING_2 == (0x1 << 20)) {

    // Handle the interrupt associated with GPIO pin 23
    // now that now gpio pin is causing interrupt,
    // check which pin is causing interrupt
    if (*GPEDS0 == (0x1 << 23)) {

      uart_puts("Inside 23\n");

      // Clear the interrupt by writing a 1 to the GPIO Event Detect
      // Status Register at bit 23 (p. 96 of the Broadcom Manual)
      *GPEDS0 = (0x1 << 23);

      // Handle the interrupt:
      // If the sharedValue is 0, change it to 1, else do nothing
      if (sharedValue == 0){
        sharedValue = 1;
      }

    // Handle the interrupt associated with GPIO pin 22
    }else if (*GPEDS0 == (0x1 << 22)) {

      uart_puts("Inside 22\n");

      // Clear the interrupt by writing a 1 to the GPIO Event Detect
      // Status Register at bit 22 (p. 96 of the Broadcom Manual)
      *GPEDS0 = (0x1 << 22);

      // Handle the interrupt:
      // If the sharedValue is 1, change it to 0, else do nothing
      if (sharedValue == 1){
        sharedValue = 0;
      }
    }
  }

    // Return to the IRQ exception handler stub
    uart_puts("exit handler\n");
    return;
}
