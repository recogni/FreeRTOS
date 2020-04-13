/*
 * FreeRTOS Kernel V10.3.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/******************************************************************************
 * NOTE 1:  This project provides two demo applications.  A simple blinky
 * style project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky style version.
 *
 * NOTE 2:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, and functions
 * required to configure the hardware are defined in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 1000 milliseconds, before sending the value 100 to the queue that
 * was created within main_blinky().  Once the value is sent, the task loops
 * back around to block for another 1000 milliseconds...and so on.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * main_blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, writes 'Blink' to the UART
 * (the UART is used in place of the LED to allow easy execution in QEMU).  The
 * 'block time' parameter passed to the queue receive function specifies that
 * the task should be held in the Blocked state indefinitely to wait for data to
 * be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 1000 milliseconds, the queue receive
 * task leaves the Blocked state every 1000 milliseconds, and therefore toggles
 * the LED every 200 milliseconds.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Priorities used by the tasks. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 1000 )

/* The maximum number items the queue can hold.  The priority of the receiving
task is above the priority of the sending task, so the receiving task will
preempt the sending task and remove the queue items each time the sending task
writes to the queue.  Therefore the queue will never have more than one item in
it at any time, and even with a queue length of 1, the sending task will never
find the queue full. */
#define mainQUEUE_LENGTH					( 1 )

/* API for simple UART debug output */
void dbg_puts(const char *msg);
void dbg_uart_init(void);
/*-----------------------------------------------------------*/

/*
 * Called by main when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1 in
 * main.c.
 */
void main_blinky( void );

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/*-----------------------------------------------------------*/

void main_blinky( void )
{

    dbg_uart_init();
    dbg_puts("Hello From FreeRTOS\n");
    //puts("Hello\n");
	/* Create the queue. */
    dbg_puts("XQueueCreate\n");
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

	if( xQueue != NULL )
	{
		/* Start the two tasks as described in the comments at the top of this
		file. */
        dbg_puts("Create prvQueueReceiveTask\n");
		xTaskCreate( prvQueueReceiveTask,				/* The function that implements the task. */
					"Rx", 								/* The text name assigned to the task - for debug only as it is not used by the kernel. */
					configMINIMAL_STACK_SIZE * 2U, 		/* The size of the stack to allocate to the task. */
					NULL, 								/* The parameter passed to the task - not used in this case. */
					mainQUEUE_RECEIVE_TASK_PRIORITY, 	/* The priority assigned to the task. */
					NULL );								/* The task handle is not required, so NULL is passed. */

        dbg_puts("Create prvQueueSendTask\n");
		xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE * 2U, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

		/* Start the tasks and timer running. */
        dbg_puts("StartScheduler\n");
		vTaskStartScheduler();
	}

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const unsigned long ulValueToSend = 100UL;
BaseType_t xReturned;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

    dbg_puts("Enter SendTask\n");
	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
        dbg_puts("SendTask Loop\n");
		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xReturned = xQueueSend( xQueue, &ulValueToSend, 0U );
		configASSERT( xReturned == pdPASS );
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
unsigned long ulReceivedValue;
const unsigned long ulExpectedValue = 100UL;
const char * const pcPassMessage = "Blink\r\n";
const char * const pcFailMessage = "Unexpected value received\r\n";
extern void vSendString( const char * const pcString );
extern void vToggleLED( void );

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
    dbg_puts("Enter SendTask\n");

	for( ;; )
	{
        dbg_puts("SendTask Loop\n");
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */
		if( ulReceivedValue == ulExpectedValue )
		{
			vSendString( pcPassMessage );
			vToggleLED();
			ulReceivedValue = 0U;
		}
		else
		{
			vSendString( pcFailMessage );
		}
	}
}
/*-----------------------------------------------------------*/


/*
 * Simple 16550 UART output driver.
 */
#define UART_BASE 0x10000000
#define N_UART_REG_QUEUE     UART_BASE + (0*4)    // rx/tx fifo data
#define N_UART_REG_DLL       UART_BASE + (0*4)    // divisor latch (LSB)
#define N_UART_REG_IER       UART_BASE + (1*4)    // interrupt enable register
#define N_UART_REG_DLM       UART_BASE + (1*4)    // divisor latch (MSB)
#define N_UART_REG_FCR       UART_BASE + (2*4)    // fifo control register
#define N_UART_REG_LCR       UART_BASE + (3*4)    // line control register
#define N_UART_REG_MCR       UART_BASE + (4*4)    // modem control register
#define N_UART_REG_LSR       UART_BASE + (5*4)    // line status register
#define N_UART_REG_MSR       UART_BASE + (6*4)    // modem status register
#define N_UART_REG_SCR       UART_BASE + (7*4)    // scratch register

#define UART_DEFAULT_BAUD	115200
#define UART_REG_STATUS_RX 0x01     /* Data is ready */
#define UART_REG_STATUS_TX 0x20     /* THR is empty and can accept data */

static void write_reg_u8(uintptr_t addr, uint8_t value)
{
    volatile uint8_t *loc_addr = (volatile uint8_t *)addr;
    *loc_addr = value;
}

static uint8_t read_reg_u8(uintptr_t addr)
{
    return *(volatile uint8_t *)addr;
}

static void dbg_serial_putc(const char c)
{
  if (c == '\n')
      dbg_serial_putc('\r');

  while ((read_reg_u8(N_UART_REG_LSR) & UART_REG_STATUS_TX) == 0) {
  }
  write_reg_u8(N_UART_REG_QUEUE, c);
}

void dbg_puts(const char *msg)
{
    while (*msg) {
        dbg_serial_putc(*msg++);
    }
}

void dbg_uart_init(void)
{
  static uint32_t uart16550_clock = 50000000;
  uint32_t divisor = uart16550_clock / (16 * UART_DEFAULT_BAUD);

  write_reg_u8(N_UART_REG_IER , 0x00);                // Disable all interrupts
  write_reg_u8(N_UART_REG_LCR , 0x80);                // Enable DLAB (set baud rate divisor)
  write_reg_u8(N_UART_REG_DLL , (uint8_t)divisor);    // Set divisor (lo byte)
  write_reg_u8(N_UART_REG_DLM , (uint8_t)(divisor >> 8));     //     (hi byte)
  write_reg_u8(N_UART_REG_LCR , 0x03);                // 8 bits, no parity, one stop bit
  write_reg_u8(N_UART_REG_FCR , 0x06);                // Enable FIFO, clear them, with 14-byte threshold
  write_reg_u8(N_UART_REG_MCR , 0x2);                // Enable RTS
}

