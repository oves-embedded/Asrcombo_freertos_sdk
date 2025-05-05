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


#ifdef AT_USER_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lega_at_api.h"
#include "elog.h"
#if (CFG_EASY_LOG_MODULE_EN == 1)
#include "elog_module.h"
#endif
#ifdef CFG_PLF_DUET
#include "duet_common.h"
#else
#include "lega_common.h"
#endif

/*
 ************************************************************
 *                    USER AT CMD START
 *
 ************************************************************
 */

int at_test_1(int argc, char **argv)
{
    int tempValue;
    switch(argc)
    {
        case 1:
            log_i("%s...%s\n",__func__, argv[0]);
            break;
        case 2:
            log_i("%s...%s\n", __func__, argv[1]);
            break;
        case 3:
            tempValue = convert_str_to_int(argv[2]);
            if(tempValue == LEGA_STR_TO_INT_ERR)
            {
                log_i("error param\n");
                break;
            }
            log_i("%s...%s...%d\n", __func__, argv[1], tempValue);
            break;
        default:
            log_i("error param\n");
            break;
    }

    return CONFIG_OK;
}
int at_test_2(int argc, char **argv)
{
    log_i("%s...\n",__func__);
    return CONFIG_OK;
}

#if (CFG_EASY_LOG_ENABLE == 1)
#if (CFG_EASY_LOG_MODULE_EN == 1)
int elog_at_config(int argc, char **argv)
{
    int module_id;
    log_i("elog config\n");
    switch(argc)
    {
        case 4:
            module_id = convert_str_to_int(argv[1]);
            if(module_id >= get_elog_module_num() || module_id < 0)
            {
                log_e("error module id:%d\n", module_id);
                break;
            }
            if(strcmp(argv[2], "switch") == 0)
            {
                if(strcmp(argv[3], "on") == 0)
                {
                    set_elog_module_swtich(module_id, MODULE_ELOG_ON);
                }
                else if(strcmp(argv[3], "off") == 0)
                {
                    set_elog_module_swtich(module_id, MODULE_ELOG_OFF);
                }
                else
                {
                    log_e("switch :%s, must be on or off\n", argv[3]);
                }
            }
            else if(strcmp(argv[2], "level") == 0)
            {
                if(strcmp(argv[3], "assert") == 0)
                {
                    set_elog_module_level(module_id, ELOG_LVL_ASSERT);
                }
                else if(strcmp(argv[3], "error") == 0)
                {
                    set_elog_module_level(module_id, ELOG_LVL_ERROR);
                }
                else if(strcmp(argv[3], "warn") == 0)
                {
                    set_elog_module_level(module_id, ELOG_LVL_WARN);
                }
                else if(strcmp(argv[3], "info") == 0)
                {
                    set_elog_module_level(module_id, ELOG_LVL_INFO);
                }
                else if(strcmp(argv[3], "debug") == 0)
                {
                    set_elog_module_level(module_id, ELOG_LVL_DEBUG);
                }
                else if(strcmp(argv[3], "verbose") == 0)
                {
                    set_elog_module_level(module_id, ELOG_LVL_VERBOSE);
                }
                else
                {
                    log_e("level :%s\n", argv[3]);
                }
            }
            else
            {
                log_e("argv[2] = %s, need switch or level\n", argv[2]);
            }
            break;
        default:
            log_e("error param number: need 4 param\n");
            break;
    }
    return CONFIG_OK;
}

void test_log(const char * aFormat, ...)
{
    va_list v;
    va_start(v, aFormat);
    test_printf("test", aFormat, v);
    va_end(v);
}

void test_elog_module()
{
    int i = 0;
    test_log("test_printf\n");
}
#endif
void test_elog(void) {
    /* test log output for all level */
    log_a("Hello EasyLogger!");
    log_e("Hello EasyLogger!");
    log_w("Hello EasyLogger!");
    log_i("Hello EasyLogger!");
    log_d("Hello EasyLogger!");
    log_v("Hello EasyLogger!");
//    elog_raw("Hello EasyLogger!");
}

int elog_test(int argc, char **argv)
{
    // elog_init();
    // /* set EasyLogger log format */
    // elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    // elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    // elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    // elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    // elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL | ELOG_FMT_T_INFO );
    // elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO ));
    /* start EasyLogger */
    // elog_start();
    // while(1){
    //     lega_rtos_delay_milliseconds(1000);
    //     test_elog();
    // }
    test_elog();
#if (CFG_EASY_LOG_MODULE_EN == 1)
    test_elog_module();
#endif
    return CONFIG_OK;
}
int elog_at_stop(int argc, char **argv)
{
    elog_stop();
    return CONFIG_OK;
}

int elog_at_start(int argc, char **argv)
{
    elog_start();
    return CONFIG_OK;
}
#endif
// lega_at_cmd_entry at_cmd_table[] =
// {
//     {"at_test_1",at_test_1},
//     {"at_test_2",at_test_2},
// };

/*
 ************************************************************
 *                    USER AT CMD END
 *
 ************************************************************
 */

cmd_entry comm_test1 = {
    .name = "at_test1",
    .help = "at_test1",
    .function = at_test_1,
};
cmd_entry comm_test2 = {
    .name = "at_test2",
    .help = "at_test2",
    .function = at_test_2,
};

#if (CFG_EASY_LOG_ENABLE==1)
cmd_entry comm_elog_test = {
    .name = "elog_test",
    .help = "elog_test: elog_test \r\n",
    .function = elog_test,
};

cmd_entry comm_elog_stop = {
    .name = "elog_at_stop",
    .help = "elog_at_stop: elog_at_stop \r\n",
    .function = elog_at_stop,
};

cmd_entry comm_elog_start = {
    .name = "elog_at_start",
    .help = "elog_at_start: elog_at_start \r\n",
    .function = elog_at_start,
};
#if (CFG_EASY_LOG_MODULE_EN == 1)
cmd_entry comm_elog_config = {
    .name = "elog_at_config",
    .help = "elog_at_config: elog_at_config module_name swtich\/level on\/off\/level_num \r\n",
    .function = elog_at_config,
};
#endif
#endif
void lega_at_user_cmd_register(void)
{
    lega_at_cmd_register(&comm_test1);
    lega_at_cmd_register(&comm_test2);
#if (CFG_EASY_LOG_ENABLE == 1)
    lega_at_cmd_register(&comm_elog_test);
    lega_at_cmd_register(&comm_elog_stop);
    lega_at_cmd_register(&comm_elog_start);
    lega_at_cmd_register(&comm_elog_test);
#if (CFG_EASY_LOG_MODULE_EN == 1)
    lega_at_cmd_register(&comm_elog_config);
#endif
#endif
}
#endif
