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
#include <stdlib.h>
#include <string.h>
#include "ota_port.h"
#include "sockets.h"
#include "netdb.h"
#include "http_ota.h"
#include "lega_rtos_api.h"

/** @brief : update the ota bin file from http server
 *  @param url    : user url pointer
 *  @example : http://192.168.21.137:5800/asr_wifi.ota.bin
            or http://test.asr_iot.com/dev/ota/download
*/
#ifdef HTTP_OTA_SUPPORT

#define HTTP_RCV_BUF_SIZE 1024
#define HTTP_GET_REQ_BUF_SIZE 256
#define DEFAULT_HTTP_PORT 80
#define HTTP_DOWNLOAD_ERROR 0
#define HTTP_DOWNLOAD_OK 1
#define HTTP_DOWNLOAD_TIMEOUT 10

static int http_get_req_send(int fd, char *path, char *domainname, uint16_t port)
{
    const char *GET_FORMAT =
    "GET /%s HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/octet-stream\r\n"
    "Accept-Encoding: identity\r\n"
    "Connection: keep-alive\r\n\r\n";

    char *http_request = NULL;
    int ret = 0;

    http_request = lega_rtos_malloc(HTTP_GET_REQ_BUF_SIZE);
    if(http_request == NULL)
    {
        printf("http_request malloc failed\r\n");
        return -1;
    }

    sprintf(http_request, GET_FORMAT, path, domainname, port);

    printf("http_request:%s", http_request);

    ret = send(fd, http_request, strlen(http_request), 0);

    lega_rtos_free(http_request);

    return ret;
}

static int ota_prepare(void)
{
    lega_ota_boot_param_t ota_boot_para;
    ota_boot_para.off_bp = 0;
    return lega_ota_init(&ota_boot_para);
}

static int ota_write(uint32_t write_offset, uint8_t* data, uint16_t len)
{
    int ret = 0, get_write_offset;

    ret = lega_ota_write(&get_write_offset, (char *)data, (int)len);

    return ret;
}

static int ota_result_handle(uint8_t download_result, uint8_t process_func)
{
    if(download_result == HTTP_DOWNLOAD_OK)
    {
        lega_ota_boot_param_t ota_boot_para;
        ota_boot_para.res_type = process_func;
        return lega_ota_set_boot(&ota_boot_para);
    }
    else
    {
        return -1;
    }
}

static int parse_url_info(void *data, http_download_ctr_t *http_download_ctr)
{
    char *head,*tail;

    if(((head = strstr(data, "http://")) == NULL) || (http_download_ctr == NULL)) //find http
    {
        return -1;
    }

    memset(http_download_ctr, 0, sizeof(http_download_ctr_t));

    head = head + strlen("http://");

    if((tail = strstr(head, ":")) != NULL)
    {
        http_download_ctr->download_info.sPort = strtoul(tail + 1, NULL, 10);
        memcpy(http_download_ctr->download_info.sDomainName, head, tail - head);
        head = tail;
    }
    else
    {
        if((tail = strstr(head, "/")) != NULL)
        {
            http_download_ctr->download_info.sPort = DEFAULT_HTTP_PORT;
            memcpy(http_download_ctr->download_info.sDomainName, head, tail - head);
            head = tail;
        }
        else
        {
            return -2;
        }
    }

    if((tail = strstr(head, "/")) != NULL)
    {
        head = tail + 1;
        memcpy(http_download_ctr->download_info.sPath, head, strlen(head));
    }
    else
    {
        return -3;
    }

    return 0;
}

/*read buffer by byte still delim ,return read bytes counts*/
static int read_until(char *buffer, char delim, int len)
{
    //  /*TODO: delim check,buffer check,further: do an buffer length limited*/
    int i = 0;

    while(buffer[i] != delim && i < len)
    {
        ++i;
    }

    return i + 1;
}

static int parse_http_header(char *text, int total_len, http_download_ctr_t *http_download_ctr)
{
    /* i means current position */
    uint16_t i = 0, i_read_len = 0;
    char *ptr = NULL, *ptr_tail = NULL, *content_type = NULL;
    char length_str[32] = {0};

    while(text[i] != 0 && i < total_len)
    {
        if((http_download_ctr->download_info.content_len == 0) && ((ptr = (char *)strstr(text, "Content-Length: ")) != NULL) &&
          ((ptr_tail = (char *)strstr(ptr, "\r\n")) != NULL))
        {
            ptr += strlen("Content-Length: ");;
            memset(length_str, 0, sizeof(length_str));
            memcpy(length_str, ptr, ptr_tail - ptr);
            http_download_ctr->download_info.content_len = strtoul((const char *)length_str, NULL, 10);
            printf("parse Content-Length:%ld\n", http_download_ctr->download_info.content_len);
        }

        if(content_type == NULL)
        {
            content_type = (char *)strstr(text, "application/octet-stream");
        }

        i_read_len = read_until(&text[i], '\n', total_len - i);

        if(i_read_len > total_len - i)
        {
            printf("recv malformed http header\r\n");
            return -1;
        }

        //if resolve \r\n line, http header is finished
        if(i_read_len == 2)
        {
            if(http_download_ctr->download_info.content_len == 0)
            {
                printf("did not parse Content-Length item\r\n");
                return -2;
            }

            if(content_type == NULL)
            {
                printf("server did not return \"Content-type: application/octet-stream\"\r\n");
                return -3;
            }

            int i_write_len = total_len - (i + 2);

            if(i_write_len < 0)
            {
                printf("recv malformed http header\r\n");
                return -4;
            }

            return i + 2;
        }

        i += i_read_len;
    }

    return -5;
}

static int http_data_process(uint8_t *data, uint16_t length, http_download_ctr_t *http_download_ctr)
{
    // first response should include state code:200
    if(!http_download_ctr->download_info.http_200_check && strstr((char *)data, "200") == NULL)
    {
        printf("ota url is invalid or bin is not exist!\r\n");
        return -1;
    }

    http_download_ctr->download_info.http_200_check = 1;

    if(!http_download_ctr->download_info.resp_body_start)
    {
        //parse http head
        int write_offset = parse_http_header((char *)data, length, http_download_ctr);
        if(write_offset > 0)
        {
            http_download_ctr->download_info.resp_body_start = 1;
            if(write_offset == length)
            {
                return 0;
            }
            else
            {
                data = data + write_offset;
                length = length - write_offset;
            }
        }
        else
        {
            return -2;
        }
    }

    // deal with http body
    // http transmit body more than content-length occasionally
    // default bin size = content-length, throw up the other http body
    if(http_download_ctr->download_info.download_length + length > http_download_ctr->download_info.content_len)
    {
        length = http_download_ctr->download_info.content_len - http_download_ctr->download_info.download_length;
    }

    if(http_download_ctr->data_write)
    {
        if(http_download_ctr->data_write(http_download_ctr->download_info.download_length, data, length) != 0)
        {
            printf("http download write error\r\n");
            return -3;
        }
    }

    http_download_ctr->download_info.download_length += length;

    printf("downloaded len:%ld,%d\r\n", http_download_ctr->download_info.download_length, length);

    return 0;
}

int http_update_ota(char *url, uint8_t process_func)
{
    int fd = 0;
    int http_data_len = 0;
    int ret = 0;
    uint8_t *http_data = NULL;
    struct sockaddr_in serv_addr;
    struct hostent *host;
    struct timeval timeout;
    http_download_ctr_t *http_download_ctr = NULL;

    http_download_ctr = lega_rtos_malloc(sizeof(http_download_ctr_t));
    if(http_download_ctr == NULL)
    {
        printf("http_download_ctr malloc failed\r\n");
        return -1;
    }
    memset(http_download_ctr, 0, sizeof(http_download_ctr_t));

    if((ret = parse_url_info(url, http_download_ctr)) != 0)
    {
        printf("parse url error: %d\r\n", ret);
        goto exit;
    }
    else
    {
        printf("download ctr:%s,%s\r\n", http_download_ctr->download_info.sDomainName, http_download_ctr->download_info.sPath);
        http_download_ctr->data_write = (int (*)(uint32_t, uint8_t *, uint16_t))ota_write;
        http_download_ctr->download_prepare = (int (*)(void))ota_prepare;
        http_download_ctr->download_result_handle = (int (*)(uint8_t, uint8_t))ota_result_handle;
    }

    if(http_download_ctr->download_prepare != NULL)
    {
        if((ret = http_download_ctr->download_prepare()) != 0)
        {
            printf("http ota prepare failed: %d\n", ret);
            goto exit;
        }
    }

    if((host = gethostbyname(http_download_ctr->download_info.sDomainName)) == NULL) //DNS
    {
        printf("Gethostname failed\r\n");
        ret = -1;
        goto exit;
    }

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(http_download_ctr->download_info.sPort);
    bcopy((char *)host->h_addr, (char *)&serv_addr.sin_addr.s_addr, host->h_length);
    printf("%s's ip address:%d.%d.%d.%d \r\n", host->h_name, (unsigned char)host->h_addr[0],\
        (unsigned char)host->h_addr[1], (unsigned char)host->h_addr[2], (unsigned char)host->h_addr[3]);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        printf("Create socket failed\r\n");
        ret = -1;
        goto exit;
    }

    if(connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connect failed\r\n");
        ret = -1;
        goto exit;
    }

    if(http_get_req_send(fd, http_download_ctr->download_info.sPath, http_download_ctr->download_info.sDomainName,
       http_download_ctr->download_info.sPort) < 0)
    {
        printf("Send http get req failed\r\n");
        ret = -1;
        goto exit;
    }

    /*
        as the first recv will get both the response of 'GET' command and the datas,
        so need locate the boundery of the response and the data, and get image size from the response;
    */
    http_data = lega_rtos_malloc(HTTP_RCV_BUF_SIZE);
    if(http_data == NULL)
    {
        printf("http_data malloc failed\r\n");
        ret = -1;
        goto exit;
    }

    timeout.tv_sec = HTTP_DOWNLOAD_TIMEOUT;
    timeout.tv_usec = 0;
    lwip_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while(1)
    {
        if((http_data_len = recv(fd, http_data, HTTP_RCV_BUF_SIZE, 0)) < 0)
        {
            printf("Recv Failed\r\n");
            ret = -1;
            goto exit;
        }

        ret = http_data_process(http_data, http_data_len, http_download_ctr);

        if(ret != 0)
        {
            goto exit;
        }
        else
        {
            if(http_download_ctr->download_info.download_length >= http_download_ctr->download_info.content_len)
            {
                http_download_ctr->download_info.download_result = HTTP_DOWNLOAD_OK;
                break;
            }
        }
    }

    if(http_download_ctr->download_result_handle != NULL)
    {
        ret = http_download_ctr->download_result_handle(http_download_ctr->download_info.download_result, process_func);
    }

exit:
    if(fd != 0)
        close(fd);
    if(http_data != NULL)
        lega_rtos_free(http_data);
    if(http_download_ctr != NULL)
        lega_rtos_free(http_download_ctr);

    return ret;
}

#endif
