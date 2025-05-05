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

#ifdef CLOUD_SUPPORT
#include "cloud.h"
#include "cloud_common.h"
#include "lwip/apps/http_client.h"
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
#include "altcp_tls.h"
#include "cJSON.h"
#include "lega_rtos_api.h"
#include "lega_at_api.h"
#include "gatt.h"
#ifdef CFG_PLF_DUET
#include "duet_common.h"
#include "duet_flash_kv.h"
#else
#include "lega_common.h"
#include "lega_flash_kv.h"

#endif
/******************test_unity_var********************** */
#ifdef TEST_UNITY_CLOUD
int test_cloud_connected_flag = 0;
int test_cloud_upload_flag = 0;
int test_cloud_download_flag = 0;
#endif
/******************private var********************** */
#ifdef CLOUD_PRIVATE_SUPPORT
mqtt_client_t *private_client = NULL;
lega_semaphore_t private_connect_Semaphore = NULL;
lega_thread_t Private_Cloud_Task_Handler;
private_para_t *private_para = NULL;
static int inpub_id = 0;
extern const char private_ca_crt[];
extern const char private_client_cert[];
extern const char private_client_private_key[];
#endif
/******************mqtt_broker var********************** */
#ifdef CLOUD_MQTT_BROKER_SUPPORT
#include "lega_wlan_api.h"
#include "private_fun.h"
mqtt_client_t *mqtt_broker_client = NULL;
lega_semaphore_t mqtt_broker_connect_Semaphore = NULL;
lega_thread_t Mqtt_broker_Cloud_Task_Handler;
mqtt_broker_para_t *mqtt_broker_para = NULL;
const char *mqtt_broker_pub_payload_on = "{\"update\":{\"switch\": \"True\"}}";
const char *mqtt_broker_pub_payload_off = "{\"update\":{\"switch\": \"False\"}}";
char        mqtt_broker_device_id[128] = {0};
char        mqtt_broker_sub_topic[64] = {0};
char        mqtt_broker_pub_topic[64] = {0};
char        mqtt_broker_will_topic[64] = {0};
char        mqtt_broker_ota_topic[64] = {0};
char        mqtt_broker_progress_topic[64] = {0};
struct altcp_tls_config *mqtt_broker_conf = NULL;
int         mqtt_broker_reconnect_cnt = 0;
char        *mqtt_manufacturer = "oves"; //default manufacturer
char        *mqtt_productid = "00001";  //default productid
extern const char mqtt_broker_ca_crt[];
extern const char mqtt_broker_client_cert[];
extern const char mqtt_broker_client_private_key[];
int         mqtt_comming_topic_idx = 0;

extern _at_user_info at_user_info;
#endif
/******************baidu var********************** */
#ifdef CLOUD_BAIDU_SUPPORT
mqtt_client_t *baidu_client = NULL;
lega_semaphore_t baidu_connect_Semaphore = NULL;
lega_thread_t Baidu_Cloud_Task_Handler;
const char *baidu_pub_payload_on = "{\"reported\":{\"switch\": \"True\"}}";
const char *baidu_pub_payload_off = "{\"reported\":{\"switch\": \"False\"}}";
char baidu_topic_buf[128] = {0};
int8_t baidu_connected_subscribe_ready = 0;
int baidu_reconnect_cnt = 0;
#endif
/******************jd var********************** */
#ifdef CLOUD_JD_SUPPORT
mqtt_client_t *jd_client = NULL;
lega_semaphore_t jd_connect_Semaphore = NULL;
lega_thread_t JD_Cloud_Task_Handler;
char *jd_pub_payload_light = NULL;
char *jd_pub_payload_shadow_get = NULL;
#endif

/******************tencent var********************** */
#ifdef CLOUD_TENCENT_SUPPORT
mqtt_client_t *tencent_client = NULL;
lega_semaphore_t tencent_connect_Semaphore = NULL;
lega_thread_t TENCENT_Cloud_Task_Handler;
char *tencent_pub_payload_light = NULL;
char *tencent_pub_payload_shadow_get = NULL;
int8_t tencent_connected_subscribe_ready = 0;
int tencent_reconnect_cnt = 0;
#endif
/******************huawei var********************** */
#ifdef CLOUD_HUAWEI_SUPPORT
mqtt_client_t *huawei_client = NULL;
lega_semaphore_t huawei_connect_Semaphore = NULL;
lega_thread_t HUAWEI_Cloud_Task_Handler;
char *huawei_pub_payload_light = NULL;
char *huawei_pub_payload_shadow_get = NULL;
int8_t huawei_connected_subscribe_ready = 0;
int huawei_reconnect_cnt = 0;
#endif
#ifdef CLOUD_AWS_SUPPORT
mqtt_client_t *aws_client = NULL;
lega_semaphore_t aws_connect_Semaphore = NULL;
lega_thread_t AWS_Cloud_Task_Handler;
int8_t aws_connected_subscribe_ready = 0;
int aws_reconnect_cnt = 0;
#endif

void mqtt_sub_request_cb(void *arg, err_t result)
{
    printf("mqtt subscribe request callback...\n");
    if(result != ERR_OK)
        printf("subscribe err in callback %d\n", result);
}
void mqtt_unsub_request_cb(void *arg, err_t result)
{
    printf("mqtt unsubscribe request callback...\n");
    if(result != ERR_OK)
        printf("unsubscribe err in callback %d\n", result);
}
void mqtt_incoming_publish_cb(void *arg, const char *topic, uint32_t tot_len)
{
    printf("topic: %s\nlen: %lu\n", topic, tot_len);
}
/******************private process********************** */
#ifdef CLOUD_PRIVATE_SUPPORT
static void private_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    printf("private connectiong callback...\r\n");
    if(status == MQTT_CONNECT_ACCEPTED)
    {
        printf("private-connecting-cb: successfully connectd\n");
        lega_rtos_set_semaphore(&private_connect_Semaphore);
    }
    else
    {
        printf("private-connecting-cb: not connected\n");
    }

}
static void private_pub_request_cb(void *arg, err_t result)
{
    if(result != ERR_OK)
         printf("publish err in callback %d\n", result);
}
static void private_sub_request_cb(void *arg, err_t result)
{
    if(result != ERR_OK)
        printf("subscribe err in callback %d\n", result);
}

static void private_incoming_publish_cb(void *arg, const char *topic, uint32_t tot_len)
{
     printf("topic: %s\nlen: %lu\n", topic, tot_len);
    if(strstr(topic, "v1/devices/me/rpc/request/") != NULL)
    {
        inpub_id = 1;
    }
}
static void private_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags)
{
    printf("payload: %s\nlen: %d\n", (char *)data, len);
    printf("doing...\n");
    char data_buf[100] = {0};
    err_t err;
    char para_buf[10] = {0};
    char send_buf[50] = {0};
    int i = 0;
    char *pdata = NULL;
    if(flags & MQTT_DATA_FLAG_LAST)
    {
        printf("MQTT_DATA_FLAG_LAST\n");
        if(inpub_id != 1)
        {
            printf("inpub_id != 1\n");
            return;
        }
        if(len > 49)
        {
            printf("len > 99\n");
            return;
        }
        strcpy(data_buf, (char *)data);
        if(strstr(data_buf, "setValue") != NULL)
        {
            if((pdata = strstr(data_buf, "params")) != NULL)
            {
                pdata = pdata + 9;
                for(i = 0; i < 9; i++)
                {
                    if(*pdata != '\"')
                    {
                        para_buf[i] = *pdata;
                        pdata ++;
                    }
                    else
                        break;
                }
                sprintf(send_buf, "{\"temperature\":%s}", para_buf);
                printf("send_buf: %s\n", send_buf);
                err = mqtt_publish(private_client, "v1/devices/me/telemetry", send_buf, strlen(send_buf), 0, 0, private_pub_request_cb,  NULL);
#ifdef TEST_UNITY_CLOUD
                test_cloud_download_flag = 1;
#endif
                if(err != ERR_OK)
                    printf("mqtt publish err:%d\n", err);
            }
        }
        inpub_id = 0;
    }
    else
    {
        printf("not MQTT_DATA_FLAG_LAST\n");
    }
}
static void private_http_init()
{
    printf("http init\n");
    uint16_t server_port = 9092;
    char uri[64] = {0};
    int i = 0;
    err_t err;
    char *content = "{\"temperature\": 27}";
    int content_len  = 19;
    //char *uri = "/api/v1/B1_TEST_TOKEN/telemetry";
    snprintf(uri, (strlen("/api/v1/%s/telemetry")+strlen(private_para->private_username)+1), "/api/v1/%s/telemetry", private_para->private_username);
    struct altcp_tls_config *conf = altcp_tls_create_config_client((const unsigned char *)mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
    altcp_allocator_t tls_allocator = {altcp_tls_alloc, conf};

    httpc_connection_t httpc_connection1;
    memset(&httpc_connection1, 0, sizeof(httpc_connection_t));
    httpc_connection_t *settings = &httpc_connection1;
    if(!strcmp(private_para->private_tls_switch, "on"))
        settings->altcp_allocator = &tls_allocator;

    ip_addr_t server_addr;
    struct hostent *hostent;
    hostent = gethostbyname(private_para->private_host);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&server_addr, hostent->h_addr_list[0], sizeof(ip_addr_t));
    server_port = private_para->private_port;
    printf("ipaddr = %s\n", ipaddr_ntoa(&server_addr));
    printf("ipport = %d\n", (int)server_port);
    printf("uri    = %s\n", uri);
    err = httpc_post(&server_addr, server_port, content, content_len, uri, settings, recv_fn, NULL, NULL);
    if(err != ERR_OK)
        printf("http post err:%d\n", err);

}

static void private_mqtt_init()
{
    const char *pub_payload = "{\"firmware_version\":\"1.0.2\", \"serial_number\":\"SN-002\"}";
    //const char *pub_payload = "{\"temperature\":27.0}";
    int connect_times = PRIVATE_CONNECT_TIMES;
    int i;
    err_t err;
    int port = 8883;
    OSStatus ret_conn_semaphore;
    struct altcp_tls_config *conf = altcp_tls_create_config_client_2wayauth( \
                                    (const unsigned char *)private_ca_crt, \
                                    strlen(private_ca_crt)+1, \
                                    (const unsigned char *)private_client_private_key, \
                                    strlen(private_client_private_key)+1, \
                                    NULL, \
                                    0, \
                                    (const unsigned char *)private_client_cert, \
                                    strlen(private_client_cert)+1);
    // altcp_allocator_t tls_allocator = {altcp_tls_alloc, conf};
    printf("mqtt start\r\n");
    port = private_para->private_port;
    private_client = mqtt_client_new();
    struct mqtt_connect_client_info_t ci;
    ip_addr_t things_ip;
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
        ci.tls_config = conf;
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

private_connecting:
    printf("mqtt connecting...\r\n");
    err = mqtt_client_connect(private_client, &things_ip, port, private_connection_cb, 0, &ci);
    if(err != ERR_OK)
        printf("mqtt connect error, return %d\n", err);
    ret_conn_semaphore = lega_rtos_get_semaphore(&private_connect_Semaphore, PRIVATE_CLOUD_TIMEOUT_MS);
    if(ret_conn_semaphore == kGeneralErr)
    {
        printf("private connect timeout\n");
        connect_times--;
        if(connect_times <= 0)
            goto private_error;
        else
        {
            goto private_connecting;
        }
    }
    else if(ret_conn_semaphore == kNoErr)
    {
        printf("get semaphore\n");
    }
    if(mqtt_client_is_connected(private_client) == 1)
    {
        printf("mqtt client connected\n");
#ifdef TEST_UNITY_CLOUD
        test_cloud_connected_flag = 1;
#endif
    }
    else
    {
        connect_times--;
        if(connect_times <= 0)
        {
            printf("mqtt client is not connected\n");
            printf("mqtt client state: %d\n", private_client->conn_state);
            goto private_error;
        }
        else
        {
            goto private_connecting;
        }
        goto private_error;
    }

    mqtt_set_inpub_callback(private_client, private_incoming_publish_cb,private_incoming_data_cb, NULL);
    err = mqtt_publish(private_client, "v1/devices/me/attributes", pub_payload, strlen(pub_payload), ci.will_qos, ci.will_retain, private_pub_request_cb,  NULL);
    if(err != ERR_OK)
        printf("mqtt publish err:%d\n", err);
    err = mqtt_subscribe(private_client, "v1/devices/me/rpc/request/+", ci.will_qos, private_sub_request_cb, NULL);
    if(err != ERR_OK)
        printf("mqtt subscribe err:%d\n", err);
    else
        printf("mqtt subscribe \"v1/devices/me/rpc/request/+\"\n");
    return;


private_error:
    printf("private disconnecting...\n");
    semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
    lega_rtos_set_semaphore(&private_Semaphore);
    return;
}
int private_get_value(private_para_t *para)
{
    int ret;
    char private_host[129] = {0};
    int32_t private_host_len = 127;
    char private_protocol[16] = {0};
    int32_t private_protocol_len = 15;
    int private_port = 8883;
    int32_t private_port_len = 4;
    char private_tls_switch[16] = {0};
    int32_t private_tls_switch_len = 15;
    char private_username[64] = {0};
    int32_t private_username_len = 63;
    int private_timeout = 100;
    int32_t private_timeout_len = 4;
    if(para == NULL)
        return -1;
    memset(para, 0, sizeof(private_para_t));

        ret = lega_flash_kv_get("private_host", private_host, &private_host_len);
        if(ret != 0)
        {
            printf("private host get error\n");
            return -1;
        }
        else
        {
            printf("private host = %s\n", private_host);
        }

        ret = lega_flash_kv_get("private_tls_switch", private_tls_switch, &private_tls_switch_len);
        if(ret != 0)
        {
            printf("private_tls_switch get error\n");
            return -1;
        }
        else
        {
            printf("private_tls_switch = %s\n", private_tls_switch);
        }

        ret = lega_flash_kv_get("private_username", private_username, &private_username_len);
        if(ret != 0)
        {
            printf("private_username get error\n");
            return -1;
        }
        else
        {
            printf("private_username = %s\n", private_username);
        }

        ret = lega_flash_kv_get("private_protocol", private_protocol, &private_protocol_len);
        if(ret != 0)
        {
            printf("private_protocol get error\n");
            return -1;
        }
        else
        {
            printf("private_protocol = %s\n", private_protocol);
        }

        ret = lega_flash_kv_get("private_port", &private_port, &private_port_len);
        if(ret != 0)
        {
            printf("private port get error\n");
            private_port = 8883;
        }
        else
        {
            printf("private port = %d\n", private_timeout);
        }

        ret = lega_flash_kv_get("private_timeout", &private_timeout, &private_timeout_len);
        if(ret != 0)
        {
            printf("private timeout get error\n");
            private_timeout = 100;
        }
        else
        {
            printf("private timeout = %d\n", private_timeout);
        }
        strncpy(para->private_host, private_host, private_host_len);
        strncpy(para->private_protocol, private_protocol, private_protocol_len);
        strncpy(para->private_tls_switch, private_tls_switch, private_tls_switch_len);
        strncpy(para->private_username, private_username, private_username_len);
        para->private_port = private_port;
        para->private_timeout = private_timeout;
        printf("private_host       = %s\n", para->private_host);
        printf("private_protocol   = %s\n", para->private_protocol);
        printf("private_tls_switch = %s\n", para->private_tls_switch);
        printf("private_username   = %s\n", para->private_username);
        printf("private_port       = %d\n", para->private_port);
        printf("private_timeout    = %d\n", para->private_timeout);
        return 0;
}
void private_cloud_task(lega_thread_arg_t arg)
{
    int timestamp = 1000;
    OSStatus ret_cloud_semaphore;
    lega_rtos_init_semaphore(&private_connect_Semaphore, 0);
    lega_rtos_init_semaphore(&private_Semaphore, 0);
    semaphore_flag = SEMAPHORE_FLAG_NONE;
    if(private_para != NULL)
        lega_rtos_free(private_para);
    private_para = lega_rtos_malloc(sizeof(private_para_t));
    if(private_para == NULL)
    {
        printf("parivate_para malloc err\n");
        goto private_task_err;
    }
    if(private_get_value(private_para) != 0)
    {
        printf("private_get_value err\n");
        goto private_task_err;
    }
    if(!strcmp(private_para->private_protocol, "mqtt"))
        private_mqtt_init();
    else if(!strcmp(private_para->private_protocol, "http"))
        private_http_init();
    else
    {
        printf("private_para protocol err\n");
        goto private_task_err;
    }
    while(1)
    {
        ret_cloud_semaphore = lega_rtos_get_semaphore(&private_Semaphore, CLOUD_TIMEOUT_MS);
        if(ret_cloud_semaphore == kGeneralErr)
        {
            printf("private cloud task...\n");
        }
        else if(ret_cloud_semaphore == kNoErr)
        {
            if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_MESSAGE)
            {


                semaphore_flag = SEMAPHORE_FLAG_NONE;
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_TASK_KILL)
            {
 private_task_err:
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                printf("get semaphore\n");
                printf("mqtt disconnecting...\n");
                if(private_client != NULL)
                {
                    mqtt_disconnect(private_client);
                    mqtt_client_free(private_client);
                    private_client = NULL;
                }
                lega_rtos_deinit_semaphore(&private_connect_Semaphore);
                lega_rtos_deinit_semaphore(&private_Semaphore);
                lega_rtos_delete_thread(&Private_Cloud_Task_Handler);
            }
        }
        lega_rtos_delay_milliseconds(timestamp);
    }
}
#endif
/******************mqtt_broker process********************** */
#ifdef CLOUD_MQTT_BROKER_SUPPORT

static void mqtt_broker_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    printf("mqtt_broker connectiong callback...\r\n");
    if(status == MQTT_CONNECT_ACCEPTED)
    {
        printf("mqtt_broker-connecting-cb: successfully connectd\n");
        lega_rtos_set_semaphore(&mqtt_broker_connect_Semaphore);
    }
    else
    {
        printf("mqtt_broker-connecting-cb: not connected\n");
    }

}
static void mqtt_broker_pub_request_cb(void *arg, err_t result)
{
    if(result != ERR_OK)
         printf("publish err in callback %d\n", result);
}
static void mqtt_broker_sub_request_cb(void *arg, err_t result)
{
    if(result != ERR_OK)
        printf("subscribe err in callback %d\n", result);
}

static void mqtt_broker_incoming_publish_cb(void *arg, const char *topic, uint32_t tot_len)
{
     printf("topic: %s\nlen: %lu\n", topic, tot_len);
    if(strstr(topic, mqtt_broker_sub_topic) != NULL)
    {
        mqtt_comming_topic_idx = MQTT_BROKER_TOPIC_SUBSCRIBE_CONTROL;
    }
    else if(strstr(topic, mqtt_broker_ota_topic) != NULL)
    {
        mqtt_comming_topic_idx = MQTT_BROKER_TOPIC_SUBSCRIBE_OTA;
    }
}
static void mqtt_broker_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags)
{
    printf("mqtt incoming data callback...\n");
    char data_buf[256] = {0};
    strncpy(data_buf, (char *)data, len);
    printf("payload: %s\nlen: %d\n", (char *)data_buf, len);
    if(mqtt_comming_topic_idx == MQTT_BROKER_TOPIC_SUBSCRIBE_CONTROL)
    {
        /* do control command fun */
        parse_mqtt_Command_json(data_buf);

    }
    else if(mqtt_comming_topic_idx == MQTT_BROKER_TOPIC_SUBSCRIBE_OTA)
    {
        /* do ota fun */
        parse_mqtt_OTA_json(data_buf);
    }
}

static void mqtt_broker_init()
{
    int connect_times = MQTT_BROKER_CONNECT_TIMES;
    int i;
    err_t err;
    int port = 1883;
    OSStatus ret_conn_semaphore;
    if( mqtt_broker_conf == NULL )
        mqtt_broker_conf = altcp_tls_create_config_client_2wayauth( \
                                    (const unsigned char *)mqtt_broker_ca_crt, \
                                    strlen(mqtt_broker_ca_crt)+1, \
                                    (const unsigned char *)mqtt_broker_client_private_key, \
                                    strlen(mqtt_broker_client_private_key)+1, \
                                    NULL, \
                                    0, \
                                    (const unsigned char *)mqtt_broker_client_cert, \
                                    strlen(mqtt_broker_client_cert)+1);
    // altcp_allocator_t tls_allocator = {altcp_tls_alloc, conf};
    printf("mqtt start\r\n");
    port = mqtt_broker_para->mqtt_broker_port;
    mqtt_broker_client = mqtt_client_new();
    struct mqtt_connect_client_info_t ci;
    ip_addr_t host_ip;
    int ret;
    uint8_t macaddr[6];
    char pwillMsg[100] = {0};
    memset(&ci, 0, sizeof(ci));
    ret = lega_wlan_get_mac_address(macaddr);
    if(ret==0)
    {
        sprintf(mqtt_broker_device_id,"%s_%s_%02x%02x%02x%02x%02x%02x",mqtt_manufacturer,mqtt_productid,macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
        ci.client_id = mqtt_broker_device_id;
    }
    else
        ci.client_id = "oves_00001_010203040506";
    if(mqtt_broker_para->mqtt_broker_username != NULL)
    ci.client_user = mqtt_broker_para->mqtt_broker_username;
    if(mqtt_broker_para->mqtt_broker_userpass != NULL)
    ci.client_pass = mqtt_broker_para->mqtt_broker_userpass;
    ci.keep_alive = 60;


	
    if(mqtt_broker_get_topic(mqtt_broker_will_topic,MQTT_BROKER_TOPIC_SUBSCRIBE_WILL,NULL)!=0)
    {
        printf("get will topic err\n");
        return;
    }

	
    if(mqtt_broker_get_topic(mqtt_broker_sub_topic,MQTT_BROKER_TOPIC_SUBSCRIBE_CONTROL,mqtt_broker_device_id)!=0)
    {
        printf("get sub topic err\n");
        return;
    }
    if(mqtt_broker_get_topic(mqtt_broker_pub_topic,MQTT_BROKER_TOPIC_PUBLISH,mqtt_broker_device_id)!=0)
    {
        printf("get pub topic err\n");
        return;
    }
    if(mqtt_broker_get_topic(mqtt_broker_ota_topic,MQTT_BROKER_TOPIC_SUBSCRIBE_OTA,mqtt_broker_device_id)!=0)
    {
        printf("get ota topic err\n");
        return;
    }
    if(mqtt_broker_get_topic(mqtt_broker_progress_topic,MQTT_BROKER_TOPIC_PUBLISH_PROGERSS,mqtt_broker_device_id)!=0)
    {
        printf("get ota topic err\n");
        return;
    }
    sprintf(pwillMsg, "will:%s",mqtt_broker_device_id);
    ci.will_msg =(char*)GattAllFieldJsonMerge();// pwillMsg;
    ci.will_qos = 0;
    ci.will_topic =(char*)AtGetPubTopicId(NULL);// mqtt_broker_will_topic;
    ci.will_retain = 0;
    if(!strcmp(mqtt_broker_para->mqtt_broker_tls_switch, "on"))
        ci.tls_config = mqtt_broker_conf;
    else if(!strcmp(mqtt_broker_para->mqtt_broker_tls_switch, "off"))
        ci.tls_config = NULL;
    else
    {
        printf("mqtt_broker_tls_switch err\n");
        ci.tls_config = NULL;
    }
    struct hostent *hostent;
    hostent = gethostbyname(mqtt_broker_para->mqtt_broker_host);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&host_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&host_ip));

mqtt_broker_connecting:
    printf("mqtt connecting...\r\n");
    err = mqtt_client_connect(mqtt_broker_client, &host_ip, port, mqtt_broker_connection_cb, 0, &ci);
    if(err != ERR_OK)
        printf("mqtt connect error, return %d\n", err);
    ret_conn_semaphore = lega_rtos_get_semaphore(&mqtt_broker_connect_Semaphore, MQTT_BROKER_CLOUD_TIMEOUT_MS);
    if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_TASK_KILL)
    {
        return;
    }
    if(ret_conn_semaphore == kGeneralErr)
    {
        printf("mqtt_broker connect timeout\n");
        connect_times--;
        if(connect_times <= 0)
            goto mqtt_broker_error;
        else
        {
            goto mqtt_broker_connecting;
        }
    }
    else if(ret_conn_semaphore == kNoErr)
    {
        printf("get semaphore\n");
    }
    if(mqtt_client_is_connected(mqtt_broker_client) == 1)
    {
        printf("mqtt client connected\n");
    }
    else
    {
        connect_times--;
        if(connect_times <= 0)
        {
            printf("mqtt client is not connected\n");
            printf("mqtt client state: %d\n", mqtt_broker_client->conn_state);
            goto mqtt_broker_error;
        }
        else
        {
            goto mqtt_broker_connecting;
        }
        goto mqtt_broker_error;
    }

    mqtt_set_inpub_callback(mqtt_broker_client, mqtt_broker_incoming_publish_cb,mqtt_broker_incoming_data_cb, NULL);
    err = mqtt_subscribe(mqtt_broker_client,  (char*)AtGetSubscribeId()/*mqtt_broker_sub_topic*/, 0, mqtt_broker_sub_request_cb, NULL);
    if(err != ERR_OK)
        printf("mqtt subscribe err:%d\n", err); 
    else
        printf("mqtt subscribe %s\n",(char*)AtGetSubscribeId()/*mqtt_broker_sub_topic*/);
    lega_rtos_delay_milliseconds(200);
    err = mqtt_subscribe(mqtt_broker_client, mqtt_broker_ota_topic, 0, mqtt_broker_sub_request_cb, NULL);
    if(err != ERR_OK)
        printf("mqtt subscribe err:%d\n", err);
    else
        printf("mqtt subscribe %s\n",mqtt_broker_ota_topic);
    return;

mqtt_broker_error:
    printf("mqtt_broker disconnecting...\n");
    semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
    lega_rtos_set_semaphore(&mqtt_broker_Semaphore);
    return;
}

int mqtt_common_pub(const char *topic,const void *payload, u16_t payload_length)
{
   return mqtt_publish(mqtt_broker_client, topic, payload, payload_length, 0, 0, mqtt_broker_pub_request_cb, NULL);
}

void mqtt_broker_cloud_task(lega_thread_arg_t arg)
{
    int timestamp = 1000;
    OSStatus ret_cloud_semaphore;
    lega_rtos_init_semaphore(&mqtt_broker_connect_Semaphore, 0);
    lega_rtos_init_semaphore(&mqtt_broker_Semaphore, 0);
    semaphore_flag = SEMAPHORE_FLAG_NONE;
    if(mqtt_broker_para != NULL)
        lega_rtos_free(mqtt_broker_para);
    mqtt_broker_para = lega_rtos_malloc(sizeof(mqtt_broker_para_t));
    if(mqtt_broker_para == NULL)
    {
        printf("mqtt_broker_para malloc err\n");
        goto mqtt_broker_task_err;
    }
    if(mqtt_broker_get_value(mqtt_broker_para) != 0)
    {
        printf("mqtt_broker_get_value err\n");
        goto mqtt_broker_task_err;
    }
   // if(light_gpio_init_flag != 1)
   //     light_gpio_init();
    mqtt_broker_init();

    while(1)
    {
        ret_cloud_semaphore = lega_rtos_get_semaphore(&mqtt_broker_Semaphore, CLOUD_TIMEOUT_MS);
        if(ret_cloud_semaphore == kGeneralErr)
        {
            printf("mqtt_broker cloud task...kGeneralErr \n");
        }
        else if(ret_cloud_semaphore == kNoErr)
        {
        	printf("mqtt_broker cloud task...kNoErr\n");
			
            if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_MESSAGE)
            {
               /* if(light_flag == LIGHT_ON)
                {
                    printf("light on\n");
                    mqtt_common_pub(mqtt_broker_pub_topic,mqtt_broker_pub_payload_on, strlen(mqtt_broker_pub_payload_on));
                }
                else if(light_flag == LIGHT_OFF)
                {
                    printf("light off\n");
                    mqtt_common_pub(mqtt_broker_pub_topic,mqtt_broker_pub_payload_off, strlen(mqtt_broker_pub_payload_off));
                }*/
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                lega_rtos_delay_milliseconds(timestamp);
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_TASK_KILL||at_user_info.sta_connected!=1)
            {
 mqtt_broker_task_err:
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                printf("get semaphore\n");
                printf("mqtt disconnecting...\n");
                if(mqtt_broker_client != NULL)
                {
                    altcp_tls_free_config(mqtt_broker_conf);
                    mqtt_broker_conf = NULL;
                    mqtt_disconnect(mqtt_broker_client);
                    mqtt_client_free(mqtt_broker_client);
                    mqtt_broker_client = NULL;
                }
                lega_rtos_deinit_semaphore(&mqtt_broker_connect_Semaphore);
                lega_rtos_deinit_semaphore(&mqtt_broker_Semaphore);
				Mqtt_broker_Cloud_Task_Handler=NULL;
                lega_rtos_delete_thread(NULL);
				
            }
        }
        //判断是否连接状态，断开连接则重新连接
        if (mqtt_client_is_connected(mqtt_broker_client) != 1&&at_user_info.sta_connected==1)
        {
            printf("==========mqtt broker reconnect cnt %d===========\n",mqtt_broker_reconnect_cnt++);
            if(mqtt_broker_client != NULL)
            {
                altcp_tls_free_config(mqtt_broker_conf);
                mqtt_broker_conf = NULL;
                mqtt_disconnect(mqtt_broker_client);
                mqtt_client_free(mqtt_broker_client);
                mqtt_broker_client = NULL;
            }
            mqtt_broker_init();
        }

		if(at_user_info.sta_connected!=1)
			goto mqtt_broker_task_err;
		
        lega_rtos_delay_milliseconds(timestamp);
    }
}
#endif
/******************baidu process********************** */
#ifdef CLOUD_BAIDU_SUPPORT
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    printf("baidu connectiong callback...\r\n");
    if(status == MQTT_CONNECT_ACCEPTED)
    {
        printf("baidu-connecting-cb: successfully connectd\n");
        lega_rtos_set_semaphore(&baidu_connect_Semaphore);
    }
    else
    {
        printf("baidu-connecting-cb: not connected\n");
        baidu_connected_subscribe_ready = 0;
    }

}
void mqtt_pub_request_cb(void *arg, err_t result)
{
    printf("mqtt publish request callback...\n");
    if(result != ERR_OK)
        printf("publish err in callback %d\n", result);
}
void mqtt_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags)
{
    printf("mqtt incoming data callback...\n");
    printf("payload: %s\nlen: %d\n", (char *)data, len);
    char data_buf[256] = {0};
    char data_tmp[32] = {0};
    char *pdata = NULL;
    strncpy(data_buf, (char *)data, len);
    pdata = NULL;
    if((pdata = strstr(data_buf, "switch")) != NULL)
    {
        strncpy(data_tmp, pdata, 16);
        if(strstr(pdata, "True") != NULL)
            light_flag = LIGHT_ON;
        else if(strstr(pdata, "False") != NULL)
            light_flag = LIGHT_OFF;
        else
            return;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
        light_control();
    }
}


uint8_t baidu_get_value(struct baidu_para_struct *baidupara)
{
    int32_t ret;
    char baidu_username[32] = {0};
    int32_t baidu_username_len = 32;
    char baidu_userpass[64] = {0};
    int32_t baidu_userpass_len = 64;
    char baidu_clientid[32] = {0};
    int32_t baidu_clientid_len = 32;
    char baidu_url[64] = {0};
    int32_t baidu_url_len = 64;
    int baidu_timeout = 100;
    int32_t baidu_timeout_len = 4;
        if(baidupara == NULL)
        {
            printf("baidu_para = NULL\n");
            return -1;
        }

        ret = lega_flash_kv_get("baidu_username", baidu_username, &baidu_username_len);
        if(ret != KV_OK)
        {
            printf("baidu username get error\n");
            return -1;
        }
        else
        {
            printf("baidu username = %s\n", baidu_username);
        }

        ret = lega_flash_kv_get("baidu_userpass", baidu_userpass, &baidu_userpass_len);
        if(ret != KV_OK)
        {
            printf("baidu userpass get error\n");
            return -1;
        }
        else
        {
            printf("baidu userpass = %s\n", baidu_userpass);
        }

        ret = lega_flash_kv_get("baidu_clientid", baidu_clientid, &baidu_clientid_len);
        if(ret != KV_OK)
        {
            printf("baidu clientid get error\n");
            return -1;
        }
        else
        {
            printf("baidu clientid = %s\n", baidu_clientid);
        }

        ret = lega_flash_kv_get("baidu_url", baidu_url, &baidu_url_len);
        if(ret != KV_OK)
        {
            printf("baidu url get error\n");
            return -1;
        }
        else
        {
            printf("baidu url = %s\n", baidu_url);
        }

        ret = lega_flash_kv_get("baidu_timeout", &baidu_timeout, &baidu_timeout_len);
        if(ret != KV_OK)
        {
            printf("baidu timeout get error\n");
        }
        else
        {
            printf("baidu timeout = %d\n", baidu_timeout);
        }
        strncpy(baidupara->baidu_username, baidu_username, baidu_username_len);
        baidupara->baidu_username_len = baidu_username_len;
        strncpy(baidupara->baidu_userpass, baidu_userpass, baidu_userpass_len);
        baidupara->baidu_userpass_len = baidu_userpass_len;
        strncpy(baidupara->baidu_clientid, baidu_clientid, baidu_clientid_len);
        baidupara->baidu_clientid_len = baidu_clientid_len;
        strncpy(baidupara->baidu_url, baidu_url, baidu_url_len);
        baidupara->baidu_url_len = baidu_url_len;
        baidupara->baidu_timeout = baidu_timeout;
        baidupara->baidu_timeout_len = baidu_timeout_len;
        return 0;
}
#define     BAIDU_TOPIC_PUBLISH          0
#define     BAIDU_TOPIC_SUBSCRIBE        1
static struct baidu_para_struct baidu_para = {0};
int baidu_mqtt_get_topic(char *topic_out, int action_in)
{
    char *ptmp_buf = NULL;
    int topic_len = 0;
    if(topic_out == NULL)
    {
        printf("topic_out is NULL\n");
        return -1;
    }
    if(action_in == BAIDU_TOPIC_PUBLISH)
    {
        ptmp_buf = "$baidu/iot/shadow/%s/update";
    }
    else if(action_in == BAIDU_TOPIC_SUBSCRIBE)
    {
        ptmp_buf = "$baidu/iot/shadow/%s/delta";
    }
    else
    {
        printf("topic input error\n");
        return -1;
    }
    topic_len = strlen(ptmp_buf) + strlen(baidu_para.baidu_clientid) + 1;
    snprintf(topic_out, topic_len, ptmp_buf, baidu_para.baidu_clientid);
    printf("topic:%s\n", topic_out);
    return 0;
}
void baidu_demo()
{
    const char *p_payload = NULL;
    int connect_times = BAIDU_CONNECT_TIMES;
    uint8_t ret = 0;
    int i = 0;
    int port = 1884;
    err_t err;
    ip_addr_t things_ip;
    struct mqtt_connect_client_info_t ci;
    uint8_t qos = 0;
    uint8_t retain = 0;
    OSStatus ret_conn_semaphore;

    memset(&baidu_para, 0, sizeof(struct baidu_para_struct));
    if((ret = baidu_get_value(&baidu_para)) != 0)
    {
        printf("get baidu para error\n");
        return;
    }
    else
    {
        printf("baidu client id = %s\n", baidu_para.baidu_clientid);
        printf("baidu user name = %s\n", baidu_para.baidu_username);
        printf("baidu user pass = %s\n", baidu_para.baidu_userpass);
        printf("baidu url       = %s\n", baidu_para.baidu_url);
    }

    struct hostent *hostent;
    hostent = gethostbyname(baidu_para.baidu_url);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));

    struct altcp_tls_config *conf = altcp_tls_create_config_client((const unsigned char *)mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
    //printf("ca:\n %s\n", mbedtls_test_cas_pem);
    printf("ca sizeof = %d\n", mbedtls_test_cas_pem_len);
    //altcp_allocator_t tls_allocator = {altcp_tls_alloc, conf};
    printf("mqtt start\n");
    baidu_client = mqtt_client_new();
    //init mqtt client
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = baidu_para.baidu_clientid;
    ci.client_user = baidu_para.baidu_username;
    ci.keep_alive = 60;
    ci.client_pass = baidu_para.baidu_userpass;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = conf;

baidu_connecting:
    err = mqtt_client_connect(baidu_client, &things_ip, port, mqtt_connection_cb, 0, &ci);
    if(err != ERR_OK)
        printf("mqtt connect error, return %d\n", err);
    ret_conn_semaphore = lega_rtos_get_semaphore(&baidu_connect_Semaphore, BAIDU_CLOUD_TIMEOUT_MS);
    if(ret_conn_semaphore == kGeneralErr)
    {
        printf("baidu connect timeout\n");
        connect_times--;
        if(connect_times <= 0)
            goto baidu_error;
        else
        {
            goto baidu_connecting;
        }
    }
    else if(ret_conn_semaphore == kNoErr)
    {
        printf("get semaphore\n");
    }
    if(mqtt_client_is_connected(baidu_client) == 1)
    {
        printf("mqtt client connected\n");
#ifdef TEST_UNITY_CLOUD
        test_cloud_connected_flag = 1;
#endif
    }
    else
    {
        connect_times--;
        if(connect_times <= 0)
        {
            printf("mqtt client is not connected\n");
            printf("mqtt client state: %d\n", baidu_client->conn_state);
            goto baidu_error;
        }
        else
        {
            goto baidu_connecting;
        }
        goto baidu_error;
    }

    {
        printf("mqtt publishing...\n");
        if(light_flag == LIGHT_ON)
            p_payload = baidu_pub_payload_on;
        else if(light_flag == LIGHT_OFF)
            p_payload = baidu_pub_payload_off;
        else
        {
            printf("baidu light get error\n");
            goto baidu_error;
        }
        printf("payload : %s\n", p_payload);
        if (baidu_mqtt_get_topic(baidu_topic_buf, BAIDU_TOPIC_PUBLISH)) goto baidu_error;  //such as "$baidu/iot/shadow/dev01_1/update"
        err = mqtt_publish(baidu_client, baidu_topic_buf, p_payload, strlen(p_payload), qos, retain, mqtt_pub_request_cb, NULL);
        if(err != ERR_OK)
            printf("mqtt publish err: %d\n", err);
        printf("mqtt subscribing...\n");
        mqtt_set_inpub_callback(baidu_client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);
        if (baidu_mqtt_get_topic(baidu_topic_buf, BAIDU_TOPIC_SUBSCRIBE)) goto baidu_error;  //such as "$baidu/iot/shadow/dev01_1/update"
        err = mqtt_subscribe(baidu_client, baidu_topic_buf, qos, mqtt_sub_request_cb, NULL);
        if(err != ERR_OK)
            printf("mqtt subscribe err:%d\n", err);
        else
        {
            printf("mqtt subscribe %s\n",baidu_topic_buf);
            baidu_connected_subscribe_ready = 1;
        }
    }
    return;
baidu_error:
    printf("baidu disconnecting...\n");
    semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
    lega_rtos_set_semaphore(&baidu_Semaphore);
    return;
}
void baidu_cloud_task(lega_thread_arg_t arg)
{
    int timestamp = 1000;
    OSStatus ret_cloud_semaphore;
    lega_rtos_init_semaphore(&baidu_connect_Semaphore, 0);
    lega_rtos_init_semaphore(&baidu_Semaphore, 0);
    baidu_connected_subscribe_ready = 0;
    if(light_gpio_init_flag != 1)
        light_gpio_init();
    semaphore_flag = SEMAPHORE_FLAG_NONE;
    baidu_demo();
    while(1)
    {
        ret_cloud_semaphore = lega_rtos_get_semaphore(&baidu_Semaphore, CLOUD_TIMEOUT_MS);
        if(ret_cloud_semaphore == kGeneralErr)
        {
            printf("baidu cloud task...\n");
        }
        else if(ret_cloud_semaphore == kNoErr)
        {
            if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_MESSAGE)
            {
                if(light_flag == LIGHT_ON)
                {
                    printf("light on\n");
                    if (!baidu_mqtt_get_topic(baidu_topic_buf, BAIDU_TOPIC_PUBLISH))
                    {
                        mqtt_publish(baidu_client, baidu_topic_buf, baidu_pub_payload_on, strlen(baidu_pub_payload_on), 0, 0, mqtt_pub_request_cb, NULL);
                    }

                }
                else if(light_flag == LIGHT_OFF)
                {
                    printf("light off\n");
                    if (!baidu_mqtt_get_topic(baidu_topic_buf, BAIDU_TOPIC_PUBLISH))
                    {
                        mqtt_publish(baidu_client, baidu_topic_buf, baidu_pub_payload_off, strlen(baidu_pub_payload_off), 0, 0, mqtt_pub_request_cb, NULL);
                    }

                }
#ifdef TEST_UNITY_CLOUD
                test_cloud_upload_flag = 1;
#endif
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                lega_rtos_delay_milliseconds(timestamp);
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_TASK_KILL)
            {
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                printf("get semaphore\n");
                printf("mqtt disconnecting...\n");
                if(baidu_client != NULL)
                {
                    mqtt_disconnect(baidu_client);
                    mqtt_client_free(baidu_client);
                    baidu_client = NULL;
                }
                baidu_connected_subscribe_ready = 0;
                lega_rtos_deinit_semaphore(&baidu_connect_Semaphore);
                lega_rtos_deinit_semaphore(&baidu_Semaphore);
                lega_rtos_delete_thread(&Baidu_Cloud_Task_Handler);
            }
        }
         //判断是否连接状态，断开连接则重新连接
        if (mqtt_client_is_connected(baidu_client) != 1)
        {
            printf("==========baidu reconnect cnt %d===========\n",baidu_reconnect_cnt++);
            baidu_connected_subscribe_ready = 0;
            if(baidu_client != NULL)
            {
                mqtt_disconnect(baidu_client);
                mqtt_client_free(baidu_client);
                baidu_client = NULL;
            }
            baidu_demo();
        }
        lega_rtos_delay_milliseconds(timestamp);
    }
}
#endif

#ifdef CLOUD_JD_SUPPORT
void jd_mqtt_pub_request_cb(void *arg, err_t result)
{
    printf("mqtt publish request callback...\n");
    if(result != ERR_OK)
        printf("publish err in callback %d\n", result);
}
void jd_light_publish(int light_switch)
{

    if(jd_pub_payload_light != NULL)
    {
        lega_rtos_free(jd_pub_payload_light);
        jd_pub_payload_light = NULL;
    }
    if(light_switch == LIGHT_ON || light_switch == LIGHT_OFF)
    {
#ifdef TEST_UNITY_CLOUD
                test_cloud_upload_flag = 1;
#endif
        jd_mqtt_get_topic(p_jd_mqtt_buf->pub_topic_buf, UP_SIDE);
        jd_pub_payload_light = jd_get_pub_payload(light_switch, UPDATE_SHADOW);
        if(jd_pub_payload_light == NULL)
            return ;
        mqtt_publish(jd_client, p_jd_mqtt_buf->pub_topic_buf, jd_pub_payload_light, strlen(jd_pub_payload_light), 0, 0, jd_mqtt_pub_request_cb, NULL);
        if(jd_pub_payload_light != NULL)
        {
            lega_rtos_free(jd_pub_payload_light);
            jd_pub_payload_light = NULL;
        }
        jd_mqtt_get_topic(p_jd_mqtt_buf->pub_topic_buf, UP_PROPERTY);
        jd_pub_payload_light = jd_get_pub_payload(light_switch, UPDATE_PROPERTY);
        mqtt_publish(jd_client, p_jd_mqtt_buf->pub_topic_buf, jd_pub_payload_light, strlen(jd_pub_payload_light), 0, 0, jd_mqtt_pub_request_cb, NULL);
        if(jd_pub_payload_light != NULL)
        {
            lega_rtos_free(jd_pub_payload_light);
            jd_pub_payload_light = NULL;
        }
    }
    else
    {
        printf("jd light publish para error\n");
        return;
    }

}
void jd_incoming_publish_cb(void *arg, const char *topic, uint32_t tot_len)
{
    printf("topic: %s\nlen: %lu\n", topic, tot_len);
}
void jd_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags)
{
    printf("jd incoming data callback:\n");
    char data_buf[512] = {0};
    uint32_t version_tmp = 0;
    int tempValue;
    char *pdata = NULL;
    int i;
    err_t err;
    char para_buf[15] = {0};
    memset(data_buf, 0, sizeof(data_buf));
    strncpy(data_buf, (char *)data, len);
    printf("%s\nlen: %d\n", data_buf, len);
    pdata = data_buf;
    while((pdata = strstr(pdata, "version")) != NULL)
    {
        pdata += 9;
        for(i = 0; i < 9; i++)
        {
            if(*pdata <= '9' && *pdata >= '0')
            {
                para_buf[i] = *pdata;
                pdata ++;
            }
            else
                break;
        }
        if(strlen(para_buf) != 0)
        {
            tempValue = convert_str_to_int(para_buf);
            if(tempValue == LEGA_STR_TO_INT_ERR || tempValue < 0)
            {
                printf("version param err\n");
            }
            if(version_tmp < tempValue)
                version_tmp = tempValue;
        }
        memset(para_buf, 0, sizeof(para_buf));
    }
    if(version_tmp != 0)
    {
        _shadowVersion = version_tmp + 1;
        printf("jd_shadowVersion = %ld\n", _shadowVersion);
    }
    if(((strstr(data_buf, "metadata") == NULL)) && (strstr(data_buf, "desired") != NULL) && ((pdata = strstr(data_buf, "switch")) != NULL))
    {
        if(strstr(data_buf, "True") != NULL)
        {
            light_flag = LIGHT_ON;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
            light_control();
        }
        if(strstr(data_buf, "False") != NULL)
        {
            light_flag = LIGHT_OFF;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
            light_control();
        }
    }
    if(((pdata=strstr(data_buf, "state")) != NULL) && ((pdata=strstr(pdata, "desired")) != NULL) && ((pdata=strstr(pdata, "switch")) != NULL))
    {
        pdata = pdata + 9;
        strncpy(para_buf, pdata, 10);
        if(strstr(para_buf, "True") != NULL)
        {
            light_flag = LIGHT_ON;
            light_control();
        }
        if(strstr(para_buf, "False") != NULL)
        {
            light_flag = LIGHT_OFF;
            light_control();
        }
        memset(para_buf, 0, sizeof(para_buf));
    }
    if(strstr(data_buf, "Parameter version with error") != NULL)
    {
        jd_mqtt_get_topic(p_jd_mqtt_buf->pub_topic_buf, GET_SHADOW);
        printf("jd publishing get shadow...\n");
        printf("publish_payload : %s\n", jd_pub_payload_shadow_get);
        err = mqtt_publish(jd_client, p_jd_mqtt_buf->pub_topic_buf, jd_pub_payload_shadow_get, strlen(jd_pub_payload_shadow_get), 1, 0, jd_mqtt_pub_request_cb, NULL);
        if(err != ERR_OK)
            printf("mqtt publish err: %d\n", err);
        else
        {
            printf("mqtt publish topic:%s\nmqtt publish payload\%s\n", p_jd_mqtt_buf->pub_topic_buf, jd_pub_payload_shadow_get);
        }
    }
}
void jd_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    printf("mqtt connectiong callback...\r\n");
    if(status == MQTT_CONNECT_ACCEPTED)
    {
        printf("jd-connecting-cb: successfully connectd\n");
        lega_rtos_set_semaphore(&jd_connect_Semaphore);
    }
    else
    {
        printf("jd-connecting-cb: not connected, status = %d\n", status);
    }
}
uint8_t jd_get_value(jd_mqtt_auth_info_t *jd_para)
{
    int32_t ret;
    jd_dev_meta_info_t jd_meta_para;
    char productKey[IOT_PRODUCT_KEY_LEN + 1] = {0};
    int32_t productKey_len = IOT_PRODUCT_KEY_LEN;
    char productSecret[IOT_PRODUCT_SECRET_LEN +1] = {0};
    int32_t productSecret_len = IOT_PRODUCT_SECRET_LEN;
    char identifer[IOT_IDENTIFIER_LEN + 1] = {0};
    int32_t identifer_len = IOT_IDENTIFIER_LEN;
    char deviceName[IOT_DEVICE_NAME_LEN + 1] = {0};
    int32_t deviceName_len = IOT_DEVICE_NAME_LEN;
    char deviceSecret[IOT_DEVICE_SECRET_LEN + 1] = {0};
    int32_t deviceSecret_len = IOT_DEVICE_SECRET_LEN;
    memset(&jd_meta_para, 0, sizeof(jd_dev_meta_info_t));
        if(jd_para == NULL)
        {
            printf("jd_para = NULL\n");
            return -1;
        }

        ret = lega_flash_kv_get("jd_productKey", productKey, &productKey_len);
        if(ret != KV_OK)
        {
            printf("jd productKey get error\n");
            return -1;
        }
        else
        {
            printf("jd productKey = %s\n", productKey);
        }

        ret = lega_flash_kv_get("jd_productSecret", productSecret, &productSecret_len);
        if(ret != KV_OK)
        {
            printf("jd productSecret get error\n");
            //return -1;
        }
        else
        {
            printf("jd productSecret = %s\n", productSecret);
        }

        ret = lega_flash_kv_get("jd_identifer", identifer, &identifer_len);
        if(ret != KV_OK)
        {
            printf("jd identifer get error\n");
            return -1;
        }
        else
        {
            printf("jd identifer = %s\n", identifer);
        }

        ret = lega_flash_kv_get("jd_deviceName", deviceName, &deviceName_len);
        if(ret != KV_OK)
        {
            printf("jd deviceName get error\n");
            return -1;
        }
        else
        {
            printf("jd deviceName = %s\n", deviceName);
        }

        ret = lega_flash_kv_get("jd_deviceSecret", &deviceSecret, &deviceSecret_len);
        if(ret != KV_OK)
        {
            printf("jd deviceSecret get error\n");
        }
        else
        {
            printf("jd deviceSecret = %s\n", deviceSecret);
        }

        strncpy(jd_meta_para.product_key, productKey, productKey_len);
#if defined(DYNAMIC_REGISTER) || defined(DEVICE_GATEWAY)
        strncpy(jd_meta_para.product_secret, productSecret, productSecret_len);
#endif
        strncpy(jd_meta_para.device_id, identifer, identifer_len);
        strncpy(jd_meta_para.device_name, deviceName, deviceName_len);
        strncpy(jd_meta_para.device_secret, deviceSecret, deviceSecret_len);
        if(SUCCESS_RETURN != iot_get_auth_info(&jd_meta_para, jd_para))
        {
            printf("iot_get_auth_info error\n");
            return 1;
        }
        return 0;
}
void jd_demo()
{
    uint8_t ret = 0;
    int i = 0;
    int port = 8883;
    int connect_times = JD_CONNECT_TIMES;
    err_t err;
    ip_addr_t things_ip;
    struct mqtt_connect_client_info_t ci;
    uint8_t qos = 0;
    uint8_t retain = 0;
    OSStatus ret_conn_semaphore;
    jd_mqtt_auth_info_t jd_para;
    memset(&jd_para, 0, sizeof(jd_mqtt_auth_info_t));
    if((ret = jd_get_value(&jd_para)) != 0)
    {
        printf("get jd para error\n");
        return;
    }
    else
    {
        printf("jd client id = %s\n", jd_para.clientid);
        printf("jd user name = %s\n", jd_para.username);
        printf("jd user pass = %s\n", jd_para.password);
        printf("jd host name = %s\n", jd_para.hostname);
        printf("jd port      = %d\n", jd_para.port);
    }

    struct hostent *hostent;
    hostent = gethostbyname(jd_para.hostname);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));

    struct altcp_tls_config *conf = altcp_tls_create_config_client((const unsigned char *)jd_ca_crt, strlen(jd_ca_crt)+1);
    printf("ca strlen = %d\n", strlen(jd_ca_crt));
    printf("mqtt start\n");
    jd_client = mqtt_client_new();
    //init mqtt jd_client
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = jd_para.clientid;
    ci.client_user = jd_para.username;
    ci.keep_alive = 60;
    ci.client_pass = jd_para.password;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = conf;

connecting:
    err = mqtt_client_connect(jd_client, &things_ip, port, jd_connection_cb, 0, &ci);
    if(err != ERR_OK)
        printf("mqtt connect error, return %d\n", err);
    ret_conn_semaphore = lega_rtos_get_semaphore(&jd_connect_Semaphore, CLOUD_TIMEOUT_MS);
    if(ret_conn_semaphore == kGeneralErr)
    {
        printf("cloud connect timeout\n");
        connect_times--;
        if(connect_times <= 0)
            goto jd_error;
        else
        {
            goto connecting;
        }
    }
    else if(ret_conn_semaphore == kNoErr)
    {
        printf("get semaphore\n");
    }
    if(mqtt_client_is_connected(jd_client) == 1)
    {
        printf("mqtt client connected\n");
#ifdef TEST_UNITY_CLOUD
        test_cloud_connected_flag = 1;
#endif
    }
    else
    {
        printf("mqtt client is not connected\n");
        printf("mqtt client state: %d\n", jd_client->conn_state);
        goto jd_error;
    }
    ret = jd_mqtt_get_topic(p_jd_mqtt_buf->sub_topic_buf, DOWN_SIDE);
    if (ret == 0)
    {
        printf("mqtt subscribing...DOWN_SIDE\n\n");
        mqtt_set_inpub_callback(jd_client, jd_incoming_publish_cb, jd_incoming_data_cb, NULL);
        err = mqtt_subscribe(jd_client, p_jd_mqtt_buf->sub_topic_buf, qos, mqtt_sub_request_cb, NULL);
        if(err != ERR_OK)
            printf("mqtt subscribe err:%d\n", err);
        else
            printf("mqtt subscribe %s\n", p_jd_mqtt_buf->sub_topic_buf);
    }
    lega_rtos_delay_milliseconds(1000);
    ret = jd_mqtt_get_topic(p_jd_mqtt_buf->sub_topic_buf, GET_SHADOW_RPY);
    if (ret == 0)
    {
        printf("mqtt subscribing...get reply\n\n");
        err = mqtt_subscribe(jd_client, p_jd_mqtt_buf->sub_topic_buf, qos, mqtt_sub_request_cb, NULL);
        if(err != ERR_OK)
            printf("mqtt subscribe err:%d\n", err);
        else
            printf("mqtt subscribe %s\n", p_jd_mqtt_buf->sub_topic_buf);
    }
    lega_rtos_delay_milliseconds(1000);
    ret = jd_mqtt_get_topic(p_jd_mqtt_buf->pub_topic_buf, GET_SHADOW);
    if(ret == 0)
    {
        printf("mqtt publishing...GET_SHADOW\n\n");
        printf("payload : %s\n", jd_pub_payload_shadow_get);
        err = mqtt_publish(jd_client, p_jd_mqtt_buf->pub_topic_buf, jd_pub_payload_shadow_get, strlen(jd_pub_payload_shadow_get), qos, retain, jd_mqtt_pub_request_cb, NULL);
        if(err != ERR_OK)
            printf("mqtt publish err: %d\n", err);
        else
        {
            printf("mqtt publish topic:%s\n", p_jd_mqtt_buf->pub_topic_buf);
        }

    }
    lega_rtos_delay_milliseconds(1000);
    ret = jd_mqtt_get_topic(p_jd_mqtt_buf->sub_topic_buf, POST_REPLY);
    if (ret == 0)
    {
        printf("mqtt subscribing...post reply\n\n");
        err = mqtt_subscribe(jd_client, p_jd_mqtt_buf->sub_topic_buf, qos, mqtt_sub_request_cb, NULL);
        if(err != ERR_OK)
            printf("mqtt subscribe err:%d\n", err);
        else
            printf("mqtt subscribe %s\n", p_jd_mqtt_buf->sub_topic_buf);
    }
    jd_light_publish(light_flag);
    return;
jd_error:
    printf("mqtt disconnecting...\n");
    semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
    lega_rtos_set_semaphore(&jd_Semaphore);
    return;
}

void jd_cloud_task(lega_thread_arg_t arg)
{
    int timestamp = 1000;
    OSStatus ret_cloud_semaphore;
    cJSON_InitHooks(NULL);
    lega_rtos_init_semaphore(&jd_connect_Semaphore, 0);
    lega_rtos_init_semaphore(&jd_Semaphore, 0);
    p_jd_mqtt_buf = lega_rtos_malloc(sizeof(jd_mqtt_buf_t));
    if(p_jd_mqtt_buf ==NULL)
        return ;
    memset(p_jd_mqtt_buf, 0, sizeof(jd_mqtt_buf_t));
    jd_pub_payload_shadow_get = jd_get_pub_payload(0, GET_SHADOW);
    if(light_gpio_init_flag != 1)
        light_gpio_init();
    semaphore_flag = SEMAPHORE_FLAG_NONE;
    jd_demo();
    while(1)
    {
        ret_cloud_semaphore = lega_rtos_get_semaphore(&jd_Semaphore, CLOUD_TIMEOUT_MS);
        if(ret_cloud_semaphore == kGeneralErr)
        {
            printf("jd cloud task...\n");
        }
        else if(ret_cloud_semaphore == kNoErr)
        {
            if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_MESSAGE)
            {
                if(light_flag == LIGHT_ON)
                {
                    printf("light on\n");
                    jd_light_publish(LIGHT_ON);
                }
                else if(light_flag == LIGHT_OFF)
                {
                    printf("light off\n");
                    jd_light_publish(LIGHT_OFF);
                }
                lega_rtos_delay_milliseconds(timestamp);
                semaphore_flag = SEMAPHORE_FLAG_NONE;
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_TASK_KILL || semaphore_flag == SEMAPHORE_FLAG_CLOUD_RECONNECTING)
            {
                printf("get semaphore\n");
                printf("mqtt disconnecting...\n");
                if(jd_client != NULL)
                {
                    mqtt_disconnect(jd_client);
                    mqtt_client_free(jd_client);
                    jd_client = NULL;
                }
                lega_rtos_deinit_semaphore(&jd_connect_Semaphore);
                lega_rtos_deinit_semaphore(&jd_Semaphore);
                lega_rtos_delete_thread(&JD_Cloud_Task_Handler);
                if(jd_pub_payload_light != NULL)
                    lega_rtos_free(jd_pub_payload_light);
                if(jd_pub_payload_shadow_get != NULL)
                    lega_rtos_free(jd_pub_payload_shadow_get);
                if(p_jd_mqtt_buf != NULL)
                    lega_rtos_free(p_jd_mqtt_buf);
                jd_pub_payload_light= NULL;
                jd_pub_payload_shadow_get = NULL;
                p_jd_mqtt_buf = NULL;
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_RECONNECTING)
                {
                    jd_demo();
                }
            }
        }
        lega_rtos_delay_milliseconds(timestamp);
    }
}

#endif
#ifdef CLOUD_TENCENT_SUPPORT
void tencent_pub_request_cb(void *arg, err_t result)
{
    printf("mqtt publish request callback...\n");
    if(result != ERR_OK)
        printf("publish err in callback %d\n", result);
}
int tencent_publish(int publish_info)
{
    char topic_buf[64] = {0};
    char payload_buf[128] = {0};
    if(publish_info == LIGHT_ON)
    {
        if(tencent_mqtt_get_topic(topic_buf, TENCENT_TOPIC_PUBLISH) != 0 || tencent_mqtt_get_publish_payload(payload_buf, PAYLOAD_LIGHT_ON) != 0)
        {
            return -1;
        }
        mqtt_publish(tencent_client, topic_buf, payload_buf, strlen(payload_buf), 0, 0, tencent_pub_request_cb, NULL);
    }
    else if(publish_info == LIGHT_OFF)
    {
        if(tencent_mqtt_get_topic(topic_buf, TENCENT_TOPIC_PUBLISH) != 0 || tencent_mqtt_get_publish_payload(payload_buf, PAYLOAD_LIGHT_OFF) != 0)
        {
            return -1;
        }
        mqtt_publish(tencent_client, topic_buf, payload_buf, strlen(payload_buf), 0, 0, tencent_pub_request_cb, NULL);
    }
    else if(publish_info == TENCENT_GET_SHADOW)
    {
        if(tencent_mqtt_get_topic(topic_buf, TENCENT_TOPIC_PUBLISH)  != 0 || tencent_mqtt_get_publish_payload(payload_buf, PAYLOAD_GET_SHADOW) != 0)
        {
            return -1;
        }
        mqtt_publish(tencent_client, topic_buf, payload_buf, strlen(payload_buf), 0, 0, tencent_pub_request_cb, NULL);
    }
    else
    {
        printf("tencent_light_publish input error\n");
        return -1;
    }
#ifdef TEST_UNITY_CLOUD
                test_cloud_upload_flag = 1;
#endif
    return 0;
}

int tencent_subscribe(int subscribe_info)
{
    err_t err;
    char topic_buf[64] = {0};
    tencent_mqtt_get_topic(topic_buf, subscribe_info);
    err = mqtt_subscribe(tencent_client, topic_buf, 0, mqtt_sub_request_cb, NULL);
    if(err != ERR_OK)
    {
        printf("mqtt subscribe err:%d\n", err);
        printf("topic:%s\n", topic_buf);
        return -1;
    }
    return 0;
}
void tencent_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    printf("mqtt connectiong callback...\r\n");
    if(status == MQTT_CONNECT_ACCEPTED)
    {
        printf("tencent-connecting-cb: successfully connectd\n");
        lega_rtos_set_semaphore(&tencent_connect_Semaphore);
    }
    else
    {
        printf("tencent-connecting-cb: not connected, status = %d\n", status);
        tencent_connected_subscribe_ready = 0;
    }

}

void tencent_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags)
{
    printf("tencent incoming data callback...\n");
    printf("payload: %s\nlen: %d\n", (char *)data, len);
    char data_buf[512] = {0};
    char *pdata = NULL;
    int i;
    int tempValue;
    char para_buf[15] = {0};
    strncpy(data_buf, (char *)data, len);
    pdata = NULL;
    if((pdata = strstr(data_buf, "version")) != NULL)
    {
        pdata = pdata+9;
        for(i = 0; i < 9; i++)
        {
            if(*pdata <= '9' && *pdata >= '0')
            {
                para_buf[i] = *pdata;
                pdata ++;
            }
            else
                break;
        }
        if(strlen(para_buf) != 0)
        {
            tempValue = convert_str_to_int(para_buf);
            if(tencent_shadowVersion != tempValue)
                tencent_shadowVersion = tempValue;
            printf("tencent_shadowVersion = %d\n", tencent_shadowVersion);
        }
    }
    if(strstr(data_buf, "result") != NULL)
    {
        printf("result...\n");
        return ;
    }
    if((strstr(data_buf, "switch")) != NULL)
    {
        if(strstr(data_buf, "True") != NULL)
        {
            printf("-->True\n");
            light_flag = LIGHT_ON;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
            light_control();
        }
        if(strstr(data_buf, "False") != NULL)
        {
            printf("-->False\n");
            light_flag = LIGHT_OFF;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
            light_control();
        }
    }
    // if(pdata = strstr(data_buf, "result") != NULL)
    // {
    //     jd_mqtt_get_topic(p_jd_mqtt_buf->pub_topic_buf, GET_SHADOW);
    //     printf("jd publishing get shadow...\n");
    //     printf("payload : %s\n", jd_pub_payload_shadow_get);
    //     err = mqtt_publish(jd_client, p_jd_mqtt_buf->pub_topic_buf, jd_pub_payload_shadow_get, strlen(jd_pub_payload_shadow_get), 1, 0, mqtt_pub_request_cb, NULL);
    //     if(err != ERR_OK)
    //         printf("mqtt publish err: %d\n", err);
    //     else
    //     {
    //         printf("mqtt publish topic:%s\nmqtt publish payload\%s\n", p_jd_mqtt_buf->pub_topic_buf, jd_pub_payload_shadow_get);
    //     }
    // }
}

uint8_t tencent_get_value(mqtt_auth_info_t *tencent_para)
{
    uint8_t ret = -1;
    char productID[TENCENT_PRODUCT_ID_LEN + 1] = {0};
    int32_t productID_len = TENCENT_PRODUCT_ID_LEN;
    char deviceName[TENCENT_DEVICE_NAME_LEN + 1] = {0};
    int32_t deviceName_len = TENCENT_DEVICE_NAME_LEN;
    char hostName[TENCENT_HOSTNAME_MAXLEN + 1] = {0};
    int32_t hostName_len = TENCENT_HOSTNAME_MAXLEN;
        if(tencent_para == NULL)
        {
            printf("tencent_para = NULL\n");
            return ret;
        }
        ret = lega_flash_kv_get("tencent_productID", productID, &productID_len);
        if(ret != KV_OK)
        {
            printf("tencent productID get error\n");
            return ret;
        }
        else
        {
            printf("tencent productID = %s\n", productID);
        }

        ret = lega_flash_kv_get("tencent_deviceName", deviceName, &deviceName_len);
        if(ret != KV_OK)
        {
            printf("tencent deviceName get error\n");
            return ret;
        }
        else
        {
            printf("tencent deviceName = %s\n", deviceName);
        }

        ret = lega_flash_kv_get("tencent_hostName", hostName, &hostName_len);
        if(ret != KV_OK)
        {
            printf("tencent hostName get error\n");
        }
        else
        {
            printf("tencent hostName = %s\n", hostName);
        }

        strncpy(tencent_para->clientid, productID, productID_len);
        strcat(tencent_para->clientid, deviceName);
        strcpy(tencent_para->username, tencent_para->clientid);
        strcat(tencent_para->username, ";12010126;12345");
        strcpy(tencent_para->password, "123456");
        strncpy(tencent_para->hostname, productID, productID_len);
        strcat(tencent_para->hostname, ".iotcloud.tencentdevices.com");
        tencent_para->port = 8883;

        return 0;
}
void tencent_connection_init()
{
    uint8_t ret = 0;
    int i = 0;
    int port = 8883;
    int connect_times = TENCENT_CONNECT_TIMES;
    err_t err;
    ip_addr_t things_ip;
    struct mqtt_connect_client_info_t ci;
    OSStatus ret_conn_semaphore;
    mqtt_auth_info_t tencent_para;
    memset(&tencent_para, 0, sizeof(mqtt_auth_info_t));
    if((ret = tencent_get_value(&tencent_para)) != 0)
    {
        printf("get tencent para error\n");
        return;
    }
    else
    {
        printf("tencent client id = %s\n", tencent_para.clientid);
        printf("tencent user name = %s\n", tencent_para.username);
        printf("tencent user pass = %s\n", tencent_para.password);
        printf("tencent host name = %s\n", tencent_para.hostname);
        printf("tencent port      = %d\n", tencent_para.port);
    }

    struct hostent *hostent;
    hostent = gethostbyname(tencent_para.hostname);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));

    struct altcp_tls_config *conf = altcp_tls_create_config_client_2wayauth( \
                                    (const unsigned char *)tencent_ca_crt, \
                                    strlen(tencent_ca_crt)+1, \
                                    (const unsigned char *)tencent_client_private_key, \
                                    strlen(tencent_client_private_key)+1, \
                                    NULL, \
                                    0, \
                                    (const unsigned char *)tencent_client_cert, \
                                    strlen(tencent_client_cert)+1);
    printf("ca strlen = %d\n", strlen(tencent_ca_crt)+1);
    // printf("%s", tencent_ca_crt);
    printf("mqtt start\n");
    tencent_client = mqtt_client_new();
    //init mqtt jd_client
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = tencent_para.clientid;
    ci.client_user = tencent_para.username;
    ci.keep_alive = 60;
    ci.client_pass = tencent_para.password;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = conf;

connecting:
    err = mqtt_client_connect(tencent_client, &things_ip, port, tencent_connection_cb, 0, &ci);
    if(err != ERR_OK)
        printf("mqtt connect error, return %d\n", err);
    ret_conn_semaphore = lega_rtos_get_semaphore(&tencent_connect_Semaphore, CLOUD_TIMEOUT_MS);
    if(ret_conn_semaphore == kGeneralErr)
    {
        printf("cloud connect timeout\n");
        connect_times--;
        if(connect_times <= 0)
            goto tencent_error;
        else
        {
            goto connecting;
        }

    }
    else if(ret_conn_semaphore == kNoErr)
    {
        printf("get semaphore\n");
    }
    if(mqtt_client_is_connected(tencent_client) == 1)
    {
        printf("mqtt client connected\n");
#ifdef TEST_UNITY_CLOUD
        test_cloud_connected_flag = 1;
#endif
    }
    else
    {
        printf("mqtt client is not connected\n");
        printf("mqtt client state: %d\n", tencent_client->conn_state);
        goto tencent_error;
    }
    mqtt_set_inpub_callback(tencent_client, mqtt_incoming_publish_cb, tencent_incoming_data_cb, NULL);
    tencent_subscribe(TENCENT_TOPIC_SUBSCRIBE_DATA);
    tencent_subscribe(TENCENT_TOPIC_SUBSCRIBE_CONTROL);
    tencent_subscribe(TENCENT_TOPIC_SUBSCRIBE_RESULT);
    tencent_publish(light_flag);
    tencent_connected_subscribe_ready = 1;
    return;
tencent_error:
    printf("mqtt disconnecting...\n");
    semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
    lega_rtos_set_semaphore(&tencent_Semaphore);
    return;
}

void tencent_cloud_task(lega_thread_arg_t arg)
{
    int timestamp = 1000;
    OSStatus ret_cloud_semaphore;
    if(init_tencent_info() != KV_OK)
        lega_rtos_delete_thread(&TENCENT_Cloud_Task_Handler);
    cJSON_InitHooks(NULL);
    lega_rtos_init_semaphore(&tencent_connect_Semaphore, 0);
    lega_rtos_init_semaphore(&tencent_Semaphore, 0);
    tencent_connected_subscribe_ready = 0;
    if(light_gpio_init_flag != 1)
        light_gpio_init();
    semaphore_flag = SEMAPHORE_FLAG_NONE;
    tencent_connection_init();
    while(1)
    {
        ret_cloud_semaphore = lega_rtos_get_semaphore(&tencent_Semaphore, CLOUD_TIMEOUT_MS);
        if(ret_cloud_semaphore == kGeneralErr)
        {
            printf("tencent cloud task...\n");
        }
        else if(ret_cloud_semaphore == kNoErr)
        {
            if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_MESSAGE)
            {
                if(light_flag == LIGHT_ON)
                {
                    printf("light on\n");
                    tencent_publish(LIGHT_ON);
                }
                else if(light_flag == LIGHT_OFF)
                {
                    printf("light off\n");
                    tencent_publish(LIGHT_OFF);
                }
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                lega_rtos_delay_milliseconds(timestamp);
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_SYNC)
            {
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                printf("cloud sync\n");
                tencent_publish(TENCENT_GET_SHADOW);
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_TASK_KILL)
            {
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                printf("tencent mqtt disconnecting...\n");
                if(tencent_client != NULL)
                {
                    mqtt_disconnect(tencent_client);
                    mqtt_client_free(tencent_client);
                    tencent_client = NULL;
                }
                tencent_connected_subscribe_ready = 0;
                lega_rtos_deinit_semaphore(&tencent_connect_Semaphore);
                lega_rtos_deinit_semaphore(&tencent_Semaphore);
                lega_rtos_delete_thread(&TENCENT_Cloud_Task_Handler);
            }
        }
        //判断是否连接状态，断开连接则重新连接
        if (mqtt_client_is_connected(tencent_client) != 1)
        {
            printf("==========tencent reconnect cnt %d===========\n",tencent_reconnect_cnt++);
            tencent_connected_subscribe_ready = 0;
            if(tencent_client != NULL)
            {
                mqtt_disconnect(tencent_client);
                mqtt_client_free(tencent_client);
                tencent_client = NULL;
            }
            tencent_connection_init();
        }
        lega_rtos_delay_milliseconds(timestamp);
    }
}
#endif

#ifdef CLOUD_HUAWEI_SUPPORT
void huawei_pub_request_cb(void *arg, err_t result)
{
    printf("mqtt publish request callback...\n");
    if(result != ERR_OK)
        printf("publish err in callback %d\n", result);
}
int huawei_publish(int publish_info)
{
    char topic_buf[128] = {0};
    char payload_buf[256] = {0};
    if(publish_info == LIGHT_ON)
    {
        if(huawei_mqtt_get_topic(topic_buf, HUAWEI_TOPIC_PUBLISH) != 0 || huawei_mqtt_get_publish_payload(payload_buf, HUAWEI_PAYLOAD_LIGHT_ON) != 0)
        {
            return -1;
        }
        mqtt_publish(huawei_client, topic_buf, payload_buf, strlen(payload_buf), 0, 0, huawei_pub_request_cb, NULL);
    }
    else if(publish_info == LIGHT_OFF)
    {
        if(huawei_mqtt_get_topic(topic_buf, HUAWEI_TOPIC_PUBLISH) != 0 || huawei_mqtt_get_publish_payload(payload_buf, HUAWEI_PAYLOAD_LIGHT_OFF) != 0)
        {
            return -1;
        }
        mqtt_publish(huawei_client, topic_buf, payload_buf, strlen(payload_buf), 0, 0, huawei_pub_request_cb, NULL);
    }
    else if(publish_info == HUAWEI_GET_SHADOW)
    {
        if(huawei_mqtt_get_topic(topic_buf, HUAWEI_TOPIC_PUBLISH)  != 0 || huawei_mqtt_get_publish_payload(payload_buf, HUAWEI_PAYLOAD_GET_SHADOW) != 0)
        {
            return -1;
        }
        mqtt_publish(huawei_client, topic_buf, payload_buf, strlen(payload_buf), 0, 0, huawei_pub_request_cb, NULL);
    }
    else
    {
        printf("huawei_light_publish input error\n");
        return -1;
    }
#ifdef TEST_UNITY_CLOUD
                test_cloud_upload_flag = 1;
#endif
    return 0;
}

int huawei_subscribe(int subscribe_info)
{
    err_t err;
    char topic_buf[128] = {0};
    huawei_mqtt_get_topic(topic_buf, subscribe_info);
    err = mqtt_subscribe(huawei_client, topic_buf, 0, mqtt_sub_request_cb, NULL);
    if(err != ERR_OK)
    {
        printf("huawei subscribe err:%d\n", err);
        printf("topic:%s\n", topic_buf);
        return -1;
    }
    return 0;
}
int huawei_unsubscribe(int subscribe_info)
{
    err_t err;
    char topic_buf[128] = {0};
    huawei_mqtt_get_topic(topic_buf, subscribe_info);
    err = mqtt_unsubscribe(huawei_client, topic_buf, mqtt_unsub_request_cb, NULL);
    if(err != ERR_OK)
    {
        printf("huawei unsubscribe err:%d\n", err);
        printf("topic:%s\n", topic_buf);
        return -1;
    }
    return 0;
}
void huawei_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    printf("mqtt connectiong callback...\r\n");
    if(status == MQTT_CONNECT_ACCEPTED)
    {
        printf("huawei-connecting-cb: successfully connectd\n");
        lega_rtos_set_semaphore(&huawei_connect_Semaphore);
    }
    else
    {
        printf("huawei-connecting-cb: not connected, status = %d\n", status);
        huawei_connected_subscribe_ready = 0;
    }
}

void huawei_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags)
{
    printf("huawei incoming data callback...\n");
    printf("payload: %s\nlen: %d\n", (char *)data, len);
    char data_buf[512] = {0};
    char *pdata = NULL;
    char para_buf[15] = {0};
    strncpy(data_buf, (char *)data, len);
    pdata = NULL;
    if((pdata = strstr(data_buf, "control")) != NULL)
    {
        pdata = pdata+9;
        strncpy(para_buf, pdata, 9);
        if(strlen(para_buf) != 0)
        {
            printf("para_buf = %s\n", para_buf);
            if(strstr(para_buf, "True") != NULL)
            {
                light_flag = LIGHT_ON;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
                light_control();
            }
            if(strstr(para_buf, "False") != NULL)
            {
                light_flag = LIGHT_OFF;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
                light_control();
            }
        }
    }
}

int huawei_get_value(mqtt_connect_para_t *huawei_para)
{
    int ret = -1;
    char hostName[HUAWEI_HOSTNAME_MAXLEN + 1] = {0};
    int32_t hostName_len = HUAWEI_HOSTNAME_MAXLEN;
    ret = lega_flash_kv_get("huawei_hostName", hostName, &hostName_len);
    if(ret != 0)
    {
        printf("huawei hostName get error\n");
    }
    else
    {
        printf("huawei hostName = %s\n", hostName);
    }
    ret = mqtt_connect_para_gernerate(huawei_para, &tuple);
    if(ret != 0)
        return -1;
    if(strlen(hostName) != 0)
        strncpy(huawei_para->hostname, hostName, hostName_len);
    else
        strcpy(huawei_para->hostname, "iot-acc.cn-north-4.myhuaweicloud.com");
    huawei_para->port = 8883;
    return 0;
}
struct altcp_tls_config *huawei_conf = NULL;
void huawei_connection_init()
{
    int ret = 0;
    int i = 0;
    int port = 8883;
    int connect_times = HUAWEI_CONNECT_TIMES;
    err_t err;
    ip_addr_t things_ip;
    struct mqtt_connect_client_info_t ci;
    OSStatus ret_conn_semaphore;
    mqtt_connect_para_t huawei_para;
    memset(&huawei_para, 0, sizeof(mqtt_connect_para_t));
    if((ret = huawei_get_value(&huawei_para)) != 0)
    {
        printf("get huawei para error\n");
        return;
    }
    else
    {
        printf("huawei client id = %s\n", huawei_para.clientid);
        printf("huawei user name = %s\n", huawei_para.username);
        printf("huawei user pass = %s\n", huawei_para.password);
        printf("huawei host name = %s\n", huawei_para.hostname);
        printf("huawei port      = %d\n", huawei_para.port);
    }

    struct hostent *hostent;
    hostent = gethostbyname(huawei_para.hostname);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));
    if (huawei_conf == NULL){
         huawei_conf = altcp_tls_create_config_client( \
                                    (const unsigned char *)huawei_ca_crt, \
                                    strlen(huawei_ca_crt)+1);
    }
    printf("ca strlen = %d\n", strlen(huawei_ca_crt)+1);
    // printf("%s", huawei_ca_crt);
    printf("mqtt start\n");
    huawei_client = mqtt_client_new();
    //init mqtt jd_client
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = huawei_para.clientid;
    ci.client_user = huawei_para.username;
    ci.keep_alive = 60;
    ci.client_pass = huawei_para.password;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = huawei_conf;

connecting:
    err = mqtt_client_connect(huawei_client, &things_ip, port, huawei_connection_cb, 0, &ci);
    if(err != ERR_OK)
        printf("mqtt connect error, return %d\n", err);
    ret_conn_semaphore = lega_rtos_get_semaphore(&huawei_connect_Semaphore, CLOUD_TIMEOUT_MS);
    if(ret_conn_semaphore == kGeneralErr)
    {
        printf("cloud connect timeout\n");
        connect_times--;
        if(connect_times <= 0)
            goto huawei_error;
        else
        {
            goto connecting;
        }

    }
    else if(ret_conn_semaphore == kNoErr)
    {
        printf("get semaphore\n");
    }
    if(mqtt_client_is_connected(huawei_client) == 1)
    {
        printf("mqtt client connected\n");
#ifdef TEST_UNITY_CLOUD
        test_cloud_connected_flag = 1;
#endif
    }
    else
    {
        printf("mqtt client is not connected\n");
        printf("mqtt client state: %d\n", huawei_client->conn_state);
        goto huawei_error;
    }
    mqtt_set_inpub_callback(huawei_client, mqtt_incoming_publish_cb, huawei_incoming_data_cb, NULL);
    huawei_subscribe(HUAWEI_TOPIC_SUBSCRIBE_CONTROL);
    huawei_publish(light_flag);
    huawei_connected_subscribe_ready = 1;
    return;
huawei_error:
    printf("mqtt disconnecting...\n");
    semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
    lega_rtos_set_semaphore(&huawei_Semaphore);
    return;
}

void huawei_cloud_task(lega_thread_arg_t arg)
{
    int timestamp = 1000;
    OSStatus ret_cloud_semaphore;
    if(init_huawei_info() != 0)
        lega_rtos_delete_thread(&HUAWEI_Cloud_Task_Handler);
    cJSON_InitHooks(NULL);
    lega_rtos_init_semaphore(&huawei_connect_Semaphore, 0);
    lega_rtos_init_semaphore(&huawei_Semaphore, 0);
    huawei_connected_subscribe_ready = 0;
    if(light_gpio_init_flag != 1)
        light_gpio_init();
    semaphore_flag = SEMAPHORE_FLAG_NONE;
    huawei_connection_init();
    while(1)
    {
        ret_cloud_semaphore = lega_rtos_get_semaphore(&huawei_Semaphore, CLOUD_TIMEOUT_MS);
        if(ret_cloud_semaphore == kGeneralErr)
        {
            printf("huawei cloud task...\n");
        }
        else if(ret_cloud_semaphore == kNoErr)
        {
            if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_MESSAGE)
            {
                if(light_flag == LIGHT_ON)
                {
                    printf("light on\n");
                    huawei_publish(LIGHT_ON);
                }
                else if(light_flag == LIGHT_OFF)
                {
                    printf("light off\n");
                    huawei_publish(LIGHT_OFF);
                }
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                lega_rtos_delay_milliseconds(timestamp);
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_SYNC)
            {
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                printf("cloud sync\n");
                huawei_publish(HUAWEI_GET_SHADOW);
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_TASK_KILL)
            {
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                printf("huawei mqtt disconnecting...\n");
                if(huawei_client != NULL)
                {
                    huawei_unsubscribe(HUAWEI_TOPIC_SUBSCRIBE_CONTROL);
                    altcp_tls_free_config(huawei_conf);
                    huawei_conf = NULL;
                    mqtt_disconnect(huawei_client);
                    mqtt_client_free(huawei_client);
                    huawei_client = NULL;
                }
                huawei_connected_subscribe_ready = 0;
                lega_rtos_deinit_semaphore(&huawei_connect_Semaphore);
                lega_rtos_deinit_semaphore(&huawei_Semaphore);
                lega_rtos_delete_thread(&HUAWEI_Cloud_Task_Handler);
            }
        }
         //判断是否连接状态，断开连接则重新连接
        if (mqtt_client_is_connected(huawei_client) != 1)
        {
            printf("==========huawei reconnect cnt %d===========\n",huawei_reconnect_cnt++);
            huawei_connected_subscribe_ready = 0;
            if(huawei_client != NULL)
            {
                altcp_tls_free_config(huawei_conf);
                huawei_conf = NULL;
                mqtt_disconnect(huawei_client);
                mqtt_client_free(huawei_client);
                huawei_client = NULL;
            }
            huawei_connection_init();
        }
        lega_rtos_delay_milliseconds(timestamp);
    }
}
#endif
#ifdef CLOUD_AWS_SUPPORT
void aws_pub_request_cb(void *arg, err_t result)
{
    printf("aws publish request callback...\n");
    if(result != ERR_OK)
        printf("publish err in callback %d\n", result);
}
int aws_publish(int publish_info)
{
    char topic_buf[64] = {0};
    char payload_buf[128] = {0};
    if(publish_info == LIGHT_ON)
    {
        if(aws_mqtt_get_topic(topic_buf, AWS_TOPIC_PUBLISH) != 0 || aws_mqtt_get_publish_payload(payload_buf, AWS_PAYLOAD_LIGHT_ON) != 0)
        {
            return -1;
        }
        mqtt_publish(aws_client, topic_buf, payload_buf, strlen(payload_buf), 0, 0, aws_pub_request_cb, NULL);
    }
    else if(publish_info == LIGHT_OFF)
    {
        if(aws_mqtt_get_topic(topic_buf, AWS_TOPIC_PUBLISH) != 0 || aws_mqtt_get_publish_payload(payload_buf, AWS_PAYLOAD_LIGHT_OFF) != 0)
        {
            return -1;
        }
        mqtt_publish(aws_client, topic_buf, payload_buf, strlen(payload_buf), 0, 0, aws_pub_request_cb, NULL);
    }
    else
    {
        printf("aws_light_publish input error\n");
        return -1;
    }
#ifdef TEST_UNITY_CLOUD
                test_cloud_upload_flag = 1;
#endif
    return 0;
}

int aws_subscribe(int subscribe_info)
{
    err_t err;
    char topic_buf[64] = {0};
    aws_mqtt_get_topic(topic_buf, subscribe_info);
    err = mqtt_subscribe(aws_client, topic_buf, 0, mqtt_sub_request_cb, NULL);
    if(err != ERR_OK)
    {
        printf("mqtt subscribe err:%d\n", err);
        printf("topic:%s\n", topic_buf);
        return -1;
    }
    return 0;
}
void aws_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    printf("mqtt connectiong callback...\r\n");
    if(status == MQTT_CONNECT_ACCEPTED)
    {
        printf("aws-connecting-cb: successfully connectd\n");
        lega_rtos_set_semaphore(&aws_connect_Semaphore);
    }
    else
    {
        printf("aws-connecting-cb: not connected, status = %d\n", status);
        aws_connected_subscribe_ready = 0;
    }

}

void aws_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags)
{
    printf("aws incoming data callback...\n");
    printf("payload: %s\nlen: %d\n", (char *)data, len);
    char data_buf[512] = {0};
    char *pdata = NULL;
    int i;
    int tempValue;
    char para_buf[15] = {0};
    strncpy(data_buf, (char *)data, len);
    pdata = NULL;
    if((pdata = strstr(data_buf, "version")) != NULL)
    {
        pdata = pdata+9;
        for(i = 0; i < 9; i++)
        {
            if(*pdata <= '9' && *pdata >= '0')
            {
                para_buf[i] = *pdata;
                pdata ++;
            }
            else
                break;
        }
        if(strlen(para_buf) != 0)
        {
            tempValue = convert_str_to_int(para_buf);
            if(aws_shadowVersion != tempValue)
                aws_shadowVersion = tempValue;
            printf("aws_shadowVersion = %d\n", aws_shadowVersion);
        }
    }
    if((strstr(data_buf, "switch")) != NULL)
    {
        if(strstr(data_buf, "True") != NULL)
        {
            printf("-->True\n");
            light_flag = LIGHT_ON;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
            light_control();
        }
        if(strstr(data_buf, "False") != NULL)
        {
            printf("-->False\n");
            light_flag = LIGHT_OFF;
#ifdef TEST_UNITY_CLOUD
        test_cloud_download_flag = 1;
#endif
            light_control();
        }
    }
}

uint8_t aws_get_value(mqtt_auth_info_t *aws_para)
{
    uint8_t ret = -1;
    char productID[AWS_PRODUCT_ID_LEN + 1] = {0};
    int32_t productID_len = AWS_PRODUCT_ID_LEN;
    char clientId[AWS_CLIENT_ID_LEN + 1] = {0};
    int32_t clientId_len = AWS_CLIENT_ID_LEN;
    char hostName[AWS_HOSTNAME_MAXLEN + 1] = {0};
    int32_t hostName_len = AWS_HOSTNAME_MAXLEN;
        if(aws_para == NULL)
        {
            printf("aws_para = NULL\n");
            return ret;
        }
        ret = lega_flash_kv_get("aws_productID", productID, &productID_len);
        if(ret != KV_OK)
        {
            printf("aws productID get error\n");
            return ret;
        }
        else
        {
            printf("aws productID = %s\n", productID);
        }

        ret = lega_flash_kv_get("aws_clientId", clientId, &clientId_len);
        if(ret != KV_OK)
        {
            printf("aws clientId get error\n");
            return ret;
        }
        else
        {
            printf("aws clientId = %s\n", clientId);
        }

        ret = lega_flash_kv_get("aws_hostName", hostName, &hostName_len);
        if(ret != KV_OK)
        {
            printf("aws hostName get error\n");
        }
        else
        {
            printf("aws hostName = %s\n", hostName);
        }

        strncpy(aws_para->clientid, clientId, clientId_len);
        strncpy(aws_para->hostname, productID, productID_len);
        strcat(aws_para->hostname, ".ats.iot.cn-north-1.amazonaws.com.cn");    //.ats.iot.cn-north-1.amazonaws.com.cn
        aws_para->port = 8883;
        strcpy(aws_para->username, "qianmingjun");
        strcpy(aws_para->password, "123456");

        return 0;
}
void aws_connection_init()
{
    uint8_t ret = 0;
    int i = 0;
    int port = 8883;
    int connect_times = AWS_CONNECT_TIMES;
    err_t err;
    ip_addr_t things_ip;
    struct mqtt_connect_client_info_t ci;
    OSStatus ret_conn_semaphore;
    mqtt_auth_info_t aws_para;
    memset(&aws_para, 0, sizeof(mqtt_auth_info_t));
    if((ret = aws_get_value(&aws_para)) != 0)
    {
        printf("get aws para error\n");
        return;
    }
    else
    {
        printf("aws client id = %s\n", aws_para.clientid);
        printf("aws host name = %s\n", aws_para.hostname);
        printf("aws port      = %d\n", aws_para.port);
    }

    struct hostent *hostent;
    hostent = gethostbyname(aws_para.hostname);
    for(i = 0; hostent->h_addr_list[i]; i++)
    {
        printf("h_addr_list[%d] = %s\n", i, ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }
    memcpy(&things_ip, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&things_ip));

    struct altcp_tls_config *conf = altcp_tls_create_config_client_2wayauth( \
                                    (const unsigned char *)aws_ca_crt, \
                                    strlen(aws_ca_crt)+1, \
                                    (const unsigned char *)aws_client_private_key, \
                                    strlen(aws_client_private_key)+1, \
                                    NULL, \
                                    0, \
                                    (const unsigned char *)aws_client_cert, \
                                    strlen(aws_client_cert)+1);
    printf("ca strlen = %d\n", strlen(aws_ca_crt)+1);
    printf("mqtt start\n");
    aws_client = mqtt_client_new();
    //init mqtt aws_client
    memset(&ci, 0, sizeof(struct mqtt_connect_client_info_t));
    ci.client_id = aws_para.clientid;
    ci.client_user = aws_para.username;
    ci.keep_alive = 60;
    ci.client_pass = aws_para.password;
    ci.will_msg = NULL;
    ci.will_qos = 0;
    ci.will_topic = NULL;
    ci.will_retain = 0;
    ci.tls_config = conf;

connecting:
    err = mqtt_client_connect(aws_client, &things_ip, port, aws_connection_cb, 0, &ci);
    if(err != ERR_OK)
        printf("mqtt connect error, return %d\n", err);
    ret_conn_semaphore = lega_rtos_get_semaphore(&aws_connect_Semaphore, CLOUD_TIMEOUT_MS);
    if(ret_conn_semaphore == kGeneralErr)
    {
        printf("cloud connect timeout\n");
        connect_times--;
        if(connect_times <= 0)
            goto aws_error;
        else
        {
            goto connecting;
        }

    }
    else if(ret_conn_semaphore == kNoErr)
    {
        printf("get semaphore\n");
    }
    if(mqtt_client_is_connected(aws_client) == 1)
    {
        printf("mqtt client connected\n");
#ifdef TEST_UNITY_CLOUD
        test_cloud_connected_flag = 1;
#endif
    }
    else
    {
        printf("mqtt client is not connected\n");
        printf("mqtt client state: %d\n", aws_client->conn_state);
        goto aws_error;
    }
    mqtt_set_inpub_callback(aws_client, mqtt_incoming_publish_cb, aws_incoming_data_cb, NULL);
    aws_subscribe(AWS_TOPIC_SUBSCRIBE);
    aws_publish(light_flag);
    aws_connected_subscribe_ready = 1;
    return;
aws_error:
    printf("mqtt disconnecting...\n");
    semaphore_flag = SEMAPHORE_FLAG_CLOUD_TASK_KILL;
    lega_rtos_set_semaphore(&aws_Semaphore);
    return;
}

void aws_cloud_task(lega_thread_arg_t arg)
{
    int timestamp = 1000;

    OSStatus ret_cloud_semaphore;
    if(init_aws_info() != KV_OK)
        lega_rtos_delete_thread(&AWS_Cloud_Task_Handler);
    cJSON_InitHooks(NULL);
    lega_rtos_init_semaphore(&aws_connect_Semaphore, 0);
    lega_rtos_init_semaphore(&aws_Semaphore, 0);
    aws_connected_subscribe_ready = 0;
    if(light_gpio_init_flag != 1)
        light_gpio_init();
    semaphore_flag = SEMAPHORE_FLAG_NONE;
    aws_connection_init();
    while(1)
    {
        ret_cloud_semaphore = lega_rtos_get_semaphore(&aws_Semaphore, CLOUD_TIMEOUT_MS);
        printf("aws cloud lega_rtos_get_semaphore %d \n",ret_cloud_semaphore);
        if(ret_cloud_semaphore == kGeneralErr)
        {
            printf("aws cloud task...\n");
        }
        else if(ret_cloud_semaphore == kNoErr)
        {

            if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_MESSAGE)
            {
                if(light_flag == LIGHT_ON)
                {
                    printf("light on\n");
                    aws_publish(LIGHT_ON);
                }
                else if(light_flag == LIGHT_OFF)
                {
                    printf("light off\n");
                    aws_publish(LIGHT_OFF);
                }
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                lega_rtos_delay_milliseconds(timestamp);
            }
            else if(semaphore_flag == SEMAPHORE_FLAG_CLOUD_TASK_KILL)
            {
                semaphore_flag = SEMAPHORE_FLAG_NONE;
                printf("aws mqtt disconnecting...\n");
                if(aws_client != NULL)
                {
                    mqtt_disconnect(aws_client);
                    mqtt_client_free(aws_client);
                    aws_client = NULL;
                }
                aws_connected_subscribe_ready = 0;
                lega_rtos_deinit_semaphore(&aws_connect_Semaphore);
                lega_rtos_deinit_semaphore(&aws_Semaphore);
                lega_rtos_delete_thread(&AWS_Cloud_Task_Handler);
            }
        }
        //判断是否连接状态，断开连接则重新连接
        if (mqtt_client_is_connected(aws_client) != 1)
        {
            printf("==========aws reconnect cnt %d===========\n",aws_reconnect_cnt++);
            aws_connected_subscribe_ready = 0;
            if(aws_client != NULL)
            {
                mqtt_disconnect(aws_client);
                mqtt_client_free(aws_client);
                aws_client = NULL;
            }
            aws_connection_init();
        }
        lega_rtos_delay_milliseconds(timestamp);
    }
}
#endif
#endif