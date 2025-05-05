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

#ifdef CLOUD_CONNECT_TEST
#include "cloud.h"
#include "cloud_common.h"
#include "lwip/apps/mqtt.h"
#include "lwip/err.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/def.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/certs.h"
#include "Altcp_tls.h"
#include "lega_flash_kv.h"
#include "lega_rtos_api.h"

lega_thread_t Cloud_Connect_Task_Handler;
#ifdef CLOUD_CONNECT_HUAWEI
extern int huawei_get_value(mqtt_connect_para_t *huawei_para);
#elif  CLOUD_CONNECT_AWS
extern uint8_t aws_get_value(mqtt_auth_info_t *aws_para);
#elif  CLOUD_CONNECT_TENCENT
extern uint8_t tencent_get_value(mqtt_auth_info_t *tencent_para);
#elif  CLOUD_CONNECT_BAIDU
extern uint8_t baidu_get_value(struct baidu_para_struct *baidupara);
#elif  CLOUD_CONNECT_PRIVATE
extern private_para_t *private_para;
extern int private_get_value(private_para_t *para);
#endif
static mqtt_client_t *_client = NULL;
lega_semaphore_t _connect_test_semaphore = NULL;
static struct altcp_tls_config *_conf = NULL;

static void client_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    if(status == MQTT_CONNECT_ACCEPTED)
    {
        printf("client_connection_cb: successful\n");
        lega_rtos_set_semaphore(&_connect_test_semaphore);
    }
    else
    {
        printf("client_connection_cb: fail, status = %d\n", status);
    }
}
void cloud_connect_test(lega_thread_arg_t arg)
{
    int ret = 0;
    int test_cnt = 0;
    int i = 0;
    u16_t success_cnt = 0;
    int port = 8883;
    int connect_times = 10;
    err_t err;
    ip_addr_t things_ip;
    struct mqtt_connect_client_info_t ci;
    test_cnt =  65535;
    lega_rtos_init_semaphore(&_connect_test_semaphore, 0);
//huawei cloud connect info
#ifdef CLOUD_CONNECT_HUAWEI
    if(init_huawei_info() != 0)
    {
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return ;
    }
    mqtt_connect_para_t huawei_para;
    memset(&huawei_para, 0, sizeof(mqtt_connect_para_t));
    if((ret = huawei_get_value(&huawei_para)) != 0)
    {
        printf("get huawei para error\n");
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return ;
    }
    struct hostent *hostent;
    hostent = gethostbyname(huawei_para.hostname);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));
    if (_conf == NULL){
         _conf = altcp_tls_create_config_client( \
                                    (const unsigned char *)huawei_ca_crt, \
                                    strlen(huawei_ca_crt)+1);
    }
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = huawei_para.clientid;
    ci.client_user = huawei_para.username;
    ci.keep_alive = 60;
    ci.client_pass = huawei_para.password;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = _conf;

#elif  CLOUD_CONNECT_AWS
    if(init_aws_info() != 0)
    {
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return;
    }
    mqtt_auth_info_t aws_para;
    memset(&aws_para, 0, sizeof(mqtt_auth_info_t));
    if((ret = aws_get_value(&aws_para)) != 0)
    {
        printf("get aws para error\n");
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return;
    }
    struct hostent *hostent;
    hostent = gethostbyname(aws_para.hostname);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));
    if (_conf == NULL){
         _conf = altcp_tls_create_config_client_2wayauth( \
                                    (const unsigned char *)aws_ca_crt, \
                                    strlen(aws_ca_crt)+1, \
                                    (const unsigned char *)aws_client_private_key, \
                                    aws_client_private_key_len+1, \
                                    NULL, \
                                    0, \
                                    (const unsigned char *)aws_client_cert, \
                                    aws_client_cert_len+1);
    }
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = aws_para.clientid;
    ci.client_user = aws_para.username;
    ci.keep_alive = 60;
    ci.client_pass = aws_para.password;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = _conf;
#elif  CLOUD_CONNECT_TENCENT
    if(init_tencent_info() != KV_OK)
    {
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return;

    }
    mqtt_auth_info_t tencent_para;
    memset(&tencent_para, 0, sizeof(mqtt_auth_info_t));
    if((ret = tencent_get_value(&tencent_para)) != 0)
    {
        printf("get tencent para error\n");
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return;
    }
    struct hostent *hostent;
    hostent = gethostbyname(tencent_para.hostname);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));
    if (_conf == NULL)
    {
        _conf = altcp_tls_create_config_client_2wayauth( \
                                    (const unsigned char *)tencent_ca_crt, \
                                    strlen(tencent_ca_crt)+1, \
                                    (const unsigned char *)tencent_client_private_key, \
                                    tencent_client_private_key_len+1, \
                                    NULL, \
                                    0, \
                                    (const unsigned char *)tencent_client_cert, \
                                    tencent_client_cert_len+1);
    }
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = tencent_para.clientid;
    ci.client_user = tencent_para.username;
    ci.keep_alive = 60;
    ci.client_pass = tencent_para.password;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = _conf;
#elif  CLOUD_CONNECT_BAIDU
    struct baidu_para_struct baidu_para = {0};
    memset(&baidu_para, 0, sizeof(struct baidu_para_struct));
    if((ret = baidu_get_value(&baidu_para)) != 0)
    {
        printf("get baidu para error\n");
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return;
    }
    struct hostent *hostent;
    hostent = gethostbyname(baidu_para.baidu_url);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));
    if (_conf == NULL)
        _conf = altcp_tls_create_config_client((const unsigned char *)mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = baidu_para.baidu_clientid;
    ci.client_user = baidu_para.baidu_username;
    ci.keep_alive = 60;
    ci.client_pass = baidu_para.baidu_userpass;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = _conf;
    port = 1884;
#elif  CLOUD_CONNECT_JD
#elif  CLOUD_CONNECT_PRIVATE
    if(private_para != NULL)
        lega_rtos_free(private_para);
    private_para = lega_rtos_malloc(sizeof(private_para_t));
    if(private_para == NULL)
    {
        printf("parivate_para malloc err\n");
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return;
    }
    if(private_get_value(private_para) != 0)
    {
        printf("private_get_value err\n");
        lega_rtos_deinit_semaphore(&_connect_test_semaphore);
        lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);
        return;
    }
    if(!strcmp(private_para->private_protocol, "mqtt"))
    {
        if (_conf == NULL)
            _conf = altcp_tls_create_config_client((const unsigned char *)mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
        port = private_para->private_port;

        memset(&ci, 0, sizeof(ci));
        ci.client_id = "mos";
        if(private_para->private_username != NULL)
            ci.client_user = private_para->private_username;
        ci.keep_alive = 60;
        ci.client_pass = NULL;
        ci.will_msg = NULL;
        ci.will_qos = 0;
        ci.will_topic = NULL;
        ci.will_retain = 0;
        if(!strcmp(private_para->private_tls_switch, "on"))
            ci.tls_config = _conf;
        else if(!strcmp(private_para->private_tls_switch, "off"))
            ci.tls_config = NULL;
        else
        {
            printf("private_tls_switch err\n");
            ci.tls_config = NULL;
        }
        struct hostent *hostent;
        hostent = gethostbyname(private_para->private_host);
        for(i = 0; hostent->h_addr_list[i]; i++)
        {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
        }
        memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
        printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));
    }

#endif

    _client = mqtt_client_new();
    if(_client ==NULL)
        return ;
    printf("cloud connect test %d start.\n",test_cnt);

    //进行总的期望次数连接、断开
    for (i = 0; i < test_cnt; i++)
    {
        //开始单次连接
        connect_times = 10;
        while(connect_times)
        {
            connect_times--;
            err = mqtt_client_connect(_client, &things_ip, port, client_connection_cb, 0, &ci);
            if(err != ERR_OK){
                printf("connect error, return %d\n", err);
                mqtt_disconnect(_client);
                lega_rtos_delay_milliseconds(2000);
                continue;
            }
            ret = lega_rtos_get_semaphore(&_connect_test_semaphore, 20000);
            if(ret == kGeneralErr)
            {
                printf("connect timeout.\n");
                mqtt_disconnect(_client);
                lega_rtos_delay_milliseconds(2000);
                continue;

            }
            if(mqtt_client_is_connected(_client) == 1)
            {
                printf("connected.\n");
                success_cnt++;
                break;
            }
            else
            {
                printf("connected fail, state = %d\n",_client->conn_state);
                mqtt_disconnect(_client);
                lega_rtos_delay_milliseconds(2000);
                continue;
            }
        }
        lega_rtos_delay_milliseconds(2000);
        mqtt_disconnect(_client);
        printf("disconnected.\n");
        lega_rtos_delay_milliseconds(1000);
        printf("test status:  %d / %d\n",success_cnt, i+1);
        lega_rtos_delay_milliseconds(3000);
    }
    printf("cloud connect test %d end.\n",test_cnt);
    while(1)
    {
        printf("cloud connect test over, please stop!\n");
        ret = lega_rtos_get_semaphore(&_connect_test_semaphore, 2000);
        if(ret == kNoErr)
        {
#ifdef CLOUD_CONNECT_PRIVATE
            if (private_para != NULL)
            {
                lega_rtos_free(private_para);
                private_para = NULL;
            }
#endif
            if ( _conf != NULL)
            {
                altcp_tls_free_config(_conf);
                _conf = NULL;
            }
            if(_client != NULL)
            {
                mqtt_client_free(_client);
                _client = NULL;
            }
            lega_rtos_deinit_semaphore(&_connect_test_semaphore);
            lega_rtos_delete_thread(&Cloud_Connect_Task_Handler);

        }
    }
}
#endif
