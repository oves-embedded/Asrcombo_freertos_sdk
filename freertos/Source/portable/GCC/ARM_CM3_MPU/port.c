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
 * Implementation of functions defined in portable.h for the ARM CM3 port.
 *----------------------------------------------------------*/

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#include "mpu_prototypes.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Constants required to access and manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL_REG                ( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG                ( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSPRI2_REG                    ( *    ( ( volatile uint32_t * ) 0xe000ed20 ) )
#define portNVIC_SYSPRI1_REG                    ( * ( ( volatile uint32_t * ) 0xe000ed1c ) )
#define portNVIC_SYS_CTRL_STATE_REG                ( * ( ( volatile uint32_t * ) 0xe000ed24 ) )
#define portNVIC_MEM_FAULT_ENABLE                ( 1UL << 16UL )

/* Constants required to access and manipulate the MPU. */
#define portMPU_TYPE_REG                        ( * ( ( volatile uint32_t * ) 0xe000ed90 ) )
#define portMPU_REGION_BASE_ADDRESS_REG            ( * ( ( volatile uint32_t * ) 0xe000ed9C ) )
#define portMPU_REGION_ATTRIBUTE_REG            ( * ( ( volatile uint32_t * ) 0xe000edA0 ) )
#define portMPU_CTRL_REG                        ( * ( ( volatile uint32_t * ) 0xe000ed94 ) )
#define portEXPECTED_MPU_TYPE_VALUE                ( 8UL << 8UL ) /* 8 regions, unified. */
#define portMPU_ENABLE                            ( 0x01UL )
#define portMPU_BACKGROUND_ENABLE                ( 1UL << 2UL )
#define portPRIVILEGED_EXECUTION_START_ADDRESS    ( 0UL )
#define portMPU_REGION_VALID                    ( 0x10UL )
#define portMPU_REGION_ENABLE                    ( 0x01UL )
#define portPERIPHERALS_START_ADDRESS            0x40000000UL
#define portPERIPHERALS_END_ADDRESS                0x5FFFFFFFUL

/* Constants required to access and manipulate the SysTick. */
#define portNVIC_SYSTICK_CLK                    ( 0x00000004UL )
#define portNVIC_SYSTICK_INT                    ( 0x00000002UL )
#define portNVIC_SYSTICK_ENABLE                    ( 0x00000001UL )
#define portNVIC_PENDSV_PRI                        ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 16UL )
#define portNVIC_SYSTICK_PRI                    ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )
#define portNVIC_SVC_PRI                        ( ( ( uint32_t ) configMAX_SYSCALL_INTERRUPT_PRIORITY - 1UL ) << 24UL )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR                        ( 0x01000000 )
#define portINITIAL_CONTROL_IF_UNPRIVILEGED        ( 0x03 )
#define portINITIAL_CONTROL_IF_PRIVILEGED        ( 0x02 )

/* Constants required to check the validity of an interrupt priority. */
#define portFIRST_USER_INTERRUPT_NUMBER        ( 16 )
#define portNVIC_IP_REGISTERS_OFFSET_16     ( 0xE000E3F0 )
#define portAIRCR_REG                        ( * ( ( volatile uint32_t * ) 0xE000ED0C ) )
#define portMAX_8_BIT_VALUE                    ( ( uint8_t ) 0xff )
#define portTOP_BIT_OF_BYTE                    ( ( uint8_t ) 0x80 )
#define portMAX_PRIGROUP_BITS                ( ( uint8_t ) 7 )
#define portPRIORITY_GROUP_MASK                ( 0x07UL << 8UL )
#define portPRIGROUP_SHIFT                    ( 8UL )

/* Offsets in the stack to the parameters when inside the SVC handler. */
#define portOFFSET_TO_PC                        ( 6 )

/* For strict compliance with the Cortex-M spec the task start address should
have bit-0 clear, as it is loaded into the PC on exit from an ISR. */
#define portSTART_ADDRESS_MASK                ( ( StackType_t ) 0xfffffffeUL )

/* Each task maintains its own interrupt status in the critical nesting
variable.  Note this is not saved as part of the task context as context
switches can only occur when uxCriticalNesting is zero. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/*
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt( void ) PRIVILEGED_FUNCTION;

/*
 * Configure a number of standard MPU regions that are used by all tasks.
 */
static void prvSetupMPU( void ) PRIVILEGED_FUNCTION;

/*
 * Return the smallest MPU region size that a given number of bytes will fit
 * into.  The region size is returned as the value that should be programmed
 * into the region attribute register for that region.
 */
static uint32_t prvGetMPURegionSizeSetting( uint32_t ulActualSizeInBytes ) PRIVILEGED_FUNCTION;

/*
 * Checks to see if being called from the context of an unprivileged task, and
 * if so raises the privilege level and returns false - otherwise does nothing
 * other than return true.
 */
BaseType_t xPortRaisePrivilege( void ) __attribute__(( naked ));

/*
 * Standard FreeRTOS exception handlers.
 */
void xPortPendSVHandler( void ) __attribute__ (( naked )) PRIVILEGED_FUNCTION;
void xPortSysTickHandler( void )  __attribute__ ((optimize("3"))) PRIVILEGED_FUNCTION;
void vPortSVCHandler( void ) __attribute__ (( naked )) PRIVILEGED_FUNCTION;

/*
 * Starts the scheduler by restoring the context of the first task to run.
 */
static void prvRestoreContextOfFirstTask( void ) __attribute__(( naked )) PRIVILEGED_FUNCTION;

/*
 * C portion of the SVC handler.  The SVC handler is split between an asm entry
 * and a C wrapper for simplicity of coding and maintenance.
 */
static void prvSVCHandler( uint32_t *pulRegisters ) __attribute__(( noinline )) PRIVILEGED_FUNCTION;

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
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters, BaseType_t xRunPrivileged )
{
    /* Simulate the stack frame as it would be created by a context switch
    interrupt. */
    pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
    *pxTopOfStack = portINITIAL_XPSR;    /* xPSR */
    pxTopOfStack--;
    *pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;    /* PC */
    pxTopOfStack--;
    *pxTopOfStack = 0;    /* LR */
    pxTopOfStack -= 5;    /* R12, R3, R2 and R1. */
    *pxTopOfStack = ( StackType_t ) pvParameters;    /* R0 */
    pxTopOfStack -= 9;    /* R11, R10, R9, R8, R7, R6, R5 and R4. */

    if( xRunPrivileged == pdTRUE )
    {
        *pxTopOfStack = portINITIAL_CONTROL_IF_PRIVILEGED;
    }
    else
    {
        *pxTopOfStack = portINITIAL_CONTROL_IF_UNPRIVILEGED;
    }

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void vPortSVCHandler( void )
{
    /* Assumes psp was in use. */
    __asm volatile
    (
        #ifndef USE_PROCESS_STACK    /* Code should not be required if a main() is using the process stack. */
            "    tst lr, #4                        \n"
            "    ite eq                            \n"
            "    mrseq r0, msp                    \n"
            "    mrsne r0, psp                    \n"
        #else
            "    mrs r0, psp                        \n"
        #endif
            "    b %0                            \n"
            ::"i"(prvSVCHandler):"r0"
    );
}
/*-----------------------------------------------------------*/

static void prvSVCHandler(    uint32_t *pulParam )
{
uint8_t ucSVCNumber;

    /* The stack contains: r0, r1, r2, r3, r12, r14, the return address and
    xPSR.  The first argument (r0) is pulParam[ 0 ]. */
    ucSVCNumber = ( ( uint8_t * ) pulParam[ portOFFSET_TO_PC ] )[ -2 ];
    switch( ucSVCNumber )
    {
        case portSVC_START_SCHEDULER    :    portNVIC_SYSPRI1_REG |= portNVIC_SVC_PRI;
                                            prvRestoreContextOfFirstTask();
                                            break;

        case portSVC_YIELD                :    portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
                                            /* Barriers are normally not required
                                            but do ensure the code is completely
                                            within the specified behaviour for the
                                            architecture. */
                                            __asm volatile( "dsb" );
                                            __asm volatile( "isb" );

                                            break;

        case portSVC_RAISE_PRIVILEGE    :    __asm volatile
                                            (
                                                "    mrs r1, control        \n" /* Obtain current control value. */
                                                "    bic r1, #1            \n" /* Set privilege bit. */
                                                "    msr control, r1        \n" /* Write back new control value. */
                                                :::"r1"
                                            );
                                            break;

        default                            :    /* Unknown SVC call. */
                                            break;
    }
}
/*-----------------------------------------------------------*/

static void prvRestoreContextOfFirstTask( void )
{
    __asm volatile
    (
        "    ldr r0, =0xE000ED08                \n" /* Use the NVIC offset register to locate the stack. */
        "    ldr r0, [r0]                    \n"
        "    ldr r0, [r0]                    \n"
        "    msr msp, r0                        \n" /* Set the msp back to the start of the stack. */
        "    ldr    r3, pxCurrentTCBConst2        \n" /* Restore the context. */
        "    ldr r1, [r3]                    \n"
        "    ldr r0, [r1]                    \n" /* The first item in the TCB is the task top of stack. */
        "    add r1, r1, #4                    \n" /* Move onto the second item in the TCB... */
        "    ldr r2, =0xe000ed9c                \n" /* Region Base Address register. */
        "    ldmia r1!, {r4-r11}                \n" /* Read 4 sets of MPU registers. */
        "    stmia r2!, {r4-r11}                \n" /* Write 4 sets of MPU registers. */
        "    ldmia r0!, {r3, r4-r11}            \n" /* Pop the registers that are not automatically saved on exception entry. */
        "    msr control, r3                    \n"
        "    msr psp, r0                        \n" /* Restore the task stack pointer. */
        "    mov r0, #0                        \n"
        "    msr    basepri, r0                    \n"
        "    ldr r14, =0xfffffffd            \n" /* Load exec return code. */
        "    bx r14                            \n"
        "                                    \n"
        "    .align 4                        \n"
        "pxCurrentTCBConst2: .word pxCurrentTCB    \n"
    );
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
    /* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.  See
    http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
    configASSERT( ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) );

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

    /* Make PendSV and SysTick the same priority as the kernel, and the SVC
    handler higher priority so it can be used to exit a critical section (where
    lower priorities are masked). */
    portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
    portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

    /* Configure the regions in the MPU that are common to all tasks. */
    prvSetupMPU();

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    prvSetupTimerInterrupt();

    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    /* Start the first task. */
    __asm volatile(
                    " ldr r0, =0xE000ED08     \n" /* Use the NVIC offset register to locate the stack. */
                    " ldr r0, [r0]             \n"
                    " ldr r0, [r0]             \n"
                    " msr msp, r0            \n" /* Set the msp back to the start of the stack. */
                    " cpsie i                \n" /* Globally enable interrupts. */
                    " cpsie f                \n"
                    " dsb                    \n"
                    " isb                    \n"
                    " svc %0                \n" /* System call to start first task. */
                    " nop                    \n"
                    :: "i" (portSVC_START_SCHEDULER) );

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
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;

    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    configASSERT( uxCriticalNesting );
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

void xPortPendSVHandler( void )
{
    /* This is a naked function. */

    __asm volatile
    (
        "    mrs r0, psp                            \n"
        "                                        \n"
        "    ldr    r3, pxCurrentTCBConst            \n" /* Get the location of the current TCB. */
        "    ldr    r2, [r3]                        \n"
        "                                        \n"
        "    mrs r1, control                        \n"
        "    stmdb r0!, {r1, r4-r11}                \n" /* Save the remaining registers. */
        "    str r0, [r2]                        \n" /* Save the new top of stack into the first member of the TCB. */
        "                                        \n"
        "    stmdb sp!, {r3, r14}                \n"
        "    mov r0, %0                            \n"
        "    msr basepri, r0                        \n"
        "    bl vTaskSwitchContext                \n"
        "    mov r0, #0                            \n"
        "    msr basepri, r0                        \n"
        "    ldmia sp!, {r3, r14}                \n"
        "                                        \n"    /* Restore the context. */
        "    ldr r1, [r3]                        \n"
        "    ldr r0, [r1]                        \n" /* The first item in the TCB is the task top of stack. */
        "    add r1, r1, #4                        \n" /* Move onto the second item in the TCB... */
        "    ldr r2, =0xe000ed9c                    \n" /* Region Base Address register. */
        "    ldmia r1!, {r4-r11}                    \n" /* Read 4 sets of MPU registers. */
        "    stmia r2!, {r4-r11}                    \n" /* Write 4 sets of MPU registers. */
        "    ldmia r0!, {r3, r4-r11}                \n" /* Pop the registers that are not automatically saved on exception entry. */
        "    msr control, r3                        \n"
        "                                        \n"
        "    msr psp, r0                            \n"
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
uint32_t ulDummy;

    ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        /* Increment the RTOS tick. */
        if( xTaskIncrementTick() != pdFALSE )
        {
            /* Pend a context switch. */
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
static void prvSetupTimerInterrupt( void )
{
    /* Configure SysTick to interrupt at the requested rate. */
    portNVIC_SYSTICK_LOAD_REG = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
    portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
}
/*-----------------------------------------------------------*/

static void prvSetupMPU( void )
{
extern uint32_t __privileged_functions_end__[];
extern uint32_t __FLASH_segment_start__[];
extern uint32_t __FLASH_segment_end__[];
extern uint32_t __privileged_data_start__[];
extern uint32_t __privileged_data_end__[];

    /* Check the expected MPU is present. */
    if( portMPU_TYPE_REG == portEXPECTED_MPU_TYPE_VALUE )
    {
        /* First setup the entire flash for unprivileged read only access. */
        portMPU_REGION_BASE_ADDRESS_REG =    ( ( uint32_t ) __FLASH_segment_start__ ) | /* Base address. */
                                            ( portMPU_REGION_VALID ) |
                                            ( portUNPRIVILEGED_FLASH_REGION );

        portMPU_REGION_ATTRIBUTE_REG =    ( portMPU_REGION_READ_ONLY ) |
                                        ( portMPU_REGION_CACHEABLE_BUFFERABLE ) |
                                        ( prvGetMPURegionSizeSetting( ( uint32_t ) __FLASH_segment_end__ - ( uint32_t ) __FLASH_segment_start__ ) ) |
                                        ( portMPU_REGION_ENABLE );

        /* Setup the first 16K for privileged only access (even though less
        than 10K is actually being used).  This is where the kernel code is
        placed. */
        portMPU_REGION_BASE_ADDRESS_REG =    ( ( uint32_t ) __FLASH_segment_start__ ) | /* Base address. */
                                            ( portMPU_REGION_VALID ) |
                                            ( portPRIVILEGED_FLASH_REGION );

        portMPU_REGION_ATTRIBUTE_REG =    ( portMPU_REGION_PRIVILEGED_READ_ONLY ) |
                                        ( portMPU_REGION_CACHEABLE_BUFFERABLE ) |
                                        ( prvGetMPURegionSizeSetting( ( uint32_t ) __privileged_functions_end__ - ( uint32_t ) __FLASH_segment_start__ ) ) |
                                        ( portMPU_REGION_ENABLE );

        /* Setup the privileged data RAM region.  This is where the kernel data
        is placed. */
        portMPU_REGION_BASE_ADDRESS_REG =    ( ( uint32_t ) __privileged_data_start__ ) | /* Base address. */
                                            ( portMPU_REGION_VALID ) |
                                            ( portPRIVILEGED_RAM_REGION );

        portMPU_REGION_ATTRIBUTE_REG =    ( portMPU_REGION_PRIVILEGED_READ_WRITE ) |
                                        ( portMPU_REGION_CACHEABLE_BUFFERABLE ) |
                                        prvGetMPURegionSizeSetting( ( uint32_t ) __privileged_data_end__ - ( uint32_t ) __privileged_data_start__ ) |
                                        ( portMPU_REGION_ENABLE );

        /* By default allow everything to access the general peripherals.  The
        system peripherals and registers are protected. */
        portMPU_REGION_BASE_ADDRESS_REG =    ( portPERIPHERALS_START_ADDRESS ) |
                                            ( portMPU_REGION_VALID ) |
                                            ( portGENERAL_PERIPHERALS_REGION );

        portMPU_REGION_ATTRIBUTE_REG =    ( portMPU_REGION_READ_WRITE | portMPU_REGION_EXECUTE_NEVER ) |
                                        ( prvGetMPURegionSizeSetting( portPERIPHERALS_END_ADDRESS - portPERIPHERALS_START_ADDRESS ) ) |
                                        ( portMPU_REGION_ENABLE );

        /* Enable the memory fault exception. */
        portNVIC_SYS_CTRL_STATE_REG |= portNVIC_MEM_FAULT_ENABLE;

        /* Enable the MPU with the background region configured. */
        portMPU_CTRL_REG |= ( portMPU_ENABLE | portMPU_BACKGROUND_ENABLE );
    }
}
/*-----------------------------------------------------------*/

static uint32_t prvGetMPURegionSizeSetting( uint32_t ulActualSizeInBytes )
{
uint32_t ulRegionSize, ulReturnValue = 4;

    /* 32 is the smallest region size, 31 is the largest valid value for
    ulReturnValue. */
    for( ulRegionSize = 32UL; ulReturnValue < 31UL; ( ulRegionSize <<= 1UL ) )
    {
        if( ulActualSizeInBytes <= ulRegionSize )
        {
            break;
        }
        else
        {
            ulReturnValue++;
        }
    }

    /* Shift the code by one before returning so it can be written directly
    into the the correct bit position of the attribute register. */
    return ( ulReturnValue << 1UL );
}
/*-----------------------------------------------------------*/

BaseType_t xPortRaisePrivilege( void )
{
    __asm volatile
    (
        "    mrs r0, control                        \n"
        "    tst r0, #1                            \n" /* Is the task running privileged? */
        "    itte ne                                \n"
        "    movne r0, #0                        \n" /* CONTROL[0]!=0, return false. */
        "    svcne %0                            \n" /* Switch to privileged. */
        "    moveq r0, #1                        \n" /* CONTROL[0]==0, return true. */
        "    bx lr                                \n"
        :: "i" (portSVC_RAISE_PRIVILEGE) : "r0"
    );

    return 0;
}
/*-----------------------------------------------------------*/

void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions, StackType_t *pxBottomOfStack, uint32_t ulStackDepth )
{
extern uint32_t __SRAM_segment_start__[];
extern uint32_t __SRAM_segment_end__[];
extern uint32_t __privileged_data_start__[];
extern uint32_t __privileged_data_end__[];
int32_t lIndex;
uint32_t ul;

    if( xRegions == NULL )
    {
        /* No MPU regions are specified so allow access to all RAM. */
        xMPUSettings->xRegion[ 0 ].ulRegionBaseAddress =
                ( ( uint32_t ) __SRAM_segment_start__ ) | /* Base address. */
                ( portMPU_REGION_VALID ) |
                ( portSTACK_REGION );

        xMPUSettings->xRegion[ 0 ].ulRegionAttribute =
                ( portMPU_REGION_READ_WRITE ) |
                ( portMPU_REGION_CACHEABLE_BUFFERABLE ) |
                ( prvGetMPURegionSizeSetting( ( uint32_t ) __SRAM_segment_end__ - ( uint32_t ) __SRAM_segment_start__ ) ) |
                ( portMPU_REGION_ENABLE );

        /* Re-instate the privileged only RAM region as xRegion[ 0 ] will have
        just removed the privileged only parameters. */
        xMPUSettings->xRegion[ 1 ].ulRegionBaseAddress =
                ( ( uint32_t ) __privileged_data_start__ ) | /* Base address. */
                ( portMPU_REGION_VALID ) |
                ( portSTACK_REGION + 1 );

        xMPUSettings->xRegion[ 1 ].ulRegionAttribute =
                ( portMPU_REGION_PRIVILEGED_READ_WRITE ) |
                ( portMPU_REGION_CACHEABLE_BUFFERABLE ) |
                prvGetMPURegionSizeSetting( ( uint32_t ) __privileged_data_end__ - ( uint32_t ) __privileged_data_start__ ) |
                ( portMPU_REGION_ENABLE );

        /* Invalidate all other regions. */
        for( ul = 2; ul <= portNUM_CONFIGURABLE_REGIONS; ul++ )
        {
            xMPUSettings->xRegion[ ul ].ulRegionBaseAddress = ( portSTACK_REGION + ul ) | portMPU_REGION_VALID;
            xMPUSettings->xRegion[ ul ].ulRegionAttribute = 0UL;
        }
    }
    else
    {
        /* This function is called automatically when the task is created - in
        which case the stack region parameters will be valid.  At all other
        times the stack parameters will not be valid and it is assumed that the
        stack region has already been configured. */
        if( ulStackDepth > 0 )
        {
            /* Define the region that allows access to the stack. */
            xMPUSettings->xRegion[ 0 ].ulRegionBaseAddress =
                    ( ( uint32_t ) pxBottomOfStack ) |
                    ( portMPU_REGION_VALID ) |
                    ( portSTACK_REGION ); /* Region number. */

            xMPUSettings->xRegion[ 0 ].ulRegionAttribute =
                    ( portMPU_REGION_READ_WRITE ) | /* Read and write. */
                    ( prvGetMPURegionSizeSetting( ulStackDepth * ( uint32_t ) sizeof( StackType_t ) ) ) |
                    ( portMPU_REGION_CACHEABLE_BUFFERABLE ) |
                    ( portMPU_REGION_ENABLE );
        }

        lIndex = 0;

        for( ul = 1; ul <= portNUM_CONFIGURABLE_REGIONS; ul++ )
        {
            if( ( xRegions[ lIndex ] ).ulLengthInBytes > 0UL )
            {
                /* Translate the generic region definition contained in
                xRegions into the CM3 specific MPU settings that are then
                stored in xMPUSettings. */
                xMPUSettings->xRegion[ ul ].ulRegionBaseAddress =
                        ( ( uint32_t ) xRegions[ lIndex ].pvBaseAddress ) |
                        ( portMPU_REGION_VALID ) |
                        ( portSTACK_REGION + ul ); /* Region number. */

                xMPUSettings->xRegion[ ul ].ulRegionAttribute =
                        ( prvGetMPURegionSizeSetting( xRegions[ lIndex ].ulLengthInBytes ) ) |
                        ( xRegions[ lIndex ].ulParameters ) |
                        ( portMPU_REGION_ENABLE );
            }
            else
            {
                /* Invalidate the region. */
                xMPUSettings->xRegion[ ul ].ulRegionBaseAddress = ( portSTACK_REGION + ul ) | portMPU_REGION_VALID;
                xMPUSettings->xRegion[ ul ].ulRegionAttribute = 0UL;
            }

            lIndex++;
        }
    }
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
/*-----------------------------------------------------------*/


