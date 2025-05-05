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

#include <sys/time.h>
#include <fcntl.h>
#include "joylink_sdk.h"
#include "joylink_platform.h"
#include "joylink_adapter.h"

#include "joylink_dev_active.h"
#include "joylink_softap.h"

/*************************************************
Function: jl_adapter_send_data
Description: SDK适配接口，GATTS Characteristic发送数据
Calls: GATTS Characteristic发送数据接口
Called By: @jl_send_frame：SDK内部函数
Input: @data：发送的数据
       @data_len：发送的数据长度
Output: None
Return: 0：成功
       -1：失败
Others:
*************************************************/
// int jl_adapter_send_data(uint8_t* data, uint32_t data_len)
int32_t jl_adapter_send_data(uint8_t *buf, uint32_t size)
{
    jl_platform_printf("JL_ADAPTER: %s  \r\n", __FUNCTION__);

    //to do
    extern int app_jl_adapter_send_data(uint8_t*data, uint32_t data_len);
    int ret = app_jl_adapter_send_data(buf, size);

    return ret;
}

/*************************************************
Function: jl_adapter_set_config_data
Description: SDK适配接口，获取配网与绑定参数
Calls: 配网与绑定接口
Called By: @jl_process_user_data：SDK内部函数
Input: @data->ssid：配网数据，WiFi SSID
       @data->ssid_len：WiFi SSID长度
       @data->password：配网数据，WiFi密码
       @data->password_len：WiFi密码长度
       @data->url：绑定云端链接
       @data->url_len：绑定云端链接长度
       @data->token：绑定参数
       @data->token_len：绑定参数长度
Output: None
Return: 0：成功
       -1：失败
Others: 设置激活绑定接口：joylink_dev_active_start(data->url, data->token);
*************************************************/
// int jl_adapter_set_config_data(jl_net_config_data_t *data)
int32_t jl_adapter_set_config_data(jl_net_config_data_t *data)
{
    int status = 0;

    jl_platform_printf("ssid: %s\n", data->ssid);
    jl_platform_printf("password: %s\n", data->password);
    jl_platform_printf("url: %s\n", data->url);
    jl_platform_printf("token: %s\n", data->token);

    //设置设备激活绑定所需的url和token
    joylink_dev_active_start((char *)data->url, (char *)data->token);

    /*
    BLE SDK 正常流程是依次这四个状态更新APP的UI,其中：
      1. E_JL_NET_CONF_ST_WIFI_CONNECT_START
      2. E_JL_NET_CONF_ST_WIFI_CONNECT_SUCCEED, 1和2这两个一起发送就OK，UI只跟进1.进行更新UI
      3. E_JL_NET_CONF_ST_IOT_CONNECT_SUCCEED
      4. E_JL_NET_CONF_ST_CLOUD_CONNECT_SUCCEED, 3和4这两个也可以一起发送就好。
      */
    //1. E_JL_NET_CONF_ST_WIFI_CONNECT_START.
    jl_send_net_config_state(E_JL_NET_CONF_ST_WIFI_CONNECT_START, NULL, 0);
    //2. E_JL_NET_CONF_ST_WIFI_CONNECT_SUCCEED
    jl_send_net_config_state(E_JL_NET_CONF_ST_WIFI_CONNECT_SUCCEED, NULL, 0);

    //connect router.
    jl_softap_connect_wifi_router((char *)data->ssid, (char *)data->password);

    return status;
}

/*************************************************
Function: jl_adapter_set_system_time
Description: SDK适配接口，设置系统时间
Calls: 设置系统时间接口
Called By: SDK内部函数
Input: @time->second：秒
       @time->usecond：微妙
Output: None
Return: 0：成功
       -1：失败
Others: None
*************************************************/
// int jl_adapter_set_system_time(jl_timestamp_t *time)
int32_t jl_adapter_set_system_time(jl_timestamp_t *time)
{
  jl_platform_printf("JL_ADAPTER: %s  \r\n", __FUNCTION__);
#if 0
    struct timeval now = { .tv_sec = time->second , .tv_usec = time->usecond};
    settimeofday(&now, NULL);

    struct tm *lt;
    lt = localtime(&lt);
    printf("%d/%d/%d %d:%d:%d\n",lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
 #endif
    return 0;
}

