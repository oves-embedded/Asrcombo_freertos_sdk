/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
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

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include "stdint.h"
    #include <stdio.h>
    #include "lega_rtos_api.h"
#ifdef CFG_PLF_DUET
    #include "duet_cm4.h"
#else
    #include "lega_cm4.h"
#endif
#endif
#if LEGA_DEBUG_MALLOC_TRACE
#define FRESSRTOS_DEBUG_MEMORY_TRACE    1
#endif
#ifdef PERI_TEST
#define vAssertCalled(char,int)    {printf("os assert %s %d\n",char,int); while(1);}
#else
#ifdef CFG_PLF_DUET
extern void assert_wf_err(const char *condition, const char * file, int line);
#define vAssertCalled(char,int)    assert_wf_err("", char, int)
#else
extern void assert_err(const char *condition, const char * file, int line);
#define vAssertCalled(char,int)    assert_err("", char, int)
#endif
#endif
#define configASSERT(x) if((x)==0) vAssertCalled(__FILE__,__LINE__)

/***************************************************************************************************************/
/*                                        FreeRTOS                                             */
/***************************************************************************************************************/
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    1

#if (defined PERI_TEST) || (defined CFG_XTAL_40M)
#define configUSE_TICKLESS_IDLE                    0
#else
#define configUSE_TICKLESS_IDLE                    1
#endif
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP      5
#define configUSE_QUEUE_SETS                    1

#if (defined CFG_PLF_DUET) || (defined HIGHFREQ_MCU160_SUPPORT)
#define configCPU_CLOCK_HZ                        (system_core_clk)
#else
#define configCPU_CLOCK_HZ                        (SYSTEM_CLOCK)
#endif
#define configTICK_RATE_HZ                        (1000)
#define configMAX_PRIORITIES                    (32)
#if FRESSRTOS_DEBUG_MEMORY_TRACE>1
#define configMINIMAL_STACK_SIZE                ((unsigned short)512)//182
#else
#define configMINIMAL_STACK_SIZE                ((unsigned short)512)
#endif
#define configMAX_TASK_NAME_LEN                    (16)

#define configUSE_16_BIT_TICKS                    0

#define configIDLE_SHOULD_YIELD                    1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_MUTEXES                        1
#define configQUEUE_REGISTRY_SIZE                8

#define configCHECK_FOR_STACK_OVERFLOW            2


#define configUSE_RECURSIVE_MUTEXES                1
#define configUSE_MALLOC_FAILED_HOOK            0
#define configUSE_APPLICATION_TASK_TAG            0
#define configUSE_COUNTING_SEMAPHORES            1

/***************************************************************************************************************/
/*                                                                              */
/***************************************************************************************************************/
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_USE_DYNAMIC_HEAP            1
//#define configTOTAL_HEAP_SIZE                    ((size_t)(90*1024))

/***************************************************************************************************************/
/*                                                                            */
/***************************************************************************************************************/
#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        0

/***************************************************************************************************************/
/*                                                              */
/***************************************************************************************************************/
#ifdef CFG_CPU_USAGE
#define configGENERATE_RUN_TIME_STATS           1
#else
#define configGENERATE_RUN_TIME_STATS           0
#endif
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1


/***************************************************************************************************************/
/*                                                                            */
/***************************************************************************************************************/
#define configUSE_CO_ROUTINES                     0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

/***************************************************************************************************************/
/*                                FreeRTOS                                           */
/***************************************************************************************************************/
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY                (configMAX_PRIORITIES-1)
#define configTIMER_QUEUE_LENGTH                5
#define configTIMER_TASK_STACK_DEPTH            (configMINIMAL_STACK_SIZE*2)
#define configRECORD_STACK_HIGH_ADDRESS         1
/***************************************************************************************************************/
/*                                                                                    */
/***************************************************************************************************************/
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet                1
#define INCLUDE_vTaskDelete                        1
#define INCLUDE_vTaskCleanUpResources            1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                    1
#define INCLUDE_vTaskDelay                        1
#define INCLUDE_eTaskGetState                    1
#define INCLUDE_xTimerPendFunctionCall            1

/***************************************************************************************************************/
/*                                                                           */
/***************************************************************************************************************/
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS               __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS               4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY            15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/***************************************************************************************************************/
/*                                                                        */
/***************************************************************************************************************/
#define xPortPendSVHandler     PendSV_Handler
#define vPortSVCHandler     SVC_Handler

#endif /* FREERTOS_CONFIG_H */
