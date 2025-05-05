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

#ifdef AT_USER_DEBUG
#include <math.h>
#include <string.h>
#include "lega_at_api.h"
#ifdef CFG_PLF_DUET
#include "duet_common.h"
#include "duet_uart.h"
#include "duet_cm4.h"
#include "duet_flash.h"
#include "duet_flash_kv.h"
#include "duet_version.h"
#define lega_flash_kv_set duet_flash_kv_set
#define lega_flash_kv_get duet_flash_kv_get
#define lega_flash_kv_del duet_flash_kv_del
#define lega_flash_erase  duet_flash_erase
#define lega_flash_write  duet_flash_write
#define lega_flash_read   duet_flash_read
#define lega_system_reset duet_system_reset
#else
#include "lega_common.h"
#include "lega_uart.h"
#include "lega_cm4.h"
#include "lega_flash.h"
#include "lega_flash_kv.h"
#include "lega_version.h"
#include "system_cm4.h"
#endif
#include "FreeRTOS.h"
#include "task.h"

#ifdef CFG_SDIO_SUPPORT
#include "lega_sdio.h"
#endif

#define UART_RXBUF_LEN            128
#define MIN_USEFUL_DEC            32
#define MAX_USEFUL_DEC            127
#define UART_CMD_NB               5
#define ARGCMAXLEN                14
#define AT_QUEUE_SIZE             UART_CMD_NB

uint8_t uart_idx=0;
uint8_t cur_idx = 0;
uint8_t start_idx = 0;
uint8_t uart_buf_len[UART_CMD_NB] = {0};
char uart_buf[UART_CMD_NB][UART_RXBUF_LEN];
lega_thread_t AT_Task_Handler = 0;
lega_queue_t  at_task_msg_queue = 0;

#ifdef CFG_PLF_DUET
duet_uart_dev_t lega_at_uart;
#else
lega_uart_dev_t lega_at_uart;
#endif

_at_user_info at_user_info = {0};
struct cli_cmd_t *cli_cmd = NULL;
lega_semaphore_t at_cmd_protect;

#define AT_CMD_TIMEOUT          3000
#define TXPWR_ADD_MAX           2.0
#define TXPWR_11B_MAX           18.0
#define TXPWR_MAX               20
#define CHN_FREQ_INTERV         5
#define CHN1_FREQ               2412

float rf_txpwr_max = TXPWR_ADD_MAX;

extern float pwr_user_set_k;
extern _at_user_info at_user_info;
extern void lega_rfinit_pwr_cal(void);
extern const char *lega_get_wifi_version(void);

char at_dbgflg = 1;

#ifdef CFG_SDIO_SUPPORT
extern uint8_t g_sdio_msg_enable;
#endif
void lega_at_response(lega_at_rsp_status_t status)
{

#ifndef CFG_SDIO_SUPPORT
    lega_rtos_declare_critical();
    lega_rtos_enter_critical();
#endif

    if(status == CONFIG_OK)
    {
#ifdef  CFG_SDIO_SUPPORT
        if (g_sdio_msg_enable)
        {
            uint8_t msg[2] = {'O', 'K'};
            sdio_host_rx_msg_hdr(msg, 2);
        }
        else
        {
            at_rspinfor("OK");
        }
#else
        at_rspinfor("OK");
#endif
        lega_rtos_set_semaphore(&at_cmd_protect);
    }
    else if(status == WAIT_PEER_RSP)
    {
        lega_rtos_set_semaphore(&at_cmd_protect);
    }
    else if(status != RSP_NULL)
    {
#ifdef  CFG_SDIO_SUPPORT
        if (g_sdio_msg_enable)
        {
            uint8_t msg[7] = {'F','A','I','L',':','-',0};
            msg[6] = (uint8_t)status + '0';
            sdio_host_rx_msg_hdr(msg, 7);
        }
        else
        {
            at_rspinfor("FAIL:-%d",status);
        }
#else
        at_rspinfor("FAIL:-%d",status);
#endif
        lega_rtos_set_semaphore(&at_cmd_protect);
    }

#ifndef CFG_SDIO_SUPPORT
    lega_rtos_exit_critical();
#endif
}

/*
 ************************************************************
 *                    COMMON AT CMD START
 *
 ************************************************************
 */
static int lega_at_dbg(int argc, char **argv)
{
    int onoff = 0;

    if(argc < 2)
    {
        return PARAM_MISS;
    }

    onoff = convert_str_to_int(argv[1]);
    if((onoff > 1) || (onoff < 0))
    {
        return PARAM_RANGE;
    }
    else
    {
        at_dbgflg = onoff;
        return CONFIG_OK;
    }
}

static int lega_at_reset(int argc, char **argv)
{
    dbg_at("doing...");
    lega_at_response(CONFIG_OK);
    delay(5000);
    lega_system_reset();

    return RSP_NULL;
}
static int lega_at_show_version(int argc, char **argv)
{
    at_rspdata("version sdk:%s", lega_get_wifi_version());
    at_rspdata("version app:%s", LEGA_VERSION_STR);
    at_rspdata("version at:%s", LEGA_AT_VERSION);

    return CONFIG_OK;
}
static int lega_at_echo(int argc, char **argv)
{
    int echo = 0;

    if(argc < 2)
    {
        return PARAM_MISS;
    }

    echo = convert_str_to_int(argv[1]);
    if((echo > 1) || (echo < 0))
    {
        return PARAM_RANGE;
    }
    else
    {
        at_user_info.uart_echo = echo?0:1;
        return CONFIG_OK;
    }
}
static int lega_at_recovery(int argc, char **argv)
{
    int ret = 0;

    dbg_at("doing...");
#ifdef CFG_PLF_DUET
#ifdef CFG_DUET_5822S
    ret = lega_flash_erase(PARTITION_PARAMETER_2, 0, KV_MAX_SIZE);
    ret |= lega_flash_erase(PARTITION_PARAMETER_4, 0, NVDS_MAX_SIZE);
#else
    ret = lega_flash_erase(PARTITION_PARAMETER_2, 0, KV_MAX_SIZE);
#endif
#else
    lega_rtos_declare_critical();
    lega_rtos_enter_critical();
    ret = lega_flash_erase(PARTITION_PARAMETER_2, 0, KV_MAX_SIZE);
    lega_rtos_exit_critical();
#endif

    if(ret)
        return CONFIG_FAIL;
    else
    {
        lega_at_response(CONFIG_OK);
        lega_system_reset();
        return RSP_NULL;
    }
}

static int lega_at_set_max_rftxpwr(int argc, char **argv)
{
    int max_txpwr;
    if(argc < 2)
    {
        return PARAM_MISS;
    }

    max_txpwr = convert_str_to_int(argv[1]);
    if((max_txpwr > TXPWR_MAX) || (max_txpwr < 0))
    {
        dbg_at("max txpwr range [0, %d] dbm.\r\n", TXPWR_MAX);
        return PARAM_RANGE;
    }
    else
    {
        at_user_info.max_txpwr = max_txpwr;

        // max power base on 11B tx power
        rf_txpwr_max = at_user_info.max_txpwr -TXPWR_11B_MAX;

        return CONFIG_OK;
    }
}

extern uint16_t lega_rf_get_center_freq(void);
extern void lega_rfinit_pwr_cal(void);
extern void lega_ram_cfg_clip(uint8_t chn);
extern void lega_dfecail_write(uint16_t chn);
static int lega_at_add_rftxpow(int argc, char**argv)
{
    uint8_t chn = 0;
    float gain = 1.0;

    if(argc < 2)
    {
        return PARAM_MISS;
    }

    gain = atof(argv[1]);

    if (gain < (0 - TXPWR_MAX))
    {
        return PARAM_RANGE;
    }

    if(rf_txpwr_max < 0)
        gain += rf_txpwr_max;

    if(gain > rf_txpwr_max)
    {
        dbg_at("tx limit to %u dbm\n",at_user_info.max_txpwr);

        return PARAM_RANGE;
    }

    pwr_user_set_k = pow(10.0,((double)gain/20.0));

    lega_rfinit_pwr_cal();
    chn = (lega_rf_get_center_freq() -CHN1_FREQ)/CHN_FREQ_INTERV + 1;
    dbg_at("channel is %d\r\n",chn);
    lega_dfecail_write(chn);
    lega_ram_cfg_clip(chn);

    return CONFIG_OK;
}

#ifdef CFG_PLF_DUET
static char* uart_flowCtl_to_str(uart_flow_control_t flowCtl)
#else
static char* uart_flowCtl_to_str(lega_uart_flow_control_t flowCtl)
#endif
{
    switch(flowCtl){
        case FLOW_CTRL_DISABLED:     return "DISABLE";
        case FLOW_CTRL_CTS:            return "CTS";
        case FLOW_CTRL_RTS:            return "RTS";
        case FLOW_CTRL_CTS_RTS:        return "CTS_RTS";
        default:                     return "invalid";
    }
}

#ifdef CFG_PLF_DUET
static char* uart_parity_to_str(uart_parity_t parity)
#else
static char* uart_parity_to_str(lega_uart_parity_t parity)
#endif
{
    switch(parity){
        case PARITY_NO:        return "None";
        case PARITY_ODD:    return "ODD";
        case PARITY_EVEN:    return "EVEN";
        default:             return "invalid";
    }
}
static int lega_at_uart_config(int argc, char **argv)
{
    int tempValue;
#ifdef CFG_PLF_DUET
    duet_uart_dev_t uart_modify_dev = {0};
#else
    lega_uart_dev_t uart_modify_dev = {0};
#endif

    if( argc < 5)
    {
        dbg_at("param num not enough!!!\r\n");
        return PARAM_MISS;
    }
    tempValue = convert_str_to_int(argv[1]);
    if(tempValue == LEGA_STR_TO_INT_ERR || tempValue <= 0)
    {
        dbg_at("baud rate param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.baud_rate = tempValue;
    tempValue = convert_str_to_int(argv[2]);
    if((tempValue > 9) || (tempValue < 5))
    {
        dbg_at("data_width param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.data_width = (tempValue - 5);
    tempValue = convert_str_to_int(argv[3]);
    if((tempValue <= 0) || (tempValue > 2))
    {
        dbg_at("stop_bits param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.stop_bits = (tempValue - 1);
    tempValue = convert_str_to_int(argv[4]);
    if((tempValue > 2) || (tempValue < 0))
    {
        dbg_at("parity param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.parity = tempValue;
    tempValue = convert_str_to_int(argv[5]);
    if((tempValue > 3) || (tempValue < 0))
    {
        dbg_at("flow_control param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.flow_control = tempValue;

    dbg_at("uart baud:%d ,databits:%d ,stop:%d ,parity:%s ,flow:%s\r\n",
        (int)uart_modify_dev.config.baud_rate,
        uart_modify_dev.config.data_width+5,
        uart_modify_dev.config.stop_bits+1,
        uart_parity_to_str(uart_modify_dev.config.parity),
        uart_flowCtl_to_str(uart_modify_dev.config.flow_control));

    uart_modify_dev.port = lega_at_uart.port;

    //register uart callback func for receiving at command
    uart_modify_dev.priv = (void *)(at_handle_uartirq);
    #ifdef CFG_PLF_DUET
    #else
    if(lega_uart_dev_check_valid(&uart_modify_dev)<0)
        return CONFIG_FAIL;
    #endif

    lega_at_response(CONFIG_OK);

    delay(5000);

    #ifdef CFG_PLF_DUET
    duet_uart_finalize(&uart_modify_dev);
    duet_uart_init(&uart_modify_dev);
    #else
    lega_uart_finalize(&uart_modify_dev);
    lega_uart_init(&uart_modify_dev);
    #endif

    return RSP_NULL;
}

static int lega_at_uart_config_def(int argc, char **argv)
{
#ifdef CFG_PLF_DUET
    duet_uart_dev_t uart_modify_dev = {0};
#else
    lega_uart_dev_t uart_modify_dev = {0};
#endif

    int32_t len = 0;
    int tempValue;
    if( argc < 5)
    {
        dbg_at("config param num not enough!!!\r\n");
        return PARAM_MISS;
    }
    tempValue = convert_str_to_int(argv[1]);
    if(tempValue == LEGA_STR_TO_INT_ERR || tempValue <= 0)
    {
        dbg_at("baud rate param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.baud_rate = tempValue;
    tempValue = convert_str_to_int(argv[2]);
    if((tempValue > 9) || (tempValue < 5))
    {
        dbg_at("data_width param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.data_width = (tempValue - 5);
    tempValue = convert_str_to_int(argv[3]);
    if((tempValue <= 0) || (tempValue > 2))
    {
        dbg_at("stop_bits param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.stop_bits = (tempValue - 1);
    tempValue = convert_str_to_int(argv[4]);
    if((tempValue > 2) || (tempValue < 0))
    {
        dbg_at("parity param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.parity = tempValue;
    tempValue = convert_str_to_int(argv[5]);
    if((tempValue > 3)  || (tempValue < 0))
    {
        dbg_at("flow_control param err!!!\r\n");
        return PARAM_RANGE;
    }
    uart_modify_dev.config.flow_control = tempValue;

    dbg_at("uart baud:%d ,databits:%d ,stop:%d ,parity:%s ,flow:%s\r\n",
        (int)uart_modify_dev.config.baud_rate,
        uart_modify_dev.config.data_width+5,
        uart_modify_dev.config.stop_bits+1,
        uart_parity_to_str(uart_modify_dev.config.parity),
        uart_flowCtl_to_str(uart_modify_dev.config.flow_control));

    len = sizeof(uart_modify_dev);
    lega_flash_kv_set("uart_config_def", &uart_modify_dev,len,1);

    uart_modify_dev.port = lega_at_uart.port;

    delay(5000);//wait for all data output, or else have messy code

    //register uart callback func for receiving at command
    uart_modify_dev.priv = (void *)(at_handle_uartirq);
    #ifdef CFG_PLF_DUET
    duet_uart_finalize(&uart_modify_dev);
    duet_uart_init(&uart_modify_dev);
    #else
    lega_uart_finalize(&uart_modify_dev);
    lega_uart_init(&uart_modify_dev);
    #endif
    return CONFIG_OK;
}

static int lega_at_uart_config_def_del(int argc, char **argv)
{
    lega_flash_kv_del("uart_config_def");

    return CONFIG_OK;
}
//#define AT_SET_KV_TEST
#ifdef AT_SET_KV_TEST
#define AT_SET_KV_LEN   256
uint8_t kv_index = 0;
static int lega_at_comm_set_kv(int argc, char **argv)
{
    uint8_t kv_data[AT_SET_KV_LEN];
    uint8_t kv_get[AT_SET_KV_LEN];
    int32_t len= AT_SET_KV_LEN;
    //(*(volatile uint32_t *)(0x08)) = 0x123;   //for coredump test
    for(int i = 0; i < AT_SET_KV_LEN; i++)
        kv_data[i] = kv_index + i;
    kv_index++;
    duet_flash_kv_set("kv_for_test", kv_data, AT_SET_KV_LEN, 1);
    duet_flash_kv_get("kv_for_test", kv_get, &len);
    if(len != AT_SET_KV_LEN)
    {
        printf("%s kv get error len:%ld!!!!!\n", __func__, len);
        return CONFIG_OK;
    }
    for(int j = 0; j < AT_SET_KV_LEN; j++)
    {
        if(kv_data[j] != kv_get[j])
        {
            printf("%s kv get error j:%d!!!!!\n", __func__, j);
            return CONFIG_OK;
        }
    }
    printf("%s kv get OK!!!!!\n", __func__);

    return CONFIG_OK;
}

void kv_list_callback(char *key, uint8_t key_len, char *value, uint16_t val_len)
{
    printf("key :%s   val_len :%d\n", key, val_len);
}

static int lega_at_comm_list_kv(int argc, char **argv)
{
    printf("List all kv items:\n");

    duet_flash_kv_list(kv_list_callback);

    return CONFIG_OK;
}
#endif

#ifdef SYSTEM_COREDUMP
extern void coredump_command_register(int argc, char **argv);
static int lega_at_coredump(int argc, char **argv)
{
    coredump_command_register(argc, argv);

    return CONFIG_OK;
}
#endif

#define VLIST_DEBUG_SIZE 2048
int lega_at_vlist(int argc, char **argv)
{
    char v_list[VLIST_DEBUG_SIZE] = {0};
    vTaskList(v_list);
    at_rspinfor("%s",v_list);
    at_rspinfor("left free heap %d\r\n",xPortGetFreeHeapSize());
    memset(v_list,0,VLIST_DEBUG_SIZE);

    return CONFIG_OK;
}
#ifdef CFG_CPU_USAGE
#define STATS_DEBUG_SIZE 2048
int lega_at_vtasktime(int argc, char **argv)
{
    char buffer[STATS_DEBUG_SIZE] = {0};
    at_rspinfor("Tasks           \tAbs Time(0.1ms)\t%% Time");
    vTaskGetRunTimeStats(buffer);
    at_rspinfor("%s",buffer);
    memset(buffer,0,STATS_DEBUG_SIZE);

    return CONFIG_OK;
}
#endif

int lega_at_show_command(int argc, char **argv)
{
    int i;

    if(argc ==  1)
    {
        at_rspinfor("%s",cli_cmd->cmds[0]->help);

        for(i = 1; i < cli_cmd->cmds_num; i++)
        {
            //cmd begin with "lega_" is internal cmd
            if(memcmp(cli_cmd->cmds[i]->name, "lega", 4) != 0)
            {
                at_rspinfor("%s\r\n",cli_cmd->cmds[i]->name);
            }
        }
    }
    else if(argc == 2)
    {
        for(i = 0; i < cli_cmd->cmds_num; i++)
        {
            if(strcmp(argv[1],cli_cmd->cmds[i]->name) == 0)
            {
                if(cli_cmd->cmds[i]->help)
                {
                    at_rspinfor("%s",cli_cmd->cmds[i]->help);
                }

                break;
            }
        }
        if(i == cli_cmd->cmds_num)
        {
            at_rspinfor("help not support cmd %s ",argv[1]);
            return CONFIG_FAIL;
        }

    }
    else
    {
        return CONFIG_FAIL;
    }

    return CONFIG_OK;
}

/*
 ************************************************************
 *                    COMMON AT CMD END
 *
 ************************************************************
 */

cmd_entry comm_help = {
    .name = "help",
    .help = "cmd format:   help\r\n\
cmd function: AT command as below, use \"help name\" for more details\r\n",
    .function = lega_at_show_command,
};
cmd_entry comm_dbg = {
    .name = "dbg",
    .help = "cmd format:   dbg <on/off>\r\n\
cmd function: show at set execute prompt info\r\n\
onoff: 0:off, 1:on, default off\r\n",
    .function = lega_at_dbg,
};
cmd_entry comm_reset = {
    .name = "reset",
    .help = "cmd format:   reset\r\n\
cmd function: soc soft reset\r\n",
    .function = lega_at_reset,
};
cmd_entry comm_version = {
    .name = "version",
    .help = "cmd format:   version\r\n\
cmd function: show release version\r\n",
    .function = lega_at_show_version,
};
cmd_entry comm_echo = {
    .name = "echo",
    .help = "cmd format:   echo <on/off>\r\n\
cmd function: open or close uart display for input cmd\r\n\
onoff: 0:off, 1:on, default on\r\n",
    .function = lega_at_echo,
};
cmd_entry comm_recovery = {
    .name = "recovery",
    .help = "cmd format:   recovery\r\n\
cmd function: erase all kv in flash and reboot\r\n",
    .function = lega_at_recovery,
};
cmd_entry comm_setmaxpwr = {
    .name = "set_max_txpwr",
    .help = "cmd format:   set_max_txpwr <power>\r\n\
cmd function: set max RF tx power\r\n",
    .function = lega_at_set_max_rftxpwr,
};
cmd_entry comm_setpwr = {
    .name = "set_txpwr",
    .help = "cmd format:   set_txpwr <power>\r\n\
cmd function: add or decrease RF tx power base on current power\r\n",
    .function = lega_at_add_rftxpow,
};

cmd_entry comm_uart_config = {
    .name = "uart_config",
    .help = "cmd format:  uart_config <baudrate> <databits> <stopbits> <parity> <flow control>\r\n\
data_bit:   5,6,7,8,9\r\n\
stop_bit:   1,2\r\n\
parity_bit: 0:no parity,1:odd,2:even\r\n\
flow_ctrl:  0:flow_ctrl disable,1:flow_ctrl cts,2:flow_ctrl rts,3:flow_ctrl cts rts\r\n\
cmd function: set uart config param\r\n",
    .function = lega_at_uart_config,
};
cmd_entry comm_uart_config_def = {
    .name = "uart_config_def",
    .help = "cmd format:   uart_config_def <baudrate> <databits> <stopbits> <parity> <flow control>\r\n\
data_bit:   5,6,7,8,9\r\n\
stop_bit:   1,2\r\n\
parity_bit: 0:no parity,1:odd,2:even\r\n\
flow_ctrl:  0:flow_ctrl disable,1:flow_ctrl cts,2:flow_ctrl rts,3:flow_ctrl cts rts\r\n\
cmd function: set uart config param & save to kv\r\n",
    .function = lega_at_uart_config_def,
};
cmd_entry comm_uart_config_def_del = {
    .name = "uart_config_def_del",
    .help = "cmd format:   uart_config_def_del\r\n\
cmd function: del uart def config param from kv\r\n",
    .function = lega_at_uart_config_def_del,
};
#ifdef AT_SET_KV_TEST
cmd_entry comm_set_kv = {
    .name = "set_kv",
    .help = "cmd format:   comm_set_kv key val\r\n\
cmd function: set a kv item\r\n",
    .function = lega_at_comm_set_kv,
};
cmd_entry comm_list_kv = {
    .name = "kv_list",
    .help = "cmd format:   kv_list\r\n\
cmd function: list all kv items\r\n",
    .function = lega_at_comm_list_kv,
};
#endif
#ifdef SYSTEM_COREDUMP
cmd_entry comm_coredump = {
    .name = "coredump",
    .help = "cmd format:   coredump [ram|flash] [all|pos [memory|wifi|info|rf|peri]]\r\n\
cmd function: dump the key memory and information stored after system crashed, use together with DOGO tool\r\n",
    .function = lega_at_coredump,
};
#endif
cmd_entry comm_vtasklist = {
    .name = "vtasklist",
    .help = "cmd format:   vtasklist\r\n\
cmd function: show vtasklist status\r\n",
    .function = lega_at_vlist,
};

#ifdef CFG_CPU_USAGE
cmd_entry comm_vtasktime = {
    .name = "vtasktime",
    .help = "cmd format:   vtasktime\r\n\
cmd function: show task time stats\r\n",
    .function = lega_at_vtasktime,
};
#endif

void lega_at_comm_cmd_register(void)
{
    lega_at_cmd_register(&comm_help);
    lega_at_cmd_register(&comm_dbg);
    lega_at_cmd_register(&comm_reset);
    lega_at_cmd_register(&comm_version);
    lega_at_cmd_register(&comm_echo);
    lega_at_cmd_register(&comm_recovery);
    lega_at_cmd_register(&comm_setmaxpwr);
    lega_at_cmd_register(&comm_setpwr);
    lega_at_cmd_register(&comm_uart_config);
    lega_at_cmd_register(&comm_uart_config_def);
    lega_at_cmd_register(&comm_uart_config_def_del);
#ifdef SYSTEM_COREDUMP
    lega_at_cmd_register(&comm_coredump);
#endif
    lega_at_cmd_register(&comm_vtasklist);
#ifdef CFG_CPU_USAGE
    lega_at_cmd_register(&comm_vtasktime);
#endif
#ifdef AT_SET_KV_TEST
    lega_at_cmd_register(&comm_set_kv);
    lega_at_cmd_register(&comm_list_kv);
#endif
}

/*
 ************************************************************
 *               at cmd register start
 *
 ************************************************************
 */
void lega_at_cmd_register(cmd_entry *cmd)
{
    if(cli_cmd->cmds_num > AT_MAX_COMMANDS - 1)
    {
        printf("register cmd:%s fail,reach max cmd table list.",cmd->name);
        return;
    }

    cli_cmd->cmds[cli_cmd->cmds_num] = cmd;
    cli_cmd->cmds_num++;
}

extern void lega_at_dbg_cmd_register(void);
extern void lega_at_rftest_cmd_register(void);
extern void lega_at_wifi_cmd_register(void);
#ifdef CFG_PLF_DUET
extern void lega_at_ble_cmd_register(void);
extern void lega_at_ble_perf_test_cmd_register(void);
#endif
extern void lega_at_net_cmd_register(void);
extern void lega_at_user_cmd_register(void);
extern void lega_at_test_cmd_register(void);
extern void lega_at_cloud_cmd_register(void);
extern void lega_at_ota_cmd_register(void);
void lega_at_cmd_register_all(void)
{
    cli_cmd = lega_rtos_malloc(sizeof(struct cli_cmd_t));
    if(cli_cmd == NULL)
    {
        printf("cli_cmd malloc failed!");
        return;
    }

    lega_at_dbg_cmd_register();
    lega_at_rftest_cmd_register();
    lega_at_comm_cmd_register();
    lega_at_wifi_cmd_register();
#if (defined CFG_PLF_DUET && defined CFG_DUET_BLE)
    lega_at_ble_cmd_register();
#ifdef SONATA_CFG_PERF_TEST
    lega_at_ble_perf_test_cmd_register();
#endif
#endif
    lega_at_net_cmd_register();
    lega_at_user_cmd_register();
    lega_at_test_cmd_register();
    lega_at_cloud_cmd_register();
    lega_at_ota_cmd_register();
}
/*
 ************************************************************
 *               at cmd register end
 *
 ************************************************************
 */

/*
 ************************************************************
 *           AT task and cmd process start
 *
 ************************************************************
 */
void at_handle_uartirq(char ch)
{
    uint32_t msg_queue_elmt = 0;

    if(AT_Task_Handler && !(((start_idx + 1)%UART_CMD_NB) == cur_idx))
    {
        if(ch =='\r')//in windows platform '\r\n' added to end
        {
            uart_buf[start_idx][uart_idx]='\0';
            uart_buf_len[start_idx]=uart_idx;
            uart_idx=0;
            if(start_idx == UART_CMD_NB -1)
                start_idx = 0;
            else
                start_idx++;

            lega_rtos_push_to_queue(&at_task_msg_queue, &msg_queue_elmt, LEGA_NO_WAIT);
        }
        else if(ch =='\b')
        {
            if(uart_idx>0)
                uart_buf[start_idx][--uart_idx] = '\0';
        }
        else if(ch > (MIN_USEFUL_DEC - 1) && ch<(MAX_USEFUL_DEC + 1))
        {
            uart_buf[start_idx][uart_idx++] = ch;
            if(uart_idx > UART_RXBUF_LEN - 1)
            {
                dbg_at("error:uart char_str length must <= 127,return\n");
                uart_idx=0;
            }
        }
    }
}

/*
 *********************************************************************
 * @brief make argv point to uart_buf address,return number of command
 *
 * @param[in] buf point to uart_buf
 * @param[in] argv point to char *argv[ARGCMAXLEN];
 *
 * @return number of command param
 *********************************************************************
 */
static int parse_cmd(char *buf, char **argv)
{
    int argc = 0;
    //buf point to uart_buf
    while((argc < ARGCMAXLEN) && (*buf != '\0'))
    {
        argv[argc] = buf;
        argc++;
        buf++;
        //space and NULL character
        while((*buf != ' ') && (*buf != '\0'))
            buf++;
        //one or more space characters
        while(*buf == ' ')
        {
            *buf = '\0';
            buf ++;
         }
    }
    return argc;
}

void lega_at_cmd_protect(void)
{
    if(lega_rtos_get_semaphore(&at_cmd_protect, AT_CMD_TIMEOUT))
        dbg_at("pre cmd is running\n");
}

void at_command_process(void)
{
    int i, argc;
    char *argv[ARGCMAXLEN];
    int ret;

    if((argc = parse_cmd(uart_buf[cur_idx], argv)) > 0)
    {
        for(i = 0; i < cli_cmd->cmds_num; i++)
        {
            if(strcmp(argv[0], cli_cmd->cmds[i]->name) == 0)
            {
                lega_at_cmd_protect();
                ret = cli_cmd->cmds[i]->function(argc, argv);
                lega_at_response(ret);
                break;
            }
        }
    }
    memset(uart_buf[cur_idx],0,UART_RXBUF_LEN);

    if(cur_idx == UART_CMD_NB - 1)
    {
        cur_idx = 0;
    }else
        cur_idx ++;
}

#ifdef CFG_SDIO_SUPPORT
static void lega_at_host_tx_msg_hdlr_register(sdio_cb_hdr msg_hdr)
{
    sdio_register_msg_callback(msg_hdr);
}

static void lega_at_sdio_msg_hdlr(uint8_t * data, uint32_t data_len)
{
    if (AT_Task_Handler && data != NULL && data_len > 0 && data_len <= 128)
    {
        char ch;
        uint32_t cp_len = 0;
        uint8_t * p_sdio_at_cmd = NULL;
        for (uint32_t index = 0; index < data_len; ++index)
        {
            ch = *(data + index);
            if(ch =='\r' || ch == '\0' || ch == '\n')
            {
                cp_len = index;
                break;
            }
            else if (index == data_len - 1)
            {
                cp_len = index + 1;
                break;
            }
        }

        if (cp_len > 0)
        {
            p_sdio_at_cmd = (uint8_t *)lega_rtos_malloc(cp_len + 1);
            if (p_sdio_at_cmd == NULL)
            {
                printf("alloc NULL\n");
            }

            g_sdio_msg_enable = 1;
            memcpy(p_sdio_at_cmd, data, cp_len);
            /* set last char '\0'*/
            *(p_sdio_at_cmd + cp_len) = 0;
            at_msg_t at_msg = {0};
            at_msg.sdio_data_len = data_len;
            at_msg.p_sdio_at_cmd = (uint32_t)p_sdio_at_cmd;
            lega_rtos_push_to_queue(&at_task_msg_queue, &at_msg, LEGA_NO_WAIT);
        }
        else
        {
            printf("invalid cmd\n");
            return ;
        }

    }
    else
    {
        printf("param invalid\n");
    }
}
static void at_sdio_command_process(char * p_sdio_cmd)
{
    if (p_sdio_cmd == NULL)
    {
        printf("cmd NULL\n");
        return;
    }

    int i, argc;
    char *argv[ARGCMAXLEN];
    int ret;

    if((argc = parse_cmd(p_sdio_cmd, argv)) > 0)
    {
        for(i = 0; i < cli_cmd->cmds_num; i++)
        {
            if(strcmp(argv[0], cli_cmd->cmds[i]->name) == 0)
            {
                lega_at_cmd_protect();
                ret = cli_cmd->cmds[i]->function(argc, argv);
                lega_at_response(ret);
                break;
            }
        }
    }
    if (p_sdio_cmd)
    {
        lega_rtos_free(p_sdio_cmd);
        p_sdio_cmd = NULL;
    }
}
#endif

extern int32_t lega_at_wifi_setup();
void AT_task(lega_thread_arg_t arg)
{
    int32_t ret = 0;

#ifdef CFG_SDIO_SUPPORT
    at_msg_t msg_queue_elmt = {0};
#else
    uint32_t msg_queue_elmt;
#endif

#ifndef QUICK_CONNECT
    lega_rtos_delay_milliseconds(1000);
#endif

    ret = lega_at_wifi_setup();
    if(ret)
        lega_rtos_set_semaphore(&at_cmd_protect);

    while(1)
    {
        lega_rtos_pop_from_queue(&at_task_msg_queue, &msg_queue_elmt, LEGA_WAIT_FOREVER);

#ifdef CFG_SDIO_SUPPORT
        if (g_sdio_msg_enable)
        {
            at_sdio_command_process((char *)(msg_queue_elmt.p_sdio_at_cmd));
        }
        else
        {
            if(!at_user_info.uart_echo)
            {
                at_rspinfor("%s",uart_buf[cur_idx]);
            }

            at_command_process();
        }
#else
        if(!at_user_info.uart_echo)
        {
            at_rspinfor("%s",uart_buf[cur_idx]);
        }

        at_command_process();
#endif
    }
}

extern void lega_at_net_socket_init(void);
extern void lega_at_wifi_event_cb_register(void);

#define     AT_TASK_NAME                "AT_task"
#define     AT_TASK_PRIORITY            (20)
#define     AT_TASK_STACK_SIZE          4096
int lega_at_init(void)
{
    if(!AT_Task_Handler)
    {
        lega_rtos_init_semaphore(&at_cmd_protect, 0);

        lega_at_net_socket_init();
        lega_at_wifi_event_cb_register();

#ifdef CFG_SDIO_SUPPORT
        lega_rtos_init_queue(&at_task_msg_queue, "AT_TASK_QUEUE", sizeof(at_msg_t), AT_QUEUE_SIZE);

        lega_at_host_tx_msg_hdlr_register(lega_at_sdio_msg_hdlr);
#else
        lega_rtos_init_queue(&at_task_msg_queue, "AT_TASK_QUEUE", sizeof(uint32_t), AT_QUEUE_SIZE);
#endif
        lega_rtos_create_thread(&AT_Task_Handler, AT_TASK_PRIORITY, AT_TASK_NAME, AT_task, AT_TASK_STACK_SIZE, 0);
    }
    return 0;
}

int lega_at_deinit(void)
{
    if(AT_Task_Handler)
    {
        lega_rtos_delete_thread(&AT_Task_Handler);
        AT_Task_Handler = NULL;
        lega_rtos_deinit_queue(&at_task_msg_queue);
        at_task_msg_queue = NULL;
    }
    return 0;
}
/*
 ************************************************************
 *           AT task and cmd process end
 *
 ************************************************************
 */

#endif
