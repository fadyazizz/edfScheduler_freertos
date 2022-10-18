/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"
#include "string.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/
void vApplicationIdleHook( void ){
	
}
void vApplicationTickHook(void){
	
	GPIO_write(PORT_0,PIN0,PIN_IS_HIGH);
	GPIO_write(PORT_0,PIN0,PIN_IS_LOW);
}

volatile int button1RisingEdge=0;
volatile int button1FallingEdge=0;

char button1RisingEdgeString[30]="rising edge button 1 \n";
char button1fallinfEdgeString[30]="falling edge button 1 \n";
void Button_1_Monitor(void* pvParamaters){
	TickType_t xLastWakeTime=xTaskGetTickCount();
	int didEdgeRise=0;
	vTaskSetApplicationTaskTag(NULL, (void *)1);
	
	for( ;; )
    {
     pinState_t button1Value= GPIO_read(PORT_1,PIN0);
			if(button1Value == (pinState_t) 1 && didEdgeRise == 0){
				didEdgeRise=1;
				button1RisingEdge=1;
			}
			if(button1Value == (pinState_t) 0 && didEdgeRise== 1){
				didEdgeRise=0;
				button1FallingEdge=1;
				
			}
			//GPIO_write(PORT_0,PIN1,PIN_IS_LOW);
			vTaskDelayUntil(&xLastWakeTime,50);
			//GPIO_write(PORT_0,PIN1,PIN_IS_HIGH);
		}
}

volatile int button2RisingEdge=0;
volatile int button2FallingEdge=0;

char button2RisingEdgeString[30]="rising edge button 2 \n";
char button2fallinfEdgeString[30]="falling edge button 2 \n";
void Button_2_Monitor(void* pvParamaters){
	TickType_t xLastWakeTime=xTaskGetTickCount();
	int didEdgeRise=0;
	vTaskSetApplicationTaskTag(NULL, (void *)2);
	
	for( ;; )
    {
			
     pinState_t button2Value= GPIO_read(PORT_1,PIN1);
			if(button2Value==1 && didEdgeRise==0){
				didEdgeRise=1;
				button2RisingEdge=1;
			}
			if(button2Value==0 && didEdgeRise==1){
				didEdgeRise=0;
				button2FallingEdge=1;
			}
			//GPIO_write(PORT_0,PIN2,PIN_IS_LOW);
			vTaskDelayUntil(&xLastWakeTime,50);
			//GPIO_write(PORT_0,PIN2,PIN_IS_HIGH);
		}
}

volatile int periodicTransmitFlag=0;

char periodicTransmitString[30]="periodic transmition \n";
void Periodic_Transmitter(void* pvParamaters){
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL, (void *)3);
	for( ;; )
    {
			periodicTransmitFlag=1;
			//GPIO_write(PORT_0,PIN3,PIN_IS_LOW);
			vTaskDelayUntil(&xLastWakeTime,100);
			//GPIO_write(PORT_0,PIN3,PIN_IS_HIGH);
		}
}
void Uart_Receiver(void* pvParamaters){
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL, (void *)4);
	for( ;; )
    {
			if(button1RisingEdge==1){
				button1RisingEdge=0;
				vSerialPutString(button1RisingEdgeString,strlen(button1RisingEdgeString));
			}
			if(button1FallingEdge==1){
				button1FallingEdge=0;
				vSerialPutString(button1fallinfEdgeString,strlen(button1fallinfEdgeString));
			}
			if(button2RisingEdge==1){
				button2RisingEdge=0;
				vSerialPutString(button2RisingEdgeString,strlen(button2RisingEdgeString));
			}
			if(button2FallingEdge==1){
				button2FallingEdge=0;
				vSerialPutString(button2fallinfEdgeString,strlen(button2fallinfEdgeString));
			}
			if(periodicTransmitFlag==1){
				periodicTransmitFlag=0;
				vSerialPutString(periodicTransmitString,strlen(periodicTransmitString));
			}
			
			//GPIO_write(PORT_0,PIN4,PIN_IS_LOW);
			vTaskDelayUntil(&xLastWakeTime,20);
			//GPIO_write(PORT_0,PIN4,PIN_IS_HIGH);
		}
}

void Load_1_Simulation(void* pvParamaters){
	int i=0;
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL, (void *)5);
	for(;;){
		for(i=0;i<33333;i++){
			i=i;
		}
		//GPIO_write(PORT_0,PIN5,PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime,10);
		//GPIO_write(PORT_0,PIN5,PIN_IS_HIGH);
		
	}
}

void Load_2_Simulation(void* pvParamaters){
	int i=0;
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL, (void *)6);
	for(;;){
		for(i=0;i<80000;i++){
			i=i;
		}
		//GPIO_write(PORT_0,PIN6,PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime,100);
		//GPIO_write(PORT_0,PIN6,PIN_IS_HIGH);
		
	}
}
/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */

TaskHandle_t button1Handler=NULL;
TaskHandle_t button2Handler=NULL;
TaskHandle_t periodicTransimitterHandler=NULL;
TaskHandle_t uartHandler=NULL;
TaskHandle_t loadSimulation1=NULL;
TaskHandle_t loadSimulation2=NULL;
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
    /* Create Tasks here */
	xTaskPeriodicCreate(
                    Button_1_Monitor,       /* Function that implements the task. */
                    "Button_1_Monitor",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &button1Handler,
										50); 
	xTaskPeriodicCreate(
                    Button_2_Monitor,       /* Function that implements the task. */
                    "Button_2_Monitor",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &button2Handler,
										50);
	xTaskPeriodicCreate(
                    Periodic_Transmitter,       /* Function that implements the task. */
                    "Periodic_Transmitter",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &periodicTransimitterHandler,
										100);
	xTaskPeriodicCreate(
                    Uart_Receiver,       /* Function that implements the task. */
                    "Uart_Receiver",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &uartHandler,
										20);
	xTaskPeriodicCreate(
                    Load_1_Simulation,       /* Function that implements the task. */
                    "Load_1_Simulation",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &loadSimulation1,
										10);
	xTaskPeriodicCreate(
                    Load_2_Simulation,       /* Function that implements the task. */
                    "Load_2_Simulation",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &loadSimulation2,
										100);
	
	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


