
#ifndef _JOYLINK_CONFIG_H_
#define _JOYLINK_CONFIG_H_

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "joylink.h"

/*
 * user set jlp version
 */
#define JLP_VERSION  0

/*
 * Create dev and get the index from developer center
 */
#define JLP_DEV_TYPE    E_JLDEV_TYPE_NORMAL
#define JLP_LAN_CTRL    E_LAN_CTRL_DISABLE   //E_LAN_CTRL_DISABLE E_LAN_CTRL_ENABLE
#define JLP_CMD_TYPE    E_CMD_TYPE_LUA_SCRIPT
#define JLP_SNAPSHOT    E_SNAPSHOT_YES       //E_SNAPSHOT_NO

#if 1
//BLE 配网设备
#define JLP_UUID          "534411"
#define IDT_CLOUD_PUB_KEY "03A9F7A1B4DB8D4B07C4CFBE7B55CD2596878096B29384691A6CA55EE0B6C5E199"
#define JLP_CID           "102009"
#define JLP_BRAND         "2"
#define F_PRIVATE_KEY     "F9482BF05CE388E3139C3D1E43568C150F9C2AC859C5F30F78084FE99BC2E250"
#define AP_SSID           "JDDeng7595"
#else
//热点AP 配网设备
#define JLP_UUID          "EE62BD"
#define IDT_CLOUD_PUB_KEY "0214B59DE3C6EB740C55664A0D075043392664B1F9A94E762B28D1DCB95C75AD74"
#define JLP_CID           "102009"
#define JLP_BRAND         "2"
#define F_PRIVATE_KEY     "6E698CA85F407AC0189B264EAC3ACC2720ACA9705AF3A5DF5F5563A14157ABEC"
#define AP_SSID           "JDDeng7595"
#endif

/*
 * DEBUG
 */
// #define JOYLINK_EXTERN_DEBUG_INFO
// #define JOYLINK_FLASH_DEBUG_INFO
// #define JOYLINK_MEMORY_DEBUG_INFO
// #define JOYLINK_SOFTAP_DEBUG_INFO
// #define JOYLINK_STRING_DEBUG_INFO
// #define JOYLINK_THREAD_DEBUG_INFO
// #define JOYLINK_TIME_DEBUG_INFO
// #define JOYLINK_SOCKET_DEBUG_INFO

#define USER_DATA_POWER   "Power"    //参考example
#define USER_DATA_MODE    "Mode"
#define USER_DATA_STATE   "State"

typedef struct _user_dev_status_t {  //参考example
    int Power;
    int Mode;
    int State;
} user_dev_status_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
