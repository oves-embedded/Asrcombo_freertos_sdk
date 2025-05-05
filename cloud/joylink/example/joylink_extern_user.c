/* --------------------------------------------------
 * @file: joylink_extern_tool.C
 *
 * @brief:
 *
 * @version: 2.0
 *
 * @date: 2018/07/26 PM
 *
 * --------------------------------------------------
 */

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "joylink_extern_user.h"
#include "joylink_stdio.h"
#include "joylink_stdint.h"

//duet platform
#include "duet_gpio.h"

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
//#define HAL_GPIO_DEBUG

/****************************************************************************
 * Public Data
****************************************************************************/


/****************************************************************************
 * Private Data
****************************************************************************/
static FILE *file_fp = NULL;
static duet_gpio_dev_t s_duet_gpio;


/****************************************************************************
 * Private Function
****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/
int joylink_memory_init(void *index, int flag)
{
    char *save_path = (char *)index;

    if(file_fp > 0)
        return -1;

    jl_platform_printf("joylink_memory_init save_path: %s\n", save_path);

    if(flag == MEMORY_WRITE){
        file_fp = jl_platform_fopen(save_path, "wb+");
        if(file_fp == NULL){
            jl_platform_printf("Open file error!\n");
            return -1;
        }
    }
    else if(flag == MEMORY_READ){
        file_fp = jl_platform_fopen(save_path, "rb");
        if(file_fp == NULL){
            jl_platform_printf("Open file error!\n");
            return -1;
        }
    }
    return 0;
}

int joylink_memory_write(int offset, char *data, int len)
{
    if(file_fp == NULL || data == NULL)
        return -1;

    return jl_platform_fwrite(data, 1, len, file_fp);
}

int joylink_memory_read(int offset, char *data, int len)
{
    if(file_fp == NULL || data == NULL)
        return -1;

    return jl_platform_fread(data, 1, len, file_fp);
}

int joylink_memory_finish(void)
{
    if(file_fp == NULL)
        return -1;

    jl_platform_fclose(file_fp);
    file_fp = NULL;
    return 0;
}

int joylink_led_init(void)
{
    int pin = GPIO6_INDEX;

    if ((pin < 0) || (pin >= DUET_GPIO_NUM_PER_GROUP))
    {
      return 1;
    }

    /* Configure GPIO_InitStructure */
    s_duet_gpio.port = pin;
    s_duet_gpio.config = DUET_OUTPUT_PUSH_PULL;
    s_duet_gpio.priv = NULL;

    duet_gpio_init(&s_duet_gpio);
    duet_gpio_output_low(&s_duet_gpio);

    return 0;
}

int joylink_led_on(void)
{
    duet_gpio_output_low(&s_duet_gpio);

    return 0;
}

int joylink_led_off(void)
{
    duet_gpio_output_high(&s_duet_gpio);

    return 0;
}


