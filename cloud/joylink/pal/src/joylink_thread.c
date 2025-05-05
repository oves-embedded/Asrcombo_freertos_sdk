#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>

#include "joylink.h"

//example.
#include "joylink_extern.h"
#include "joylink_extern_ota.h"
#include "joylink_extern_json.h"

//joylink.
#include "joylink_ret_code.h"

//pal.
#include "joylink_flash.h"
#include "joylink_log.h"
#include "joylink_memory.h"
#include "joylink_socket.h"
#include "joylink_softap.h"
#include "joylink_stdint.h"
#include "joylink_stdio.h"
#include "joylink_string.h"
#include "joylink_log.h"
#include "joylink_thread.h"
#include "joylink_time.h"

//lega board.
#include "lega_wlan_api.h"
// #include "lega_flash_kv.h"

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define JOYLINK_THREAD_MAX_LEN 5

/****************************************************************************
 * Public Data
****************************************************************************/
static lega_thread_t s_lega_thread[JOYLINK_THREAD_MAX_LEN];
static uint8_t s_task_id = 0, s_thread_st = 0;

/****************************************************************************
 * Private Data
****************************************************************************/

/****************************************************************************
 * Public Function
****************************************************************************/
/*********************************** mutex interface ***********************/
/** @defgroup group_platform_mutex mutex
 *  @{
 */

/**
 * @brief 创建互斥锁
 *
 * @retval NULL : Initialize mutex failed.
 * @retval NOT_NULL : The mutex handle.
 * @see None.
 * @note None.
 */
jl_mutex_t jl_platform_mutex_create(int32_t type)
{
    jl_mutex_t jl_platform_mutex = NULL;

    int stat = lega_rtos_init_mutex(&jl_platform_mutex);

    if (stat != kNoErr) {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_mutex_create error.\n");
#endif
        return NULL;
    }
    else
    {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_mutex_create ok --- %p\n", jl_platform_mutex);
#endif
        return jl_platform_mutex;
    }
}

/**
 * @brief 等待指定的互斥锁
 *
 * @param [in] handle @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
int32_t jl_platform_mutex_lock(jl_mutex_t handle)
{
    int32_t ret;

    if( lega_rtos_lock_mutex(&handle, LEGA_WAIT_FOREVER) == kNoErr)
    {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_mutex_lock ok --- %p\n", handle);
#endif
        ret = 0;
    }
    else
    {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_mutex_lock error\n");
#endif
        ret = -1;
    }
    return ret;
}

/**
 * @brief 释放指定的互斥锁
 *
 * @param [in] handle @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
int32_t jl_platform_mutex_unlock(jl_mutex_t handle)
{
    int32_t ret;

    if( lega_rtos_unlock_mutex(&handle) == kNoErr)
    {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_mutex_unlock ok --- %p\n", handle);
#endif
        ret = 0;
    }
    else
    {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_mutex_unlock error\n");
#endif
        ret = -1;
    }

    return ret;
}

/**
 * @brief 销毁互斥锁，并回收所占用的资源
 *
 * @param [in] handle @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void jl_platform_mutex_delete(jl_mutex_t handle)
{
    if( lega_rtos_deinit_mutex(&handle) == kNoErr)
    {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_mutex_delete ok --- %p\n", handle);
#endif
    }
    else
    {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_mutex_delete error\n");
#endif
    }
    return;
}

/**
 * @brief   创建一个计数信号量
 *
 * @return semaphore handle.
 * @see None.
 * @note The recommended value of maximum count of the semaphore is 255.
 */
jl_semaphore_t jl_platform_semaphore_create(void)
{
#ifdef __LINUX_PAL__
    jl_semaphore_t semaphore_t = NULL;
    return semaphore_t;
#else//

#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_semaphore_create\n");
#endif
    return NULL;
#endif
}

/**
 * @brief   销毁一个计数信号量, 回收其所占用的资源
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
void jl_platform_semaphore_destroy(jl_semaphore_t handle)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_semaphore_destroy\n");
#endif
}

/**
 * @brief   在指定的计数信号量上做自减操作并等待
 *
 * @param[in] sem @n the specified semaphore.
 * @param[in] timeout_ms @n timeout interval in millisecond.
     If timeout_ms is PLATFORM_WAIT_INFINITE, the function will return only when the semaphore is signaled.
 * @return
   @verbatim
   =  0: The state of the specified object is signaled.
   =  -1: The time-out interval elapsed, and the object's state is nonsignaled.
   @endverbatim
 * @see None.
 * @note None.
 */
void jl_platform_semaphore_wait(jl_semaphore_t handle, uint32_t timeout_ms)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_semaphore_wait\n");
#endif
}

/**
 * @brief   在指定的计数信号量上做自增操作, 解除其它线程的等待
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
void jl_platform_semaphore_post(jl_semaphore_t handle)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_semaphore_post\n");
#endif
}

/**
 * @brief   按照指定入参创建一个线程
 *
 * @param[out] thread_handle @n The new thread handle, memory allocated before thread created and return it, free it after thread joined or exit.
 * @param[in] pri @n thread priority
 * @param[in] stacksize @n stack size requirements in bytes
 * @return
   @verbatim
     = 0 : on success.
     = -1: error occur.
   @endverbatim
 * @see None.
 * @note None.
 */
int32_t  jl_platform_thread_create(jl_thread_t* thread_handle, JL_THREAD_PRI_T pri, size_t stacksize)
{
    int32_t ret = 0;

#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_thread_create\n");
#endif
    return ret;
}

/**
 * @brief   通过线程句柄启动指定得任务
 *
 * @param[in] thread_handle @n the thread handle
 * @param[in] task @n specify the task to start on thread_handle
 * @param[in] parameter @n user parameter input
 * @return
 * @see None.
 * @note None.
 */
void jl_platform_thread_start(jl_thread_t* thread_handle)
{
    jl_platform_printf("JL_PLATFORM_THREAD: %s  \r\n", __FUNCTION__);

#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_thread_start, thread_handle->p:%p\n", thread_handle->thread_task);
    jl_platform_printf("jl_platform_thread_start, thread_handle->stackSize: %ld\n", thread_handle->stackSize);
    jl_platform_printf("jl_platform_thread_start, thread_handle->priority: %d\n", thread_handle->priority);
#endif

    /* 由于joylink_main_start线程的子线程的name为乱码，导致freertos创建线程的时候，程序奔溃，因此对该子线程name重新赋值*/
    if (thread_handle->stackSize == 0x1000) //joylink_main_start线程的子线程stacksize，
    {
        thread_handle->name = "Joylink_Log";
    }

    s_thread_st = s_thread_st +1;

#ifdef __FREERTOS_PAL__
    if (0 != lega_rtos_create_thread(&s_lega_thread[s_task_id], thread_handle->priority, thread_handle->name, (lega_thread_function_t)thread_handle->thread_task, thread_handle->stackSize, 0))
    {
        jl_platform_printf("lega_rtos_create_thread failed(%d): %s\n", errno, strerror(errno));
    }
    s_task_id++;
#endif
}

/**
 * @brief   设置指定的线程为`Detach`状态
 *
 * @param[in] thread_handle: pointer to thread handle.
 * @return None.
 * @see None.
 * @note None.
 */
void jl_platform_thread_detach(jl_thread_t* thread_handle)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_thread_detach\n");
#endif
}

/**
 * @brief   线程主动退出
 *
 * @param[in] thread_handle: pointer to thread handle.
 * @return None.
 * @see None.
 * @note None.
 */
void jl_platform_thread_exit(jl_thread_t* thread_handle)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_thread_exit\n");
#endif
}

/**
 * @brief   杀死指定的线程
 *
 * @param[in] thread_handle: pointer to thread handle, NULL means itself
 * @return None.
 * @see None.
 * @note None.
 */
void jl_platform_thread_delete(jl_thread_t* thread_handle)
{
    if (s_thread_st == 0)
      return;

    for (uint8_t i = 0; i < JOYLINK_THREAD_MAX_LEN; i++)
    {
#ifdef JOYLINK_THREAD_DEBUG_INFO
        jl_platform_printf("jl_platform_thread_exit->pthread[%d]: %p %p\n", i, &s_lega_thread[i], *(&s_lega_thread[i]));
#endif
        if(*(&s_lega_thread[i]) != NULL)
        {
#ifdef JOYLINK_THREAD_DEBUG_INFO
            jl_platform_printf("lega_rtos_delete_thread->p:%p\n", *(&s_lega_thread[i]));
#endif
            lega_rtos_delete_thread(&s_lega_thread[i]);

            s_thread_st = s_thread_st - 1;
        }
    }
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_thread_exit\n");
#endif
}

/**
 * @brief   获取线程执行状态
 *
* @param[in] thread_handle: pointer to thread handle.
 * @return
    0:idel
    1:running
 * @see None.
 * @note None.
 */
int32_t jl_platform_thread_isrunning(jl_thread_t* thread_handle)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_platform_thread_isrunning\n");
#endif
    return 0;
}

/**
 * @brief 毫秒级休眠
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void  jl_platform_msleep(uint32_t ms)
{
#if defined __FREERTOS_PAL__
    lega_rtos_delay_milliseconds(ms);
#endif
}

/**
 * 创建定时器
 *
 * @param htimer:Timer handler
 * @return 0:success, -1:failed.
 *
 */
int32_t  jl_timer_create(jl_timer_t *htimer)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_timer_create\n");
#endif

    return 0;
}

/**
 * 启动定时器
 *
 * @param htimer:Timer handler
 * @return 0:success, -1:failed.
 */
int32_t  jl_timer_start(jl_timer_t *htimer)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_timer_start\n");
#endif

    return 0;
}

/**
 * 停止定时器
 *
 * @param htimer:Timer handler
 * @return 0:success, -1:failed.
 *
 */
int32_t jl_timer_stop(jl_timer_t *htimer)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_timer_stop\n");
#endif

    return 0;
}

/**
 * 删除定时器
 *
 * @param htimer:Timer handler
 * @return 0:success, -1:failed.
 *
 */
int32_t  jl_timer_delete(jl_timer_t *htimer)
{
#ifdef JOYLINK_THREAD_DEBUG_INFO
    jl_platform_printf("jl_timer_delete\n");
#endif

    return 0;
}

/** @} */ /* end of platform_mutex */
