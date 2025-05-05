

#ifndef    _FT_2_4G_H_
#define    _FT_2_4G_H_

#define HAL_2_4G_STATE_IDLE        0
#define HAL_2_4G_STATE_RX          1
#define HAL_2_4G_STATE_TX          2
#define HAL_2_4G_STATE_STOPPING    3
#define HAL_2_4G_STATE_DELAY       4

#define HAL_2_4G_NO_ERROR    0
#define HAL_2_4G_ERROR       -1


//#define ACCESS_ADDR  0x8E89BED6 //SIG
//#define HAL_ACCESS_ADDR 0x74737271

//#define HAL_ACCESS_ADDR 0x8E89BED6

#define HAL_ACCESS_ADDR 0x75577537

#define APP_HAL_DELAY_TIMER_ID      11
#define APP_HAL_CH_TIMER_ID         12
#define APP_HAL_END_MSG             21


#define APP_HAL_SCAN_CHG_INTERVAL   APP_RX_SCAN_CHG_INTERVAL   //uints:ms
#define APP_HAL_TX_INTERVAL   17   //uints:ms
typedef enum
{
    USER_INVALID_MODULE_ID ,
    USER_FT_APP_MODULE_ID   ,
    USER_WIFI_MODULE_ID   ,
    USER_MAX_MODULE_ID
}ft_module_id_t;


void ft_hal_init(void);
int ft_hal_2_4g_rx_data(uint16_t channel ,uint32_t access_word, uint16_t timeout_ms, PF_SONATA_2_4G_RX_CALLBACK cb,PF_SONATA_2_4G_TIMEOUT_CALLBACK end_cb);
int ft_hal_2_4g_tx_data(uint16_t channel ,uint32_t access_word, uint8_t *data, uint8_t len, PF_SONATA_2_4G_TX_CALLBACK cb);
int ft_hal_2_4g_repeat_tx_data (uint16_t channel ,uint32_t access_word, uint8_t *data, uint8_t len, PF_SONATA_2_4G_TX_CALLBACK cb,uint16_t repeat);
int ft_hal_2_4g_rx_stop(void);
void ft_hal_2_4g_set_rx_param_by_module(uint32_t interval, uint32_t windowSize,ft_module_id_t module_id);

#endif
