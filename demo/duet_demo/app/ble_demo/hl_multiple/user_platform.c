/**
 ****************************************************************************************
 *
 * @file user_platform.c
 *
 * @brief source file - user platform specific implementation
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "arch.h"
#include "user_platform.h"
#include "duet.h"
#include "system_cm4.h"
#include "app_platform.h"



//#include "reg_assert_mgr.h"
#include "sonata_ble_api.h"
#include "sonata_ble_transport.h"

/*
 * DEFINES
 ****************************************************************************************
 */


//pf_mem_config_t pf_mem_config_patch = {
//        .ble_activity_max   = SONATA_CFG_ACT,           //BLE_ACTIVITY_MAX,
//        .ble_connection_max = SONATA_CFG_CON,           //BLE_CONNECTION_MAX,
//        .ble_wlist_max      = SONATA_CFG_WLIST,         //
//        .ble_tx_desc_nb     = SONATA_CFG_ACT * (2 + 5), //BLE_TX_DESC_NB,
//        .ble_rx_desc_nb     = SONATA_CFG_RX_DESC_NB,    //BLE_RX_DESC_NB,
//        .ble_adv_buf_nb_tx  = SONATA_CFG_ACT,           //BLE_ADV_BUF_NB_TX,
//        .ble_acl_buf_nb_tx  = SONATA_CFG_ACT + 2,       //BLE_ACL_BUF_NB_TX,
//        .ble_ral_max        = SONATA_CFG_RAL,           //BLE_RAL_MAX,
//        .gap_max_le_mtu     = SONATA_GAP_MAX_LE_MTU,    //GAP_MAX_LE_MTU,
//        .hl_heap_db_size    = SONATA_CFG_HL_HEAP_DB_SIZE, //BLEHL_HEAP_DB_SIZE,
//};


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
//uint32_t app_clk_sel = DIG_SEL_DPLL_64M;
void (*pf_app_clk_reconfig)(void) = NULL;

/*
* FUNCTION DEFINITIONS
****************************************************************************************
*/
/*****************************************************************************************
* @brief direction finding pinmux initialization
* @param[in] uint32_t sys_clk : system clock
*
*
* @return void
****************************************************************************************/
#if (PLF_DF)
void df_init(void)
{
    sonata_pinmux_config(PAD8,PF_DF); // P08 - DD_ANTSW_SEL[0]
    sonata_pinmux_config(PAD9,PF_DF); // P09 - DD_ANTSW_SEL[1]
}
#endif //PLF_DF

/*****************************************************************************************
* @brief wlan coex init
* @param[in] uint32_t sys_clk : system clock
*
*
* @return void
****************************************************************************************/
#if (PLF_PTA)
void pta_init(void)
{
    sonata_pinmux_config(PAD13,PF_PTA); // P13 - PTA_BLE_PTI0
    sonata_pinmux_config(PAD14,PF_PTA); // P14 - PTA_BLE_PTI1
    sonata_pinmux_config(PAD23,PF_PTA); // P23 - PTA_BLE_PTI2
    sonata_pinmux_config(PAD24,PF_PTA); // P24 - PTA_BLE_PTI3
    sonata_pinmux_config(PAD19,PF_PTA); // P19 - PTA_BLE_INPROCESS
    sonata_pinmux_config(PAD20,PF_PTA); // P20 - PTA_WLAN_ABORT

    *(volatile int *)(0x40106000 + 0x4*0x2A) &= ~(3 << 7);
    *(volatile int *)(0x40106000 + 0x4*0x2A) |= (2 << 7);

    *(volatile int *)(0x40100140) = 0x000500f3; // PTA tx/rx mask
}
#endif // (PLF_PTA)

/*****************************************************************************************
* @brief ble hci transport definition
* @param[in] void
*
*
* @return void
****************************************************************************************/
#if (PLF_BLE_TRANSPORT)
void ble_trans_init_pad_config(void)
{
    app_pinmux_config(PAD0,PF_UART0);
    app_pinmux_config(PAD1,PF_UART0);
    app_pinmux_config(PAD10,PF_UART0);
    app_pinmux_config(PAD11,PF_UART0);
}

void ble_trans_wakeup_pad_config(void)
{

}

void ble_trans_flow_on_pad_config(void)
{

}

void ble_trans_flow_off_pad_config(void)
{

}

void ble_trans_init(void)
{
    sonata_ble_transport_pad_config_t config;
    config.pf_init_pad_config = ble_trans_init_pad_config;
    config.pf_wakeup_pad_config= ble_trans_wakeup_pad_config;
    config.pf_flow_on_pad_config= ble_trans_flow_on_pad_config;
    config.pf_flow_off_pad_config = ble_trans_flow_off_pad_config;

    sonata_ble_transport_init(&config);
}

#endif // (PLF_BLE_TRANSPORT)


/*****************************************************************************************
* @brief user platform peripheral initialization
* @param[in] void
*
*
* @return void
****************************************************************************************/
void user_peri_init(void)
{
    // set system clock
    //app_clk_sel = DIG_SEL_DPLL_64M;
    pf_app_clk_reconfig = NULL;

    // Initialize UART component
#if PLF_UART1
    asr_uart_set_callback(UART1_INDEX, at_handle_uartirq);
    app_pinmux_config(PAD2,PF_UART1); // pad04 uart1 tx
    app_pinmux_config(PAD3,PF_UART1); // pad05 uart1 rx
    app_uart_dev_t uart_param;
    app_uart_struct_init(&uart_param);
    uart_param.port = UART1_INDEX;
    asr_uart_init(&uart_param);
    printf_uart_register(UART1_INDEX);
    //asr_uart_interrupt_config(UART1, (UART_RX_INTERRUPT | UART_RX_TIMEOUT_INTERRUPT), ENABLE);
    ////NVIC_EnableIRQ(UART1_IRQn);
#endif //PLF_UART1

#if PLF_UART2
    app_pinmux_config(PAD8,PF_UART2); // pad08 uart1 tx
    app_pinmux_config(PAD9,PF_UART2); // pad09 uart1 rx
    app_uart_dev_t uart_param;
    app_uart_struct_init(&uart_param);
    uart_param.port = UART2_INDEX;
    asr_uart_init(&uart_param);
    printf_uart_register(UART2_INDEX);
    asr_uart_interrupt_config(UART2, (UART_RX_INTERRUPT | UART_RX_TIMEOUT_INTERRUPT), ENABLE);
    ////NVIC_EnableIRQ(UART2_IRQn);
#endif //PLF_UART2

#if PLF_BLE_TRANSPORT
    app_dma_init();
    ble_trans_init();
#endif //PLF_UART0

#if PLF_DF
    df_init();
#endif

#if PLF_PTA
    pta_init();
#endif

}

/*****************************************************************************************
* @brief user platform peripheral initialization
* @param[in] void
*
*
* @return void
****************************************************************************************/
void user_peri_deinit(void)
{
#if PLF_UART1
    ECLIC_DisableIRQ(UART1_IRQn);
#endif //PLF_UART1

#if PLF_UART2
    ECLIC_DisableIRQ(UART2_IRQn);
#endif //PLF_UART2

#if PLF_BLE_TRANSPORT
//    {
//        asr_uart_dev_t uart_config_struct  = {0};
//        uart_config_struct.port = UART0_INDEX;
//        asr_uart_finalize(&uart_config_struct);
//    }
#endif
}

/*****************************************************************************************
* @brief assert err: user specific asser implementation
* @param[in]
*
*
* @return void
****************************************************************************************/

extern void assert_wf_err(const char *condition, const char * file, int line);

#if defined(CFG_PLF_SONATA) || defined(CFG_PLF_RV32)
void assert_err(uint16_t id, int cond)
{
    USER_PLF_TRC("ASSERT ERROR: id=%d cond=%d\r\n",id, cond);
    while(1);
}
void assert_param(uint16_t id, int param0, int param1)
{
    USER_PLF_TRC("ASSERT PARAM:id %d p0 %d p1 %d \r\n", id, param0,param1);
    while(1);
}
void assert_warn(uint16_t id, int param0, int param1)
{
    USER_PLF_TRC("ASSERT WARN:id %d p0 %d p1 %d \r\n",id, param0,param1);
}
#elif defined(CFG_PLF_DUET)
void assert_err(int cond)
{
    USER_PLF_TRC("BLE_ERR:  cond=%d\r\n", cond);
    assert_wf_err(0, "ble_assert", 100);
}
void assert_param(int param0, int param1)
{
    USER_PLF_TRC("BLE_PARA:p0 %d p1 %d \r\n",  param0,param1);
}

void assert_warn(int param0, int param1)
{
    USER_PLF_TRC("BLE_WRN: p0 %d p1 %d \r\n",param0,param1);
}
#endif


/*****************************************************************************************
* @brief dump data: user specific dump implementation
* @param[in]
*
*
* @return void
****************************************************************************************/
void dump_data(uint8_t* data, uint16_t length)
{


}


/*****************************************************************************************
* @brief get stack usage: user specific implementation
* @param[in]
*
*
* @return void
****************************************************************************************/
uint16_t get_stack_usage(void)
{

      return 0xFFFF; //@asr
}

/*****************************************************************************************
* @brief platform reset: user specific implementation
* @param[in]
*
*
* @return void
****************************************************************************************/
void platform_reset(uint32_t error)
{
    reset_error = error;
//    NVIC_SystemReset();
}




