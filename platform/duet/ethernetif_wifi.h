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

#ifndef __ETHERNETIF_WIFI_H__
#define __ETHERNETIF_WIFI_H__

#include <stdint.h>

#define MAX_PBUF_NUM_A_PACKET    6
// TCPIP packet info struct
typedef struct _packet_info_tag
{
    uint8_t**          data_ptr;   /* data pointer */
    uint16_t*          data_len;   /* data length */
    uint16_t          pbuf_num;
    uint16_t          packet_len;
} TCPIP_PACKET_INFO_T;

typedef struct _rx_packet_info_tag
{
    uint8_t*    data_ptr;   /* data pointer */
    uint32_t    data_len;   /* data length - full packet encapsulation length */
} RX_PACKET_INFO_T;

#endif
