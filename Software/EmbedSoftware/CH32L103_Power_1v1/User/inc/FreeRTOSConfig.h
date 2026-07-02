/*
    FreeRTOS V202112.00
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "ch32l103.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* See https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html */

/* don't have MTIME */
#define configMTIME_BASE_ADDRESS 	             ( 0 )
#define configMTIMECMP_BASE_ADDRESS              ( 0 )

/******************************************************************************/
/* Hook and callback function related definitions. ****************************/
/******************************************************************************/

/* Set the following configUSE_* constants to 1 to include the named hook
 * functionality in the build.  Set to 0 to exclude the hook functionality from
 * the build.  The application writer is responsible for providing the hook
 * function for any set to 1.  See https://www.freertos.org/a00016.html. */
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configUSE_MALLOC_FAILED_HOOK             1
#define configUSE_DAEMON_TASK_STARTUP_HOOK       0

/* Set configUSE_SB_COMPLETED_CALLBACK to 1 to have send and receive completed
 * callbacks for each instance of a stream buffer or message buffer. When the
 * option is set to 1, APIs xStreamBufferCreateWithCallback() and
 * xStreamBufferCreateStaticWithCallback() (and likewise APIs for message
 * buffer) can be used to create a stream buffer or message buffer instance
 * with application provided callbacks. Defaults to 0 if left undefined. */
#define configUSE_SB_COMPLETED_CALLBACK          0

/* Set configCHECK_FOR_STACK_OVERFLOW to 1 or 2 for FreeRTOS to check for a
 * stack overflow at the time of a context switch.  Set to 0 to not look for a
 * stack overflow.  If configCHECK_FOR_STACK_OVERFLOW is 1 then the check only
 * looks for the stack pointer being out of bounds when a task's context is
 * saved to its stack - this is fast but somewhat ineffective.  If
 * configCHECK_FOR_STACK_OVERFLOW is 2 then the check looks for a pattern
 * written to the end of a task's stack having been overwritten.  This is
 * slower, but will catch most (but not all) stack overflows.  The application
 * writer must provide the stack overflow callback when
 * configCHECK_FOR_STACK_OVERFLOW is set to 1. See
 * https://www.freertos.org/Stacks-and-stack-overflow-checking.html  Defaults to
 * 0 if left undefined. */
#define configCHECK_FOR_STACK_OVERFLOW           0

/******************************************************************************/
/* Hardware description related definitions. **********************************/
/******************************************************************************/

/* In most cases, configCPU_CLOCK_HZ must be set to the frequency of the clock
 * that drives the peripheral used to generate the kernels periodic tick
 * interrupt. The default value is set to 200MHz and matches the QEMU demo
 * settings.  Your application will certainly need a different value so set this
 * correctly. This is very often, but not always, equal to the main system clock
 * frequency. */
#define configCPU_CLOCK_HZ				          SystemCoreClock

/******************************************************************************/
/* Scheduling behaviour related definitions. **********************************/
/******************************************************************************/

/* configTICK_RATE_HZ sets frequency of the tick interrupt in Hz, normally
 * calculated from the configCPU_CLOCK_HZ value. */
#define configTICK_RATE_HZ				           ( ( TickType_t ) 1000 )

/* Set configUSE_PREEMPTION to 1 to use pre-emptive scheduling.  Set
 * configUSE_PREEMPTION to 0 to use co-operative scheduling.
 * See https://www.freertos.org/single-core-amp-smp-rtos-scheduling.html. */
#define configUSE_PREEMPTION			           1

/* Set configUSE_TIME_SLICING to 1 to have the scheduler switch between Ready
 * state tasks of equal priority on every tick interrupt.  Set
 * configUSE_TIME_SLICING to 0 to prevent the scheduler switching between Ready
 * state tasks just because there was a tick interrupt.  See
 * https://freertos.org/single-core-amp-smp-rtos-scheduling.html. */
#define configUSE_TIME_SLICING                     1

/* Set configUSE_PORT_OPTIMISED_TASK_SELECTION to 1 to select the next task to
 * run using an algorithm optimised to the instruction set of the target
 * hardware - normally using a count leading zeros assembly instruction.  Set to
 * 0 to select the next task to run using a generic C algorithm that works for
 * all FreeRTOS ports.  Not all FreeRTOS ports have this option.  Defaults to 0
 * if left undefined. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    0

/* configMAX_PRIORITIES Sets the number of available task priorities.  Tasks can
 * be assigned priorities of 0 to (configMAX_PRIORITIES - 1).  Zero is the
 * lowest priority. */
#define configMAX_PRIORITIES			           ( 4 )

/* configMINIMAL_STACK_SIZE defines the size of the stack used by the Idle task
 * (in words, not in bytes!).  The kernel does not use this constant for any
 * other purpose.  Demo applications use the constant to make the demos somewhat
 * portable across hardware architectures. */
#define configMINIMAL_STACK_SIZE		           ( 128 )  /*  128*4Byte = 512Byte */

/* configMAX_TASK_NAME_LEN sets the maximum length (in characters) of a task's
 * human readable name.  Includes the NULL terminator. */
 #define configMAX_TASK_NAME_LEN			       ( 16 )

 /* Time is measured in 'ticks' - which is the number of times the tick interrupt
 * has executed since the RTOS kernel was started.
 * The tick count is held in a variable of type TickType_t.
 *
 * configTICK_TYPE_WIDTH_IN_BITS controls the type (and therefore bit-width) of
 * TickType_t:
 *
 * Defining configTICK_TYPE_WIDTH_IN_BITS as TICK_TYPE_WIDTH_16_BITS causes
 * TickType_t to be defined (typedef'ed) as an unsigned 16-bit type.
 *
 * Defining configTICK_TYPE_WIDTH_IN_BITS as TICK_TYPE_WIDTH_32_BITS causes
 * TickType_t to be defined (typedef'ed) as an unsigned 32-bit type.
 *
 * Defining configTICK_TYPE_WIDTH_IN_BITS as TICK_TYPE_WIDTH_64_BITS causes
 * TickType_t to be defined (typedef'ed) as an unsigned 64-bit type. */
//#define configTICK_TYPE_WIDTH_IN_BITS              TICK_TYPE_WIDTH_32_BITS

#define configUSE_16_BIT_TICKS			           0

/* Set configSUPPORT_STATIC_ALLOCATION to 1 to include FreeRTOS API functions
 * that create FreeRTOS objects (tasks, queues, etc.) using statically allocated
 * memory in the build.  Set to 0 to exclude the ability to create statically
 * allocated objects from the build.  Defaults to 0 if left undefined.  See
 * https://www.freertos.org/Static_Vs_Dynamic_Memory_Allocation.html. */
#define configSUPPORT_STATIC_ALLOCATION            0

/* Sets the total size of the FreeRTOS heap, in bytes, when heap_1.c, heap_2.c
 * or heap_4.c are included in the build.  This value is defaulted to 4096 bytes
 * but it must be tailored to each application.  Note the heap will appear in
 * the .bss section.  See https://www.freertos.org/a00111.html. */
#define configTOTAL_HEAP_SIZE			           ( ( size_t ) ( 8 * 1024 ) )

/* Set configIDLE_SHOULD_YIELD to 1 to have the Idle task yield to an
 * application task if there is an Idle priority (priority 0) application task
 * that can run.  Set to 0 to have the Idle task use all of its timeslice.
 * Default to 1 if left undefined. */
#define configIDLE_SHOULD_YIELD			           0

/* configQUEUE_REGISTRY_SIZE sets the maximum number of queues and semaphores
 * that can be referenced from the queue registry.  Only required when using a
 * kernel aware debugger.  Defaults to 0 if left undefined. */
#define configQUEUE_REGISTRY_SIZE                  8

/******************************************************************************/
/* Run time and task stats gathering related definitions. *********************/
/******************************************************************************/

/* Set configGENERATE_RUN_TIME_STATS to 1 to have FreeRTOS collect data on the
 * processing time used by each task.  Set to 0 to not collect the data.  The
 * application writer needs to provide a clock source if set to 1.  Defaults to
 * 0 if left undefined.  See https://www.freertos.org/rtos-run-time-stats.html.
 */
#define configGENERATE_RUN_TIME_STATS              0

/* Set configUSE_TRACE_FACILITY to include additional task structure members
 * are used by trace and visualisation functions and tools.  Set to 0 to exclude
 * the additional information from the structures. Defaults to 0 if left
 * undefined. */
#define configUSE_TRACE_FACILITY		           0

/******************************************************************************/
/* Definitions that include or exclude functionality. *************************/
/******************************************************************************/

/* Set the following configUSE_* constants to 1 to include the named feature in
 * the build, or 0 to exclude the named feature from the build. */
#define configUSE_MUTEXES				           1
#define configUSE_RECURSIVE_MUTEXES		           1
#define configUSE_COUNTING_SEMAPHORES	           1
#define configUSE_QUEUE_SETS                       0
#define configUSE_APPLICATION_TASK_TAG	           0

/******************************************************************************/
/* Co-routine related definitions. ********************************************/
/******************************************************************************/

/* Set configUSE_CO_ROUTINES to 1 to include co-routine functionality in the
 * build, or 0 to omit co-routine functionality from the build. To include
 * co-routines, croutine.c must be included in the project. Defaults to 0 if
 * left undefined. */
#define configUSE_CO_ROUTINES                      0

/* configMAX_CO_ROUTINE_PRIORITIES defines the number of priorities available
 * to the application co-routines. Any number of co-routines can share the same
 * priority. Defaults to 0 if left undefined. */
#define configMAX_CO_ROUTINE_PRIORITIES            (2)

/******************************************************************************/
/* Software timer related definitions. ****************************************/
/******************************************************************************/

/* Set configUSE_TIMERS to 1 to include software timer functionality in the
 * build.  Set to 0 to exclude software timer functionality from the build.  The
 * FreeRTOS/source/timers.c source file must be included in the build if
 * configUSE_TIMERS is set to 1.  Default to 0 if left undefined.  See
 * https://www.freertos.org/RTOS-software-timer.html. */
#define configUSE_TIMERS                            0

/* configTIMER_TASK_PRIORITY sets the priority used by the timer task.  Only
 * used if configUSE_TIMERS is set to 1.  The timer task is a standard FreeRTOS
 * task, so its priority is set like any other task.  See
 * https://www.freertos.org/RTOS-software-timer-service-daemon-task.html  Only
 * used if configUSE_TIMERS is set to 1. */
#define configTIMER_TASK_PRIORITY                   ( configMAX_PRIORITIES - 1 )

/* configTIMER_TASK_STACK_DEPTH sets the size of the stack allocated to the
 * timer task (in words, not in bytes!).  The timer task is a standard FreeRTOS
 * task.  See
 * https://www.freertos.org/RTOS-software-timer-service-daemon-task.html Only
 * used if configUSE_TIMERS is set to 1. */
#define configTIMER_TASK_STACK_DEPTH                configMINIMAL_STACK_SIZE

/* configTIMER_QUEUE_LENGTH sets the length of the queue (the number of discrete
 * items the queue can hold) used to send commands to the timer task.  See
 * https://www.freertos.org/RTOS-software-timer-service-daemon-task.html  Only
 * used if configUSE_TIMERS is set to 1. */
#define configTIMER_QUEUE_LENGTH                    8

/* Set the following INCLUDE_* constants to 1 to include the named API function,
 * or 0 to exclude the named API function.  Most linkers will remove unused
 * functions even when the constant is 1. */
#define INCLUDE_vTaskPrioritySet			        1
#define INCLUDE_uxTaskPriorityGet			        1
#define INCLUDE_vTaskDelete					        1
#define INCLUDE_vTaskCleanUpResources		        1
#define INCLUDE_vTaskSuspend				        1
#define INCLUDE_vTaskDelayUntil				        1
#define INCLUDE_vTaskDelay					        1
#define INCLUDE_eTaskGetState				        0
#define INCLUDE_xTimerPendFunctionCall		        0
#define INCLUDE_xTaskAbortDelay				        0
#define INCLUDE_xTaskGetHandle				        0
#define INCLUDE_xSemaphoreGetMutexHolder	        1
#define INCLUDE_xTaskGetSchedulerState              1

#if configUSE_FPU == 1
	/* Include the header that define the traceTASK_SWITCHED_IN() and
	traceTASK_SWITCHED_OUT() macros to save and restore the floating
	point registers for tasks that have requested this behaviour. */
	#include "FPU_Macros.h"
#endif

#endif /* FREERTOS_CONFIG_H */
