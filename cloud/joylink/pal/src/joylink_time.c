#include <stddef.h>
#include <stdint.h>

#include <time.h>
#include <sys/time.h>

#include "joylink_time.h"
#include "joylink_string.h"
#include "joylink_memory.h"
#include "joylink_socket.h"
#include "joylink_stdio.h"
#include "joylink_stdint.h"
#include "joylink_log.h"
#include "joylink_thread.h"

//lega board.
#include "lega_wlan_api.h"
// #include "lega_flash_kv.h"

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/

/****************************************************************************
 * Public Data
****************************************************************************/


/****************************************************************************
 * Private Data
****************************************************************************/
static jl_time_stamp_t s_UTC_timestamp;
static uint32_t s_UTC_timestamp_temp = 0;

/****************************************************************************
 * Public Function
****************************************************************************/
/**
 * 初始化系统时间
 *
 * @return: 0 - success or -1 - fail
 *
 */
int32_t jl_set_UTCtime(jl_time_stamp_t time_stamp)
{
    s_UTC_timestamp.second = time_stamp.second;
    s_UTC_timestamp.ms = time_stamp.ms;

    s_UTC_timestamp_temp = lega_rtos_get_time();

#ifdef JOYLINK_TIME_DEBUG_INFO
    jl_platform_printf("jl_set_UTCtime->time_stamp.second: %ld  time_stamp.ms: %ld\n", time_stamp.second, time_stamp.ms);//1
    jl_platform_printf("jl_set_UTCtime->s_UTC_timestamp_temp: %ldms\n", s_UTC_timestamp_temp);
#endif

    return 0;
}

/**
 * 获取系统时间
 *
 * @out param: time
 * @return: success or fail
 *
 */
int32_t jl_time_get_time(jl_time_t *jl_time)
{
// #ifdef __LINUX_PAL__
//     time_t timep;
//     struct tm *p;
//     jl_time->timestamp = (uint32_t) time(&timep);
//     p = gmtime(&timep);
//     jl_time->year      = p->tm_year;
//     jl_time->month     = p->tm_mon;
//     jl_time->week      = p->tm_wday;
//     jl_time->day       = p->tm_mday;
//     jl_time->hour      = p->tm_hour;
//     jl_time->minute    = p->tm_min;
//     jl_time->second    = p->tm_sec;
// #endif

#ifdef JOYLINK_TIME_DEBUG_INFO
    jl_platform_printf("jl_time_get_time\n");
#endif

    return 0;
}

/**
* @brief 获取当前系统时间ms
* @param none
* @return time ms
*/
uint32_t jl_time_get_timestamp_ms(jl_time_stamp_t *time_stamp)
{
    return lega_rtos_get_time();
}

/**
 * 获取系统UTC时间
 *
 * @return: UTC Second
 *
 */
uint32_t jl_time_get_timestamp(jl_time_t *jl_time)
{
#ifdef JOYLINK_TIME_DEBUG_INFO
    jl_platform_printf("jl_time_get_timestamp->year: %ld\n", jl_time->year);
    jl_platform_printf("jl_time_get_timestamp->month: %d\n", jl_time->month);
    jl_platform_printf("jl_time_get_timestamp->week: %d\n", jl_time->week);
    jl_platform_printf("jl_time_get_timestamp->day: %d\n", jl_time->day);
    jl_platform_printf("jl_time_get_timestamp->hour: %d\n", jl_time->hour);
    jl_platform_printf("jl_time_get_timestamp->minute: %d\n", jl_time->minute);
    jl_platform_printf("jl_time_get_timestamp->second: %d\n", jl_time->second);
    jl_platform_printf("jl_time_get_timestamp->timestamp: %ld\n", jl_time->timestamp);
    jl_platform_printf("1:jl_time_get_timestamp->s_UTC_timestamp.second: %ld  s_UTC_timestamp.ms: %ld\n", s_UTC_timestamp.second, s_UTC_timestamp.ms);
#endif

    uint32_t time_temp = lega_rtos_get_time() - s_UTC_timestamp_temp;
    s_UTC_timestamp.ms += time_temp;
    if(s_UTC_timestamp.ms >= 1000)//1000ms
    {
        s_UTC_timestamp.second += s_UTC_timestamp.ms/1000;
        s_UTC_timestamp.ms = s_UTC_timestamp.ms%1000;
    }

#ifdef JOYLINK_TIME_DEBUG_INFO
    jl_platform_printf("jl_time_get_timestamp->time_temp: %ldms\n", time_temp);
    jl_platform_printf("2:jl_time_get_timestamp->s_UTC_timestamp.second: %ld  s_UTC_timestamp.ms: %ld\n", s_UTC_timestamp.second, s_UTC_timestamp.ms);
#endif

    return s_UTC_timestamp.second;
}

/**
 * 获取系统时间
 *
 * @out param: "year-month-day hour:minute:second"
 * @return: success or fail
 *
 */
int32_t jl_get_time_str(char *out, int32_t len)
{
#ifdef JOYLINK_TIME_DEBUG_INFO
    jl_platform_printf("jl_get_time_str\n");
#endif

    return 0;
}

/**
 * get os time
 *
 * @out param: none
 * @return: sys time ticks ms since sys start
*/
uint32_t jl_get_os_time(void)
{
#ifdef JOYLINK_TIME_DEBUG_INFO
    jl_platform_printf("jl_get_os_time\n");
#endif

#ifdef __FREERTOS_PAL__
        return (uint32_t) jl_time_get_timestamp_ms(NULL);// FIXME do not recommand this method
#else
        return 0;
#endif
}
