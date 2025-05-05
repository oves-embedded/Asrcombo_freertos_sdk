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

#ifdef CLOUD_JD_SUPPORT
#ifndef _DEV_AUTH_H_
#define _DEV_AUTH_H_

#include <string.h>
#include "jd_iot_defs.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define DEV_AUTH_HOSTNAME_MAXLEN    (128)
#define DEV_AUTH_CLIENT_ID_MAXLEN   (128)
#define DEV_AUTH_DEVICE_NAME_MAXLEN (128)
#define DEV_AUTH_USERNAME_MAXLEN    (64)
#define DEV_AUTH_PASSWORD_MAXLEN    (64)
#define DEV_AUTH_PASSWORD_BYTE_LEN  (20)

typedef struct {
    uint16_t port;
    char hostname[DEV_AUTH_HOSTNAME_MAXLEN];    /**< buffer containing host name of JDCloud IoT Hub MQTT broker */
    char clientid[DEV_AUTH_CLIENT_ID_MAXLEN];   /**< buffer containing client ID for JDCloud IoT Hub MQTT connection */
    char username[DEV_AUTH_USERNAME_MAXLEN];    /**< buffer containing username for JDCloud IoT Hub MQTT connection */
    char password[DEV_AUTH_PASSWORD_MAXLEN];    /**< buffer containing password for JDCloud IoT Hub MQTT connection */
} jd_mqtt_auth_info_t;

extern const char jd_ca_crt[];
/**
 * @brief Generates the authentication parameters for MQTT connection between device and IoT Hub.
 *      This connnection serves for regular device communication, such as property, event, service and shadow control
 *
 * @param[in]   meta    configs containing device and its product information
 * @param[out]  auth    the auto configs for MQTT connection to IoT Hub
 * @return      error code defined in iot_error_t
 */
int32_t iot_get_auth_info(jd_dev_meta_info_t *meta, jd_mqtt_auth_info_t *auth);

#if defined(DYNAMIC_REGISTER) || defined(DEVICE_GATEWAY)
/**
 * @brief Generates the authentication parameters for MQTT connection between device and IoT Hub
 *      This connection serves for acquiring device secret from IoT Hub. After receiving device secret, the program should
 *      build new MQTT connection with iot_get_auth_info for regular device communication
 *
 * @param[in]   meta    configs containing device and its product information
 * @param[out]  auth    the auto configs for MQTT connection to IoT Hub
 * @return      error code defined in iot_error_t
 */
int32_t iot_get_dynamic_auth_info(jd_dev_meta_info_t *meta, jd_mqtt_auth_info_t *auth);
#endif // DYNAMIC_REGISTER

#if defined(__cplusplus)
}
#endif

#endif // _DEV_AUTH_H_
#endif