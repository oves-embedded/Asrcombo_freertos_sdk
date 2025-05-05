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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lega_at_api.h"
#include "ota_port.h"
#include "http_ota.h"

#ifdef AT_USER_DEBUG

#ifdef HTTP_OTA_SUPPORT
int at_ota_http(int argc, char **argv)
{
    int ret = 0;

    switch(argc)
    {
        case 1:
            dbg_at("there is not address to access!");
            ret = CONFIG_FAIL;
            break;
        case 2:
            dbg_at("http ota doing ... ");
            if(http_update_ota(argv[1], LEGA_OTA_FINISH) != 0)
            {
                dbg_at("http ota failed!");
                ret = CONFIG_FAIL;
            }
            else
            {
                dbg_at("http ota successed!");
                ret = CONFIG_OK;
            }
            break;
        default:
            ret = CONFIG_FAIL;
            break;
    }

    return ret;
}

cmd_entry comm_ota_http = {
    .name = "ota_http",
    .help = "ota_http url",
    .function = at_ota_http,
};
#endif

void lega_at_ota_cmd_register(void)
{
#ifdef HTTP_OTA_SUPPORT
    lega_at_cmd_register(&comm_ota_http);
#endif
}

#endif

