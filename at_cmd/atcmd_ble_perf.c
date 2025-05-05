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

#if (defined CFG_PLF_DUET && defined CFG_DUET_BLE)
#include "sonata_utils_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "app.h"
#include "sonata_stack_user_config.h"

#ifdef SONATA_CFG_PERF_TEST
static int sonata_at_set_perf_test_param(int argc, char**argv)
{
    uint16_t conn_intv=0;
    uint16_t ce_len=0;
    if(argc < 2)
    {
        printf("miss param\r\n");
    }
    conn_intv= atof(argv[1]);
    ce_len= atof(argv[2]);
    sonata_set_perf_test_param(conn_intv,ce_len);
    printf("set connection interval:%f ms event length:% fms\r\n",conn_intv*1.25,ce_len*0.625);
    return 0;
}

static int sonata_at_set_perf_test_pkt_num(int argc, char**argv)
{
    uint16_t number=0;
    if(argc < 2)
    {
        printf("miss param\r\n");
    }
    number= atof(argv[1]);
    if(number<10)
    {
        printf("too small\r\n");
    }
    sonata_set_perf_test_pkt_num(number);
    printf("set packet number is:%d\r\n",number);
    return 0;
}
static int sonata_at_set_perf_test_pkt_size(int argc, char**argv)
{
    uint8_t size=0;
    if(argc < 2)
    {
        printf("miss param\r\n");
    }
    size= atof(argv[1]);

    sonata_set_perf_test_pkt_size(size);
    printf("set packet size is:%d\r\n",size);
    return 0;
}
static int sonata_at_set_perf_test_rate(int argc, char**argv)
{
    if(argc < 2)
    {
        printf("miss param\r\n");
    }
    uint8_t rate_flag=0;
    uint8_t status=0;
    rate_flag=atof(argv[1]);
    sonata_fs_len_t len = APP_RATE_SAVE_LEN;
    sonata_fs_tag_t tag = APP_RATE_SAVE_TAG;
    status=sonata_fs_write(tag,len,&rate_flag);
    if(!status)
    {
        if(rate_flag==1)
        {
            printf("set ok,rate is:2Mbps\r\n");
        }
        else if(rate_flag==2)
        {
            printf("set ok,rate is:125kbps\r\n");
        }
        else if(rate_flag==3)
        {
            printf("set ok,rate is:500kbps\r\n");
        }
        else
        {
            printf("set ok,rate is:1Mbps\r\n");
        }
    }
    return 0;
}

cmd_entry comm_set_perf_param = {
    .name = "set_perf_param",
    .help = "cmd format:   set_perf_param {connection interval} {event length}\r\n\
cmd function: set_perf_param exampale:set_perf_param 6 12\r\n",
    .function = sonata_at_set_perf_test_param,
};
cmd_entry comm_set_pkt_num = {
    .name = "set_pkt_num",
    .help = "cmd format:   set_pkt_num {number}\r\n\
cmd function: set_pkt_num  exampale:set_pkt_num 40000\r\n",
    .function = sonata_at_set_perf_test_pkt_num,
};
cmd_entry comm_set_pkt_size = {
    .name = "set_pkt_size",
    .help = "cmd format:   set_pkt_size {size}\r\n\
cmd function: set_pkt_size  exampale:set_pkt_size 244\r\n",
    .function = sonata_at_set_perf_test_pkt_size,
};
cmd_entry comm_set_perf_rate = {
    .name = "set_perf_rate",
    .help = "cmd format:   set_perf_rate {rate_flag}\r\n\
    cmd function: set_perf_rate exampale:set_perf_rate 1 [3:500kbps 2:125kbps 1:2Mbps 0:1Mbps]\r\n",
    .function = sonata_at_set_perf_test_rate,
};
#endif

void lega_at_ble_perf_test_cmd_register(void)
{
    #ifdef SONATA_CFG_PERF_TEST
    lega_at_cmd_register(&comm_set_perf_param);
    lega_at_cmd_register(&comm_set_pkt_num);
    lega_at_cmd_register(&comm_set_pkt_size);
    lega_at_cmd_register(&comm_set_perf_rate);
    #endif
}
#endif


