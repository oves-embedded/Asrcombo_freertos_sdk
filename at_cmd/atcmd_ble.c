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

#include "lega_at_api.h"
#include "app.h"

#if (defined CFG_PLF_DUET && defined CFG_DUET_BLE)
#include "sonata_utils_api.h"
#include "sonata_api_task.h"
#include "sonata_gap_api.h"
#ifdef SONATA_CFG_BLE_MESH_TEST
#include "compiler.h"
#include "sonata_mesh_api.h"
#include "duet_common.h"
#include "sonata_stack_user_config.h"
#endif

extern int init_ble_task(void);
extern int ble_close(void);


uint8_t ble_open = 0;
bool atcmd_is_ble_open()
{
    if (ble_open == 1)
    {
        return true;
    }
    return false;
}

void atcmd_set_ble_open(uint8_t value)
{
    ble_open = value;
}

static int lega_at_ble_open(int argc, char **argv)
{
    if(ble_open == 0)
    {
        init_ble_task();
    }
    else
        printf("ble is alread open!\n");
    return 0;
}

static int lega_at_ble_close(int argc, char **argv)
{
    if(ble_open == 0x1)
    {
        ble_open = 0;
        ble_close();
    }
    else
        printf("ble don't need close!\n");
    return 0;
}

#if (defined(SONATA_CFG_BLE_MESH_TEST)||defined(ASR_2_4G_DEMO))
static uint8_t string_asc_to_hex(char param)
{
    uint8_t val;

    if((param >= 48) && (param <= 57))
        val = param-48;
    else if((param >= 65) && (param <= 70))
        val = param-55;
    else if((param >= 97) && (param <= 102))
        val = param-87;
    else
    {
        val = 0xA;
    }
    return val;
}
static void tool_string_to_array(char *in_str, uint8_t in_len,uint8_t *out_arr, uint8_t *out_len) {
    int i;
    in_len = (in_len + 1) &0xFFFE;
    for(i = 0; i < in_len; i++)
    {
        if(i%2 || i==1)
            out_arr[i>>1] = string_asc_to_hex(in_str[i]) | (out_arr[i>>1]&0xF0);
        else
            out_arr[i>>1] = (string_asc_to_hex(in_str[i])<<4) | (out_arr[i>>1]&0xF);
    }
    *out_len = in_len/2;
}
#endif

#ifdef ASR_2_4G_DEMO
extern int sonata_2_4g_stack_close(void);
extern int sonata_2_4g_stack_open(void);

extern int init_2_4g_task(void);
uint8_t at_2_4g_open = 0;
extern void sonata_2_4g_isr_open(void);
extern void sonata_2_4g_isr_close(void);
extern void sonata_2_4g_reset(void);

extern uint8_t stack_mode;
extern void sonata_2_4g_driver_reg_error_cb(void* cb);
extern uint8_t app_sonata_2_4g_error_callback(uint16_t err_code);
extern int init_2_4g_task_test(void);
extern void sonata_2_4g_driver_debug_levle_Set(uint8_t level);
extern void app_2_4g_set_rx_param(uint32_t interval, uint32_t windowSize);
extern int app_hal_2_4g_tx_data (uint16_t channel,uint8_t *data, uint8_t len);
extern int app_hal_2_4g_rx_data (uint8_t role,uint16_t channel);
extern void ft_hal_2_4g_set_rx_param(uint32_t interval, uint32_t windowSize);
extern int ft_hal_2_4g_rx_stop(void);


static int lega_at_2_4g_open(int argc, char **argv)
{
    if(at_2_4g_open == 0)
    {
        at_2_4g_open = 1;
        sonata_2_4g_stack_open();
    }
    else
        printf("2.4g is alread open!\n");
   return 0;
}


static int lega_at_2_4g_close(int argc, char **argv)
{
    if(at_2_4g_open == 0x1)
    {
        sonata_2_4g_stack_close();
        at_2_4g_open = 0;
    }
    else
        printf("2.4g don't need close!\n");
    return 0;
}

static int lega_at_app_mode(int argc, char **argv)
{
   #ifdef ASR_2_4G_DEMO
    if (argc == 2 && !strcmp(argv[1], "2.4g"))
    {
        stack_mode = 1;

        printf("2.4g\n");
    }
    else
    {
       stack_mode = 0;
       printf("ble\n");
    }
    #endif
    return 0;
}

static int lega_at_2_4g_rx_param(int argc, char **argv)
{
   #ifdef ASR_2_4G_DEMO
    if (argc == 3 )
    {
        uint32_t interval = atoi(argv[1]);
        uint32_t wind = atoi(argv[2]);
        ft_hal_2_4g_set_rx_param(interval,wind);
    }
    else
    {
       printf("error param\n");
    }
    #endif
    return 0;
}

static int lega_at_2_4g_log_level_set(int argc, char **argv)
{
    if (argc == 2 )
    {
        uint8_t level = atoi(argv[1]);
        sonata_2_4g_driver_debug_levle_Set(level);
        printf("level %d\n",level);
    }

    return 0;
}

#include "sonata_private_protocol_api.h"
#define TEST_ACCESS_ADDR  0x8E89BED6
//#define TEST_ACCESS_ADDR  0x75577537
/// rx callback


/// tx callback
uint8_t test_sonata_2_4g_tx_callback(uint16_t channel,uint8_t status)
{
    printf("APP: %s  \r\n", __FUNCTION__);
    return 0;
}

/// timeout callback
uint8_t test_sonata_2_4g_timeout_callback(uint16_t channel,uint8_t status)
{
    printf("APP: %s  \r\n", __FUNCTION__);
    return 0;
}


#define APP_SNIFFER_MAX_BUF_LEN 50

#define APP_RX_SNIFF_MSG         5
#define APP_RX_SNIFF_BUF_NUM      64
uint8_t sniffer_current_output_idx = 0;
uint8_t sniffer_current_input_idx = 0;
static uint8_t sniffer_buf[APP_RX_SNIFF_BUF_NUM][APP_SNIFFER_MAX_BUF_LEN + 2];
uint16_t ch_num = 0;

uint8_t app_sonata_2_4g_sniffer_save_pkt(int8_t rssi ,uint8_t *data,uint8_t length)
{
     
     if((sniffer_current_input_idx +1)%APP_RX_SNIFF_BUF_NUM == sniffer_current_output_idx)
     {
         printf("buf full !\r\n");
         return 0;
     }
     if(APP_SNIFFER_MAX_BUF_LEN < length)
     {
         return 0;
     }
     sniffer_buf[sniffer_current_input_idx][0] = rssi;
     sniffer_buf[sniffer_current_input_idx][1] = length;
     memmove(&sniffer_buf[sniffer_current_input_idx][2],data,length);
     sniffer_current_input_idx = (sniffer_current_input_idx +1) & (APP_RX_SNIFF_BUF_NUM -1);
     return 0;
}

uint8_t app_sonata_2_4g_sniffer_msg_handler(void *param)
{

    if(sniffer_current_input_idx == sniffer_current_output_idx)
    {
         return 0;
    }
    uint8_t msghead = sniffer_buf[sniffer_current_output_idx][1];
    printf("%d :rssi %d length %d ",ch_num,(int8_t)sniffer_buf[sniffer_current_output_idx][0],msghead);

    for(int i = 2; i < msghead+2 ; i++)
    {
        printf("%02x",sniffer_buf[sniffer_current_output_idx][i]);
    }
    printf("\r\n");
    sniffer_current_output_idx =  (sniffer_current_output_idx + 1)&(APP_RX_SNIFF_BUF_NUM -1);
    return 0;
}

static const sonata_api_app_msg_t snifferMsG[] = {
        {APP_RX_SNIFF_MSG, app_sonata_2_4g_sniffer_msg_handler},
};


uint8_t test_sonata_2_4g_rx_callback(int8_t rssi,uint16_t channel,uint8_t *data,uint8_t length)
{
    app_sonata_2_4g_sniffer_save_pkt(rssi,data,length);
    sonata_api_send_app_msg(APP_RX_SNIFF_MSG, NULL);
    return 0;
}



void lega_at_reg_sniffer_msg(void)
{
     static uint8_t reg_flag = 0;
     if(reg_flag == 1)
     return;
     reg_flag = 1;
     sonata_api_app_msg_register(&snifferMsG[0]);
}

uint8_t app_sniff_2_4g_error_callback(uint16_t channel,uint8_t status)
{
    sonata_2_4g_rx_data(ch_num,TEST_ACCESS_ADDR,0,test_sonata_2_4g_rx_callback,test_sonata_2_4g_timeout_callback);
    return 0;
}


static int sonata_at_2_4g_rx(int argc, char**argv)
{
    uint16_t channel = atoi(argv[1]);
	uint16_t timeout = atoi(argv[2]);
    sonata_2_4g_driver_reg_error_cb(app_sniff_2_4g_error_callback);

	if (argc == 2 && !strcmp(argv[1], "stop"))
	{
	    GLOBAL_INT_DISABLE();
		sonata_2_4g_rx_stop();
        GLOBAL_INT_RESTORE();
	}
	else if(argc == 3)
	{
	    ch_num = channel;
        lega_at_reg_sniffer_msg();
        sonata_2_4g_rx_data(channel,TEST_ACCESS_ADDR,timeout,test_sonata_2_4g_rx_callback,test_sonata_2_4g_timeout_callback);
	}
	else
	{
	    ch_num = channel;
        lega_at_reg_sniffer_msg();
        sonata_2_4g_rx_data(channel,TEST_ACCESS_ADDR,0,test_sonata_2_4g_rx_callback,test_sonata_2_4g_timeout_callback);
	}

    return 0;
}
static int sonata_at_2_4g_rate_set(int argc, char**argv)
{
    uint8_t set_rate = atoi(argv[2]);
	if (argc == 3 && !strcmp(argv[1], "tx"))
	{
		extern void sonata_rf_2_4g_set_tx_rate(uint8_t rate);
		sonata_rf_2_4g_set_tx_rate(set_rate);
        if(set_rate==1)
        {
            printf("set tx rate is:2Mbps\r\n");
        }
        else if(set_rate==2)
        {
            printf("set tx rate is:125kbps\r\n");
        }
        else if(set_rate==3)
        {
            printf("set tx rate is:500kbps\r\n");
        }
        else if(set_rate==0)
        {
            printf("set tx rate is:1Mbps\r\n");
        }

	}
	else if(argc == 3 && !strcmp(argv[1], "rx"))
	{
		extern void sonata_rf_2_4g_set_rx_rate(uint8_t rate);
		sonata_rf_2_4g_set_rx_rate(set_rate);
        if(set_rate==1)
        {
            printf("set rx rate is:2Mbps\r\n");
        }
        else if(set_rate==2)
        {
            printf("set rx rate is:125kbps\r\n");
        }
        else if(set_rate==3)
        {
            printf("set rx rate is:500kbps\r\n");
        }
        else if(set_rate==0)
        {
            printf("set rx rate is:1Mbps\r\n");
        }

	}
    return 0;
}

static uint8_t seq_num=0;
static int sonata_at_2_4g_tx(int argc, char**argv)
{
    uint16_t channel = atoi(argv[1]);
    uint8_t test_data[128];
    int i;
    for(i = 0; i < sizeof(test_data); i++)
    {
        test_data[i] = i;
    }
    //test_data[0]=0x15;
    test_data[19]=seq_num;
    seq_num++;
    sonata_2_4g_tx_data(channel,TEST_ACCESS_ADDR,test_data,20,test_sonata_2_4g_tx_callback);
    return 0;
}
extern void app_ble_config_legacy_advertising();
static int sonata_at_2_4g_ble_adv(int argc, char**argv)
{
    if(atof(argv[1]) == 0)
		{
       sonata_ble_stop_advertising();
			//sonata_ble_delete_all_active();
		}
		else
		{
       app_ble_config_legacy_advertising();
		}
    return 0;
}
static int sonata_at_2_4g_ble_switch(int argc, char**argv)
{
    if(atof(argv[1]) == 0)
		{
			  sonata_switch_to_2_4g();
		}
		else
		{
			sonata_switch_to_ble();
		}
    return 0;
}

int app_hal_2_4g_tx_data (uint16_t channel,uint8_t *data, uint8_t len);
int app_hal_2_4g_rx_data (uint8_t role,uint16_t channel);

static int sonata_at_2_4g_app_tx(int argc, char**argv)
{
    extern uint16_t app_time_code;
    extern    uint16_t  app_one_time_step ;
    extern    uint32_t  app_one_tx_step ;
    uint8_t data_buf[256];
    uint8_t len = strlen(argv[1]);
    uint8_t outlen = 0;
    
    if(at_2_4g_open == 0x0)
        return -1;
    uint16_t ch = atoi(argv[2]);
	
    if(argc == 4)
    {
        app_time_code = atoi(argv[2]);
    }
    else if(argc == 5)
    {
        app_time_code = atoi(argv[2]);
        app_one_time_step = atoi(argv[3]);
    }
    else if(argc == 6)
    {
        app_time_code = atoi(argv[2]);
        app_one_time_step = atoi(argv[3]);
        app_one_tx_step = atoi(argv[4]);
    }
    tool_string_to_array(argv[1],len,data_buf,&outlen);
    app_hal_2_4g_tx_data(ch,data_buf,outlen);
    return 0;
}

static int sonata_at_2_4g_app_rx(int argc, char**argv)
{
    uint8_t role = atoi(argv[1]);
    if(at_2_4g_open == 0x0)
        return -1;
   
   if (argc == 2 && !strcmp(argv[1], "stop"))
   {
       ft_hal_2_4g_rx_stop();
   }
   else
   {
	   uint16_t ch = atoi(argv[2]);
       app_hal_2_4g_rx_data(role,ch);
   }
   return 0;
}

#endif

#ifdef SONATA_CFG_BLE_MESH_TEST

/*
    ble_connect stop    // terminate connection
    ble_connect stop {conidx} // disconnect id
*/
static int sonata_at_ble_connect(int argc, char **argv) {

    if (ble_open == 0)
    {
        printf("ble is close");
        return 1;
    }
    uint8_t outlen = 0;

    uint8_t add_arr[6];
    if (argc == 3 && !strcmp(argv[1], "start") && strlen(argv[2])==12)
    {
        tool_string_to_array(argv[2], strlen(argv[2]), add_arr,&outlen);
        app_ble_config_initiating(add_arr, 40, 0, 700);
        return 0;
    }
    else if (argc == 6 && !strcmp(argv[1], "start"))
    {
        tool_string_to_array(argv[2], strlen(argv[2]), add_arr,&outlen);
        int interval = convert_str_to_int(argv[3]);
        int latency = convert_str_to_int(argv[4]);
        int timeout = convert_str_to_int(argv[5]);
        app_ble_config_initiating(add_arr, interval, latency, timeout);

        return 0;
    }
    else if (argc == 2 && !strcmp(argv[1], "stop"))
    {
        app_ble_stop_initiating();
        return 0;
    }
    else if (argc == 3 && !strcmp(argv[1], "stop"))
    {
        int conidx = convert_str_to_int(argv[2]);
        if (conidx < app_ble_get_max_connection())
        {
            app_ble_disconnect((uint8_t)conidx);
            return 0;
        }
    }

    return 1;
}

static int sonata_at_ble_wrtie(int argc, char **argv) {
    if (ble_open == 0 )
    {
        printf("ble is closer\r\n");
        return 1;
    }

    uint8_t conidx = convert_str_to_int(argv[2]);
    if (argc == 3 && !strcmp(argv[1], "start") && conidx<app_ble_get_max_connection())
    {
        if (app_ble_get_connection_status(conidx))
        {
            app_ble_config_write(conidx, 0x27, 16, 1);
            return 0;
        }
        else
        {
            printf("no connection established\r\n");
        }
    }
    else if (argc == 6 && !strcmp(argv[1], "start") && conidx<app_ble_get_max_connection())
    {
        int handle = convert_str_to_int(argv[3]);
        int data_len = convert_str_to_int(argv[4]);
        uint8_t interval=1;
        interval = (uint8_t)convert_str_to_int(argv[5]);
        if (app_ble_get_connection_status(conidx))
        {
            app_ble_config_write(conidx, (uint16_t)handle, (uint8_t)data_len, interval);
            return 0;
        }
        else
        {
            printf("no connection established\r\n");
        }
    }
    else if (argc == 2 && !strcmp(argv[1], "stop"))
    {
        app_ble_stop_write();
        return 0;
    }

    return 1;
}

/*
    ble_scan start {interval(time)} {window(time)}
    ble_scan start // use default time
    ble_scan stop
*/
static int sonata_at_ble_scan(int argc, char **argv)
{
    if (ble_open == 0)
    {
        printf("ble is close");
        return 1;
    }

    if (argc == 2 && !strcmp(argv[1],"stop"))
    {
        app_ble_stop_scanning();
        return 0;
    }
    else if (argc == 2 && !strcmp(argv[1], "start"))
    {
        app_ble_start_scan_with_mesh(0xFFFF, 0xFFFF);
        return 0;
    }
    else if (argc == 4 && !strcmp(argv[1], "start"))
    {
        int scan_intv = convert_str_to_int(argv[2]);
        int scan_wnd = convert_str_to_int(argv[3]);
        if (scan_intv < 0xFFFF && scan_wnd < 0xFFFF && scan_intv > 0 && scan_wnd > 0)
        {
            app_ble_start_scan_with_mesh((uint16_t)scan_intv, (uint16_t)scan_wnd);
            return 0;
        }

    }

    return 1;
}

static int sonata_at_vendor_packet_loss_test(int argc, char **argv)
{
    if (ble_open == 0)
    {
        printf("ble is close");
        return 1;
    }

    mesh_core_evt_cb_params_t set_param;
    set_param.local_pub_set.element_addr = atoi(argv[5]);
    set_param.local_pub_set.pub_addr = 0xC000;
    set_param.local_pub_set.appkey_index = 0;
    set_param.local_pub_set.credentialFlag  = 0;
    set_param.local_pub_set.ttl = 11;
    set_param.local_pub_set.period = 0;
    set_param.local_pub_set.retransmitCount =0;
    set_param.local_pub_set.retransmitIntervalSteps =1;
    set_param.local_pub_set.model_id = MESH_MODELID_VENC_ASR;

    mesh_core_evt_cb(MESH_SET_LOCAL_PUBLISH_PARAM,&set_param);

    vendor_packet_loss_test(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),(uint8_t)atoi(argv[4]));
    return 0;
}

#endif

cmd_entry comm_ble_open = {
    .name = "ble_open",
    .help ="cmd format:   ble_open\r\n\
cmd function: open ble as slave and boardcast\r\n" ,
    .function = lega_at_ble_open,
};

cmd_entry comm_ble_close = {
        .name = "ble_close",
        .help ="cmd format:   ble_close\r\n\
cmd function: close ble boardcast\r\n" ,
        .function = lega_at_ble_close,
};


#ifdef SONATA_CFG_BLE_MESH_TEST
cmd_entry comm_ble_write = {
        .name = "ble_write",
        .help ="cmd format: ble_write start conidx\r\n" ,
        .function = sonata_at_ble_wrtie,
};
cmd_entry comm_ble_scan = {
        .name = "ble_scan",
        .help ="cmd format: ble_scan start|stop\r\n" ,
        .function = sonata_at_ble_scan,
};
cmd_entry comm_ble_connect = {
        .name = "ble_connect",
        .help ="\
cmd format: ble_connect start peer_address\r\n\
cmd function: connect to slave device\r\n\
cmd format: ble_connect stop\r\n\
cmd function: disconnect all connection\r\n" ,
        .function = sonata_at_ble_connect,
};

cmd_entry cmd_vendor_packet_loss_test = {
    .name = "vendor_packet_loss_test",
    .help = "cmd format:   vendor_packet_loss_test {Isresponse} {time} {number} {length} {element addr}\r\n\
cmd function: vendor_packet_loss_test\r\n",
    .function = sonata_at_vendor_packet_loss_test,
};

#endif

#ifdef ASR_2_4G_DEMO
cmd_entry comm_2_4g_open = {
    .name = "2_4g_open",
    .help ="cmd format:   2_4g_open\r\n\
cmd function: open 2_4g\r\n" ,
    .function = lega_at_2_4g_open,
};

cmd_entry comm_2_4g_close = {
        .name = "2_4g_close",
        .help ="cmd format:   2_4g_close\r\n\
cmd function: close 2_4g\r\n" ,
        .function = lega_at_2_4g_close,
};

cmd_entry comm_2_4g_mode = {
        .name = "app_mode",
        .help ="cmd format:   app_mode {2.4g|ble}\r\n\
cmd function: stack mode setting\r\n" ,
        .function = lega_at_app_mode,
};

cmd_entry comm_2_4g_log_level = {
    .name = "2_4g_log_level",
    .help ="cmd format:   2_4g_log_level {0|1|2}\r\n\
cmd function: adjust log level\r\n" ,
    .function = lega_at_2_4g_log_level_set,
};

cmd_entry comm_2_4g_rx_param = {
    .name = "2_4g_rx_param",
    .help ="cmd format:   2_4g_rx_param {interval {winsz}}\r\n\
cmd function: 2_4g scan interval and winsize\r\n" ,
    .function = lega_at_2_4g_rx_param,
};
cmd_entry comm_2_4g_tx = {
    .name = "test_2_4g_tx",
    .help = "cmd format:   test_2_4g_tx {ch} \r\n\
cmd function: tx test\r\n",
    .function = sonata_at_2_4g_tx,
};

cmd_entry comm_2_4g_rx_test = {
    .name = "test_2_4g_rx",
    .help = "cmd format:   test_2_4g_rx {ch} \r\n\
cmd function: rx test\r\n",
    .function = sonata_at_2_4g_rx,
};
cmd_entry comm_2_4g_rate_set = {
    .name = "rate_set",
    .help = "cmd format:   rate_set {rate} \r\n\
cmd function: rate_set\r\n",
    .function = sonata_at_2_4g_rate_set,
};
cmd_entry comm_2_4g_ble_switch = {
    .name = "test_2_4g_ble_switch",
    .help = "cmd format:   test_2_4g_ble_switch {mode} \r\n\
cmd function: test_2_4g_ble_switch 0 or 1\r\n",
    .function = sonata_at_2_4g_ble_switch,
};

cmd_entry comm_app_tx = {
    .name = "app_tx",
    .help = "cmd format:   arrow_tx {data} [ch]\r\n\
cmd function: \r\n",
    .function = sonata_at_2_4g_app_tx,
};

cmd_entry comm_app_rx = {
    .name = "app_rx",
    .help = "cmd format:   arrow_rx {role} {ch} \r\n\
cmd function: role is 0 or 1\r\n",
    .function = sonata_at_2_4g_app_rx,
};

cmd_entry comm_adv = {
    .name = "test_adv",
    .help = "cmd format:   test_adv {mode} \r\n\
cmd function: test_adv 0 or 1\r\n",
    .function = sonata_at_2_4g_ble_adv,
};
#endif

void lega_at_ble_cmd_register(void)
{
    #ifdef SONATA_CFG_BLE_MULT_TEST
    {
        extern void app_command_register_lega();
        app_command_register_lega();
    }
    #else
    lega_at_cmd_register(&comm_ble_open);
    lega_at_cmd_register(&comm_ble_close);
    #endif
    #ifdef ASR_2_4G_DEMO
    lega_at_cmd_register(&comm_2_4g_open);
    lega_at_cmd_register(&comm_2_4g_close);
    lega_at_cmd_register(&comm_2_4g_mode);
    lega_at_cmd_register(&comm_2_4g_log_level);
    lega_at_cmd_register(&comm_2_4g_tx);
    lega_at_cmd_register(&comm_2_4g_rx_test);
    lega_at_cmd_register(&comm_2_4g_rx_param);
    lega_at_cmd_register(&comm_app_tx);
    lega_at_cmd_register(&comm_app_rx);
    lega_at_cmd_register(&comm_2_4g_ble_switch);
    lega_at_cmd_register(&comm_adv);
    lega_at_cmd_register(&comm_2_4g_rate_set);
    #endif
    #ifdef SONATA_CFG_BLE_MESH_TEST
    lega_at_cmd_register(&comm_ble_write);
    lega_at_cmd_register(&comm_ble_scan);
    lega_at_cmd_register(&comm_ble_connect);
    lega_at_cmd_register(&cmd_vendor_packet_loss_test);
    #endif
}

#endif


