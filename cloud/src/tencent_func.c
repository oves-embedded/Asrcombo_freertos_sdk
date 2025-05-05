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

#ifdef CLOUD_TENCENT_SUPPORT
#include "tencent_func.h"
#include "lega_rtos_api.h"
#include "cloud_common.h"
// #include "lega_flash.h"
#include "cJSON.h"

#ifdef CFG_PLF_DUET
#include "duet_flash_kv.h"
#else
#include "lega_flash_kv.h"
#endif
const char tencent_ca_crt[] = \
{
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIDxTCCAq2gAwIBAgIJALM1winYO2xzMA0GCSqGSIb3DQEBCwUAMHkxCzAJBgNV\r\n" \
    "BAYTAkNOMRIwEAYDVQQIDAlHdWFuZ0RvbmcxETAPBgNVBAcMCFNoZW5aaGVuMRAw\r\n" \
    "DgYDVQQKDAdUZW5jZW50MRcwFQYDVQQLDA5UZW5jZW50IElvdGh1YjEYMBYGA1UE\r\n" \
    "AwwPd3d3LnRlbmNlbnQuY29tMB4XDTE3MTEyNzA0MjA1OVoXDTMyMTEyMzA0MjA1\r\n" \
    "OVoweTELMAkGA1UEBhMCQ04xEjAQBgNVBAgMCUd1YW5nRG9uZzERMA8GA1UEBwwI\r\n" \
    "U2hlblpoZW4xEDAOBgNVBAoMB1RlbmNlbnQxFzAVBgNVBAsMDlRlbmNlbnQgSW90\r\n" \
    "aHViMRgwFgYDVQQDDA93d3cudGVuY2VudC5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
    "A4IBDwAwggEKAoIBAQDVxwDZRVkU5WexneBEkdaKs4ehgQbzpbufrWo5Lb5gJ3i0\r\n" \
    "eukbOB81yAaavb23oiNta4gmMTq2F6/hAFsRv4J2bdTs5SxwEYbiYU1teGHuUQHO\r\n" \
    "iQsZCdNTJgcikga9JYKWcBjFEnAxKycNsmqsq4AJ0CEyZbo//IYX3czEQtYWHjp7\r\n" \
    "FJOlPPd1idKtFMVNG6LGXEwS/TPElE+grYOxwB7Anx3iC5ZpE5lo5tTioFTHzqbT\r\n" \
    "qTN7rbFZRytAPk/JXMTLgO55fldm4JZTP3GQsPzwIh4wNNKhi4yWG1o2u3hAnZDv\r\n" \
    "UVFV7al2zFdOfuu0KMzuLzrWrK16SPadRDd9eT17AgMBAAGjUDBOMB0GA1UdDgQW\r\n" \
    "BBQrr48jv4FxdKs3r0BkmJO7zH4ALzAfBgNVHSMEGDAWgBQrr48jv4FxdKs3r0Bk\r\n" \
    "mJO7zH4ALzAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQDRSjXnBc3T\r\n" \
    "d9VmtTCuALXrQELY8KtM+cXYYNgtodHsxmrRMpJofsPGiqPfb82klvswpXxPK8Xx\r\n" \
    "SuUUo74Fo+AEyJxMrRKlbJvlEtnpSilKmG6rO9+bFq3nbeOAfat4lPl0DIscWUx3\r\n" \
    "ajXtvMCcSwTlF8rPgXbOaSXZidRYNqSyUjC2Q4m93Cv+KlyB+FgOke8x4aKAkf5p\r\n" \
    "XR8i1BN1OiMTIRYhGSfeZbVRq5kTdvtahiWFZu9DGO+hxDZObYGIxGHWPftrhBKz\r\n" \
    "RT16Amn780rQLWojr70q7o7QP5tO0wDPfCdFSc6CQFq/ngOzYag0kJ2F+O5U6+kS\r\n" \
    "QVrcRBDxzx/G\r\n" \
    "-----END CERTIFICATE-----"
};

const size_t tencent_ca_crt_len = sizeof(tencent_ca_crt);

#if 0  //donghui
const char tencent_client_cert[] = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIDTTCCAjWgAwIBAgIBADANBgkqhkiG9w0BAQsFADB5MQswCQYDVQQGEwJDTjES\r\n" \
    "MBAGA1UECAwJR3VhbmdEb25nMREwDwYDVQQHDAhTaGVuWmhlbjEQMA4GA1UECgwH\r\n" \
    "VGVuY2VudDEXMBUGA1UECwwOVGVuY2VudCBJb3RodWIxGDAWBgNVBAMMD3d3dy50\r\n" \
    "ZW5jZW50LmNvbTAeFw0xOTEyMjMwNjMyMTFaFw0yOTEyMjAwNjMyMTFaMBoxGDAW\r\n" \
    "BgNVBAMTD1dVMlFWT0dVVjR0ZXN0MTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\r\n" \
    "AQoCggEBAKhI0yS0k4ZHlgegouBiTOSDT0QSNs/1K0yOvncNpA6Yb8xNHHPa3Tmw\r\n" \
    "EgyWm8NIyaVrYQ6iOXc/HajydNU5rj3e+kNqNcPns1JkuovdG5S4N9nOTfh5jZZb\r\n" \
    "VQVUgNouoeQOOBs2+gLP0cX67CX7Gm8ELeIMabrgmxpHrAc4JNOcPmo3UCSf1t8O\r\n" \
    "kDL2PsNRRYrCQey1DCgMlACLSCqX0bA0aIv/EvkxDu2wssE3eXkEA2dQa9L671WN\r\n" \
    "1PIdmQ+3vFN/RhzHajlV29YPIXtWXooRHtNoAtgHUs0qBEv3eSkiwym4XLHOO4To\r\n" \
    "lAaXPezUpwYd1KXjOxvi9QRIcPW/0u8CAwEAAaM/MD0wDAYDVR0TAQH/BAIwADAO\r\n" \
    "BgNVHQ8BAf8EBAMCB4AwHQYDVR0OBBYEFD76BI4lArWNSO21UX8e8WWGmM5qMA0G\r\n" \
    "CSqGSIb3DQEBCwUAA4IBAQDLIItWp7QbRrsvZ9+OZDwFisULk0FexfV/MWxFZjxG\r\n" \
    "IYJGt99k4iQZG59BRa+ETlHtGSbxA7jU9ijFs4PlDzxqKIySMFlUcAEHaDK1ygfF\r\n" \
    "fSHElxMtwXLz+3SlOrDj+meZHujkNCGG55Hpo1DQxsSBYcZQtN6K2EUiEgNLce6z\r\n" \
    "xc3khDGmMW1g4WggoURf7ofjvV+ttcNhkQe1n3zhQOLnDC+qsFYoLpKAZTmm3bCT\r\n" \
    "j44zr/g/tfeu9HxcvsSq7SyJH2rUVDMyitseAYlEQ7l167Rxw+/leDNnviI/cTtX\r\n" \
    "RHTEcYEHHe4vwCvr4jMeIzrIlsJ21gR25ThKNT3Q7KGR\r\n"
    "-----END CERTIFICATE-----\r\n"
};
#endif
//zhiwei
const char tencent_client_cert[] = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIDTzCCAjegAwIBAgIBADANBgkqhkiG9w0BAQsFADB5MQswCQYDVQQGEwJDTjES\r\n" \
    "MBAGA1UECAwJR3VhbmdEb25nMREwDwYDVQQHDAhTaGVuWmhlbjEQMA4GA1UECgwH\r\n" \
    "VGVuY2VudDEXMBUGA1UECwwOVGVuY2VudCBJb3RodWIxGDAWBgNVBAMMD3d3dy50\r\n" \
    "ZW5jZW50LmNvbTAeFw0yMDA0MDExMDU3MjhaFw0zMDAzMzAxMDU3MjhaMBwxGjAY\r\n" \
    "BgNVBAMMETlYM0hBWFkyVkN0ZXN0bGl1MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\r\n" \
    "MIIBCgKCAQEAvj7BP2cpyEOcbko8AKPNosRZYPYn0ivdzL6BwNiHl6AfxtehxswN\r\n" \
    "YsBwZbTeKbs7f8r8cQPz4Khb1IZWOv6nkSVDCyg4v7U1AfnLlcLTVWsJq1T6GbUB\r\n" \
    "K8s5cmwoh5Lh4+EjeuWsdDADeN5+UiaSpWgU+LYsb45/oOanjHb59u88lKEJ/s3K\r\n" \
    "GfN4Bz+5c5ehu8EKgZdnNI7ofn4MunbVKXcuVcPvKdVpr8i/MdtyeZFbpz3E3nfK\r\n" \
    "UrnSki2d7vhRSZmwG4+YUP+E394vfGyKmpGVFhpOvbmq4U07DSD1IgODXHdGInBC\r\n" \
    "ef1vrMNI6e/LOMmDTQukvy6uNzj1MhmN/wIDAQABoz8wPTAMBgNVHRMBAf8EAjAA\r\n" \
    "MA4GA1UdDwEB/wQEAwIHgDAdBgNVHQ4EFgQUG3OrwgBrnT00aqdSJKcXWk9yZpkw\r\n" \
    "DQYJKoZIhvcNAQELBQADggEBAElFn0Rykl9WwI5QJ14PfX2sHS/Q1i4byNNe4sdt\r\n" \
    "eFYUE28g9rF2xlwjh6aC6kNF4uheejolaEQdIpO97630145tjxHvB6EB8IZti16E\r\n" \
    "ufdkpwrJQ2nX2lR43o03+W48zI00vHheo8vGjH2fxLYIj5ZY+cyhSyCLRrz4EEEt\r\n" \
    "2kUuTKk2fQN8PDgfTymVsslPe8Z6nIL9l/PUGBbXVOv5aPBwT8in41LQGX1yCgP7\r\n" \
    "A66sMxpiSLXjkoGQC5ObKMxQe565xKU483XWZeN2Bi7f4rgKpQUCCMN1TmWo0jwL\r\n" \
    "GhWdbLBVOBT92/v2q+KwA3AaQoc9Sga4RIS3Ljh1DevRViQ=\r\n" \
    "-----END CERTIFICATE-----\r\n"
};
const size_t tencent_client_cert_len = sizeof(tencent_client_cert);
#if 0 //donghui
const char tencent_client_private_key[] = \
{
    \
    "-----BEGIN PRIVATE KEY-----\r\n"                                      \
    "MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCoSNMktJOGR5YH\r\n" \
    "oKLgYkzkg09EEjbP9StMjr53DaQOmG/MTRxz2t05sBIMlpvDSMmla2EOojl3Px2o\r\n" \
    "8nTVOa493vpDajXD57NSZLqL3RuUuDfZzk34eY2WW1UFVIDaLqHkDjgbNvoCz9HF\r\n" \
    "+uwl+xpvBC3iDGm64JsaR6wHOCTTnD5qN1Akn9bfDpAy9j7DUUWKwkHstQwoDJQA\r\n" \
    "i0gql9GwNGiL/xL5MQ7tsLLBN3l5BANnUGvS+u9VjdTyHZkPt7xTf0Ycx2o5VdvW\r\n" \
    "DyF7Vl6KER7TaALYB1LNKgRL93kpIsMpuFyxzjuE6JQGlz3s1KcGHdSl4zsb4vUE\r\n" \
    "SHD1v9LvAgMBAAECggEAECQcDHM9iy2mmEiyFq+XDHwDyFWu6Ddt1cQDRMwMJlI8\r\n" \
    "tu3Wgs5FVdkrggWYHx0GKlWoU4z/0g49vAy1ZBZKPij6/Chdy9KXgpmiwvOMR7F7\r\n" \
    "uept1B1klT1hZEd8rHC4mnU6sHGPfWGNj13H9MSm/mr6M6pZIdGVTun8+BxMe/di\r\n" \
    "tc7RdesWx90J2u4vMPKjEYEy1dJji9vYm3WRQ2nJxZd8rpNMHuezE0qi2YNHGxqK\r\n" \
    "sHzjV/zHwNIAbUAAeoyEugBp3BS5e5FeOq/Dgnize8AfGL68dI4ntZ19X/ra3r8Z\r\n" \
    "+GPWYGOZBWqEKUoBr//nvI+aMCPoGUmSF/t92FvP4QKBgQDW+bFyzuJKQiQcDNg8\r\n" \
    "3RGG0sMPT8EdZ1Ry14N/eYQe1ijmi2ziQJOGH2/jd/320M+0+quZv1aZvtJx9Rb9\r\n" \
    "fRy7gNOR5kwzHNYwQPs8p/0vPAUMSTJPgZM7w68Nccz0RPoYUt8EuPEXRAW1+Iho\r\n" \
    "b0BiOr3mMCJr9r5Hzgm524nvRwKBgQDIZh3M2hNet6OaWcFkklyT84bMrTF0KMG4\r\n" \
    "xMHepLwsE2qhmkUHLEhrR7VHVqiqY7fweDogem2HSkJ2fFDm5GxHqfr2Fh8ctZd2\r\n" \
    "T66PnlC3OlomNzfZz7V79sd83EZCXLUPNRhv0/e39sjm3ApQeyb48dixTQYdNTPa\r\n" \
    "By7rucljGQKBgBiNagvfvw6WIUOm3+eLpB3O4oJULI0sKQP4w0cC3Okcso0qLWRl\r\n" \
    "1bOC0aecI8BVtvNHkeO8a6qhla14PYj0Ch0MKJ3IPLm1uGzIbylNUmhbpYPytDFK\r\n" \
    "bkNsJQkXDz5yxlGS3W/Po5FxrmlLGmE7G5JcfSXWJ4hHJRJUYN3+SfGvAoGAD3jy\r\n" \
    "7BIaVMHQ++XaLK2wQCwZSExG+TT8YAuw/EOz06NcJEYI9ljQ7peGw49JTn6MgSCH\r\n" \
    "PGK3ei6dmxBWHr2cLpxNKBrlJA5RN21HiQcsLDlxJGo3AbYryPpwxI8tTYpi4F21\r\n" \
    "GU1AXw/KyyhKE5SumaHynR3ZTwAAlODhGooppmECgYB206F5UMtg+hiMsZc3rebm\r\n" \
    "cgZSUI9czXppxPli/VKcxsP1zO9kYrrY7frYcV6m8x9OO2787YdUSeumpmbBb1Rw\r\n" \
    "BXSOWGdS3oOsKQRIk0noQqx8DthOS+p62QiyZfuosFgiLawDoVfsXmB0f+nH+Nu9\r\n" \
    "T21SBFGY/6QoiQB8YcORwg==\r\n"
    "-----END PRIVATE KEY-----\r\n"

};
#endif
//zhiwei
const char tencent_client_private_key[] = \
{
    \
    "-----BEGIN PRIVATE KEY-----\r\n"                                      \
    "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQC+PsE/ZynIQ5xu\r\n" \
    "SjwAo82ixFlg9ifSK93MvoHA2IeXoB/G16HGzA1iwHBltN4puzt/yvxxA/PgqFvU\r\n" \
    "hlY6/qeRJUMLKDi/tTUB+cuVwtNVawmrVPoZtQEryzlybCiHkuHj4SN65ax0MAN4\r\n" \
    "3n5SJpKlaBT4tixvjn+g5qeMdvn27zyUoQn+zcoZ83gHP7lzl6G7wQqBl2c0juh+\r\n" \
    "fgy6dtUpdy5Vw+8p1WmvyL8x23J5kVunPcTed8pSudKSLZ3u+FFJmbAbj5hQ/4Tf\r\n" \
    "3i98bIqakZUWGk69uarhTTsNIPUiA4Ncd0YicEJ5/W+sw0jp78s4yYNNC6S/Lq43\r\n" \
    "OPUyGY3/AgMBAAECggEAPWv3amf9RmoFzFntS2uXTbDFBpgSvTlqb9sl+0QtatfI\r\n" \
    "yqL92TgyLirhXwTpIWIQ0K6RmE9lYqgYleIVXxCuux7jqWt//u0bQxsKDr0LDC31\r\n" \
    "vjo+USRuT1xanMWsxDC4+6DJmrkiV5TU2iZDdHR9riePdckjSZUhQaqWd7RF+eIu\r\n" \
    "p+BfZW7Thq69hn8wzxnETUwFnk+MwmGq/7BJU1s8qTHCXIfUnELjdo5jFox+RYdo\r\n" \
    "eFC6bBwbvzpU1wauPrxGjY0aWSje0OKXMiBvnQndREBnO3B+wFI9sga6gOWTJxBO\r\n" \
    "qKTtBJx+9Nv7sGSWZJeOKpYRSOwk6Twp6E5eKBmuiQKBgQDudXC8Pxse1OgTCMHz\r\n" \
    "Z9wMgA61S76p9/EWzJ6VyVhL/JIONci62X8rzPlWs5b/TEIT3PT/AW1jOjP9TS3g\r\n" \
    "oVxtLADTwoYSiqHCTS/BkzT0t9zk5LpiPev4DdHFRrRGCh+KCUsFDmRS4yv0C6Nk\r\n" \
    "NAN4X6rHZmnhZAn+EsAcGmQpTQKBgQDMPWABmdNln1jluSTeDv4Szn0AhrzWnvy6\r\n" \
    "66EkplE3wcEjAD5dDLK6lWpf88O7cXqFlRt6WCDMqWfjNxdRH0OT81oeAVRwZizh\r\n" \
    "L8Db65hGpg7kPn5A3eu0GvgoHfJjUo1o7OsR0ElnRFEbdoiAuTDB/JQ6SSwH8J+7\r\n" \
    "WjlY+NWOewKBgQCRza1I7S0gBP6B5PDya6bY3dQgdlf2hWwcjLDK1XnvvFdx6OD7\r\n" \
    "ntFrbsFmihjhsTcV/hyvb6f67DOtxesvVnq1Jhq7YE2qOKn/rEJvEhtnJqUiL9jn\r\n" \
    "CQWnfoutHDZOPRJbzKLaTJp7M/xEFHmrY0Jb7+L0WWKUNh8GA2EDzf9AYQKBgFv7\r\n" \
    "DCDMcTskH3od/q+PGvfHTN6Oz77sjS91bPY/fPZaaSvlMTDgxiFnpyeddboWKjK8\r\n" \
    "60oXJHZnmOToa7TaGqon/57hxHB2Y6HqyrWAjezLXouGZX+7MirDtp0Cl6YJiKwH\r\n" \
    "fvpfzp8WUlzG7kTiokXLBcqWJ6FPyfArewZJ0ou/AoGBAKwSm9ugDrktQyjfSdhS\r\n" \
    "DymZCrF4TccqHCksK6rU0RDksKtw7u5Z/kOvBvr2voi75cSvDLlJut3nYyK2tl73\r\n" \
    "19gQCL7LvzNIeZTJrOnAKDv0mpnlkrttixHQ663iuIrYHUQXvggBNH+AoWR/qTm6\r\n" \
    "yvufNRGeSkStI4NstyPn8T9t\r\n" \
    "-----END PRIVATE KEY-----\r\n"

};
const size_t tencent_client_private_key_len = sizeof(tencent_client_private_key);
int tencent_shadowVersion = 0;
char tencent_productID[32] = {0};
int32_t tencent_productID_len = TENCENT_PRODUCT_ID_LEN;
char tencent_deviceName[32] = {0};
int32_t tencent_deviceName_len = TENCENT_DEVICE_NAME_LEN;
int init_tencent_info()
{
    int ret = 0;
    ret = lega_flash_kv_get("tencent_productID", tencent_productID, &tencent_productID_len);
    if(ret != 0)
    {
    printf("tencent productID get error\n");
        return ret;
    }
    else
    {
        printf("tencent productID = %s\n", tencent_productID);
    }

    ret = lega_flash_kv_get("tencent_deviceName", tencent_deviceName, &tencent_deviceName_len);
    if(ret != 0)
    {
        printf("tencent deviceName get error\n");
        return ret;
    }
    else
    {
        printf("tencent deviceName = %s\n", tencent_deviceName);
    }
    return ret;
}

int tencent_mqtt_get_topic(char *topic_out, int action_in)
{
    char *ptmp_buf = NULL;
    int topic_len = 0;
    if(topic_out == NULL)
    {
        printf("topic_out is NULL\n");
        return -1;
    }
    if(action_in == TENCENT_TOPIC_PUBLISH)
    {
        ptmp_buf = "$shadow/operation/%s/%s";
    }
    else if(action_in == TENCENT_TOPIC_SUBSCRIBE_RESULT)
    {
        ptmp_buf = "$shadow/operation/result/%s/%s";
    }
    else if(action_in == TENCENT_TOPIC_SUBSCRIBE_CONTROL)
    {
        ptmp_buf = "%s/%s/control";
    }
    else if(action_in == TENCENT_TOPIC_SUBSCRIBE_DATA)
    {
        ptmp_buf = "%s/%s/data";
    }
    else
    {
        printf("topic input error\n");
        return -1;
    }
    topic_len = strlen(ptmp_buf) + strlen(tencent_productID) + strlen(tencent_deviceName) + 1;
    snprintf(topic_out, topic_len, ptmp_buf, tencent_productID, tencent_deviceName);
    printf("topic:%s\n", topic_out);
    return 0;
}

int tencent_mqtt_get_publish_payload(char *payload_out, int action_in)
{
    char *ptmp = NULL;
    cJSON * pJsonData = cJSON_CreateObject();
    cJSON * pJsonState = cJSON_CreateObject();
    cJSON * pJsonReported = cJSON_CreateObject();
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
    if(action_in == PAYLOAD_GET_SHADOW)
    {
        cJSON_AddStringToObject(pJsonData, "type", "get");
        cJSON_AddStringToObject(pJsonData, "clientToken", "clientToken");
    }
    else if(action_in == PAYLOAD_LIGHT_OFF)
    {
        cJSON_AddStringToObject(pJsonData, "type", "update");
        cJSON_AddItemToObject(pJsonData, "state", pJsonState);
        cJSON_AddItemToObject(pJsonState, "reported", pJsonReported);
        cJSON_AddStringToObject(pJsonReported, "switch", "False");
        cJSON_AddNumberToObject(pJsonData, "version", tencent_shadowVersion);
        cJSON_AddStringToObject(pJsonData, "clientToken", "clientToken");
    }
    else if(action_in == PAYLOAD_LIGHT_ON)
    {
        cJSON_AddStringToObject(pJsonData, "type", "update");
        cJSON_AddItemToObject(pJsonData, "state", pJsonState);
        cJSON_AddItemToObject(pJsonState, "reported", pJsonReported);
        cJSON_AddStringToObject(pJsonReported, "switch", "True");
        cJSON_AddNumberToObject(pJsonData, "version", tencent_shadowVersion);
        cJSON_AddStringToObject(pJsonData, "clientToken", "clientToken");
    }
    else
    {
        printf("get publish payload input error\n");
        cJSON_Delete(pJsonData);
        cJSON_Delete(pJsonState);
        cJSON_Delete(pJsonReported);
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