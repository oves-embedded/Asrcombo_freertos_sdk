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

#ifndef __JOYLINK_SDK_H__
#define __JOYLINK_SDK_H__

#include "joylink_platform.h"
#include "joylink_utils.h"

typedef struct{
    uint8_t  product_uuid[6];
    uint8_t  mac[6];
    uint8_t  shared_key[16];//if JL_SECURITY_LEVEL == 1
}jl_dev_info_t;

typedef struct{
    uint8_t service_uuid16[2];
    uint8_t manufacture_data[14];
}jl_gap_data_t;

typedef struct{
    uint8_t service_uuid128[16];
    uint8_t chra_uuid128_write[16];
    uint8_t chra_uuid128_indicate[16];
}jl_gatt_data_t;

typedef enum {
    E_JL_CHRA_WRITE    = 0x00,
    E_JL_CHRA_WRITE_NR = 0x01,
}JL_CHRA_TYPE_E;

typedef enum
{
    E_JL_NET_CONF_ST_WIFI_CONNECT_FAILED        = 0x00,
    E_JL_NET_CONF_ST_WIFI_CONNECT_START         = 0x01,
    E_JL_NET_CONF_ST_WIFI_CONNECT_SUCCEED       = 0x02,
    E_JL_NET_CONF_ST_WIFI_CONNECT_TIMEOUT       = 0x03,
    E_JL_NET_CONF_ST_WIFI_CONNECT_SSID_ERROR    = 0x04,
    E_JL_NET_CONF_ST_WIFI_CONNECT_PSK_ERROR     = 0x05,
    E_JL_NET_CONF_ST_WIFI_CONNECT_WAN_ERROR     = 0x06,
    E_JL_NET_CONF_ST_IOT_CONNECT_TIMEOUT        = 0x07,
    E_JL_NET_CONF_ST_IOT_CONNECT_FAILED         = 0x08,
    E_JL_NET_CONF_ST_IOT_CONNECT_SUCCEED        = 0x09,
    E_JL_NET_CONF_ST_CLOUD_CONNECT_TIMEOUT      = 0x0A,
    E_JL_NET_CONF_ST_CLOUD_CONNECT_SUCCEED      = 0x0B,
    E_JL_NET_CONF_ST_TIMEOUT_EXIT               = 0x0C,
    E_JL_NET_CONF_ST_EXIT                       = 0x0D,
}E_JL_NET_CONF_ST;

typedef struct
{
    uint8_t *ssid;
    uint8_t ssid_len;

    uint8_t *password;
    uint8_t password_len;

    uint8_t *url;
    uint8_t url_len;

    uint8_t *token;
    uint8_t token_len;
}jl_net_config_data_t;

#define jl_data_handle_t  void*

#define JL_GAP_SERVICE_UUID16           {0x70, 0xFE}
#define JL_GATT_SERVICE_UUID128         {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x70, 0xFE, 0x00, 0x00}
#define JL_GATT_CHRA_UUID128_WRITE      {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x71, 0xFE, 0x00, 0x00}
#define JL_GATT_CHRA_UUID128_INDICATE   {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x72, 0xFE, 0x00, 0x00}

/*************************************************
Function: jl_init
Description: SDK初始化
Calls: SDK内部函数
Called By: 一般是调用其他SDK接口之前调用
Input: @dev_info->product_uuid: 产品ID，在基本信息界面获取
       @dev_info->mac: 设备端Mac
       @dev_info->license: 需要在设备端开发界面，点击导入Mac生成license按钮，
                           每个Mac都有对应的license key
       @dev_info->shared_key: 加密共享秘钥，基本信息界面选择安全级别是1才会使用
Output: None
Return: 0：成功
       -1：失败
Others: 上电后调用其他SDK之前，必须要先初始化，
        查看以上界面：小京鱼.IoT开发平台-控制中心-IoT控制台-产品中心-产品管理-编辑
*************************************************/
int32_t jl_init(jl_dev_info_t *dev_info);

/*************************************************
Function: jl_disconnet_reset
Description: BLE断开后调用该接口释放SDK与通信相关的资源。
Calls: SDK内部函数
Called By: BLE断开后调用
Input: None
Output: None
Return: 0：成功
       -1：失败
Others: None
*************************************************/
int32_t jl_disconnet_reset();

/*************************************************
Function: jl_get_gap_config_data
Description: 获取gap信息，添加service uuid和manufacturer data
Calls: SDK内部函数
Called By: 一般是设置gatt前调用
Input: @gap_data->service_uuid16: service uuid 长度2字节
       @gap_data->manufacture_data: 厂商数据，长度14字节
Output: None
Return: 0：成功
       -1：失败
Others: 广播包里必须包含这两个信息才能被识别
*************************************************/
int32_t jl_get_gap_config_data(jl_gap_data_t *gap_data);


/*************************************************
Function: jl_get_gatt_config_data
Description: 获取gatt配置信息
Calls: None
Called By: 一般是设置gap广播包前调用
Input: @gatt_data->service_uuid128：service uuid值，长度：16字节
       @gatt_data->chra_uuid128_write：service characteristic uuid值，长度：16字节，属性：write
       @gatt_data->chra_uuid128_indicate：service characteristic uuid值，长度：16字节，属性：indicate
Output: None
Return: 0：成功
       -1：失败
Others: 必须添加service和characteristic才能通信
*************************************************/
int32_t jl_get_gatt_config_data(jl_gatt_data_t *gatt_data);


/*************************************************
Function: jl_write_data
Description: ble characteristic通道接收到的数据调用该函数处理
Calls: SDK内部函数
Called By: 一般是收到WRITE事件后调用
Input: @data: 数据指针
       @data_len: 数据长度
Output: None
Return: 0：成功
       -1：失败
Others: None
*************************************************/
int32_t jl_write_data(uint8_t* data, int32_t data_len);


/*************************************************
Function: jl_send_confirm
Description: ble indicate通道发送数据成功后会返回confirm事件调用该函数通知SDK发送成功
Calls: SDK内部函数
Called By: 一般是收到CONFIRM事件后调用
Input: None
Output: None
Return: 0：成功
       -1：失败
Others: None
*************************************************/
int32_t jl_send_confirm(void);


/*************************************************
Function: jl_send_net_config_state
Description: 发送配网状态给APP
Calls: SDK内部函数
Called By: 配网状态改变之后
Input: @st: 配网状态
Output: None
Return: 0：成功
       -1：失败
Others: None
*************************************************/
int32_t jl_send_net_config_state(E_JL_NET_CONF_ST st, uint8_t* data, int32_t data_len);
#endif

