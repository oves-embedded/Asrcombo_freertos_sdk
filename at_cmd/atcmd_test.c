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
#ifdef TEST_UNITY_CLOUD
#include "ProductionCode.h"
#include "TestProductionCode_Runner.h"
#endif
#ifdef TEST_UNITY_EXAMPLE
#include "all_tests.h"
#endif

/*
 ************************************************************
 *                    USER AT CMD START
 *
 ************************************************************
 */

int at_test_unity_cloud(int argc, char **argv)
{
#ifdef TEST_UNITY_CLOUD
    switch(argc)
    {
        case 4:
            downside_timeout_s = atoi(argv[3]);
            if(downside_timeout_s <= 0 || downside_timeout_s >= 100)
                return CONFIG_FAIL;
        case 3:
            disconnect_delay_s = atoi(argv[2]);
            if(disconnect_delay_s <= 0 || disconnect_delay_s >= 100)
                return CONFIG_FAIL;
        case 2:
            if(!strcmp(argv[1], "all")) break;
            else if(!strcmp(argv[1], "baidu")) break;
            else if(!strcmp(argv[1], "jd")) break;
            else if(!strcmp(argv[1], "tencent")) break;
            else if(!strcmp(argv[1], "huawei")) break;
            else if(!strcmp(argv[1], "aws")) break;
            else if(!strcmp(argv[1], "private")) break;
            else if(!strcmp(argv[1], "emq")) break;
            else return CONFIG_FAIL;
            break;
        case 1:
            return PARAM_MISS;
        default:
            return PARAM_RANGE;
    }
    test_unity_cloud(argv[1]);
    return CONFIG_OK;
#endif
    printf("need open macro TEST_UNITY_CLOUD\n");
    return CONFIG_FAIL;
}

int at_test_unity_example(int argc, char **argv)
{
    printf("%s...\n",__func__);
#ifdef TEST_UNITY_EXAMPLE
    test_main2(argc, argv);
#endif
    return CONFIG_OK;
}

// lega_at_cmd_entry at_cmd_table[] =
// {
//     {"at_test_1",at_test_unity_cloud},
//     {"at_test_2",at_test_2},
// };

/*
 ************************************************************
 *                    USER AT CMD END
 *
 ************************************************************
 */

cmd_entry comm_test_unity_cloud = {
    .name = "at_test_unity_cloud",
    .help = "at_test_unity_cloud",
    .function = at_test_unity_cloud,
};
cmd_entry comm_test_test_unity_example = {
    .name = "at_test_unity_example",
    .help = "at_test_unity_example",
    .function = at_test_unity_example,
};

void lega_at_test_cmd_register(void)
{
    lega_at_cmd_register(&comm_test_unity_cloud);
    lega_at_cmd_register(&comm_test_test_unity_example);
}