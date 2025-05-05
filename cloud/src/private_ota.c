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

#ifdef CLOUD_MQTT_BROKER_SUPPORT
#include "private_ota.h"
#include "lwip/apps/http_client.h"
#include "lwip/netdb.h"
#include "lwip/ip_addr.h"
#include "mbedtls/md5.h"
#include "ota_port.h"
#include "lega_rtos_api.h"

lega_thread_t ota_task_Handler;
httpc_state_t *ota_connection = NULL;
httpc_connection_t *ota_settings = NULL;
u32_t file_len = 0;
u32_t check_file_len = 0;
unsigned char md5sum[16];
mbedtls_md5_context ctx;
int   private_ota_step = PRIVATE_OTA_FAILED;
int   ota_idx = 0;
lega_ota_boot_param_t ota_boot_para;
int *ota_offset;

extern char url_str[64];
extern char md5_str[64];

unsigned int HexStringtoByte(char *dest, char *src)
{
	unsigned int i = 0;
	int j=0;
    unsigned int length=strlen(src)/2;
	for (; i < length; i++)
	{
		sscanf(src + 2 * i, "%02X", &j);
		*(dest+i)=(char)j;
	}
    return length;
}


err_t ota_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    struct pbuf* q;
    char oatprt[120]={0x00};
    int  idx_tmp=0;
    if((file_len == 0)&&(private_ota_step == PRIVATE_OTA_INIT)) //the first payload
    {
        printf("recv:%s\n",(char *)(p->payload));
        if (strncmp((char *)(p->payload),"WIFI 5501 A0V2",strlen("WIFI 5501 A0V2")) == 0)
        {
            private_ota_step = PRIVATE_OTA_START;
            printf("Firmware upgrade start\r\n");
        }
        else
        {
            printf("received the ASR OTA bin falied! \n");
            private_ota_step = PRIVATE_OTA_FAILED;
        }
    }

    file_len += p->tot_len;

    if(private_ota_step == PRIVATE_OTA_START)
    {
        idx_tmp = (file_len*100)/check_file_len;
        if(idx_tmp!=ota_idx)
        {
            ota_idx = idx_tmp;
            memset(oatprt,'=',ota_idx);
            printf("ota download: %s %d%%\n",oatprt,ota_idx);
        }
        for (q = p; q != NULL; q = q->next) {
            mbedtls_md5_update_ret( &ctx, (unsigned char *)(q->payload), q->len );
            lega_ota_write(ota_offset, (char *)(q->payload), q->len);
        }
    }
    altcp_recved(conn, p->tot_len);
    pbuf_free(p);
	return ERR_OK;
}

err_t RecvHttpHeaderCallback (httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len) {

   printf("content_len: %lu\n", content_len);
   check_file_len = content_len;
   return ERR_OK;
}

void HttpClientResultCallback (void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {

   char md5_hex[16]={0x00};
   int idx=0;
   printf("received number of bytes: %lu\n", rx_content_len);
   if((check_file_len == rx_content_len)&&(private_ota_step == PRIVATE_OTA_START))
   {
        printf("file len check pass !\n");
        private_ota_step = PRIVATE_OTA_LEN_PASS;

        mbedtls_md5_finish_ret( &ctx, md5sum );
        mbedtls_md5_free( &ctx );

        printf("ota file md5 :");
        for(idx=0;idx<16;idx++)
            printf("%02x",md5sum[idx]);
        printf("\n");

        HexStringtoByte(md5_hex,md5_str);

        if( memcmp( md5_hex, md5sum, 16 ) != 0 )
        {
            printf("md5 check failed !\n");
            private_ota_step = PRIVATE_OTA_FAILED;
            if(ota_task_Handler!=NULL)
                lega_rtos_delete_thread(&ota_task_Handler);
        }
        else
        {
            printf("md5 check pass !\n");
            private_ota_step = PRIVATE_OTA_MD5_PASS;
        }
   }
   else
   {
        printf("file len check fail !\n");
        private_ota_step = PRIVATE_OTA_FAILED;
        if(ota_task_Handler!=NULL)
            lega_rtos_delete_thread(&ota_task_Handler);
   }
}

extern char  mqtt_broker_progress_topic[64];
extern int mqtt_common_pub(const char *topic,const void *payload, u16_t payload_length);

void ota_task(lega_thread_arg_t arg)
{
    int timestamp = 500;
    char *host_name, *down_file_path;
    ip_addr_t server_addr;
    struct hostent *hostent;
    char str_temp[64]={0x00};
    int ret = 0;
    file_len = 0;
    private_ota_step = PRIVATE_OTA_INIT;
	memcpy(str_temp,url_str,sizeof(url_str));
    host_name = strtok(str_temp+strlen("http://"), "/");
    printf("host name:%s \r\n", host_name);
    down_file_path = url_str +strlen("http://")+ strlen(host_name);
    printf("download file path: %s \r\n",down_file_path);
    hostent = gethostbyname(host_name);
    memcpy(&server_addr, hostent->h_addr_list[0], sizeof(ip_addr_t));
    printf("ipaddr = %s\n", ipaddr_ntoa(&server_addr));

    if(ota_settings == NULL)
        ota_settings = (httpc_connection_t *)mem_malloc((mem_size_t)sizeof(httpc_connection_t));
    memset(ota_settings, 0, sizeof(httpc_connection_t));
    ota_settings->use_proxy = 0;
    ota_settings->headers_done_fn = RecvHttpHeaderCallback;
    ota_settings->result_fn = HttpClientResultCallback;

    mbedtls_md5_init( &ctx );
    mbedtls_md5_starts_ret( &ctx );

    ota_boot_para.off_bp = 0;
    lega_ota_init(&ota_boot_para);

    ret = httpc_get_file(&server_addr, 80, down_file_path, ota_settings, ota_recv_fn, NULL, &ota_connection);
    printf("httpc_get_file: %d \r\n", ret);

    memset(str_temp,0x00,sizeof(str_temp));
    sprintf(str_temp,"{\"progress\":{\"otadownload\": \"start\"}}");
    mqtt_common_pub(mqtt_broker_progress_topic,str_temp, strlen(str_temp));

    if(ret!=0)
    {
        if(ota_task_Handler!=NULL)
            lega_rtos_delete_thread(&ota_task_Handler);
    }

    while(1)
    {
        lega_rtos_delay_milliseconds(timestamp);
        if(private_ota_step == PRIVATE_OTA_MD5_PASS)
        {
            printf("Firmware download done ,reboot...\n");
            memset(str_temp,0x00,sizeof(str_temp));
            sprintf(str_temp,"{\"progress\":{\"otadownload\": \"done\"}}");
            mqtt_common_pub(mqtt_broker_progress_topic,str_temp, strlen(str_temp));
            ota_boot_para.res_type = LEGA_OTA_FINISH;
            ret = lega_ota_set_boot(&ota_boot_para);
            printf("lega ota set mode:%d\n", ret);
            private_ota_step = PRIVATE_OTA_FAILED;
            if(ota_task_Handler!=NULL)
                lega_rtos_delete_thread(&ota_task_Handler);
        }
    }
}

void private_ota_entry()
{
    lega_rtos_create_thread(&ota_task_Handler, OTA_TASK_PRIORITY, OTA_TASK_NAME, ota_task, OTA_STACK_SIZE, 0);
}

#endif
