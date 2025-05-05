

#include <stdio.h>
#include "arch.h"
#include "app.h"                     // Application Definition
#include "sonata_ble_api.h"
#include "user_platform.h"
#include "sonata_log.h"
#include "sonata_utils_api.h"
#include "lega_rtos_api.h"
#include "sonata_private_protocol_api.h"
#ifdef CFG_PLF_DUET
#include "duet_gpio.h"
#endif
#include "ft_2_4g_hal.h"


typedef struct
{
    uint8_t is_valid;
    uint32_t interval;
    uint32_t win;
}hal_rx_param_t;


#define HAL_MAX_BUF 128
PF_SONATA_2_4G_TX_CALLBACK  hal_tx_cb = NULL;
PF_SONATA_2_4G_RX_CALLBACK  hal_rx_cb = NULL;
PF_SONATA_2_4G_TIMEOUT_CALLBACK hal_tx_end_cb = NULL;
PF_SONATA_2_4G_TIMEOUT_CALLBACK hal_rx_end_cb = NULL;
uint32_t hal_2_4g_rx_interval = APP_RX_INTERVAL;
uint32_t hal_2_4g_rx_win = APP_RX_WINSZ;
uint8_t  hal_2_4g_tx_data_buf[HAL_MAX_BUF];
uint8_t  hal_buf_len = 0;
uint8_t  ch_change_flag = 0;
uint16_t hal_last_ch = 2424;
uint16_t hal_rx_backoff = 0;
uint16_t hal_tx_counter = 0;
uint32_t hal_access_word = HAL_ACCESS_ADDR;
hal_rx_param_t hal_rx_param[USER_MAX_MODULE_ID] = {0};
uint16_t hal_tx_ch = 2424;
#define FT_HAL_DBG 0

#if FT_HAL_DBG
#define FT_HAL_TRC    printf
#else
#define FT_HAL_TRC(...)
#endif //FT_HAL_DBG

static uint8_t hal_current_state = HAL_2_4G_STATE_IDLE;

uint8_t ft_hal_2_4g_end_msg_handler(void *param);
static uint8_t ft_hal_timer_handler(uint16_t id);
uint8_t ft_hal_rx_end_cb(uint16_t channel,uint8_t status);
uint8_t ft_hal_rx_pkt_cb(int8_t rssi,uint16_t channel,uint8_t *data,uint8_t length);
extern void sonata_2_4g_driver_reg_check_cb(void* cb);
uint8_t arrow_hal_2_4g_rx_msg_handler(void *param);
void arrow_repeat_tx_data(void);
void arrow_start_action(void);
uint8_t arrow_hal_2_4g_timeout_msg_handler(void *param);
extern uint16_t app_register_app_timer_callback(sonata_app_timer_callback_t *cb);

static sonata_app_timer_callback_t ft_hal_timer_callbacks = {
        .timeout                  = ft_hal_timer_handler,
};

static const sonata_api_app_msg_t halMsgs[] = {
        {APP_HAL_END_MSG, ft_hal_2_4g_end_msg_handler},
};

uint8_t ft_hal_pkt_check(uint8_t *data,uint8_t length)
{
    if(length > 10 && length <  50 )
    {
        return 1;
    }
    return 0;
}

uint16_t hal_get_other_ch(uint16_t channel)
{
    if(2424 == channel)
    {
        return 2424;
    }
    return 2424;
}

void ft_hal_2_4g_set_rx_param(uint32_t interval, uint32_t windowSize)
{
    hal_2_4g_rx_interval = interval;
    hal_2_4g_rx_win      = windowSize;
}

static bool ft_hal_compare(uint32_t interval_a, uint32_t windowSize_a,uint32_t interval_b, uint32_t windowSize_b)
{
    if(interval_a*windowSize_b < windowSize_a*interval_b)
    {
        return true;
    }
    return false;
}

void ft_hal_2_4g_set_rx_param_by_module(uint32_t interval, uint32_t windowSize,ft_module_id_t module_id)
{
    if(module_id >= USER_MAX_MODULE_ID)
    {
        return;
    }
    FT_HAL_TRC("APP: %s  %lu %lu\r\n", __FUNCTION__,interval,windowSize);
    hal_rx_param[module_id].is_valid = true;
    hal_rx_param[module_id].interval = interval;
    hal_rx_param[module_id].win = windowSize;
    uint32_t init_interval = 0xFFFF;
    uint32_t init_windowSize = 0xFFFF;
    for(int i = USER_INVALID_MODULE_ID + 1; i < USER_MAX_MODULE_ID; i++)
    {
         if(hal_rx_param[i].is_valid)
         {
             if(ft_hal_compare(init_interval,init_windowSize,hal_rx_param[i].interval,hal_rx_param[i].win))
             {
                 init_interval = hal_rx_param[i].interval;
                 init_windowSize = hal_rx_param[i].win;
             }
         }
    }
    if(init_interval != 0xFFFF && init_windowSize != 0xFFFF)
    {
         ft_hal_2_4g_set_rx_param(init_interval, init_windowSize);
    }
    else
    {
        ft_hal_2_4g_set_rx_param(APP_RX_INTERVAL, APP_RX_WINSZ);
    }
}

static uint8_t ft_hal_timer_handler(uint16_t id)
{
    FT_HAL_TRC("APP: %s  %d\r\n", __FUNCTION__,id);

    if (id  == APP_HAL_CH_TIMER_ID)
    {
        //if(hal_rx_backoff)
        {
            GLOBAL_INT_DISABLE();
            ch_change_flag = 1;
            sonata_api_app_timer_set(APP_HAL_CH_TIMER_ID,APP_HAL_SCAN_CHG_INTERVAL);
            GLOBAL_INT_RESTORE();
        }
    }
    else if (id  == APP_HAL_DELAY_TIMER_ID)
    {
        GLOBAL_INT_DISABLE();
        if(hal_current_state == HAL_2_4G_STATE_DELAY)
        {
            hal_current_state = HAL_2_4G_STATE_RX;
            //hal_rx_end_cb  = NULL;
            if(ch_change_flag)
            {
                hal_last_ch = hal_get_other_ch(hal_last_ch);
                ch_change_flag = 0;
            }
            sonata_2_4g_rx_data(hal_last_ch,hal_access_word,hal_2_4g_rx_win,ft_hal_rx_pkt_cb,ft_hal_rx_end_cb);
        }
        else
        {
             FT_HAL_TRC("APP: error %s  %d \r\n", __FUNCTION__,hal_current_state);
        }
        GLOBAL_INT_RESTORE();

    }
	return 0;
}

uint8_t ft_hal_2_4g_end_msg_handler(void *param)
{
    FT_HAL_TRC("APP: %s  \r\n", __FUNCTION__);

    //uint16_t channel = (uint16_t)param;
    if(hal_2_4g_rx_interval == hal_2_4g_rx_win && hal_current_state == HAL_2_4G_STATE_DELAY)
    {
        GLOBAL_INT_DISABLE();
        hal_current_state = HAL_2_4G_STATE_RX;
        //hal_rx_end_cb  = NULL;
        if(ch_change_flag)
        {
            hal_last_ch = hal_get_other_ch(hal_last_ch);
            ch_change_flag = 0;
        }
        sonata_2_4g_rx_data(hal_last_ch,hal_access_word,hal_2_4g_rx_win,ft_hal_rx_pkt_cb,ft_hal_rx_end_cb);
        GLOBAL_INT_RESTORE();
    }
    else
    {
        FT_HAL_TRC("APP: %s  %d %d\r\n", __FUNCTION__,hal_rx_backoff,hal_current_state);
        GLOBAL_INT_DISABLE();
        if(hal_rx_backoff && hal_current_state  == HAL_2_4G_STATE_DELAY)
        {
            sonata_api_app_timer_set(APP_HAL_DELAY_TIMER_ID,hal_2_4g_rx_interval-hal_2_4g_rx_win);
        }
        GLOBAL_INT_RESTORE();
    }
    return 0;
}

uint8_t ft_hal_send_data_cb(uint16_t channel,uint8_t status)
{
    FT_HAL_TRC("APP: %s  %d %d\r\n", __FUNCTION__,hal_current_state,hal_tx_counter);
    hal_current_state = HAL_2_4G_STATE_IDLE;
    if(hal_tx_counter > 1)
    {
        hal_tx_counter -- ;
        hal_current_state = HAL_2_4G_STATE_TX;
        sonata_2_4g_tx_data(channel ,hal_access_word, hal_2_4g_tx_data_buf, hal_buf_len, ft_hal_send_data_cb);
    }
    else if(hal_tx_cb == NULL)   //user call
    {

    }
    else     //duty call
    {
        hal_current_state = HAL_2_4G_STATE_IDLE;
        PF_SONATA_2_4G_TX_CALLBACK  temp_hal_tx_cb = hal_tx_cb;
        hal_tx_cb = NULL;
        temp_hal_tx_cb(channel,status);
    }
    return HAL_2_4G_NO_ERROR;
}


int ft_hal_2_4g_tx_data(uint16_t channel, uint32_t access_word, uint8_t *data, uint8_t len, PF_SONATA_2_4G_TX_CALLBACK cb)
{
    FT_HAL_TRC("APP: %s  %d\r\n", __FUNCTION__,hal_current_state);
    if(len<=6)
    {
        return HAL_2_4G_NO_ERROR;
    }
    GLOBAL_INT_DISABLE();
    if(hal_current_state == HAL_2_4G_STATE_TX)
    {
        FT_HAL_TRC("APP: %s  tx is running\r\n", __FUNCTION__);
        GLOBAL_INT_RESTORE();
        return HAL_2_4G_ERROR;
    }
    else if(hal_current_state == HAL_2_4G_STATE_RX)
    {
        if(hal_rx_backoff != 1)
        {
            FT_HAL_TRC("APP: %s  RX is running\r\n", __FUNCTION__);
        }
        hal_access_word = access_word;
        hal_current_state = HAL_2_4G_STATE_STOPPING;
        hal_tx_cb = cb;
        hal_buf_len = len;
        hal_tx_ch = channel;
        memmove(hal_2_4g_tx_data_buf,data,len);
        hal_rx_backoff = 0;
        sonata_2_4g_rx_stop();
        GLOBAL_INT_RESTORE();
    }
    else if(hal_current_state == HAL_2_4G_STATE_STOPPING)
    {
        hal_access_word = access_word;
        hal_tx_cb = cb;
        hal_buf_len = len;
        hal_tx_ch = channel;
        memmove(hal_2_4g_tx_data_buf,data,len);
        GLOBAL_INT_RESTORE();
    }
    else if(hal_current_state == HAL_2_4G_STATE_DELAY)
    {
        hal_access_word = access_word;
        hal_rx_backoff = 0;
        hal_current_state = HAL_2_4G_STATE_TX;
        hal_tx_cb = cb;
        hal_tx_ch = channel;
        sonata_2_4g_tx_data( channel ,access_word, data, len, ft_hal_send_data_cb);
        GLOBAL_INT_RESTORE();
    }
    else
    {
        hal_access_word = access_word;
        hal_rx_backoff = 0;
        hal_tx_cb = cb;
        hal_current_state = HAL_2_4G_STATE_TX;
        sonata_2_4g_tx_data( channel ,access_word, data, len, ft_hal_send_data_cb);
        GLOBAL_INT_RESTORE();
    }
    return HAL_2_4G_NO_ERROR;
}


int ft_hal_2_4g_repeat_tx_data (uint16_t channel ,uint32_t access_word, uint8_t *data, uint8_t len, PF_SONATA_2_4G_TX_CALLBACK cb,uint16_t repeat)
{
    int result = HAL_2_4G_ERROR;
    hal_tx_counter = repeat;
    hal_buf_len = len;
    memmove(hal_2_4g_tx_data_buf,data,len);
    result = ft_hal_2_4g_tx_data(channel, access_word, data, len, cb);
    return result;
}


uint8_t ft_hal_rx_end_cb(uint16_t channel,uint8_t status)
{
    FT_HAL_TRC("APP: %s  %d %d\r\n", __FUNCTION__,channel,hal_current_state);

    if(hal_current_state == HAL_2_4G_STATE_RX)
    {
        if(hal_rx_backoff == 1)
        {
            //duty cicle
            hal_current_state = HAL_2_4G_STATE_DELAY;
            sonata_api_send_app_msg(APP_HAL_END_MSG, (void *)&channel);
        }
        else if(hal_rx_end_cb != NULL)
        {
            PF_SONATA_2_4G_TIMEOUT_CALLBACK temp_end_cb = hal_rx_end_cb;
            hal_rx_end_cb = NULL;
            hal_current_state = HAL_2_4G_STATE_IDLE;
            temp_end_cb(channel,status);
        }
        else
        {
		    hal_current_state = HAL_2_4G_STATE_IDLE;
            FT_HAL_TRC("APP: %s  rx error \r\n", __FUNCTION__);
        }
    }
    else if ( hal_current_state == HAL_2_4G_STATE_STOPPING)
    {
        if(hal_tx_cb != NULL)
        {
            hal_current_state = HAL_2_4G_STATE_TX;
            sonata_2_4g_tx_data(hal_tx_ch, hal_access_word, hal_2_4g_tx_data_buf, hal_buf_len, ft_hal_send_data_cb);
        }
        else
        {
		    hal_current_state = HAL_2_4G_STATE_IDLE;
	        if(hal_rx_end_cb != NULL)
	        {
	            PF_SONATA_2_4G_TIMEOUT_CALLBACK temp_end_cb = hal_rx_end_cb;
	            hal_rx_end_cb = NULL;
	            temp_end_cb(channel,status);
	        }
        }
    }
    else
    {
        hal_current_state = HAL_2_4G_STATE_IDLE;
        FT_HAL_TRC("APP: %s  rx end error\r\n", __FUNCTION__);
    }
    FT_HAL_TRC("APP: hal_current_state  %d\r\n", hal_current_state);
    return HAL_2_4G_NO_ERROR;
}

uint8_t ft_hal_rx_pkt_cb(int8_t rssi,uint16_t channel,uint8_t *data,uint8_t length)
{
       FT_HAL_TRC("APP: hal_current_state  %d %d\r\n", hal_current_state,length);
       // hal_current_state = HAL_2_4G_STATE_IDLE;
        if(hal_rx_cb)
        {
             //PF_SONATA_2_4G_RX_CALLBACK temp_rx_cb = hal_rx_cb;
             //if(hal_rx_backoff == 0)
             //{
             //hal_rx_cb = NULL;
             //}
             hal_rx_cb(rssi, channel,data,length);
        }

        return 0;
}

/***********************************************************
*  Function: 2.4G开始接收数据
*  Input:  channel：rx的信道
*  Input:  access_word：access code
*  Input:  timeout_ms：接收超时。单位ms。0：底层开始自动AB接收。其他：普通的一次RX
*  Input:  cb：收到数据的cb
*  Input:  end_cb：普通RX结束的cb
*  Output: none
*  Return: none
***********************************************************/
int ft_hal_2_4g_rx_data(uint16_t channel , uint32_t access_word, uint16_t timeout_ms, PF_SONATA_2_4G_RX_CALLBACK cb, PF_SONATA_2_4G_TIMEOUT_CALLBACK end_cb)
{
    FT_HAL_TRC("APP: %s  %d \r\n", __FUNCTION__,hal_current_state);

    GLOBAL_INT_DISABLE();
    if(hal_current_state != HAL_2_4G_STATE_IDLE  && hal_current_state != HAL_2_4G_STATE_DELAY)
    {
        GLOBAL_INT_RESTORE();
        return HAL_2_4G_ERROR;
    }
    else if(hal_current_state == HAL_2_4G_STATE_STOPPING)  //todo for next
    {
        FT_HAL_TRC("duplicate rx !!!\r\n");
        hal_rx_end_cb = end_cb;
        hal_access_word = access_word;
        hal_last_ch = channel;
        if(timeout_ms == 0)
        {
            hal_rx_backoff  = 1;
        }
		hal_rx_cb	= cb;
        GLOBAL_INT_RESTORE();
    }
    else
    {
        if(hal_current_state == HAL_2_4G_STATE_DELAY)
        {
            //sonata_api_app_timer_clear(APP_HAL_DELAY_TIMER_ID);
        }
        GLOBAL_INT_RESTORE();
        hal_access_word = access_word;
        hal_current_state = HAL_2_4G_STATE_RX;
        hal_rx_end_cb  = end_cb;
        hal_last_ch = channel;
        hal_rx_cb   = cb;
        if(timeout_ms == 0)
        {
            hal_rx_backoff  = 1;
            timeout_ms = hal_2_4g_rx_win;
        }
        else
        {
            hal_rx_backoff = 0;
        }
        GLOBAL_INT_DISABLE();
        sonata_2_4g_rx_data(channel, access_word, timeout_ms, ft_hal_rx_pkt_cb, ft_hal_rx_end_cb);
        if(hal_rx_backoff)
        {
            //sonata_api_app_timer_set(APP_HAL_CH_TIMER_ID, APP_HAL_SCAN_CHG_INTERVAL);
        }
        GLOBAL_INT_RESTORE();
    }
    return HAL_2_4G_NO_ERROR;
}

/***********************************************************
*  Function: 2.4G停止接收
*  Input:  none
*  Output: none
*  Return: none
***********************************************************/
int ft_hal_2_4g_rx_stop(void)
{
    FT_HAL_TRC("APP: %s  r\n", __FUNCTION__);

    hal_rx_backoff = 0;
    GLOBAL_INT_DISABLE();
    if(hal_current_state == HAL_2_4G_STATE_RX)
    {
        hal_current_state = HAL_2_4G_STATE_STOPPING;
        sonata_2_4g_rx_stop();
    }
    //sonata_api_app_timer_clear(APP_HAL_CH_TIMER_ID);
    //sonata_api_app_timer_clear(APP_HAL_DELAY_TIMER_ID);
    GLOBAL_INT_RESTORE();
    return 0;
}

/***********************************************************
*  Function: 2.4G HAL层初始化。（这个函数必须放在app.c中的app_init()中！）
*  Input:  none
*  Output: none
*  Return: none
***********************************************************/
void ft_hal_init(void)
{
    hal_rx_backoff = 0;
    hal_current_state = 0;
    hal_tx_counter = 0;
    hal_tx_cb = NULL;
    hal_rx_cb = NULL;
    hal_tx_end_cb = NULL;
    hal_rx_end_cb = NULL;
    hal_2_4g_rx_interval = APP_RX_INTERVAL;
    hal_2_4g_rx_win = APP_RX_WINSZ;
    memset(hal_rx_param,0,sizeof(hal_rx_param));
    sonata_2_4g_driver_reg_check_cb(ft_hal_pkt_check);
    app_register_app_timer_callback(&ft_hal_timer_callbacks);
    sonata_api_app_msg_register(&halMsgs[0]);
    sonata_api_app_timer_set(APP_HAL_CH_TIMER_ID, APP_HAL_SCAN_CHG_INTERVAL);
}


