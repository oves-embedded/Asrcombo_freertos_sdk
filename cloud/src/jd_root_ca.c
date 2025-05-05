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
#include <stdlib.h>
#include "jd_auth.h"
/**
 * @brief the certificate of certificate authority
 */
const char jd_ca_crt[] = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIID5TCCAs2gAwIBAgIJAJv4O0FUzUOCMA0GCSqGSIb3DQEBCwUAMIGHMQswCQYD\r\n" \
    "VQQGEwJDTjEQMA4GA1UECAwHQmVpamluZzEQMA4GA1UEBwwHQmVpamluZzELMAkG\r\n" \
    "A1UECgwCSkQxDDAKBgNVBAsMA1lVTjEYMBYGA1UEAwwPaW90LmpkY2xvdWQuY29t\r\n" \
    "MR8wHQYJKoZIhvcNAQkBFhB3dXl1ZWZlbmdAamQuY29tMCAXDTE5MDgyMDAwMjMz\r\n" \
    "M1oYDzIxMTkwNzI3MDAyMzMzWjCBhzELMAkGA1UEBhMCQ04xEDAOBgNVBAgMB0Jl\r\n" \
    "aWppbmcxEDAOBgNVBAcMB0JlaWppbmcxCzAJBgNVBAoMAkpEMQwwCgYDVQQLDANZ\r\n" \
    "VU4xGDAWBgNVBAMMD2lvdC5qZGNsb3VkLmNvbTEfMB0GCSqGSIb3DQEJARYQd3V5\r\n" \
    "dWVmZW5nQGpkLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKsm\r\n" \
    "YqPiLed1k3T43qujdtRflC+Sx+hhQSewNfEtfCMikkO0E+nuGisKCRWaCxT3PpRC\r\n" \
    "cxa4Qp21kixV29yQzFfmEzyciQ3EeO7VpCi1Rwo83r9U30VLrZDj0yVe+yDCk1qT\r\n" \
    "/aR20dG5x7jaxQvaFh9+uCEhb7MOV5LM47DlV/TQaX7iDqNa135J9/o7ULhhIOnj\r\n" \
    "cK57JcXiw4P6XF1LBF9LC60reD06CUdMT89Jmr0+d3SM0wFVe8gegdRmNOXYAkcO\r\n" \
    "qxVJdqkhWtJCUHkXuu8U744qTsZfO8Yj5vbaUFc85k2V51yUtzNghG3ijaVX4+qu\r\n" \
    "G1VwWITrBVtKVXQRiUsCAwEAAaNQME4wHQYDVR0OBBYEFJ+JoGY6OunVZDvUsowe\r\n" \
    "cGGojAAoMB8GA1UdIwQYMBaAFJ+JoGY6OunVZDvUsowecGGojAAoMAwGA1UdEwQF\r\n" \
    "MAMBAf8wDQYJKoZIhvcNAQELBQADggEBAHDoojSTmk0SEIlprpUF8FoS1Hrjf4E+\r\n" \
    "sscHKJx8TW0XpeJKPO5+OhLzqsIr+TJDnPQjH/ugtv6FoSE5xccHLWV/QicxGD8O\r\n" \
    "tw5ABMnyhpsLiJTVYIpU2nqkeDEt9hoWh5dDB+COtHHROOevkyV957yW+LDp+y19\r\n" \
    "CiLNoaGpO7QwthNTvo6g1b85Az2GEnVImchVkqeYkCA8yy7u//fbKVacTAzxCIMH\r\n" \
    "EjxzGanhNF9eIRk0fiPdhyU/egIjpC2t5bstLHhVvF+UswTOIUjeFPKxVN98H3s7\r\n" \
    "2+e+WGw56h9IWhGm56+hKIFL3Dy6tMXK5sQdMcSEAOEiTSTb9L+8rVY=\r\n"
    "-----END CERTIFICATE-----\r\n"
};
#endif