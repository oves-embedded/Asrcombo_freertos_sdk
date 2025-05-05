/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "lega_wlan_api.h"
#include "tcpip.h"
#include "lega_at_api.h"
#include "soc_init.h"
#include "lwip_app_iperf.h"
#include "duet_version.h"
#include "duet_uart.h"
#include "duet_pinmux.h"
#include "duet_flash_kv.h"
#define LEGA_UART0_INDEX DUET_UART0_INDEX
#define LEGA_UART1_INDEX DUET_UART1_INDEX
#define LEGA_UART2_INDEX DUET_UART2_INDEX
#include "lega_rtos_api.h"
#include "lega_ota_utils.h"
#include "elog.h"
#include "cloud.h"
#include "ds1302.h"
#include "payg.h"
#include "gatt.h"
#include "app_custom_svc.h"
#include "eeprom.h"
#include "ovapp.h"

#ifdef SECURITY_ENGINE_INIT
#include "asr_sec_hw_common.h"
#endif

#define     TEST_TASK_NAME              "timestamp_task"
#define     TEST_TASK_PRIORITY          2
#define     TEST_STACK_SIZE             2048
TaskHandle_t timestamp_task_Handler;
#ifdef CLOUD_TEST
void cloud_test(void);
#endif
void timestamp_task(void *arg)
{
	int timestamp = 10000;
   
    while(1)
    {
        printf("daemon task...  \n" );

		lega_rtos_delay_milliseconds(timestamp);
    	}
}


#define UART1_TX_PIN PAD2
#define UART1_RX_PIN PAD3

extern void printf_uart_init(duet_uart_dev_t* uart);
void at_uart_init(void)
{
    int32_t len = 0;
    uint8_t kv_param_ok = 0;
    memset(&lega_at_uart,0,sizeof(lega_at_uart));
    len = sizeof(lega_at_uart);
	
	
    if((duet_flash_kv_get("uart_config_def", &lega_at_uart,&len) == 0)&&(lega_at_uart.config.baud_rate !=0)\
        &&(lega_at_uart.config.baud_rate !=0xffffffff))
    {
        if(!((lega_at_uart.config.data_width > DATA_8BIT)||(lega_at_uart.config.stop_bits > STOP_2BITS)\
            ||(lega_at_uart.config.flow_control > FLOW_CTRL_CTS_RTS)||(lega_at_uart.config.parity > PARITY_EVEN)))
        {
            kv_param_ok = 1;
            lega_at_uart.config.mode = TX_RX_MODE;
        }
    }
    if(!kv_param_ok)
    {
        lega_at_uart.config.baud_rate=UART_BAUDRATE_115200;
        lega_at_uart.config.data_width = DATA_8BIT;
        lega_at_uart.config.flow_control = FLOW_CTRL_DISABLED;
        lega_at_uart.config.parity = PARITY_NO;
        lega_at_uart.config.stop_bits = STOP_1BIT;
        lega_at_uart.config.mode = TX_RX_MODE;
    }

    lega_at_uart.port=DUET_UART1_INDEX;
    duet_pinmux_config(UART1_TX_PIN,PF_UART1);
    duet_pinmux_config(UART1_RX_PIN,PF_UART1);
    //register uart callback func for receiving at command
    lega_at_uart.priv = (void *)(at_handle_uartirq);
    //duet_uart_init(&lega_at_uart);
    printf_uart_init(&lega_at_uart);
}

extern void lega_sram_rf_pta_init(void);
extern void lega_recovery_phy_fsm_config(void);
extern void lega_wlan_efuse_read(void);
extern void printf2_uart_register(uint8_t uart_idx);
extern int init_ble_task(void);


int main(void)
{
    //don't run any code before soc_pre_init.
    soc_pre_init();

    soc_init();

    duet_flash_kv_init();

    //uart init and register uart for receiving at command
    at_uart_init();

    //ota roll back,just for flash_remapping
    ota_roll_back_pro();

    //disable wdg for #7892
    ota_wdg_disable();

    //register uart for printf log, the used uart should be init before.
    printf_uart_register(LEGA_UART1_INDEX);
    //register uart for at log, the used uart should be init before.
    printf2_uart_register(LEGA_UART1_INDEX);

    /* set EasyLogger log format */
#if (CFG_EASY_LOG_ENABLE==1)
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_start();
#endif
    printf("\napp version: %s\n",LEGA_VERSION_STR);
    printf("\ninit with free heap %d\n",xPortGetFreeHeapSize());

    lega_wlan_efuse_read();

    //set certifacation type before rf init, param 0x00 is old srrc, 0x10 is new srrc
    lega_wlan_set_cert_type(0x00);

    lega_sram_rf_pta_init();

    lega_recovery_phy_fsm_config();

#ifdef SECURITY_ENGINE_INIT
    asr_security_engine_init();
#endif

    lega_at_init();

    lega_at_cmd_register_all();

    lega_wlan_init();

    tcpip_init(NULL,NULL);

	ovapp_uart_init();
	ovapp_gpio_init();
	EEpInit();
	ov_gatt_init();
	app_custom_svc_gatt_create();

    lega_wifi_iperf_init();
    init_ble_task();
	
    ovapp_init();
    xTaskCreate(timestamp_task,TEST_TASK_NAME,TEST_STACK_SIZE>>2,0,TEST_TASK_PRIORITY,&timestamp_task_Handler);

    vTaskStartScheduler();
}


