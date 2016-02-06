/*
 * task.c
 *
 *  Created on: Sep 27, 2015
 *      Author: Jason
 */
 
#include "task.h"
#include "lib_430fr.h"

#define MAX_NUM_OF_TASKS		16
#define MAX_MILLISECONDS_VAL	0xffffffff

static volatile uint32_t milliSeconds = 0;

/* create structure that consists of a function pointer and period */
typedef struct {
	void (*taskFunctPtr)();
	uint32_t period;
	uint32_t nextExecutionTime;
}Task;

static Task task[MAX_NUM_OF_TASKS];

void TASK_milliSecCounter();	// function declaration

void TASK_milliSecCounter(){
	milliSeconds++;

	/* gracefully reset the microseconds counter
	 * if it is about to roll over */
	if(milliSeconds >= 0x7ff00000){
		TASK_resetTime(0);
	}
}

uint32_t TASK_getTime(){
    uint8_t sameFlag = 0;
    uint32_t now0 = milliSeconds;
    uint32_t now1 = milliSeconds;
    
    /* this code ensures that two sequential reads get the same value
     * before returning in order to avoid problems with atomic reads
     * of the milliSeconds location */
    while(sameFlag == 0){
        now0 = milliSeconds;
        now1 = milliSeconds;
        
        if(now0 == now1){
            sameFlag = 1;
        }
    }
    
	return now0;
}

void TASK_resetTime(uint32_t time){
    uint32_t now = TASK_getTime();
	if(time != now){
		uint8_t i = 0;

		/* determine the amount of time before each task needs to execute again */
		int32_t timeUntilNextExecution[MAX_NUM_OF_TASKS];
		for(i = 0; i < MAX_NUM_OF_TASKS; i++){
			timeUntilNextExecution[i] = (int32_t)task[i].nextExecutionTime - now;
			if(timeUntilNextExecution[i] < 0)
				timeUntilNextExecution[i] = 0;
		}

        /* reset the clock */
        TA0_disableInterrupt();
		milliSeconds = time;
        TA0_enableInterrupt(&TASK_milliSecCounter);

		/* place the difference between the time that each task needed to execute and the new time */
		for(i = 0; i < MAX_NUM_OF_TASKS; i++){
			task[i].nextExecutionTime = (uint32_t)timeUntilNextExecution[i] + time;
		}
	}
}

void TASK_init(){
	TA0_init();
    TA0_enableInterrupt(&TASK_milliSecCounter);
   	TA0_setPeriod(1000);
    TA0_run();

    /* initialize all of the tasks */
    uint16_t i;
    for(i = 0; i < MAX_NUM_OF_TASKS; i++){
    	task[i].taskFunctPtr = 0;
    	task[i].period = 1;
    	task[i].nextExecutionTime = 1;
    }
}

void TASK_add(void (*functPtr)(), uint32_t period){
	uint16_t i;
	uint8_t taskExists = 0;

	/* ensure that the task does not currently already exist with the same period */
	for(i = 0; i < MAX_NUM_OF_TASKS; i++){
		if(task[i].taskFunctPtr == functPtr){
			if(task[i].period != period){
				task[i].period = period;
				task[i].nextExecutionTime = milliSeconds + period;
			}

			taskExists = 1;
		}
	}

	/* save the task */
	if(taskExists == 0){
		for(i = 0; i < MAX_NUM_OF_TASKS; i++){
			/* look for an empty task */
			if(task[i].taskFunctPtr == 0){
				task[i].taskFunctPtr = functPtr;
				task[i].period = period;
				task[i].nextExecutionTime = milliSeconds + period;

				break;
			}
		}
	}
}

void TASK_remove(void (*functPtr)()){
	uint16_t i;

	for(i = 0; i < MAX_NUM_OF_TASKS; i++){
		if(task[i].taskFunctPtr == functPtr){
			task[i].taskFunctPtr = 0;
			task[i].period = 10000;
			task[i].nextExecutionTime = MAX_MILLISECONDS_VAL;
		}
	}
}

void TASK_manage(){
	#if (!defined(__MSP430FR5989__) && !defined(__MSP430FR5889__))
	/* in linux, use threads to accomplish the 
     * equivalent of the above timer interrupt */
    pthread_t milliSecondTimerThread;	// declare the thread
    if(pthread_create(&milliSecondTimerThread, NULL, TASK_milliSecCounter, NULL)){
		printf("Error creating thread...\n");
	}
	#endif
	
	while(1){
		uint16_t i;
		for(i = 0; i < MAX_NUM_OF_TASKS; i++){
			uint32_t time = TASK_getTime();
			if(task[i].taskFunctPtr != 0){
				if(time >= task[i].nextExecutionTime){
					task[i].nextExecutionTime = task[i].period + time;
					(task[i].taskFunctPtr)();
				}
			}
		}
		
		#if (!defined(__MSP430FR5989__) && !defined(__MSP430FR5889__))
		usleep(500);
		#endif
	}
}
