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

#ifdef CLOUD_AWS_SUPPORT
#include "aws_func.h"
#include "cloud_common.h"
#include "lega_rtos_api.h"
// #include "lega_flash.h"
#include "cJSON.h"
#ifdef CFG_PLF_DUET
#include "duet_flash_kv.h"
#else
#include "lega_flash_kv.h"
#endif


const char aws_ca_crt[] = \
{
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n" \
    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n" \
    "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n" \
    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n" \
    "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n" \
    "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n" \
    "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n" \
    "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n" \
    "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n" \
    "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n" \
    "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n" \
    "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n" \
    "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n" \
    "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n" \
    "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n" \
    "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n" \
    "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n" \
    "rqXRfboQnoZsG4q5WTP468SQvvG5\r\n" \
    "-----END CERTIFICATE-----"
};

const size_t aws_ca_crt_len = sizeof(aws_ca_crt);


const char aws_client_cert[] = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIDWjCCAkKgAwIBAgIVAJ2e57+UwDo9XyfZK93T2188LW3YMA0GCSqGSIb3DQEB\r\n" \
    "CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\r\n" \
    "IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDA0MDIxMTA3\r\n" \
    "MzVaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\r\n" \
    "dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDc3GPNOp1CPcwnJ0nu\r\n" \
    "c2yBN7Cw4KxNHuL5fEi7X+IO158vQo/BEOq+1KJ1Mg94lRcsV4XsQMohmeTkt6Qr\r\n" \
    "bywWDvlLWNiEdM+6Su5GDdvWmNR4s5jkByZRozl34pcIGKbG/gkcme1z2H4ofN2K\r\n" \
    "QBMzj+35ZhRqZroq2h6p1kWhbCW3edJocEQ/rAVlXnA++iM5yp08REo5tXZVUBLN\r\n" \
    "0t6VyA/IhtEuuGCYffIPXtVWlBxDFLpBXiU5pqZc1eGPoBlOkdy5T2KQVwGFofzp\r\n" \
    "dLMlKm/xVGuPq38mI8RvDVjwgCXLkg/LifCUoEuzOW1gFo05ERmHNClKJpJoKXR5\r\n" \
    "GLeZAgMBAAGjYDBeMB8GA1UdIwQYMBaAFKcnBEEWHbuhUcLGGaIUJGDK4AotMB0G\r\n" \
    "A1UdDgQWBBTebvV7B+2Vz+3b8i+VmjffWKgPLTAMBgNVHRMBAf8EAjAAMA4GA1Ud\r\n" \
    "DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEApIznt4dw/GKnE2MjRyvIXG6/\r\n" \
    "VT5joqeVfH6EPhobfqwjpaGEfbKmIC3dc24WPDah5fYzPkRlRpBXuNUzS6Drsv2i\r\n" \
    "Rtt0QJeak8DV3Z5kjGVSPD1Ix0NdL8Gfpdnx/15wIdiUi6KUVBA/a3qTv7xch8e+\r\n" \
    "UUEpLdCJlrOY/tHJUcvM/EIes5kQZ/lmpjJUfouISzoClb0UUvUeCDgdKOYmazKL\r\n" \
    "h+J0hd4GISjeJoQ0zdFAGEQXX+r8kFnsGdlGMCw11EeFOQk82juxQizceBBa/hTC\r\n" \
    "ELMJlBICkc9qhyzONr+ntV7cAYg4chwVoj0L6zU3lxtI2o6wqowOJeOtjRUmwg==\r\n" \
    "-----END CERTIFICATE-----\r\n"
};
const size_t aws_client_cert_len = sizeof(aws_client_cert);

const char aws_client_private_key[] = \
{
    \
    "-----BEGIN RSA PRIVATE KEY-----\r\n"                                      \
    "MIIEowIBAAKCAQEA3NxjzTqdQj3MJydJ7nNsgTewsOCsTR7i+XxIu1/iDtefL0KP\r\n" \
    "wRDqvtSidTIPeJUXLFeF7EDKIZnk5LekK28sFg75S1jYhHTPukruRg3b1pjUeLOY\r\n" \
    "5AcmUaM5d+KXCBimxv4JHJntc9h+KHzdikATM4/t+WYUama6KtoeqdZFoWwlt3nS\r\n" \
    "aHBEP6wFZV5wPvojOcqdPERKObV2VVASzdLelcgPyIbRLrhgmH3yD17VVpQcQxS6\r\n" \
    "QV4lOaamXNXhj6AZTpHcuU9ikFcBhaH86XSzJSpv8VRrj6t/JiPEbw1Y8IAly5IP\r\n" \
    "y4nwlKBLszltYBaNOREZhzQpSiaSaCl0eRi3mQIDAQABAoIBADzbFTzVa8N216vq\r\n" \
    "LXrgnwMz3yG9pnxRiGUKE9hdYSKJ0zjodEfUa3pNiL4WHyEwo4P6UdJ5P7jmeKV0\r\n" \
    "3G2DYnhpg9N5FZ/Zt+sy/libNPn6foELq5MZTTutaAZajxcSA9JM6SHSYBVNj+WE\r\n" \
    "El8nrKLGg+H2eKWgzKdGFrDhc6o4aUHuwQg56W3muoFJYEtzEGL4cf4DVehgmDt+\r\n" \
    "olf7hQKyF6s2ed8tj8ny34h3gGf6duDPejT62t6kwOcQo9vTWmOYo2jbz4Q0+YpP\r\n" \
    "PQoaWEpPIR2XY0Ff/jHZ++paj4cmGFes1dSS125M30N/ll+tEf+4bS/1xQU2MnLX\r\n" \
    "ahpXAgECgYEA9wECa5Ko2Y94Ksjgad4AYqh9gcq/MchDPCy4ri5zZRGuPQXnUX1b\r\n" \
    "RzRTQDd0yezF9ssTA+GcUVl0+4zyleRXbaDSNYuLr+7c5uP7ZKolcvX1+HgPS06G\r\n" \
    "ph34v7d6Uni69alBMn5bDRQjrX01kmPNYQuXzYLGfOMbyRXmr/XpVWECgYEA5Oeh\r\n" \
    "aERmOi3GsFYtpl0Sa/YkLhfggcdn12AJqIT6aQLv2wVV9sCY6RWlz6bgiK9CCqgz\r\n" \
    "rzj7wU/EzD4wF0tZJAYq13/i0vs8YgjfkLb346i0xcZTmjf324d4igAJvcZQS6PJ\r\n" \
    "p9YBcEmBgDyNdGoa7ux7hiK5a0L1sk/wE5771TkCgYAYj+QiuFnMEKU9c+5E+MuH\r\n" \
    "BAQ/BN9QXv64nCXdREc7VK/D57Nh+ho6kWx55Unazo29BeKlLykxObkVTqXM3AEJ\r\n" \
    "qMVeh7rjSthyqVcVYooIzYahJ6c1sOBY4oAa6G7khpbo2OTf1YrOWSNXHC1Ymk4R\r\n" \
    "CWsWB5wfTSI1M9VVyBlk4QKBgQCr/mtmJ/bUAi9Hsgks9sPvwQvgYmp7uTQKPYwk\r\n" \
    "Rn7NRrqozDKSbJa4lF55T39o1J7/2PC02VE5KFHL0XJbHm+FROkN8XaiZsoxM0+g\r\n" \
    "sgpC0jG6ZVHOD/25wunMb0jigk809oWACaodD+4xR85VyaAhJCmQWSkPMkngyHWw\r\n" \
    "mIXK6QKBgCN7ARDyetCd17RviS/bQzyHTU1Ed/cwF5YI4EaQVHtaoFbbeHP25pnx\r\n" \
    "RRRBlgxo3lgsXUIhhN90438Eag/VPG4VVs8mJrJ9AOIcsofmHvMOXh56EPTPJ/Ww\r\n" \
    "VGYZ0bqgHGLv/YqyZN8DFym15I8tMy4/UeIwUBXXEdRXLL5/f+KQ\r\n" \
    "-----END RSA PRIVATE KEY-----\r\n"

};
const size_t aws_client_private_key_len = sizeof(aws_client_private_key);
int aws_shadowVersion = 0;
char aws_productID[32] = {0};
int32_t aws_productID_len = AWS_PRODUCT_ID_LEN;
char aws_clientId[32] = {0};
int32_t aws_clientid_len = AWS_CLIENT_ID_LEN;
int init_aws_info()
{
    int ret = 0;
#ifdef CFG_PLF_DUET
    ret = duet_flash_kv_get("aws_productID", aws_productID, &aws_productID_len);
#else
    ret = lega_flash_kv_get("aws_productID", aws_productID, &aws_productID_len);
#endif
    if(ret != 0)
    {
        printf("aws productID get error\n");
        return ret;
    }
    else
    {
        printf("aws productID = %s\n", aws_productID);
    }

#ifdef CFG_PLF_DUET
    ret = duet_flash_kv_get("aws_clientId", aws_clientId, &aws_clientid_len);
#else
    ret = lega_flash_kv_get("aws_clientId", aws_clientId, &aws_clientid_len);
#endif
    if(ret != 0)
    {
        printf("aws clientId get error\n");
        return ret;
    }
    else
    {
        printf("aws clientId = %s\n", aws_clientId);
    }
    return ret;
}

int aws_mqtt_get_topic(char *topic_out, int action_in)
{
    char *ptmp_buf = NULL;
    int topic_len = 0;
    if(topic_out == NULL)
    {
        printf("topic_out is NULL\n");
        return -1;
    }
    if(action_in == AWS_TOPIC_PUBLISH)             //e.g. $aws/things/thing01/shadow/update
    {
        ptmp_buf = "$aws/things/%s/shadow/update";
    }
    else if(action_in == AWS_TOPIC_SUBSCRIBE)      //e.g. $aws/things/thing01/shadow/update/delta
    {
        ptmp_buf = "$aws/things/%s/shadow/update/delta";
    }
    else
    {
        printf("topic input error\n");
        return -1;
    }
    topic_len = strlen(ptmp_buf) + strlen(aws_clientId) + 1;
    snprintf(topic_out, topic_len, ptmp_buf, aws_clientId);
    printf("topic:%s\n", topic_out);

    return 0;
}

int aws_mqtt_get_publish_payload(char *payload_out, int action_in)
{
    char *ptmp = NULL;
    cJSON * pJsonData = cJSON_CreateObject();
    cJSON * pJsonState = cJSON_CreateObject();
    cJSON * pJsonReported = cJSON_CreateObject();
    cJSON * pJsonDesired = cJSON_CreateObject();
    if(pJsonData == NULL || pJsonState == NULL || pJsonReported == NULL)
    {
        printf("json creat error\n");
        return -1;
    }
    if(payload_out == NULL)
    {
        printf("payload_out is NULL\n");
        return -1;
    }
    if(action_in == AWS_PAYLOAD_LIGHT_OFF)
    {
        cJSON_AddItemToObject(pJsonData, "state", pJsonState);
        cJSON_AddItemToObject(pJsonState, "reported", pJsonReported);
        cJSON_AddStringToObject(pJsonReported, "switch", "False");
        //cJSON_AddNumberToObject(pJsonData, "version", aws_shadowVersion);
        cJSON_AddItemToObject(pJsonState, "desired", pJsonDesired);
        cJSON_AddStringToObject(pJsonDesired, "switch", "False");
 
    }
    else if(action_in == AWS_PAYLOAD_LIGHT_ON)
    {
        cJSON_AddItemToObject(pJsonData, "state", pJsonState);
        cJSON_AddItemToObject(pJsonState, "reported", pJsonReported);
        cJSON_AddStringToObject(pJsonReported, "switch", "True");
        //cJSON_AddNumberToObject(pJsonData, "version", aws_shadowVersion);
        cJSON_AddItemToObject(pJsonState, "desired", pJsonDesired);
        cJSON_AddStringToObject(pJsonDesired, "switch", "True");
    }
    else
    {
        printf("get publish payload input error\n");
        cJSON_Delete(pJsonData);
        cJSON_Delete(pJsonState);
        cJSON_Delete(pJsonReported);
        cJSON_Delete(pJsonDesired);
        return -1;
    }
    ptmp = cJSON_PrintUnformatted(pJsonData);
    strcpy(payload_out, ptmp);
    printf("payload:%s\n", payload_out);
    lega_rtos_free(ptmp);
    if(pJsonData != NULL)
        cJSON_Delete(pJsonData);
    return 0;
}
#endif