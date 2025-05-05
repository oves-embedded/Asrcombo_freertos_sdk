/*******************************************************************************
 *   Copyright 2019 JDCLOUD.COM
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *******************************************************************************/

#ifndef _IOT_DEFS_H_
#define _IOT_DEFS_H_

#ifdef CLOUD_JD_SUPPORT
#include "jd_iot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUPPORT_TLS

#define IOT_PRODUCT_KEY_LEN             (32)
#define IOT_PRODUCT_SECRET_LEN          (32)
#define IOT_DEVICE_NAME_LEN             (32)
#define IOT_DEVICE_SECRET_LEN           (32)
#define IOT_IDENTIFIER_LEN               (32)
#define IOT_MQTT_HOST_LEN             (128)

typedef enum IOT_RETURN_CODES {
    ERROR_DEVICE_NOT_EXIST = -311,
    ERROR_NET_TIMEOUT = -310,
    ERROR_CERT_VERIFY_FAIL  = -309,
    ERROR_NET_SETOPT_TIMEOUT = -308,
    ERROR_NET_SOCKET = -307,
    ERROR_NET_CONNECT = -306,
    ERROR_NET_BIND = -305,
    ERROR_NET_LISTEN = -304,
    ERROR_NET_RECV = -303,
    ERROR_NET_SEND = -302,
    ERROR_NET_CONN = -301,
    ERROR_NET_UNKNOWN_HOST = -300,

    /**
     * @brief mqtt operation
     *
     * -10 ~ -100
     *
     */
    MQTT_SUBHANDLE_LIST_LEN_TOO_SHORT = -47,
    MQTT_OFFLINE_LIST_LEN_TOO_SHORT = -46,
    MQTT_TOPIC_LEN_TOO_SHORT = -45,
    MQTT_CONNECT_BLOCK = -44,
    MQTT_SUB_INFO_NOT_FOUND_ERROR = -43,
    MQTT_PUSH_TO_LIST_ERROR = -42,
    MQTT_TOPIC_FORMAT_ERROR = -41,
    NETWORK_RECONNECT_TIMED_OUT_ERROR = -40,/** Returned when the Network is disconnected and the reconnect attempt has timed out */
    MQTT_CONNACK_UNKNOWN_ERROR = -39,/** Connect request failed with the server returning an unknown error */
    MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = -38,/** Connect request failed with the server returning an unacceptable protocol version error */
    MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR = -37,/** Connect request failed with the server returning an identifier rejected error */
    MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR = -36,/** Connect request failed with the server returning an unavailable error */
    MQTT_CONNACK_BAD_USERDATA_ERROR = -35,/** Connect request failed with the server returning a bad userdata error */
    MQTT_CONNACK_NOT_AUTHORIZED_ERROR = -34,/** Connect request failed with the server failing to authenticate the request */
    MQTT_CONNECT_ERROR = -33,
    MQTT_CREATE_THREAD_ERROR = -32,
    MQTT_PING_PACKET_ERROR = -31,
    MQTT_CONNECT_PACKET_ERROR = -30,
    MQTT_CONNECT_ACK_PACKET_ERROR = -29,
    MQTT_NETWORK_CONNECT_ERROR = -28,
    MQTT_STATE_ERROR = -27,
    MQTT_SUBSCRIBE_PACKET_ERROR = -26,
    MQTT_SUBSCRIBE_ACK_PACKET_ERROR = -25,
    MQTT_SUBSCRIBE_ACK_FAILURE = -24,
    MQTT_SUBSCRIBE_QOS_ERROR = -23,
    MQTT_UNSUBSCRIBE_PACKET_ERROR = -22,
    MQTT_PUBLISH_PACKET_ERROR = -21,
    MQTT_PUBLISH_QOS_ERROR = -20,
    MQTT_PUBLISH_ACK_PACKET_ERROR = -19,
    MQTT_PUBLISH_COMP_PACKET_ERROR = -18,
    MQTT_PUBLISH_REC_PACKET_ERROR = -17,
    MQTT_PUBLISH_REL_PACKET_ERROR = -16,
    MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR = -15,
    MQTT_NETWORK_ERROR = -14,
    MQTT_PUBLISH_ACK_TYPE_ERROR = -13,

    /**
     * @brief device operation
     *
     * -1000 ~ -1200
     *
     */
    ERROR_INVALID_PARAMETER = -1009,
    ERROR_DEV_AUTH_PASSWORD_TOO_SHORT = -1008,
    ERROR_DEV_AUTH_USERNAME_TOO_SHORT = -1007,
    ERROR_DEV_AUTH_CLIENT_ID_TOO_SHORT = -1006,
    ERROR_DEV_AUTH_HOST_NAME_TOO_SHORT = -1005,
    ERROR_CERTIFICATE_EXPIRED = -1004,
    ERROR_MALLOC = -1003,
    ERROR_NO_ENOUGH_MEM = -1002,               /**< Writes more than size value. */
    ERROR_NULL_VALUE = -1001,

    FAIL_RETURN = -1,                        /**generic error for your logic. */
    SUCCESS_RETURN = 0,
} iot_error_t;

typedef struct {
    char product_key[IOT_PRODUCT_KEY_LEN + 1];      /**< buffer containing the key of product that device belongs to */
#if defined(DYNAMIC_REGISTER) || defined(DEVICE_GATEWAY)
    char product_secret[IOT_PRODUCT_SECRET_LEN +1]; /**< buffer containing the secret of product that device belongs to */
#endif
    char device_id [IOT_IDENTIFIER_LEN + 1];         /**< buffer containing device ID */
    char device_name[IOT_DEVICE_NAME_LEN + 1];      /**< buffer containing device name */
    char device_secret[IOT_DEVICE_SECRET_LEN + 1];  /**< buffer containing device secret */
} jd_dev_meta_info_t;

typedef enum {
    os_thread_priority_idle = -3,        /* priority: idle (lowest) */
    os_thread_priority_low = -2,         /* priority: low */
    os_thread_priority_belowNormal = -1, /* priority: below normal */
    os_thread_priority_normal = 0,       /* priority: normal (default) */
    os_thread_priority_aboveNormal = 1,  /* priority: above normal */
    os_thread_priority_high = 2,         /* priority: high */
    os_thread_priority_realtime = 3,     /* priority: realtime (highest) */
    os_thread_priority_error = 0x84,     /* system cannot determine priority or thread has illegal priority */
} hal_os_thread_priority_t;

typedef struct _hal_os_thread {
    hal_os_thread_priority_t priority;     /*initial thread priority */
    void                    *stack_addr;   /* thread stack address malloced by caller, use system stack by . */
    int                   stack_size;   /* stack size requirements in bytes; 0 is default stack size */
    int                      detach_state; /* 0: not detached state; otherwise: detached state. */
    char                    *name;         /* thread name. */
} hal_os_thread_param_t;

#ifdef __cplusplus
}
#endif
#endif
#endif


