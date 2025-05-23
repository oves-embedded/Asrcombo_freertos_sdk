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

/*
 * Implementation of the wrapper functions used to raise the processor privilege
 * before calling a standard FreeRTOS API function.
 */

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

/*
 * Checks to see if being called from the context of an unprivileged task, and
 * if so raises the privilege level and returns false - otherwise does nothing
 * other than return true.
 */
extern BaseType_t xPortRaisePrivilege( void );

/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCreateRestricted( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xTaskCreateRestricted( pxTaskDefinition, pxCreatedTask );
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    BaseType_t MPU_xTaskCreate( TaskFunction_t pvTaskCode, const char * const pcName, uint16_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    TaskHandle_t MPU_xTaskCreateStatic( TaskFunction_t pxTaskCode, const char * const pcName, const uint32_t ulStackDepth, void * const pvParameters, UBaseType_t uxPriority, StackType_t * const puxStackBuffer, StaticTask_t * const pxTaskBuffer )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskCreateStatic( pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/

void MPU_vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const xRegions )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    vTaskAllocateMPURegions( xTask, xRegions );
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )
    void MPU_vTaskDelete( TaskHandle_t pxTaskToDelete )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskDelete( pxTaskToDelete );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelayUntil == 1 )
    void MPU_vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, TickType_t xTimeIncrement )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskAbortDelay == 1 )
    BaseType_t MPU_xTaskAbortDelay( TaskHandle_t xTask )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskAbortDelay( xTask );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelay == 1 )
    void MPU_vTaskDelay( TickType_t xTicksToDelay )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskDelay( xTicksToDelay );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )
    UBaseType_t MPU_uxTaskPriorityGet( TaskHandle_t pxTask )
    {
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        uxReturn = uxTaskPriorityGet( pxTask );
        vPortResetPrivilege( xRunningPrivileged );
        return uxReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskPrioritySet == 1 )
    void MPU_vTaskPrioritySet( TaskHandle_t pxTask, UBaseType_t uxNewPriority )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskPrioritySet( pxTask, uxNewPriority );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_eTaskGetState == 1 )
    eTaskState MPU_eTaskGetState( TaskHandle_t pxTask )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();
    eTaskState eReturn;

        eReturn = eTaskGetState( pxTask );
        vPortResetPrivilege( xRunningPrivileged );
        return eReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TRACE_FACILITY == 1 )
    void MPU_vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskGetInfo( xTask, pxTaskStatus, xGetFreeStackSpace, eState );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
    TaskHandle_t MPU_xTaskGetIdleTaskHandle( void )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskGetIdleTaskHandle();
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
    void MPU_vTaskSuspend( TaskHandle_t pxTaskToSuspend )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskSuspend( pxTaskToSuspend );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
    void MPU_vTaskResume( TaskHandle_t pxTaskToResume )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskResume( pxTaskToResume );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSuspendAll( void )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    vTaskSuspendAll();
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskResumeAll( void )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xTaskResumeAll();
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

TickType_t MPU_xTaskGetTickCount( void )
{
TickType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xTaskGetTickCount();
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxTaskGetNumberOfTasks( void )
{
UBaseType_t uxReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    uxReturn = uxTaskGetNumberOfTasks();
    vPortResetPrivilege( xRunningPrivileged );
    return uxReturn;
}
/*-----------------------------------------------------------*/

char * MPU_pcTaskGetName( TaskHandle_t xTaskToQuery )
{
char *pcReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    pcReturn = pcTaskGetName( xTaskToQuery );
    vPortResetPrivilege( xRunningPrivileged );
    return pcReturn;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetHandle == 1 )
    TaskHandle_t MPU_xTaskGetHandle( const char *pcNameToQuery )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskGetHandle( pcNameToQuery );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
    void MPU_vTaskList( char *pcWriteBuffer )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskList( pcWriteBuffer );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
    void MPU_vTaskGetRunTimeStats( char *pcWriteBuffer )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskGetRunTimeStats( pcWriteBuffer );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    void MPU_vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxTagValue )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskSetApplicationTaskTag( xTask, pxTagValue );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    TaskHookFunction_t MPU_xTaskGetApplicationTaskTag( TaskHandle_t xTask )
    {
    TaskHookFunction_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskGetApplicationTaskTag( xTask );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
    void MPU_vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTaskSetThreadLocalStoragePointer( xTaskToSet, xIndex, pvValue );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
    void *MPU_pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex )
    {
    void *pvReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        pvReturn = pvTaskGetThreadLocalStoragePointer( xTaskToQuery, xIndex );
        vPortResetPrivilege( xRunningPrivileged );
        return pvReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    BaseType_t MPU_xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskCallApplicationTaskHook( xTask, pvParameter );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )
    UBaseType_t MPU_uxTaskGetSystemState( TaskStatus_t *pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t *pulTotalRunTime )
    {
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        uxReturn = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, pulTotalRunTime );
        vPortResetPrivilege( xRunningPrivileged );
        return uxReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
    UBaseType_t MPU_uxTaskGetStackHighWaterMark( TaskHandle_t xTask )
    {
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        uxReturn = uxTaskGetStackHighWaterMark( xTask );
        vPortResetPrivilege( xRunningPrivileged );
        return uxReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetCurrentTaskHandle == 1 )
    TaskHandle_t MPU_xTaskGetCurrentTaskHandle( void )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskGetCurrentTaskHandle();
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetSchedulerState == 1 )
    BaseType_t MPU_xTaskGetSchedulerState( void )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskGetSchedulerState();
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSetTimeOutState( TimeOut_t * const pxTimeOut )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    vTaskSetTimeOutState( pxTimeOut );
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xTaskCheckForTimeOut( pxTimeOut, pxTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskGenericNotify( xTaskToNotify, ulValue, eAction, pulPreviousNotificationValue );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskNotifyWait( ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
    uint32_t MPU_ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
    {
    uint32_t ulReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        ulReturn = ulTaskNotifyTake( xClearCountOnExit, xTicksToWait );
        vPortResetPrivilege( xRunningPrivileged );
        return ulReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskNotifyStateClear( TaskHandle_t xTask )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTaskNotifyStateClear( xTask );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    QueueHandle_t MPU_xQueueGenericCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize, uint8_t ucQueueType )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueGenericCreate( uxQueueLength, uxItemSize, ucQueueType );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    QueueHandle_t MPU_xQueueGenericCreateStatic( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, uint8_t *pucQueueStorage, StaticQueue_t *pxStaticQueue, const uint8_t ucQueueType )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueGenericCreateStatic( uxQueueLength, uxItemSize, pucQueueStorage, pxStaticQueue, ucQueueType );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericReset( QueueHandle_t pxQueue, BaseType_t xNewQueue )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xQueueGenericReset( pxQueue, xNewQueue );
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, BaseType_t xCopyPosition )
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xQueueGenericSend( xQueue, pvItemToQueue, xTicksToWait, xCopyPosition );
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueMessagesWaiting( const QueueHandle_t pxQueue )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
UBaseType_t uxReturn;

    uxReturn = uxQueueMessagesWaiting( pxQueue );
    vPortResetPrivilege( xRunningPrivileged );
    return uxReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueSpacesAvailable( const QueueHandle_t xQueue )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
UBaseType_t uxReturn;

    uxReturn = uxQueueSpacesAvailable( xQueue );
    vPortResetPrivilege( xRunningPrivileged );
    return uxReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericReceive( QueueHandle_t pxQueue, void * const pvBuffer, TickType_t xTicksToWait, BaseType_t xJustPeeking )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
BaseType_t xReturn;

    xReturn = xQueueGenericReceive( pxQueue, pvBuffer, xTicksToWait, xJustPeeking );
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueuePeekFromISR( QueueHandle_t pxQueue, void * const pvBuffer )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
BaseType_t xReturn;

    xReturn = xQueuePeekFromISR( pxQueue, pvBuffer );
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

void* MPU_xQueueGetMutexHolder( QueueHandle_t xSemaphore )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
void * xReturn;

    xReturn = ( void * ) xQueueGetMutexHolder( xSemaphore );
    vPortResetPrivilege( xRunningPrivileged );
    return xReturn;
}
/*-----------------------------------------------------------*/

#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateMutex( const uint8_t ucQueueType )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueCreateMutex( ucQueueType );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateMutexStatic( const uint8_t ucQueueType, StaticQueue_t *pxStaticQueue )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueCreateMutexStatic( ucQueueType, pxStaticQueue );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateCountingSemaphore( UBaseType_t uxCountValue, UBaseType_t uxInitialCount )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueCreateCountingSemaphore( uxCountValue, uxInitialCount );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )

    QueueHandle_t MPU_xQueueCreateCountingSemaphoreStatic( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount, StaticQueue_t *pxStaticQueue )
    {
    QueueHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueCreateCountingSemaphoreStatic( uxMaxCount, uxInitialCount, pxStaticQueue );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )
    BaseType_t MPU_xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xBlockTime )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueTakeMutexRecursive( xMutex, xBlockTime );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )
    BaseType_t MPU_xQueueGiveMutexRecursive( QueueHandle_t xMutex )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueGiveMutexRecursive( xMutex );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    QueueSetHandle_t MPU_xQueueCreateSet( UBaseType_t uxEventQueueLength )
    {
    QueueSetHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueCreateSet( uxEventQueueLength );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    QueueSetMemberHandle_t MPU_xQueueSelectFromSet( QueueSetHandle_t xQueueSet, TickType_t xBlockTimeTicks )
    {
    QueueSetMemberHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueSelectFromSet( xQueueSet, xBlockTimeTicks );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    BaseType_t MPU_xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueAddToSet( xQueueOrSemaphore, xQueueSet );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    BaseType_t MPU_xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xQueueRemoveFromSet( xQueueOrSemaphore, xQueueSet );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    void MPU_vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcName )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vQueueAddToRegistry( xQueue, pcName );

        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    void MPU_vQueueUnregisterQueue( QueueHandle_t xQueue )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vQueueUnregisterQueue( xQueue );

        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    const char *MPU_pcQueueGetName( QueueHandle_t xQueue )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();
    const char *pcReturn;

        pcReturn = pcQueueGetName( xQueue );

        vPortResetPrivilege( xRunningPrivileged );
        return pcReturn;
    }
#endif
/*-----------------------------------------------------------*/

void MPU_vQueueDelete( QueueHandle_t xQueue )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    vQueueDelete( xQueue );

    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

    void *MPU_pvPortMalloc( size_t xSize )
    {
    void *pvReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        pvReturn = pvPortMalloc( xSize );

        vPortResetPrivilege( xRunningPrivileged );

        return pvReturn;
    }

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

    void MPU_vPortFree( void *pv )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vPortFree( pv );

        vPortResetPrivilege( xRunningPrivileged );
    }

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

void MPU_vPortInitialiseBlocks( void )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    vPortInitialiseBlocks();

    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

size_t MPU_xPortGetFreeHeapSize( void )
{
size_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xPortGetFreeHeapSize();

    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
    TimerHandle_t MPU_xTimerCreate( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction )
    {
    TimerHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
    TimerHandle_t MPU_xTimerCreateStatic( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction, StaticTimer_t *pxTimerBuffer )
    {
    TimerHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTimerCreateStatic( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction, pxTimerBuffer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    void *MPU_pvTimerGetTimerID( const TimerHandle_t xTimer )
    {
    void * pvReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        pvReturn = pvTimerGetTimerID( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return pvReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    void MPU_vTimerSetTimerID( TimerHandle_t xTimer, void *pvNewID )
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        vTimerSetTimerID( xTimer, pvNewID );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    BaseType_t MPU_xTimerIsTimerActive( TimerHandle_t xTimer )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTimerIsTimerActive( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    TaskHandle_t MPU_xTimerGetTimerDaemonTaskHandle( void )
    {
    TaskHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTimerGetTimerDaemonTaskHandle();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( INCLUDE_xTimerPendFunctionCall == 1 ) && ( configUSE_TIMERS == 1 ) )
    BaseType_t MPU_xTimerPendFunctionCall( PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, TickType_t xTicksToWait )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTimerPendFunctionCall( xFunctionToPend, pvParameter1, ulParameter2, xTicksToWait );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    const char * MPU_pcTimerGetName( TimerHandle_t xTimer )
    {
    const char * pcReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        pcReturn = pcTimerGetName( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return pcReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    TickType_t MPU_xTimerGetPeriod( TimerHandle_t xTimer )
    {
    TickType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTimerGetPeriod( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    TickType_t MPU_xTimerGetExpiryTime( TimerHandle_t xTimer )
    {
    TickType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTimerGetExpiryTime( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
    BaseType_t MPU_xTimerGenericCommand( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t * const pxHigherPriorityTaskWoken, const TickType_t xTicksToWait )
    {
    BaseType_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xTimerGenericCommand( xTimer, xCommandID, xOptionalValue, pxHigherPriorityTaskWoken, xTicksToWait );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    EventGroupHandle_t MPU_xEventGroupCreate( void )
    {
    EventGroupHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xEventGroupCreate();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    EventGroupHandle_t MPU_xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer )
    {
    EventGroupHandle_t xReturn;
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();

        xReturn = xEventGroupCreateStatic( pxEventGroupBuffer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait )
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xEventGroupWaitBits( xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xEventGroupClearBits( xEventGroup, uxBitsToClear );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xEventGroupSetBits( xEventGroup, uxBitsToSet );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSync( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait )
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    xReturn = xEventGroupSync( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

void MPU_vEventGroupDelete( EventGroupHandle_t xEventGroup )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    vEventGroupDelete( xEventGroup );
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/





/* Functions that the application writer wants to execute in privileged mode
can be defined in application_defined_privileged_functions.h.  The functions
must take the same format as those above whereby the privilege state on exit
equals the privilege state on entry.  For example:

void MPU_FunctionName( [parameters ] )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

    FunctionName( [parameters ] );

    vPortResetPrivilege( xRunningPrivileged );
}
*/

#if configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS == 1
    #include "application_defined_privileged_functions.h"
#endif
