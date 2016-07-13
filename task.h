/*
 * task.h
 *
 *  Created on: Sep 27, 2015
 *      Author: Jason
 */

#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>

/**
 * TASK_init - Initialize the task manager - call this after the oscillator
 * has been initialized, but before any tasks are added.
 */
void TASK_init();

/**
 * TASK_add - Add a task to the execution que at a specified interval in ms.
 *
 * parameters:
 *  period - the execution period in milliseconds 
 *  functPtr - the function address of the function to be executed
 */
void TASK_add(void (*functPtr)(), uint32_t period);

/**
 * TASK_remove - Remove a task from the execution que.
 *
 * parameters:
 *  functPtr - the function address of the function to be removed
 */
void TASK_remove(void (*functPtr)());

/**
 * TASK_manage - Manages the tasks.  This is a function that DOES NOT return!  It
 * executes within an infinite while loop.
 */
void TASK_manage();

/**
 * TASK_getTime - Returns the system time in milliseconds.
 *
 * returns:
 *  the system time, in milliseconds
 */
uint32_t TASK_getTime();

/**
 * TASK_resetTime - resets the time to a particular value.  This function is useful
 * when synchronization between devices is necessary.
 *
 * parameters:
 *  time - the time to which the system time will be reset
 */
void TASK_resetTime(uint32_t time);

#endif /* TASK_H_ */
