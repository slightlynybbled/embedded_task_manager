You may find more detailed explanation at http://www.forembed.com/the-not-quite-rtos-a-task-manager/

# Purpose #

This project is to illustrate a simple task manager application that you should be able to easily drop into any 8-bit or 16-bit microcontroller.  This is NOT a full RTOS, just a simple helper that will allow multiple 'tasks' to easily be added and removed from the execution que.

# How to Use #

When I am starting with a new processor, I operate in this order:

1. Get the oscillator going at the correct frequency
2. Get the task manager going on the timer interrupt
3. Toggle a pin using the task manager
4. Measure the frequency on an oscilloscope.  If it isn't the expected frequency, go back to step 1.

## Create a Timer Interrupt ##

You must create a timer interrupt on your microcontroller for the system tick.  A template has been provided in the form of 'lib430.h' and 'lib430.c' using function pointers.

There is nothing keeping you from just putting microcontroller interrupt code directly in the 'task.c' library, but doing so directly makes the code much less portable.  I have successfully used this same code on Microchip products, TI products, and AVRs.  The only function that I had to re-code each time was 'TMR_init()' in order to get this task manager working.

## Adjusting the Task Manager ##

Near the top of 'task.c', there is a define called 'MAX_NUM_OF_TASKS'.  You should set this to its lowest expected value, especially if you are working on a low-RAM processor.  As coded, it takes 11 bytes of RAM for each task.  This can be quite expensive on smaller processors.

## Demo Application ##

The demo application is meant for operation on the MSP430 launchpad (the original), though it should be pretty portable.

When the application comes up, LED1 is blinking.  When S2 is pressed, LED1 stops blinking and LED2 starts blinking at a different frequency.  This is all done with the task manager.
