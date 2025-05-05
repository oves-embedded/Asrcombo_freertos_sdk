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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM4F port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#if configUSE_TICKLESS_IDLE == 1
#include "pmu.h"
#endif

#ifndef __VFP_FP__
    #error This port can only be used when the project options are configured to enable hardware floating point support.
#endif

#ifndef configSYSTICK_CLOCK_HZ
    #define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
    /* Ensure the SysTick is clocked at the same frequency as the core. */
    #define portNVIC_SYSTICK_CLK_BIT    ( 1UL << 2UL )
#else
    /* The way the SysTick is clocked is not modified in case it is not the same
    as the core. */
    #define portNVIC_SYSTICK_CLK_BIT    ( 0 )
#endif

/* Constants required to manipulate the core.  Registers first... */
#define portNVIC_SYSTICK_CTRL_REG            ( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG            ( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG    ( * ( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SYSPRI2_REG                ( * ( ( volatile uint32_t * ) 0xe000ed20 ) )
/* ...then bits in the registers. */
#define portNVIC_SYSTICK_INT_BIT            ( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT            ( 1UL << 0UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT        ( 1UL << 16UL )
#define portNVIC_PENDSVCLEAR_BIT             ( 1UL << 27UL )
#define portNVIC_PEND_SYSTICK_CLEAR_BIT        ( 1UL << 25UL )

/* Constants used to detect a Cortex-M7 r0p1 core, which should use the ARM_CM7
r0p1 port. */
#define portCPUID                            ( * ( ( volatile uint32_t * ) 0xE000ed00 ) )
#define portCORTEX_M7_r0p1_ID                ( 0x410FC271UL )
#define portCORTEX_M7_r0p0_ID                ( 0x410FC270UL )

#define portNVIC_PENDSV_PRI                    ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 16UL )
#define portNVIC_SYSTICK_PRI                ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )

/* Constants required to check the validity of an interrupt priority. */
#define portFIRST_USER_INTERRUPT_NUMBER        ( 16 )
#define portNVIC_IP_REGISTERS_OFFSET_16     ( 0xE000E3F0 )
#define portAIRCR_REG                        ( * ( ( volatile uint32_t * ) 0xE000ED0C ) )
#define portMAX_8_BIT_VALUE                    ( ( uint8_t ) 0xff )
#define portTOP_BIT_OF_BYTE                    ( ( uint8_t ) 0x80 )
#define portMAX_PRIGROUP_BITS                ( ( uint8_t ) 7 )
#define portPRIORITY_GROUP_MASK                ( 0x07UL << 8UL )
#define portPRIGROUP_SHIFT                    ( 8UL )

/* Masks off all bits but the VECTACTIVE bits in the ICSR register. */
#define portVECTACTIVE_MASK                    ( 0xFFUL )

/* Constants required to manipulate the VFP. */
#define portFPCCR                            ( ( volatile uint32_t * ) 0xe000ef34 ) /* Floating point context control register. */
#define portASPEN_AND_LSPEN_BITS            ( 0x3UL << 30UL )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR                    ( 0x01000000 )
#define portINITIAL_EXEC_RETURN                ( 0xfffffffd )

/* The systick is a 24-bit counter. */
#define portMAX_24_BIT_NUMBER                ( 0xffffffUL )

/* For strict compliance with the Cortex-M spec the task start address should
have bit-0 clear, as it is loaded into the PC on exit from an ISR. */
#define portSTART_ADDRESS_MASK        ( ( StackType_t ) 0xfffffffeUL )

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR            ( 45UL )

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
    #define portTASK_RETURN_ADDRESS    configTASK_RETURN_ADDRESS
#else
    #define portTASK_RETURN_ADDRESS    prvTaskExitError
#endif

/* Each task maintains its own interrupt status in the critical nesting
variable. */
//static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;
static UBaseType_t uxCriticalNesting = 0x0;

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void ) __attribute__ (( naked ));
void xPortSysTickHandler( void );
void vPortSVCHandler( void ) __attribute__ (( naked ));

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void prvPortStartFirstTask( void ) __attribute__ (( naked ));

/*
 * Function to enable the VFP.
 */
static void vPortEnableVFP( void ) __attribute__ (( naked ));

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/*
 * The number of SysTick increments that make up one tick period.
 */
#if configUSE_TICKLESS_IDLE
    static uint32_t ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if configUSE_TICKLESS_IDLE
    static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#if configUSE_TICKLESS_IDLE
    static uint32_t ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Used by the portASSERT_IF_INTERRUPT_PRIORITY_INVALID() macro to ensure
 * FreeRTOS API functions are not called from interrupts that have been assigned
 * a priority above configMAX_SYSCALL_INTERRUPT_PRIORITY.
 */
#if ( configASSERT_DEFINED == 1 )
     static uint8_t ucMaxSysCallPriority = 0;
     static uint32_t ulMaxPRIGROUPValue = 0;
     static const volatile uint8_t * const pcInterruptPriorityRegisters = ( const volatile uint8_t * const ) portNVIC_IP_REGISTERS_OFFSET_16;
#endif /* configASSERT_DEFINED */

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
unsigned long task_return_address;
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    /* Simulate the stack frame as it would be created by a context switch
    interrupt. */

    /* Offset added to account for the way the MCU uses the stack on entry/exit
    of interrupts, and to ensure alignment. */
    pxTopOfStack--;

    *pxTopOfStack = portINITIAL_XPSR;    /* xPSR */
    pxTopOfStack--;
    *pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;    /* PC */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) portTASK_RETURN_ADDRESS;    /* LR */
    task_return_address = ( StackType_t ) portTASK_RETURN_ADDRESS;

    /* Save code space by skipping register initialisation. */
    pxTopOfStack -= 5;    /* R12, R3, R2 and R1. */
    *pxTopOfStack = ( StackType_t ) pvParameters;    /* R0 */

    /* A save method is being used that requires each task to maintain its
    own exec return value. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_EXEC_RETURN;

    pxTopOfStack -= 8;    /* R11, R10, R9, R8, R7, R6, R5 and R4. */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
    /* A function that implements a task must not exit or attempt to return to
    its caller as there is nothing to return to.  If a task wants to exit it
    should instead call vTaskDelete( NULL ).

    Artificially force an assert() to be triggered if configASSERT() is
    defined, then stop here so application writers can catch the error. */
    configASSERT( uxCriticalNesting == ~0UL );
    portDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vPortSVCHandler( void )
{
    __asm volatile (
                    "    ldr    r3, pxCurrentTCBConst2        \n" /* Restore the context. */
                    "    ldr r1, [r3]                    \n" /* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
                    "    ldr r0, [r1]                    \n" /* The first item in pxCurrentTCB is the task top of stack. */
                    "    ldmia r0!, {r4-r11, r14}        \n" /* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
                    "    msr psp, r0                        \n" /* Restore the task stack pointer. */
                    "    isb                                \n"
                    "    mov r0, #0                         \n"
                    "    msr    basepri, r0                    \n"
                    "    bx r14                            \n"
                    "                                    \n"
                    "    .align 4                        \n"
                    "pxCurrentTCBConst2: .word pxCurrentTCB                \n"
                );
}
/*-----------------------------------------------------------*/

static void prvPortStartFirstTask( void )
{
    __asm volatile(
                    " ldr r0, =0xE000ED08     \n" /* Use the NVIC offset register to locate the stack. */
                    " ldr r0, [r0]             \n"
                    " ldr r0, [r0]             \n"
                    " msr msp, r0            \n" /* Set the msp back to the start of the stack. */
                    " cpsie i                \n" /* Globally enable interrupts. */
                    " cpsie f                \n"
                    " dsb                    \n"
                    " isb                    \n"
                    " svc 0                    \n" /* System call to start first task. */
                    " nop                    \n"
                );
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
    /* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.
    See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
    configASSERT( configMAX_SYSCALL_INTERRUPT_PRIORITY );

    /* This port can be used on all revisions of the Cortex-M7 core other than
    the r0p1 parts.  r0p1 parts should use the port from the
    /source/portable/GCC/ARM_CM7/r0p1 directory. */
    configASSERT( portCPUID != portCORTEX_M7_r0p1_ID );
    configASSERT( portCPUID != portCORTEX_M7_r0p0_ID );

    #if( configASSERT_DEFINED == 1 )
    {
        volatile uint32_t ulOriginalPriority;
        volatile uint8_t * const pucFirstUserPriorityRegister = ( volatile uint8_t * const ) ( portNVIC_IP_REGISTERS_OFFSET_16 + portFIRST_USER_INTERRUPT_NUMBER );
        volatile uint8_t ucMaxPriorityValue;

        /* Determine the maximum priority from which ISR safe FreeRTOS API
        functions can be called.  ISR safe functions are those that end in
        "FromISR".  FreeRTOS maintains separate thread and ISR API functions to
        ensure interrupt entry is as fast and simple as possible.

        Save the interrupt priority value that is about to be clobbered. */
        ulOriginalPriority = *pucFirstUserPriorityRegister;

        /* Determine the number of priority bits available.  First write to all
        possible bits. */
        *pucFirstUserPriorityRegister = portMAX_8_BIT_VALUE;

        /* Read the value back to see how many bits stuck. */
        ucMaxPriorityValue = *pucFirstUserPriorityRegister;

        /* Use the same mask on the maximum system call priority. */
        ucMaxSysCallPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY & ucMaxPriorityValue;

        /* Calculate the maximum acceptable priority group value for the number
        of bits read back. */
        ulMaxPRIGROUPValue = portMAX_PRIGROUP_BITS;
        while( ( ucMaxPriorityValue & portTOP_BIT_OF_BYTE ) == portTOP_BIT_OF_BYTE )
        {
            ulMaxPRIGROUPValue--;
            ucMaxPriorityValue <<= ( uint8_t ) 0x01;
        }

        /* Shift the priority group value back to its position within the AIRCR
        register. */
        ulMaxPRIGROUPValue <<= portPRIGROUP_SHIFT;
        ulMaxPRIGROUPValue &= portPRIORITY_GROUP_MASK;

        /* Restore the clobbered interrupt priority register to its original
        value. */
        *pucFirstUserPriorityRegister = ulOriginalPriority;
    }
    #endif /* conifgASSERT_DEFINED */

    /* Make PendSV and SysTick the lowest priority interrupts. */
    portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
    portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    vPortSetupTimerInterrupt();

    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    /* Ensure the VFP is enabled - it should be anyway. */
    vPortEnableVFP();

    /* Lazy save always. */
    *( portFPCCR ) |= portASPEN_AND_LSPEN_BITS;

    /* Start the first task. */
    prvPortStartFirstTask();

    /* Should never get here as the tasks will now be executing!  Call the task
    exit error function to prevent compiler warnings about a static function
    not being called in the case that the application writer overrides this
    functionality by defining configTASK_RETURN_ADDRESS. */
    prvTaskExitError();

    /* Should not get here! */
    return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented in ports where there is nothing to return to.
    Artificially force an assert. */
    configASSERT( uxCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;

    /* This is not the interrupt safe version of the enter critical function so
    assert() if it is being called from an interrupt context.  Only API
    functions that end in "FromISR" can be used in an interrupt.  Only assert if
    the critical nesting count is 1 to protect against recursive calls if the
    assert function also uses a critical section. */
    if( uxCriticalNesting == 1 )
    {
        configASSERT( ( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK ) == 0 );
    }
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
    configASSERT( uxCriticalNesting );
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/

void xPortPendSVHandler( void )
{
    /* This is a naked function. */

    __asm volatile
    (
    "    mrs r0, psp                            \n"
    "    isb                                    \n"
    "                                        \n"
    "    ldr    r3, pxCurrentTCBConst            \n" /* Get the location of the current TCB. */
    "    ldr    r2, [r3]                        \n"
    "                                        \n"
    "    tst r14, #0x10                        \n" /* Is the task using the FPU context?  If so, push high vfp registers. */
    "    it eq                                \n"
    "    vstmdbeq r0!, {s16-s31}                \n"
    "                                        \n"
    "    stmdb r0!, {r4-r11, r14}            \n" /* Save the core registers. */
    "                                        \n"
    "    str r0, [r2]                        \n" /* Save the new top of stack into the first member of the TCB. */
    "                                        \n"
    "    stmdb sp!, {r3}                        \n"
    "    mov r0, %0                             \n"
    "    msr basepri, r0                        \n"
    "    dsb                                    \n"
    "    isb                                    \n"
    "    bl vTaskSwitchContext                \n"
    "    mov r0, #0                            \n"
    "    msr basepri, r0                        \n"
    "    ldmia sp!, {r3}                        \n"
    "                                        \n"
    "    ldr r1, [r3]                        \n" /* The first item in pxCurrentTCB is the task top of stack. */
    "    ldr r0, [r1]                        \n"
    "                                        \n"
    "    ldmia r0!, {r4-r11, r14}            \n" /* Pop the core registers. */
    "                                        \n"
    "    tst r14, #0x10                        \n" /* Is the task using the FPU context?  If so, pop the high vfp registers too. */
    "    it eq                                \n"
    "    vldmiaeq r0!, {s16-s31}                \n"
    "                                        \n"
    "    msr psp, r0                            \n"
    "    isb                                    \n"
    "                                        \n"
    #ifdef WORKAROUND_PMU_CM001 /* XMC4000 specific errata workaround. */
        #if WORKAROUND_PMU_CM001 == 1
    "            push { r14 }                \n"
    "            pop { pc }                    \n"
        #endif
    #endif
    "                                        \n"
    "    bx r14                                \n"
    "                                        \n"
    "    .align 4                            \n"
    "pxCurrentTCBConst: .word pxCurrentTCB    \n"
    ::"i"(configMAX_SYSCALL_INTERRUPT_PRIORITY)
    );
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
    /* The SysTick runs at the lowest interrupt priority, so when this interrupt
    executes all interrupts must be unmasked.  There is therefore no need to
    save and then restore the interrupt mask value as its value is already
    known. */
    portDISABLE_INTERRUPTS();
    {
        /* Increment the RTOS tick. */
        if( xTaskIncrementTick() != pdFALSE )
        {
            /* A context switch is required.  Context switching is performed in
            the PendSV interrupt.  Pend the PendSV interrupt. */
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
        }
    }
    portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

#if configUSE_TICKLESS_IDLE == 1
static uint32_t lega_get_rtc_sleep_cnt(TickType_t xExpectedIdleTime, uint32_t us_in_one_tick, float us_per_systick_count, float us_per_rtc_count)
{
    float rtc_sleep_us;
    /*
    first record the current sys_tick value, then mutliply the us-per-systick-count
    according to the follow definitions:
        ulTimerCountsForOneTick
        configTICK_RATE_HZ
    1s = 1000,000us
    us_per_systick_count = 1000000/configTICK_RATE_HZ/ulTimerCountsForOneTick = 0.01923076923076923
    the time need RTC sleep will be(in us):
    portNVIC_SYSTICK_CURRENT_VALUE_REG * us_per_systick_count + (xExpectedIdleTime - 1) * (1000000 / configTICK_RATE_HZ)

    use RTC as wakeup source, the RTC xtal is 32768, so each RTC cnt will be 1000000/32768 = 30.517578125us
    */
    rtc_sleep_us = ((portNVIC_SYSTICK_CURRENT_VALUE_REG) * us_per_systick_count) + (xExpectedIdleTime - 1) * (us_in_one_tick);
    /*RTC xtal is 32768, so the maximum RTC cnt will be 32768 that is 1s in time*/
    if (rtc_sleep_us > 1000000)
        rtc_sleep_us = 1000000;

    return (uint32_t)(rtc_sleep_us / us_per_rtc_count);
}
static uint32_t lega_get_ticks_completed(float wakeup_time, uint32_t us_in_one_tick)
{
    return (uint32_t)(wakeup_time/us_in_one_tick);
}
static uint32_t lega_get_systick_compensation(float wakeup_time, uint32_t us_in_one_tick, float us_per_systick_count)
{
    float elapsed_us;
    elapsed_us = ((uint32_t)wakeup_time % us_in_one_tick);

    return (uint32_t)((us_in_one_tick - elapsed_us)/us_per_systick_count);
}

extern uint8_t wifi_ready;
extern uint8_t pmu_rtc_interrupt;
extern uint32_t g_rtc_counter;
extern pmu_state_t sleep_mode;
//set do_wakeup_time_calibration_enabled to get rtc_wakeup_compensation_estimated and other_wakeup_compensation_estimated's statistic value
uint32_t do_wakeup_time_calibration_enabled = 0;
uint32_t rtc_min = 0x7FFFFFFF, rtc_max=0, rtc_avg=0, rtc_sum = 0;
uint32_t rw_min = 0x7FFFFFFF, rw_max=0, rw_avg=0, rw_sum = 0;
uint32_t sleep_times = 0, rtc_wakeup_times, rw_wakeup_times;
//extern int get_random_int_c();
/*
    in lega design, enter into power save mode and leve power save mode, there will the follow stage need be considered:
    1, before enter into power save mode. systick account switch into RTC(in always module) account and time consumed need to be estimated,
        it is transition of high percision into low percision
    2, after leave power save mode, RTC account switch into systick account and time consumed need to be estimated.
        it is transition of low percision into high percision
    3, system can be wakeup by RTC interrupt or other interrupts, when wakeup by other interrupts, the RTC count used to calculate the time is not accurate
        as one RTC count will represent 30.517578125us in RTC at 32768's frequency, so compensate the midium value 15
*/
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
#ifdef LIGHT_SLEEP_SUPPORT
    uint32_t ulCompleteTickPeriods, xModifiableIdleTime;

    /* Stop the SysTick momentarily.  The time the SysTick is stopped for
    is accounted for as best it can be, but using the tickless mode will
    inevitably result in some tiny drift of the time maintained by the
    kernel with respect to calendar time. */
    portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __asm volatile( "cpsid i" );
    __asm volatile( "dsb" );
    __asm volatile( "isb" );

    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if( eTaskConfirmSleepModeStatus() == eAbortSleep || lega_pmu_sleep_allowed() == false)
    {
        /* Restart from whatever is left in the count register to complete
        this tick period. */
        portNVIC_SYSTICK_LOAD_REG = portNVIC_SYSTICK_CURRENT_VALUE_REG;

        /* Restart SysTick. */
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

        /* Reset the reload register to the value required for normal tick
        periods. */
        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile( "cpsie i" );
    }
    else
    {
        /* Sleep until something happens.  xModifiableIdleTime is for SOC RTC */
        xModifiableIdleTime = MS_TO_TICK_FOR_32K(xExpectedIdleTime);

        lega_pmu_rtc_count_set(xModifiableIdleTime);
        lega_pmu_goto_lightsleep();
        __WFI__();

        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile( "cpsie i" );

        ulCompleteTickPeriods = TICK_TO_MS_FOR_32K(lega_pmu_rtc_count_elapsed_get());
        if (ulCompleteTickPeriods > xExpectedIdleTime)
        {
            printf("wake other:%u %u\n", (unsigned int)ulCompleteTickPeriods, (unsigned int)xExpectedIdleTime);
            ulCompleteTickPeriods = xExpectedIdleTime;
        }

        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;
        /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
        again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
        value. The critical section is used to ensure the tick interrupt
        can only execute once in the case that the reload register is near
        zero. */
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
        portENTER_CRITICAL();
        {
            portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
            vTaskStepTick( ulCompleteTickPeriods );
            portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;
        }
        portEXIT_CRITICAL();
    }
#else
    uint32_t ulCompleteTickPeriods, xModifiableIdleTime;
    uint32_t systick_cnt_record;
    uint32_t elapsed_cnt;
    float wakeup_time;
    static float us_per_rtc_count = 1000000.00 / 32768;
    float us_per_systick_count;
    static uint32_t us_in_one_tick = 1000000/configTICK_RATE_HZ;
    static uint32_t rtc_compensation_estimated = 15;
    static uint32_t rtc_wakeup_compensation_estimated = 98;
    static uint32_t other_wakeup_compensation_estimated = 65;
    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __asm volatile( "cpsid i" );
    __asm volatile( "dsb" );
    __asm volatile( "isb" );
    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    //In lightsleep case, uart IRQcannot wakeup, so adding wifi_ready
    if (!wifi_ready || eTaskConfirmSleepModeStatus() == eAbortSleep ||
        (sleep_mode != PMU_STATE_LIGHTSLEEP && sleep_mode != PMU_STATE_MODEMSLEEP))
    {
        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile( "cpsie i" );
    }
    else
    {
        us_per_systick_count = 1000000.00/configTICK_RATE_HZ/ulTimerCountsForOneTick;
        /* Stop the SysTick momentarily.  The time the SysTick is stopped for
        is accounted for as best it can be, but using the tickless mode will
        inevitably result in some tiny drift of the time maintained by the
        kernel with respect to calendar time. */
        if (do_wakeup_time_calibration_enabled == 0)
            portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;
        else {
            //when do wakeup time calibration, keep systick work to do statistic between calculated compensation systick value and the real systick should be
            portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_INT_BIT;
            sleep_times++;
        }
        /* Sleep until something happens.  xModifiableIdleTime is for SOC RTC */
        systick_cnt_record = portNVIC_SYSTICK_CURRENT_VALUE_REG;
        xModifiableIdleTime = lega_get_rtc_sleep_cnt(xExpectedIdleTime, us_in_one_tick, us_per_systick_count, us_per_rtc_count);
        lega_drv_rtc_cc_set(xModifiableIdleTime, sleep_mode);
        lega_drv_rtc_enable();
        lega_drv_goto_sleep(sleep_mode);
        elapsed_cnt = lega_drv_rtc_counter_get();
        if (elapsed_cnt != g_rtc_counter) {
            if (do_wakeup_time_calibration_enabled == 0)
                wakeup_time = (elapsed_cnt*us_per_rtc_count + (ulTimerCountsForOneTick - systick_cnt_record) *us_per_systick_count) + rtc_compensation_estimated + other_wakeup_compensation_estimated;
            else//in time statistic branch don't do compensation as the compensation value is unknown
                wakeup_time = (elapsed_cnt*us_per_rtc_count + (ulTimerCountsForOneTick - systick_cnt_record) *us_per_systick_count) + rtc_compensation_estimated;
            rw_wakeup_times++;
        } else {
            if (do_wakeup_time_calibration_enabled == 0)
                wakeup_time = (elapsed_cnt*us_per_rtc_count + (ulTimerCountsForOneTick - systick_cnt_record) *us_per_systick_count) + rtc_wakeup_compensation_estimated;
            else//in time statistic branch don't do compensation as the compensation value is unknown
                wakeup_time = (elapsed_cnt*us_per_rtc_count + (ulTimerCountsForOneTick - systick_cnt_record) *us_per_systick_count);
            rtc_wakeup_times++;
        }
        ulCompleteTickPeriods = lega_get_ticks_completed(wakeup_time, us_in_one_tick);
        if (ulCompleteTickPeriods > xExpectedIdleTime)
            printf("wake other:%u %u\n", (unsigned int)ulCompleteTickPeriods, (unsigned int)xExpectedIdleTime);
        if (do_wakeup_time_calibration_enabled == 0) {
            portNVIC_SYSTICK_LOAD_REG = lega_get_systick_compensation(wakeup_time, us_in_one_tick, us_per_systick_count);//ulTimerCountsForOneTick/100;//10us
            portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
            portENTER_CRITICAL();
            {
                portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
                vTaskStepTick( ulCompleteTickPeriods );
                portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;
            }
            portEXIT_CRITICAL();
        } else {
            portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_INT_BIT;
            vTaskStepTick( ulCompleteTickPeriods );
            if (elapsed_cnt != g_rtc_counter) {
                uint32_t diff_val;
                diff_val = ((uint32_t)(1000 - portNVIC_SYSTICK_CURRENT_VALUE_REG* us_per_systick_count) + 1000 - (uint32_t)(1000 - lega_get_systick_compensation(wakeup_time, us_in_one_tick, us_per_systick_count) * us_per_systick_count)) % 1000;
                if (diff_val < rw_min)
                    rw_min = diff_val;
                if (diff_val > rw_max)
                    rw_max = diff_val;
                rw_sum += diff_val;
            } else {
                uint32_t diff_val;
                diff_val = ((uint32_t)(1000 - portNVIC_SYSTICK_CURRENT_VALUE_REG* us_per_systick_count) + 1000 - (uint32_t)(1000 - lega_get_systick_compensation(wakeup_time, us_in_one_tick, us_per_systick_count) * us_per_systick_count)) % 1000;
                if (diff_val < rtc_min)
                    rtc_min = diff_val;
                if (diff_val > rtc_max)
                    rtc_max = diff_val;
                rtc_sum += diff_val;
            }
            if (sleep_times % 1000 == 0) {
                printf("rtc_min:%u(us) rtc_max:%u(us) rtc_sum:%u(us) rtc_avg:%u(us)\r\n", (unsigned int)rtc_min, (unsigned int)rtc_max, (unsigned int)rtc_sum, (unsigned int)(rtc_sum/rtc_wakeup_times));
                printf("rw_min:%u(us) rw_max:%u(us) rw_sum:%u(us) rw_avg:%u(us)\r\n",  (unsigned int)rw_min, (unsigned int)rw_max, (unsigned int)rw_sum, (unsigned int)(rw_sum/rw_wakeup_times));
            }
        }
        lega_drv_goto_active();
        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile( "cpsie i" );
    }
#endif
}

#endif /* #if configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/
#ifdef HIGHFREQ_MCU160_SUPPORT
#if configUSE_TICKLESS_IDLE
void vport_update_timer_counts_for_one_tick( void )
{
    ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );
}
#else
void vport_update_timer_counts_for_one_tick( void )
{
}
#endif
#endif

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
__attribute__(( weak )) void vPortSetupTimerInterrupt( void )
{
    /* Calculate the constants required to configure the tick interrupt. */
    #if configUSE_TICKLESS_IDLE
    {
        ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );
        xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
        ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );
    }
    #endif /* configUSE_TICKLESS_IDLE */

    /* Configure SysTick to interrupt at the requested rate. */
    portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
    portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );
}
/*-----------------------------------------------------------*/

/* This is a naked function. */
static void vPortEnableVFP( void )
{
    __asm volatile
    (
        "    ldr.w r0, =0xE000ED88        \n" /* The FPU enable bits are in the CPACR. */
        "    ldr r1, [r0]                \n"
        "                                \n"
        "    orr r1, r1, #( 0xf << 20 )    \n" /* Enable CP10 and CP11 coprocessors, then save back. */
        "    str r1, [r0]                \n"
        "    bx r14                        "
    );
}
/*-----------------------------------------------------------*/

#if( configASSERT_DEFINED == 1 )

    void vPortValidateInterruptPriority( void )
    {
    uint32_t ulCurrentInterrupt;
    uint8_t ucCurrentPriority;

        /* Obtain the number of the currently executing interrupt. */
        __asm volatile( "mrs %0, ipsr" : "=r"( ulCurrentInterrupt ) );

        /* Is the interrupt number a user defined interrupt? */
        if( ulCurrentInterrupt >= portFIRST_USER_INTERRUPT_NUMBER )
        {
            /* Look up the interrupt's priority. */
            ucCurrentPriority = pcInterruptPriorityRegisters[ ulCurrentInterrupt ];

            /* The following assertion will fail if a service routine (ISR) for
            an interrupt that has been assigned a priority above
            configMAX_SYSCALL_INTERRUPT_PRIORITY calls an ISR safe FreeRTOS API
            function.  ISR safe FreeRTOS API functions must *only* be called
            from interrupts that have been assigned a priority at or below
            configMAX_SYSCALL_INTERRUPT_PRIORITY.

            Numerically low interrupt priority numbers represent logically high
            interrupt priorities, therefore the priority of the interrupt must
            be set to a value equal to or numerically *higher* than
            configMAX_SYSCALL_INTERRUPT_PRIORITY.

            Interrupts that    use the FreeRTOS API must not be left at their
            default priority of    zero as that is the highest possible priority,
            which is guaranteed to be above configMAX_SYSCALL_INTERRUPT_PRIORITY,
            and    therefore also guaranteed to be invalid.

            FreeRTOS maintains separate thread and ISR API functions to ensure
            interrupt entry is as fast and simple as possible.

            The following links provide detailed information:
            http://www.freertos.org/RTOS-Cortex-M3-M4.html
            http://www.freertos.org/FAQHelp.html */
            configASSERT( ucCurrentPriority >= ucMaxSysCallPriority );
        }

        /* Priority grouping:  The interrupt controller (NVIC) allows the bits
        that define each interrupt's priority to be split between bits that
        define the interrupt's pre-emption priority bits and bits that define
        the interrupt's sub-priority.  For simplicity all bits must be defined
        to be pre-emption priority bits.  The following assertion will fail if
        this is not the case (if some bits represent a sub-priority).

        If the application only uses CMSIS libraries for interrupt
        configuration then the correct setting can be achieved on all Cortex-M
        devices by calling NVIC_SetPriorityGrouping( 0 ); before starting the
        scheduler.  Note however that some vendor specific peripheral libraries
        assume a non-zero priority group setting, in which cases using a value
        of zero will result in unpredicable behaviour. */
        configASSERT( ( portAIRCR_REG & portPRIORITY_GROUP_MASK ) <= ulMaxPRIGROUPValue );
    }

#endif /* configASSERT_DEFINED */

#if( configGENERATE_RUN_TIME_STATS == 1 )

    #define STATS_TIMER_PORT    DUET_TIMER1_INDEX
    #define STATS_TIMER_USEC    1000000

    static duet_timer_dev_t stats_timer;
    static uint64_t run_seconds;

    void time_increase()
    {
        run_seconds++;
    }

    void configure_timer_for_run_stats()
    {
        stats_timer.port = STATS_TIMER_PORT;
        /* unit us */
        stats_timer.config.period = STATS_TIMER_USEC;
        stats_timer.config.reload_mode = TIMER_RELOAD_AUTO;
        stats_timer.config.cb = time_increase;
        stats_timer.config.arg = NULL;
        stats_timer.priv = NULL;
        duet_timer_init(&stats_timer);
        duet_timer_start(&stats_timer);
    }

    uint32_t get_run_time_counter_value()
    {
        uint64_t total = (STATS_TIMER_USEC - duet_timer_get(&stats_timer)) / 100 + run_seconds * (STATS_TIMER_USEC / 100);

        /* Unit:0.1ms
         * FreeRTOS use 32bits to store TotalRunTime
         * largest time 2^32 * 0.1ms
         */
        return (uint32_t)total;

    }

#endif
