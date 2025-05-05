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

#ifndef _HTTP_OTA_H_
#define _HTTP_OTA_H_

typedef struct _http_down_info
{
    char sDomainName[64];
    char sPath[64];
    uint16_t sPort;
    uint32_t content_len;
    uint32_t download_length;
    uint8_t http_200_check;
    uint8_t resp_body_start;
    uint8_t download_result;
}http_down_info_t;

typedef struct _http_download_ctr
{
    http_down_info_t download_info;
    int (*data_write)(uint32_t write_offset, uint8_t* data, uint16_t len);
    int (*download_prepare)(void);
    int (*download_result_handle)(uint8_t download_result, uint8_t process_func);
}http_download_ctr_t;

/**
 *
 * @param[in]  url  the http server url
 * @param[in]  process_func  lega_ota_set_boot param
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int http_update_ota(char *url, uint8_t process_func);

#endif
