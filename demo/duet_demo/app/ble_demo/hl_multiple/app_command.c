
/**
 ****************************************************************************************
 *
 * @file (app_command.h)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app_command.h"
#include "sonata_utils_api.h"
#include "app_nv.h"
#include "sonata_utils_api.h"
#include "app_utils.h"
#include "data_transport_svc.h"
#include "app_uart.h"
#include "sonata_ll_api.h"
#include "app_sec.h"
#include "app_platform.h"
#include "stdlib.h"

/*
 * MACRO DEFINES
 ****************************************************************************************
 */
#define UART_PORT  (UART1_INDEX)

#define PARA0 (0)
#define PARA1 (1)
#define PARA2 (2)
#define PARA3 (3)
#define PARA4 (4)
#define PARA5 (5)
#define PARA6 (6)
/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
static uart_data_t gCmdUartData = {0};

typedef struct cmd_env_tag{
    app_cmd_id cmdId;
    uint8_t pOffset;
    param_info pInfo;
}cmd_env;

app_cmd_info_t  cmdArray[] = {
        {ID_RESET      , ATCMD_LEN_RESET      , ATCMD_RESET},
        {ID_VERSION    , ATCMD_LEN_VERSION    , ATCMD_VERSION},
        {ID_LADDR      , ATCMD_LEN_LADDR      , ATCMD_LADDR},
        {ID_NAME       , ATCMD_LEN_NAME       , ATCMD_NAME},
        {ID_BAUD       , ATCMD_LEN_BAUD       , ATCMD_BAUD},
        {ID_UUID       , ATCMD_LEN_UUID       , ATCMD_UUID},
        {ID_PIN        , ATCMD_LEN_PIN        , ATCMD_PIN},
        {ID_MODE       , ATCMD_LEN_MODE       , ATCMD_MODE},
        {ID_AUTOCONN   , ATCMD_LEN_AUTOCONN   , ATCMD_AUTOCONN},
        {ID_SCAN       , ATCMD_LEN_SCAN       , ATCMD_SCAN},
        {ID_CONN       , ATCMD_LEN_CONN       , ATCMD_CONN},
        {ID_CHINFO     , ATCMD_LEN_CHINFO     , ATCMD_CHINFO},
        {ID_LESEND     , ATCMD_LEN_LESEND     , ATCMD_LESEND},
        {ID_DATA       , ATCMD_LEN_DATA       , ATCMD_DATA},
        {ID_LEDISC     , ATCMD_LEN_LEDISC     , ATCMD_LEDISC},
        {ID_ATTEST     , ATCMD_LEN_AT         , ATCMD_AT},
        {ID_REBOOT     , ATCMD_LEN_REBOOT     , ATCMD_REBOOT},
        {ID_NTF        , ATCMD_LEN_NTF        , ATCMD_NTF},
        {ID_IND        , ATCMD_LEN_IND        , ATCMD_IND},
        {ID_ADV        , ATCMD_LEN_ADV        , ATCMD_ADV},
        {ID_THROUGHPUT , ATCMD_LEN_THROUGHPUT , ATCMD_THROUGHPUT},
        {ID_DISCOVERY  , ATCMD_LEN_DISCOVERY  , ATCMD_DISCOVERY},
        {ID_MTU        , ATCMD_LEN_MTU        , ATCMD_MTU},
        {ID_SVCCHANGE  , ATCMD_LEN_SVCCHANGE  , ATCMD_SVCCHANGE},
        {ID_MCONN      , ATCMD_LEN_MCONN      , ATCMD_MCONN},
        {ID_NTFSEND    , ATCMD_LEN_NTFSEND    , ATCMD_NTFSEND},
        {ID_CONNPARAM  , ATCMD_LEN_CONNPARAM  , ATCMD_CONNPARAM},
        {ID_AUTOADV    , ATCMD_LEN_AUTOADV    , ATCMD_AUTOADV},
        {ID_CLEARACTIVE, ATCMD_LEN_CLEARACTIVE, ATCMD_CLEARACTIVE},
        {ID_EXTADV     , ATCMD_LEN_EXTADV     , ATCMD_EXTADV},
        {ID_ATTHDL     , ATCMD_LEN_ATTHDL     , ATCMD_ATTHDL},
        {ID_DLE        , ATCMD_LEN_DLE        , ATCMD_DLE},
        {ID_LOG        , ATCMD_LEN_LOG        , ATCMD_LOG},
        {ID_TXPWR      , ATCMD_LEN_TXPWR      , ATCMD_TXPWR},
        {ID_BONDCLEAR  , ATCMD_LEN_BONDCLEAR  , ATCMD_BONDCLEAR},
        {ID_BOND       , ATCMD_LEN_BOND       , ATCMD_BOND},
        {ID_GATTREAD   , ATCMD_LEN_GATTREAD   , ATCMD_GATTREAD},
        {ID_BONDINFO   , ATCMD_LEN_BONDINFO   , ATCMD_BONDINFO},
        {ID_ENCRYPT    , ATCMD_LEN_ENCRYPT    , ATCMD_ENCRYPT},
        {ID_NCOMPARE   , ATCMD_LEN_NCOMPARE   , ATCMD_NCOMPARE},
        {ID_GENADDR    , ATCMD_LEN_GENADDR    , ATCMD_GENADDR},
        {ID_RESOLVADDR , ATCMD_LEN_RESOLVADDR , ATCMD_RESOLVADDR},
        {ID_SETIRK     , ATCMD_LEN_SETIRK     , ATCMD_SETIRK},
        {ID_RALLIST    , ATCMD_LEN_RALLIST    , ATCMD_RALLIST},
        {ID_GETRAL     , ATCMD_LEN_GETRAL     , ATCMD_GETRAL},
        {ID_DIRECTADV  , ATCMD_LEN_DIRECTADV  , ATCMD_DIRECTADV},
        {ID_PERIODADV  , ATCMD_LEN_PERIODADV  , ATCMD_PERIODADV},
        {ID_PERIODSYNC , ATCMD_LEN_PERIODSYNC , ATCMD_PERIODSYNC},
        {ID_PRESSURE   , ATCMD_LEN_PRESSURE   , ATCMD_PRESSURE},
        {ID_AUTONTF    , ATCMD_LEN_AUTONTF    , ATCMD_AUTONTF},
        {ID_ADVDATA    , ATCMD_LEN_ADVDATA    , ATCMD_ADVDATA},
        {ID_WLIST      , ATCMD_LEN_WLIST      , ATCMD_WLIST},
        {ID_PALLIST    , ATCMD_LEN_PALLIST    , ATCMD_PALLIST},
        {ID_DEVINFO    , ATCMD_LEN_DEVINFO    , ATCMD_DEVINFO},
        {ID_TIMER      , ATCMD_LEN_TIMER      , ATCMD_TIMER},
        {ID_OOBDATA    , ATCMD_LEN_OOBDATA    , ATCMD_OOBDATA},
        {ID_BONDTYPE   , ATCMD_LEN_BONDTYPE   , ATCMD_BONDTYPE },
        {ID_CHIPINFO   , ATCMD_LEN_CHIPINFO   , ATCMD_CHIPINFO },
        {ID_EFUSEREAD  , ATCMD_LEN_EFUSEREAD  , ATCMD_EFUSEREAD },
        {ID_EFUSEWRITE , ATCMD_LEN_EFUSEWRITE , ATCMD_EFUSEWRITE },
        {ID_APPCONFIG  , ATCMD_LEN_APPCONFIG  , ATCMD_APPCONFIG },
        {ID_HLTRANS    , ATCMD_LEN_HLTRANS    , ATCMD_HLTRANS },
        {ID_ADDTARGET  , ATCMD_LEN_ADDTARGET  , ATCMD_ADDTARGET },
        {ID_REG        , ATCMD_LEN_REG        , ATCMD_REG },
        {ID_TXTONE     , ATCMD_LEN_TXTONE     , ATCMD_TXTONE },
        };
#define LEN_AT_DATA (APP_MTU+50)
#define LEN_AT_INPUT (LEN_UART_RXBUF)
static uint8_t gOutputBuffer[LEN_AT_DATA]          = {0}; //gOutputBuffer[0] is the LENGTH of input command
static uint8_t gInputBuffer[LEN_AT_INPUT]          = {0};
static uint8_t gFormatBuffer[LEN_AT_DATA]          = {0};
static uint16_t gInputCommandLength                = 0;
static uint8_t gTargetAddr[SONATA_GAP_BD_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int32_t gCount                                     = 0;
app_uart_dev_t gUartAt                             = {0};
static cmd_env gCmdEnv;
static mconn_item gMconn[MCONN_COUNT]              = {0};
static uint8_t gTempBufferBase                     = 200;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void *getTempBuffer(uint16_t size)
{
    if (size + gTempBufferBase > LEN_AT_DATA)
    {
        return NULL;
    }
    memset(gOutputBuffer + gTempBufferBase, 0, size);
    return &gOutputBuffer[gTempBufferBase];

}
void *getAllTempBuffer(uint16_t size)
{
    if (size > LEN_AT_DATA)
    {
        return NULL;
    }
    memset(gOutputBuffer, 0, size);
    return &gOutputBuffer[0];
}

void *getTempBufferAt(uint16_t index, uint16_t size)
{
    if (index + size > LEN_AT_DATA)
    {

        return NULL;
    }
    return &gOutputBuffer[index];
}

uint8_t app_cmd_get_index(app_cmd_id cmdId)
{
    uint8_t len = sizeof(cmdArray) / sizeof(app_cmd_info_t);
    for (uint8_t i = 0; i < len; i++)
    {
        if (cmdArray[i].id == cmdId)
        {
            return i;
        }
    }
    return 0;
}

void printCommand()
{
    printf("-------------------------------------------------\r\n    [ID=%02d][Name=%s][P=%d]\r\n",
           gCmdEnv.cmdId, cmdArray[app_cmd_get_index(gCmdEnv.cmdId)].cmd,gCmdEnv.pInfo.count);
    for (int i = 0; i < gCmdEnv.pInfo.count; ++i)
    {
        printf("    P%d[%d]: ",i,gCmdEnv.pInfo.para[i].length);
        //printf("(%d@%d):", gCmdEnv.pInfo.para[i].length, gCmdEnv.pInfo.para[i].start);
        uint8_t *pTemp = gCmdEnv.pInfo.para[i].data;
        for (int j = 0; j < gCmdEnv.pInfo.para[i].length; ++j)
        {
            //printf("%c", gInputBuffer[gCmdEnv.pOffset + gCmdEnv.pInfo.para[i].start + j]);
            printf("%c", pTemp[0]);
            pTemp++;
        }
        printf("    \r\n");

    }
    printf("-------------------------------------------------\r\n");

}
static void parseParameter(const uint8_t *para, uint16_t paraLen, param_info *info)
{
    uint16_t i = 0;
    info->count = 0;
    info->para[info->count].start = 0;
    info->para[info->count].data = (uint8_t *) (&para[0]);
    for (i = 0; i < paraLen; ++i)
    {
        if (para[i] == ',')
        {
            info->para[info->count].end = i - 1;
            info->para[info->count].length = info->para[info->count].end - info->para[info->count].start + 1;
            info->count++;
            info->para[info->count].start = i + 1;
            info->para[info->count].data = (uint8_t *) (&para[i + 1]);
        }
    }
    info->para[info->count].end = i - 1;
    info->para[info->count].length = info->para[info->count].end - info->para[info->count].start + 1;
    info->count++;
//        printf("PARA: count=%d\r\n", info->count);
//        printf("PARA: 1 start=%d, end=%d, len=%d\r\n", info->para[0].start, info->para[0].end, info->para[0].length);
//        printf("PARA: 2 start=%d, end=%d, len=%d\r\n", info->para[1].start, info->para[1].end, info->para[1].length);
//        printf("PARA: 3 start=%d, end=%d, len=%d\r\n", info->para[2].start, info->para[2].end, info->para[2].length);
}

static uint8_t getParameterCount()
{
    return gCmdEnv.pInfo.count;
}

static uint8_t * getParameter(uint8_t id)
{
    return gCmdEnv.pInfo.para[id].data;
}
static uint8_t  getParameterLength(uint8_t id)
{
    return gCmdEnv.pInfo.para[id].length;
}

static int16_t getParameterValue(uint8_t id)
{
    return string2Value(gCmdEnv.pInfo.para[id].data, gCmdEnv.pInfo.para[id].length);
}

static int32_t getParameterValueLong(uint8_t id)
{
    return string2ValueLong(gCmdEnv.pInfo.para[id].data, gCmdEnv.pInfo.para[id].length);
}

app_error app_cmd_is_at_command(uint16_t length, const uint8_t *value)
{
    if (length < 2)
    {
        return ERR_INVALID;
    }
    if ((value[0] == 'A' || value[0] == 'a') && (value[1] == 'T' || value[1] == 't'))
    {
        if (length == 2)
        {
            return ERR_NONE;
        }
        if (value[2] == '+')
        {
            return ERR_NONE;
        }
    }
    APP_TRC_HIGH("CMD: %s, Not AT command\r\n", __FUNCTION__);

    return ERR_INVALID;

}


void app_cmd_backup(uint16_t length, const uint8_t *value)
{
    memset(gInputBuffer, 0, LEN_AT_INPUT);
    gInputCommandLength = length;
    for (int k = 0; k < length; ++k)
    {
        gInputBuffer[k] = char2Caption(value[k]);
    }
    APP_TRC_HIGH("CMD: %s:", __FUNCTION__);
    APP_TRC_HIGH("[%d]", gInputCommandLength);
    for (int i = 0; i < length; ++i)
    {
        APP_TRC_HIGH("%c", gInputBuffer[i]);
    }
    APP_TRC_HIGH("\r\n");
}


uint8_t app_cmd_get_id(uint16_t length)
{
    if (gInputCommandLength == ATCMD_LEN_AT && gInputBuffer[0] == 'A' && gInputBuffer[1] == 'T')
    {
        return ID_ATTEST;
    }

    uint8_t len = sizeof(cmdArray) / sizeof(app_cmd_info_t);
    char *cmd = (char *) (gInputBuffer + ATCMD_LEN_AT_PLUS);
    uint16_t cmdLen = 0;
    char *equal = strchr(cmd, '=');
    if (equal == NULL)
    {
        cmdLen = length - ATCMD_LEN_AT_PLUS;
    }
    else
    {
        cmdLen = equal - cmd;
    }

    for (int i = 0; i < len; ++i)
    {
        //APP_TRC("ID:%02d  STR:[%02d]%s, Target LEN:%d\r\n", cmdArray[i].id, cmdArray[i].cmdLen, cmdArray[i].cmd, cmdLen);
        if (cmdArray[i].cmdLen == cmdLen)
        {
            //APP_TRC("Same Length, cmd:%s, Target:%s\r\n", cmdArray[i].cmd, cmd);
            if (strncmp(cmdArray[i].cmd, cmd, cmdLen) == 0)
            {
                APP_TRC("CMD: find ID:%d[%s] \r\n",cmdArray[i].id, cmdArray[i].cmd);
                return cmdArray[i].id;
            }
        }

    }
    return 0;
}


bool app_cmd_check_contex_scan(bool start)
{

    app_status status = app_get_status();
    if (start == true)
    {
        if (status != STATUS_NONE)
        {
            APP_TRC_ERROR("ERR: App status[%d] not right\r\n",status);
            return false;
        }
    }
    if (start == false)
    {
        if (status != STATUS_SCAN)
        {
            APP_TRC_ERROR("ERR: App status[%d] not right\r\n",status);
            return false;
        }
    }

    return true;
}



bool app_cmd_check_contex_adv(bool start)
{
    app_status status = app_get_status();
    if (start == true)
    {
        if (status != STATUS_NONE)
        {
            APP_TRC_ERROR("ERR: App status[%d] not right\r\n",status);
            return false;
        }
    }
    if (start == false)
    {
        if (status != STATUS_ADV)
        {
            APP_TRC_ERROR("ERR: App status[%d] not right\r\n",status);
            return false;
        }
    }
    return true;
}

bool app_cmd_check_contex_lesend(uint8_t conIdx)
{
    actives *act = app_get_active();
    if (act[conIdx].runing == false)
    {
        return false;
    }
    return true;
}


app_error app_cmd_check(uint8_t cmdId, bool isSet)
{
    return ERR_NONE;
}

bool app_cmd_is_set(uint8_t cmdId)
{
    uint8_t index = app_cmd_get_index(cmdId);
    uint8_t equlaOffset = ATCMD_LEN_AT_PLUS + cmdArray[index].cmdLen;
    bool isSet = (gInputBuffer[equlaOffset] == '=') ? true : false;
    APP_TRC("isSet: %d, cmdId=%d, cmdLen=%d, offset=%d, char=%c\r\n", isSet,
            cmdId, cmdArray[index].cmdLen, equlaOffset, gInputBuffer[equlaOffset]);

    return isSet;
}

uint8_t app_cmd_make_response_header(char *body, uint8_t bodyLen)
{
    //+BODY=
    uint8_t offset = 0;
    memset(gOutputBuffer, 0, LEN_AT_DATA);

    memcpy(gOutputBuffer + offset, ATCMD_PLUS, ATCMD_LEN_PLUS);
    offset += ATCMD_LEN_PLUS;
    memcpy(gOutputBuffer + offset, body, bodyLen);
    offset += bodyLen;
    memcpy(gOutputBuffer + offset, ATCMD_EQUAL, ATCMD_LEN_EQUAL);
    offset += ATCMD_LEN_EQUAL;
    return offset;

}
uint8_t app_cmd_append_string(uint8_t offset, uint8_t *str, uint8_t length, bool addComma)
{
    memcpy(gOutputBuffer + offset, str, length);
    return offset + length;
}

uint8_t app_cmd_append_comma(uint8_t offset)
{
    uint8_t comma[1] = {','};
    memcpy(gOutputBuffer + offset, comma, 1);
    return offset + 1;
}

uint8_t app_cmd_append_int(uint8_t offset, uint8_t value, bool addComma)
{
    sprintf((char *) (gOutputBuffer + offset), "%d", value);
    uint8_t bit = value / 100 == 0 ? (value / 10 == 0 ? (1) : (2)) : (3);
    if (addComma == true)
    {
        gOutputBuffer[offset + bit ] = ',';
        bit++;
    }
    return offset + bit;
}


void app_cmd_uart_print(char * data)
{
    app_uart_send(&gUartAt, data, strlen((const char *)data), 0);
//    printf("%s", data);
}

void app_cmd_uart_print2(char * data,uint8_t len)
{
    app_uart_send(&gUartAt, data, len, 0);
//    printf("%s", data);

}


void app_cmd_uart_print_raw(char * data, uint16_t length)
{
    app_uart_send(&gUartAt, data, length, 0);
//    printf("%s", data);
}

void app_cmd_print_end()
{
    app_cmd_uart_print("\r\n");
}

void app_cmd_print_ok()
{
    //printf("OK\r\n");
    app_cmd_uart_print("<<<<<");
    app_cmd_uart_print("OK");
    app_cmd_print_end();
    app_cmd_print_end();
}



void app_cmd_print_error(app_error id)
{
    app_cmd_uart_print("<<<<<");
    switch (id)
    {
        case ERR_FAIL:
            app_cmd_uart_print("FAIL");
            break;
        case ERR_INVALID:
            app_cmd_uart_print("INVALID");
            break;
        case ERR_PARA_COUNT:
            app_cmd_uart_print("PARA_COUNT");
            break;
        default:
            app_cmd_uart_print("NOT SUPPORT");
            break;

    }

    app_cmd_print_end();
    app_cmd_print_end();
}

void app_cmd_print_response(uint8_t *rsp, uint8_t length)
{
    app_cmd_uart_print("<<<<<");
    app_cmd_uart_print2((char *)rsp,length);
    app_cmd_print_end();
}

void app_cmd_print_raw_data(uint8_t *data, uint8_t length)
{
    app_cmd_uart_print_raw((char *)data,length);

}

void app_cmd_print_at_raw_data(uint8_t id, uint8_t *data, uint8_t length)
{

    uint8_t offset = app_cmd_make_response_header(ATCMD_DATA, ATCMD_LEN_DATA);//+DATA=
    offset = app_cmd_append_int(offset, id, true);
    offset = app_cmd_append_int(offset, length, true);
    offset = app_cmd_append_string(offset, data, length, false);
    app_cmd_print_response(gOutputBuffer, offset);
}

//Soft reset, disconnect all the connection, reset to default setting
bool app_cmd_reset()
{
    app_cmd_print_ok();
    app_reset();
    app_nv_reset();
    //sonata_dynamic_memory_buffer_free();
    sonata_ble_stack_reset();
//    NVIC_SystemReset();
    return true;
}

bool app_cmd_reboot()
{
    app_cmd_print_ok();
    app_reset();
    sonata_ble_stack_reset();
//    NVIC_SystemReset();
    return true;
}

extern const char app_version[];
bool app_cmd_version()
{
    APP_TRC("CMD: %s,app_version=%s, len=%d\r\n", __FUNCTION__, app_version, 24);
    uint8_t headLen = app_cmd_make_response_header(ATCMD_VERSION, ATCMD_LEN_VERSION);
    memcpy(gOutputBuffer + headLen, app_version, 24);
    app_cmd_print_response(gOutputBuffer, headLen + 24);
    app_cmd_print_ok();
    return true;
}


bool app_cmd_laddr(bool isSet)
{
    if (isSet)
    {
        //Input is AT+LADDR=11:22:33:44:55:66
        uint8_t inputAddr[SONATA_GAP_BD_ADDR_LEN] = {0};
        macChar2Value(&gInputBuffer[9], inputAddr);
        sonata_set_bt_address(inputAddr, SONATA_GAP_BD_ADDR_LEN);
        app_cmd_print_ok();
        //app_reset();
        //sonata_ble_stack_reset();
        //NVIC_SystemReset();
    }
    else
    {
        //Input is AT+LADDR
        //Output is +LADDR=11:22:33:44:55:66
        uint8_t *local_addr = sonata_get_bt_address();
        APP_TRC("CMD: %s,Addr=", __FUNCTION__);
        for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
        { APP_TRC("%02X ", local_addr[i]); }
        APP_TRC("\r\n");
        uint8_t headLen = app_cmd_make_response_header(ATCMD_LADDR, ATCMD_LEN_LADDR);
        macValue2Char(local_addr, &gOutputBuffer[headLen], true);
        app_cmd_print_response(gOutputBuffer, 24);
        app_cmd_print_ok();
    }

    return true;
}

bool app_cmd_name(bool isSet)
{
    if (isSet)
    {
        //AT+NAME=XXXXX
        uint8_t *name = getParameter(PARA0);
        uint8_t nameLen = getParameterLength(PARA0);

        if (nameLen > APPNV_LEN_LOCAL_DEVICE_NAME)
        {
            nameLen = APPNV_LEN_LOCAL_DEVICE_NAME;
        }
        APP_TRC("CMD: %s, name=", __FUNCTION__);
        for (int i = 0; i < nameLen; ++i)
        {
            APP_TRC("%c", name[i]);
        }
        APP_TRC("\r\n");

        if (app_nv_set_local_device_name(name,nameLen))
        {
            app_cmd_print_ok();
        }
        else
        {
            app_cmd_print_error(ERR_FAIL);
        }
    }
    else
    {
        //AT+NAME, +NAME=XXXX
        uint8_t usedLen = app_cmd_make_response_header(ATCMD_NAME, ATCMD_LEN_NAME);
        uint8_t localName[APPNV_LEN_LOCAL_DEVICE_NAME] = {0};
        app_nv_get_local_device_name(localName, APPNV_LEN_LOCAL_DEVICE_NAME, true);
        APP_TRC("CMD: %s,name=%s,len=%d\r\n", __FUNCTION__, localName + 1, localName[0]);
        usedLen = app_cmd_append_string(usedLen, localName + 1, localName[0], false);
        app_cmd_print_response(gOutputBuffer, usedLen);
        app_cmd_print_ok();
    }
    return true;

}

void app_cmd_init_uart_at()
{
    #if (CFG_PLF_SONATA)
    gUartAt.port = UART_PORT;
    gUartAt.config.baud_rate = UART_BAUDRATE_115200;
    gUartAt.config.data_width = DATA_8BIT;
    gUartAt.config.parity = PARITY_NO;
    gUartAt.config.stop_bits = STOP_1BIT;
    gUartAt.config.flow_control = FLOW_CTRL_DISABLED;
    gUartAt.config.mode = TX_RX_MODE;
    gUartAt.priv = NULL;
    asr_uart_init(&gUartAt);
    #else
    extern app_uart_dev_t lega_at_uart;
    memcpy(&gUartAt, &lega_at_uart, sizeof(app_uart_dev_t));
    #endif
}

bool app_cmd_baud(bool isSet)
{
    if (isSet)
    {
        //AT+BAUD=X (0:4800; 1:9600; 2:19200; 3:24800; 4:57600; 5:115200; 6:230400 )
        uint8_t *baud = gInputBuffer + (ATCMD_LEN_AT_PLUS + ATCMD_LEN_BAUD + ATCMD_LEN_EQUAL);
        if (app_nv_set_uart_rate(baud))
        {
            app_cmd_print_ok();
            //app_cmd_baud_apply();
        }
        else
        {
            app_cmd_print_error(ERR_FAIL);
        }
    }
    else
    {
        uint8_t usedLen = app_cmd_make_response_header(ATCMD_BAUD, ATCMD_LEN_BAUD);
        uint8_t baud[1] = {'5'};
        app_nv_get_uart_rate(baud, true);
        APP_TRC("CMD: %s,baud=%c,[0:4800; 1:9600; 2:19200; 3:24800; 4:57600; 5:115200; 6:230400]\r\n", __FUNCTION__,baud[0]);
        usedLen = app_cmd_append_string(usedLen, baud, 1, false);
        app_cmd_print_response(gOutputBuffer, usedLen);
        app_cmd_print_ok();
    }
    return true;

}

bool app_cmd_uuid(bool isSet)
{
    if (isSet)
    {
        uint8_t *uuidPoint = gInputBuffer + (ATCMD_LEN_AT_PLUS + ATCMD_LEN_UUID + ATCMD_LEN_EQUAL);
        if(app_nv_set_uuid(uuidPoint, APPNV_LEN_DTS_UUID))
        {
            if (app_att_is_db_filled())//If ATT DB filled, show att hdl directly
            {
                for (int i = 0; i < gCmdEnv.pInfo.count; ++i)
                {
                    uint8_t usedLen = 4;
                    uint8_t uuid[2] = {0};
                    uuidChar2Value(getParameter(PARA0 + i), uuid, true);
                    att_db_item *item = app_att_db_search((uuid[1] << 8) + uuid[0]);
                    if (item != NULL)
                    {
                        memcpy(gOutputBuffer, getParameter(PARA0 + i), usedLen);
                        usedLen = app_cmd_append_comma(usedLen);
                        usedLen = app_cmd_append_int(usedLen, item->hdl, false);
                        app_cmd_print_response(gOutputBuffer, usedLen);
                    }
                }
            }
            app_cmd_print_ok();
        }
        else
        {
            app_cmd_print_error(ERR_FAIL);
        }
    }
    else
    {
        //+UUID=XXXX,XXXX,XXXX,XXXX
        uint8_t usedLen = app_cmd_make_response_header(ATCMD_UUID, ATCMD_LEN_UUID);
        uint8_t uuid[APPNV_LEN_DTS_UUID] = {0};
        app_nv_get_uuid(uuid, APPNV_LEN_DTS_UUID, true);
        APP_TRC("CMD: %s,UUID=%s,\r\n", __FUNCTION__,uuid);
        usedLen = app_cmd_append_string(usedLen, uuid, APPNV_LEN_DTS_UUID, false);
        app_cmd_print_response(gOutputBuffer, usedLen);
        app_cmd_print_ok();
    }
    return true;

}

bool app_cmd_pin(bool isSet)
{
    #if (USE_APP_SEC)
    if (isSet)
    {
        if (getParameterLength(PARA0) > 6)
        {
            return false;
        }
        uint8_t *pin = getParameter(PARA0);
        uint8_t pinStr[6] = {'0','0','0','0','0','0'};
        memcpy(pinStr, pin, getParameterLength(PARA0));
        uint32_t pin_code =
                  (pinStr[0] - '0') * 100000
                + (pinStr[1] - '0') * 10000
                + (pinStr[2] - '0') * 1000
                + (pinStr[3] - '0') * 100
                + (pinStr[4] - '0') * 10
                + (pinStr[5] - '0') * 1;
        app_sec_user_send_pin_code(pin_code);
        if(app_nv_set_pin(pinStr, APPNV_LEN_PIN))
        {
            app_cmd_print_ok();
        }
        else
        {
            app_cmd_print_error(ERR_FAIL);
        }
    }
    else
    {
        uint8_t usedLen = app_cmd_make_response_header(ATCMD_PIN, ATCMD_LEN_PIN);
        uint8_t pin[APPNV_LEN_PIN + 1] = {'0','0','0','0','0','0',0};
        app_nv_get_pin(pin, APPNV_LEN_PIN, true);
        APP_TRC("CMD: %s,pin=%s,\r\n", __FUNCTION__,pin);
        usedLen = app_cmd_append_string(usedLen, pin, strlen((char *)pin), false);
        app_cmd_print_response(gOutputBuffer, usedLen);
        app_cmd_print_ok();
    }
    #endif
    return true;

}

bool app_cmd_wlist(bool isSet)
{
    if (isSet)
    {
        uint8_t size = getParameterValue(PARA0);
        sonata_gap_bdaddr_t *mac = getTempBuffer(size * sizeof(sonata_gap_bdaddr_t));
        if (mac == NULL)
        {
            app_cmd_print_error(ERR_FAIL);
            return true;
        }
        for (int i = 0; i < size; ++i)
        {
            if (i < getParameterCount()-1)
            {
                uint8_t *targetAddr = getParameter(i + 1);
                mac[i].addr_type = SONATA_ADDR_PUBLIC;
                macChar2Value(targetAddr, mac[i].addr.addr);//Change MAC form BN to LN
            }
            else
            {
                mac[i].addr_type = SONATA_ADDR_PUBLIC;
                memset(mac[i].addr.addr, '4', 6);
                mac[i].addr.addr[0] = util_rand_byte();
            }

        }
        for (int i = 0; i < size; ++i)
        {
            APP_TRC("CMD:     %02d, addr:%02X %02X %02X %02X %02X %02X\r\n", i,
                    mac[i].addr.addr[0], mac[i].addr.addr[1],
                    mac[i].addr.addr[2], mac[i].addr.addr[3],
                    mac[i].addr.addr[4], mac[i].addr.addr[5]);
        }
        sonata_ble_gap_set_white_list(size,mac);
    }
    else
    {
        sonata_ble_gap_get_dev_info(SONATA_GET_WLIST_SIZE);
    }
    return true;
}

bool app_cmd_pallist(bool isSet)
{
    if (isSet)
    {
        uint8_t size = getParameterValue(PARA0);

        sonata_gap_per_adv_bdaddr_t *info = getTempBuffer(size * sizeof(sonata_gap_per_adv_bdaddr_t));
        if (info == NULL)
        {
            app_cmd_print_error(ERR_FAIL);
            return true;
        }
        for (int i = 0; i < size; ++i)
        {
            if (i < getParameterCount()-1)
            {
                uint8_t *targetAddr = getParameter(i + 1);
                info[i].addr_type = SONATA_ADDR_PUBLIC;
                info[i].adv_sid = i;
                macChar2Value(targetAddr, info[i].addr);//Change MAC form BN to LN
            }
            else
            {
                info[i].addr_type = SONATA_ADDR_PUBLIC;
                info[i].adv_sid = i;
                memset(info[i].addr, '4', 6);
                info[i].addr[0] = util_rand_byte();
            }

        }
        for (int i = 0; i < size; ++i)
        {
            APP_TRC("CMD:     %02d, addr:%02X %02X %02X %02X %02X %02X\r\n", i,
                    info[i].addr[0], info[i].addr[1],
                    info[i].addr[2], info[i].addr[3],
                    info[i].addr[4], info[i].addr[5]);
        }
        sonata_ble_gap_set_periodic_advertiser_list(size, info);
    }
    else
    {
        sonata_ble_gap_get_dev_info(SONATA_GET_PAL_SIZE);
    }
    return true;
}

bool app_cmd_devinfo()
{
    uint8_t type = getParameterValue(PARA0);
    if (type == 0)
    {
        uint8_t nextInfo = SONATA_GET_DEV_ADV_TX_POWER;
        APP_TRC("CMD: next info:%02X(x)\r\n", nextInfo);
        sonata_ble_gap_get_dev_info(nextInfo);
    }
    else
    {
        sonata_ble_gap_get_dev_info(type);
    }

    return true;

}

void app_cmd_devinfo_done_handler(uint8_t currentInfo)
{
    if (gCmdEnv.cmdId != ID_DEVINFO)
    {
        return;
    }
    if (getParameterValue(PARA0) != 0)
    {
        return;
    }
    uint8_t nextInfo = 0;
    switch (currentInfo)
    {
        case SONATA_GET_DEV_VERSION               :// = 0x10,
            nextInfo = SONATA_GET_DEV_BDADDR;
            break;
        case SONATA_GET_DEV_BDADDR                :// = 0x11,
            nextInfo = SONATA_GET_DEV_ADV_TX_POWER;
            break;
        case SONATA_GET_DEV_ADV_TX_POWER          :// = 0x12,
            nextInfo = SONATA_GET_ANTENNA_INFO;
            break;
        case SONATA_GET_ANTENNA_INFO              :// = 0x14,
            nextInfo = SONATA_DBG_GET_MEM_INFO;
            break;
        case SONATA_DBG_GET_MEM_INFO              :// = 0x15,
            nextInfo = SONATA_GET_SUGGESTED_DFLT_LE_DATA_LEN;
            break;
        case SONATA_GET_SUGGESTED_DFLT_LE_DATA_LEN:// = 0x16,
            nextInfo = SONATA_GET_MAX_LE_DATA_LEN;
            break;
        case SONATA_GET_MAX_LE_DATA_LEN           :// = 0x17,
            nextInfo = SONATA_GET_NB_ADV_SETS;
            break;
        case SONATA_GET_NB_ADV_SETS               :// = 0x18,
            nextInfo = SONATA_GET_MAX_LE_ADV_DATA_LEN;
            break;
        case SONATA_GET_MAX_LE_ADV_DATA_LEN       :// = 0x19,
            nextInfo = SONATA_GET_DEV_TX_PWR;
            break;
        case SONATA_GET_DEV_TX_PWR                :// = 0x1A,
            nextInfo = SONATA_GET_DEV_RF_PATH_COMP;
            break;
        case SONATA_GET_DEV_RF_PATH_COMP          :// = 0x1B,
            break;
    }
    APP_TRC("CMD: Next info:%02X\r\n", nextInfo);
    sonata_ble_gap_get_dev_info(nextInfo);
}

bool app_cmd_timer_handler(uint16_t id)
{
    if (id == APP_TIMER_CMD1)
    {
        APP_TRC_HIGH("CMD: APP_TIMER_CMD1 timeout, continue\r\n");
        sonata_api_app_timer_set(id, gCount);
    }
    else if (id == APP_TIMER_CMD2)
    {
        APP_TRC_HIGH("CMD: APP_TIMER_CMD2 timeout\r\n");
    }
    else if (id == APP_TIMER_OOB)
    {
        app_set_send_oob_via_dts(0);
    }
    else if (id == APP_TIMER_MCONN)
    {
        app_cmd_mconn_continue();
    }
    return true;
}

bool app_cmd_timer()
{
    uint8_t id = getParameterValue(PARA0);
    uint8_t config = getParameterValue(PARA1);
    uint32_t delay = getParameterValueLong(PARA2);

    if (config == 0)
    {
        sonata_api_app_timer_clear(id);
        return true;
    }
    if (delay > SONATA_TIMER_DELAY_MAX)
    {
        APP_TRC("CMD: delay should <=SONATA_TIMER_DELAY_MAX(%ld)\r\n", (unsigned long )SONATA_TIMER_DELAY_MAX);
    }
    else if (id == APP_TIMER_CMD1)   //Endless timer;
    {
        gCount = delay;
        APP_TRC_HIGH("CMD: APP_TIMER_CMD1 set, delay=%ld\r\n", delay);
        sonata_api_app_timer_set(id, delay);
    }
    else if (id == APP_TIMER_CMD2)   //Normal timer
    {
        APP_TRC_HIGH("CMD: APP_TIMER_CMD2 set, delay=%ld\r\n", delay);
        sonata_api_app_timer_set(id, delay);
    }
    else if (id == APP_TIMER_CMD3)   //Long timer
    {
        APP_TRC_HIGH("CMD: APP_TIMER_CMD3 set, delay=-1\r\n");
        sonata_api_app_timer_set(id, -1);
    }
    return true;
}

bool app_cmd_oobdata()
{

    uint8_t type = getParameterValue(PARA0);

    if (type == 1) //OOB Confirm
    {
        uint8_t *inData = getParameter(PARA1);
        uint8_t oobC[16] = {0};
        keyChar2Value(inData, oobC, 16);
        APP_TRC("CMD  %s, OOB C:", __FUNCTION__);
        for (int i = 0; i < 16; ++i)
        {
            APP_TRC("%02X ", oobC[i]);
        }
        APP_TRC("\r\n");
        app_sec_set_oobdata(oobC, NULL);
    }
    else if (type == 2) // OOB Random
    {
        uint8_t *inData = getParameter(PARA1);
        uint8_t oobR[16] = {0};
        keyChar2Value(inData, oobR, 16);
        APP_TRC("CMD  %s, OOB R:", __FUNCTION__);
        for (int i = 0; i < 16; ++i)
        {
            APP_TRC("%02X ", oobR[i]);
        }
        APP_TRC("\r\n");
        app_sec_set_oobdata(NULL, oobR);
        app_sec_send_oob_bond_cfm(0);
    }
    else if (type == 3)
    {
        uint8_t *inData = getParameter(PARA1);
        uint8_t oob[16] = {0};
        keyChar2Value(inData, oob, 16);
        app_sec_set_oobdata(oob, NULL);

        inData = getParameter(PARA2);
        keyChar2Value(inData, oob, 16);
        app_sec_set_oobdata(NULL,oob);
        app_sec_send_oob_bond_cfm(0);
    }
    app_cmd_print_ok();
    return true;
}

bool app_cmd_mode(bool isSet)
{
    return true;
}

bool app_cmd_autoconn(bool isSet)
{
    if (isSet)
    {
        uint8_t autoconn[APPNV_LEN_AUTOCONN] = {0};
        uint8_t opt = getParameterValue(PARA0);
        if (opt == 1)
        {
            autoconn[0] = 1;
            if (getParameterCount() == 2)
            {
                uint8_t *targetAddr = getParameter(PARA1);
                macChar2Value(targetAddr, gTargetAddr);
                memcpy(autoconn + 1, gTargetAddr, SONATA_GAP_BD_ADDR_LEN);
            }
            printf(">>>: Auto:%d, Addr:%02X %02X %02X %02X %02X %02X\r\n", autoconn[0],
                   autoconn[1], autoconn[2], autoconn[3], autoconn[4], autoconn[5],autoconn[6]);
        }
        else if (getParameterCount() == 2)
        {
            uint8_t *targetAddr = getParameter(PARA1);
            macChar2Value(targetAddr, gTargetAddr);
            memcpy(autoconn + 1, gTargetAddr, SONATA_GAP_BD_ADDR_LEN);
        }
        if(app_nv_set_autoconn(autoconn, APPNV_LEN_AUTOCONN))
        {
            app_cmd_print_ok();
        }
        else
        {
            app_cmd_print_error(ERR_FAIL);
        }
    }
    else
    {
        uint8_t value[APPNV_LEN_AUTOCONN] = {0};
        app_nv_get_autoconn(value, APPNV_LEN_AUTOCONN);
        printf(">>>: Auto:%d, Addr:%02X %02X %02X %02X %02X %02X\r\n", value[0],
               value[1], value[2], value[3], value[4], value[5],value[6]);

    }
    return true;

}
bool app_cmd_autoadv(bool isSet)
{
    if (isSet)
    {
        uint8_t autoadv = getParameterValue(PARA0);
        if (app_nv_set_autoadv(autoadv))
        {
            app_cmd_print_ok();
        }
        else
        {
            app_cmd_print_error(ERR_FAIL);
        }
    }
    else
    {
        APP_TRC("ATUOADV=%d\r\n", app_nv_get_autoadv());
    }
    return true;
}

bool app_cmd_autontf()
{
    uint8_t value = getParameterValue(PARA0);  //0:stop,1:only NTF data;2:Master write NTF and then Slave NTF data
    if (value == TYPE_STOP)
    {
        app_nv_set_autontf(0);
        sonata_api_app_timer_clear(APP_TIMER_NTF_DATA);
    }
    else
    {
        uint16_t timer = getParameterValue(PARA1); //Timer;
        uint16_t dataLen = getParameterValue(PARA2); //Data length

        if (value == TYPE_AUTONTF_DUPLEX)
        {
            if (value != app_nv_get_autontf())
            {
                app_nv_set_autontf(value);
            }
            if (timer != app_nv_get_timer_value())
            {
                app_nv_set_timer(timer);
            }
            if (dataLen != app_nv_get_length_value())
            {
                app_nv_set_length(dataLen);
            }
            sonata_api_app_timer_clear(APP_TIMER_NTF_DATA);
            sonata_api_app_timer_set(APP_TIMER_NTF_DATA, timer);
        }
        else if (value == 3)
        {
            //Only send back some data to master
            memset(gOutputBuffer, 'S', dataLen);
            app_ble_slave_send_data_via_ntf(0, dataLen, gOutputBuffer, true);
        }
        else
        {
            if (app_nv_set_autontf(value) && app_nv_set_timer(timer) && app_nv_set_length(dataLen))
            {
                //Set DTS UUID as a target id
                app_nv_set_uuid((uint8_t *) DEFAULT_UUID, strlen(DEFAULT_UUID));
                app_cmd_print_ok();
            }
            else
            {
                app_cmd_print_error(ERR_FAIL);
            }
        }
    }
    return true;
}

bool app_cmd_advdata()
{
    uint8_t value = getParameterValue(PARA0);
    app_ble_update_adv_data(value) ;
    return true;
}


void app_cmd_ntf_switch_handler(uint8_t conidx, bool start)
{
    if (start == true)  //Client start NTF
    {
        uint8_t type = app_nv_get_autontf();
        if (type == TYPE_AUTONTF_PERIPHERAL_NTF)
        {
            //Start a timer to send data
            sonata_api_app_timer_set(APP_TIMER_NTF_DATA, 5000);//Start after 5S
        }
        else if (type == TYPE_AUTONTF_DUPLEX)
        {
            sonata_api_app_timer_clear(APP_TIMER_NTF_DATA);
            sonata_api_app_timer_set(APP_TIMER_NTF_DATA, app_nv_get_timer_value());
        }
    }
    else
    {
        sonata_api_app_timer_clear(APP_TIMER_NTF_DATA);
    }
}

void app_cmd_ntf_timer_handler()
{
    if (app_nv_get_autontf() == 0)
    {
        return;
    }

    uint16_t len = app_nv_get_length_value();
    if (len > APP_MTU)
    {
        len = APP_MTU;
    }
    uint8_t *data = getAllTempBuffer(APP_MTU);
    memcpy(data, sonata_get_bt_address(), SONATA_BD_ADDR_LEN);
    app_ble_slave_send_data_via_ntf(0, len, data, true);

}


bool app_cmd_scan()
{
    //0:Stop  1:Start
    uint8_t value = getParameterValue(PARA0);
//    if (getParameterCount() == 2)
//    {
//        uint8_t *targetAddr = getParameter(PARA1);
//        macChar2Value(targetAddr, gTargetAddr, searchChar(targetAddr, getParameterLength(PARA1), ':'));
//        APP_TRC("AAA: %s, addr:%02X %02X %02X %02X %02X %02X\r\n", __FUNCTION__,
//                gTargetAddr[0], gTargetAddr[1], gTargetAddr[2], gTargetAddr[3], gTargetAddr[4], gTargetAddr[5]);
//
//    }
//
    if (value == TYPE_STOP)//Stop scan
    {
        if (app_cmd_check_contex_scan(false) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't SCAN\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        sonata_ble_stop_scanning();
    }
    else if (value == TYPE_START)//Start scan
    {
        if (app_cmd_check_contex_scan(true) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't SCAN\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        app_ble_config_scanning(SONATA_GAP_SCAN_TYPE_GEN_DISC, NULL);
    }
    else if (value == TYPE_SCAN_START_WLIST) //Enable Whitelist
    {
        app_ble_config_scanning(SONATA_GAP_SCAN_TYPE_SEL_OBSERVER, NULL);
    }
    else if (value == TYPE_SCAN_START_WITH_WD_INTV)//type 3, should add scan_wd and scan_intv param as PARAM1 and PARAM2,see @app_cmd_update_scan_param
    {
        if (getParameterCount() == 3)
        {
            app_ble_config_scanning(SONATA_GAP_SCAN_TYPE_GEN_DISC, NULL);
        }
        else
        {
            APP_TRC_HIGH("CMD: %s, Should add scan_wd and scan_intv param as PARAM1 and PARAM2\r\n", __FUNCTION__);
            return false;
        }
    }
    else if (value == TYPE_SCAN_CONTINUOUSSCAN)
    {
        app_ble_config_scanning(SONATA_GAP_SCAN_TYPE_OBSERVER, NULL);
    }
    return true;

}

void app_cmd_update_scan_param(sonata_gap_scan_param_t * param)
{
    if (gCmdEnv.cmdId == ID_SCAN)
    {
        if (getParameterValue(PARA0) == TYPE_SCAN_START_WITH_WD_INTV)//Set up scan parameter
        {
            param->scan_param_1m.scan_wd = getParameterValue(PARA1);
            param->scan_param_1m.scan_intv = getParameterValue(PARA2);
            APP_TRC("CMD: %s, scan_wd=%d, scan_intv=%d\r\n", __FUNCTION__,param->scan_param_1m.scan_wd,param->scan_param_1m.scan_intv);
        }
    }
}

void app_cmd_scan_start()
{
    uint8_t len = sizeof(ATRSP_SCAN_START);
    memcpy(gOutputBuffer, ATRSP_SCAN_START, len);
    app_cmd_print_response(gOutputBuffer, len);
}


void app_cmd_scan_end()
{
    uint8_t len = sizeof(ATRSP_SCAN_END);
    memcpy(gOutputBuffer, ATRSP_SCAN_END, len);
    app_cmd_print_response(gOutputBuffer, len);
}


void app_cmd_scan_result_handler(uint8_t *addr, uint8_t addrType, uint8_t *report, uint16_t reportLen)
{
    if (app_command_get_cmdid() != ID_SCAN)
    {
        return;
    }
    if (getParameterCount() == 2)
    {
        if (addrCompare(addr, true, gTargetAddr, true) != true)
        {
            return;
        }
    }
    //MAC,TYPE,NAME
    uint8_t offset = 0;
    uint8_t name[SONATA_GAP_MAX_NAME_SIZE] = {0};
    uint16_t relNameLen = 0;
    sonata_get_adv_report_info(SONATA_GAP_AD_TYPE_COMPLETE_NAME, name, SONATA_GAP_MAX_NAME_SIZE, report, reportLen, &relNameLen);
    offset += macValue2Char(addr, &gOutputBuffer[0], true);
    gOutputBuffer[offset] = ',';
    offset += 1;
    gOutputBuffer[offset] = '0' + addrType;
    offset += 1;
    gOutputBuffer[offset] = ',';
    offset += 1;
    memcpy(gOutputBuffer + offset, name, relNameLen);
    offset += relNameLen;
    app_cmd_print_response(gOutputBuffer, offset);
}



bool app_cmd_clearactive()
{
    sonata_ble_stop_all_active();
    return true;
}

void app_cmd_active_clear_handler(uint16_t opt)
{
    if (gCmdEnv.cmdId != ID_CLEARACTIVE)
    {
        return;
    }
    if (opt == SONATA_GAP_CMP_STOP_ALL_ACTIVITIES)
    {
        sonata_ble_delete_all_active();
    }
    else if (SONATA_GAP_CMP_DELETE_ALL_ACTIVITIES)
    {
        app_cmd_print_ok();
    }
}

void app_cmd_conn_start(uint8_t conIdx)
{
    APP_TRC("CMD: %s \r\n", __FUNCTION__);

    uint8_t offset = app_cmd_make_response_header(ATRSP_GATTSTAT, ATRSP_LEN_GATTSTAT);
    uint8_t result[3] = {'0',',','2'};//2 means connecting
    result[0] = '0' + conIdx;
    app_cmd_append_string(offset, result, 3, false);
    app_cmd_print_response(gOutputBuffer, offset + 3);
}

void app_cmd_conn_done(uint8_t conIdx)
{
    APP_TRC("CMD: %s \r\n", __FUNCTION__);
    uint8_t offset = app_cmd_make_response_header(ATRSP_GATTSTAT, ATRSP_LEN_GATTSTAT);
    uint8_t result[3] = {'0',',','3'}; //3 means connected
    result[0] = '0' + conIdx;
    app_cmd_append_string(offset, result, 3, false);
    app_cmd_print_response(gOutputBuffer, offset + 3);
}



bool app_cmd_conn()
{
    uint8_t type = getParameterValue(PARA0);
    if (type == TYPE_STOP)
    {
        app_ble_stop_initiating();
    }
    else if (type == TYPE_START)
    {
        uint8_t *targetAddr = getParameter(PARA1);
        macChar2Value(targetAddr, gTargetAddr);
        APP_TRC("CMD: %s gTargetAddr=%02X %02X %02X %02X %02X %02X \r\n", __FUNCTION__,
                gTargetAddr[0],gTargetAddr[1],gTargetAddr[2],gTargetAddr[3],gTargetAddr[4],gTargetAddr[5]);
        app_ble_config_initiating(SONATA_GAP_INIT_TYPE_DIRECT_CONN_EST, gTargetAddr);
    }
    else if (type == TYPE_CONN_WLIST)
    {
        app_ble_config_initiating(SONATA_GAP_INIT_TYPE_AUTO_CONN_EST, NULL);
    }

    return true;

}

bool app_cmd_mconn(bool isSet)
{
    if (isSet)
    {
        if (gMconn[0].act == U8_INVALID && gMconn[0].conidx == U8_INVALID && gMconn[0].addr[0] != U8_INVALID)
        {
            addrCopy(gTargetAddr, gMconn[0].addr);
            APP_TRC_HIGH("CMD: %s gTargetAddr=%02X %02X %02X %02X %02X %02X \r\n", __FUNCTION__,
                         gTargetAddr[0],gTargetAddr[1],gTargetAddr[2],gTargetAddr[3],gTargetAddr[4],gTargetAddr[5]);
            app_ble_config_initiating(SONATA_GAP_INIT_TYPE_DIRECT_CONN_EST, gTargetAddr);
        }
        return true;
    }
    else
    {
        //Print MConn information
        for (int i = 0; i < MCONN_COUNT; ++i)
        {
            if (gMconn[i].addr[0] != U8_INVALID)
            {
                uint8_t offset = 0;
                offset = macValue2Char(gMconn[i].addr, &gOutputBuffer[0], true);
                gOutputBuffer[offset] = ',';
                offset += 1;
                gOutputBuffer[offset] = ((gMconn[i].act == U8_INVALID) ? '-' : (gMconn[i].act + '0'));
                offset += 1;
                gOutputBuffer[offset] = ',';
                offset += 1;
                gOutputBuffer[offset] = ((gMconn[i].conidx == U8_INVALID) ? '-' : (gMconn[i].conidx + '0'));
                offset += 1;
                app_cmd_print_response(gOutputBuffer, offset);
            }
        }
        app_cmd_print_ok();
        return true;
    }
}

bool app_cmd_mconn_continue()
{
//    for (int i = 0; i < MCONN_COUNT; ++i)
//    {
//        APP_TRC_HIGH("APP: conidx=%02X, act=%02X addr[%d]:%02X %02X %02X %02X %02X %02X\r\n",gMconn[i].conidx, gMconn[i].act, i,
//                     gMconn[i].addr[0], gMconn[i].addr[1], gMconn[i].addr[2], gMconn[i].addr[3], gMconn[i].addr[4], gMconn[i].addr[5]);
//    }
    for (int i = 0; i < MCONN_COUNT; ++i)
    {
        if (gMconn[i].act == U8_INVALID && gMconn[i].conidx == U8_INVALID && gMconn[i].addr[0] != U8_INVALID)
        {
            addrCopy(gTargetAddr, gMconn[i].addr);
            APP_TRC_HIGH("----------------------------------------------\r\n");
            APP_TRC_HIGH("CMD: %s gTargetAddr=%02X %02X %02X %02X %02X %02X \r\n", __FUNCTION__,
                    gTargetAddr[0],gTargetAddr[1],gTargetAddr[2],gTargetAddr[3],gTargetAddr[4],gTargetAddr[5]);
            app_ble_config_initiating(SONATA_GAP_INIT_TYPE_DIRECT_CONN_EST, gTargetAddr);
            break;
        }
    }

    return true;

}

void app_cmd_mconn_update_status(uint8_t act, uint8_t conidx, uint8_t *addr)
{
    if (gCmdEnv.cmdId != ID_MCONN)
    {
        return;
    }
    for (int i = 0; i < MCONN_COUNT; ++i)
    {
        if (memcmp(gMconn[i].addr, addr, SONATA_GAP_BD_ADDR_LEN) == 0)
        {
            if (act != U8_INVALID)//Dont care U8_INVALID
            {
                gMconn[i].act = act;
            }
            if (conidx != U8_INVALID)
            {
                gMconn[i].conidx = conidx;
            }
        }
    }

}

void app_cmd_conn_result_handler(uint8_t conidx)
{
    uint8_t result[3] = {'0', ',', '2'};
    uint8_t offset = app_cmd_make_response_header(ATRSP_GATTSTAT, ATRSP_LEN_GATTSTAT);
    result[0] = '0' + conidx;
    app_cmd_append_string(offset, result, 3, false);
    app_cmd_print_response(gOutputBuffer, offset + 3);
}

bool app_cmd_check_context_chinfo()
{
    actives *act = app_get_active();
    for (int i = 0; i < APP_ACTIVE_MAX; ++i)
    {
        if (act[i].runing == true)
        {
            return true;
        }
    }
    return false;
}

bool app_cmd_chinfo()
{
    if (app_cmd_check_context_chinfo() == false)
    {
        APP_TRC("CMD: %s,No chinfo\r\n", __FUNCTION__);
        app_cmd_print_error(ERR_INVALID);
        return false;
    }
    actives *act = app_get_active();
    for (int i = 0; i < APP_ACTIVE_MAX; ++i)
    {
        if (act[i].runing == true)
        {
            APP_TRC_HIGH("CMD: [%d], indxe=%d, addr:%02X %02X %02X %02X %02X %02X\r\n", i,act[i].assign_id,
                   act[i].peer[0], act[i].peer[1], act[i].peer[2], act[i].peer[3], act[i].peer[4], act[i].peer[5]);
        }
    }
    return true;

}
//bool app_cmd_discovery(int argc, char **argv)
//{
//    uint8_t conidx = char2HexValue(argv[PARA_ID_1][0]);
//    sonata_ble_gatt_disc_all_svc(conidx);//--->app_gatt_disc_svc_callback()
//    return CONFIG_OK;
//}
bool app_cmd_lesend()
{
    //AT+LESEND=0,10,0123456789
    uint8_t conIdx = getParameterValue(PARA0);

    if (app_cmd_check_contex_lesend(conIdx) == false)
    {
        APP_TRC_HIGH("CMD: %s, Contex error, can't LESEND\r\n", __FUNCTION__);
        app_cmd_print_error(ERR_FAIL);
        return false;
    }
    uint16_t dataLen = string2Value(gCmdEnv.pInfo.para[PARA1].data, gCmdEnv.pInfo.para[PARA1].length);
    APP_TRC("CMD: %s,conIdx=%d, dataLen=%d,data=%s\r\n", __FUNCTION__, conIdx, dataLen, gCmdEnv.pInfo.para[PARA1].data);
    app_ble_master_write_data(conIdx, dataLen, gCmdEnv.pInfo.para[PARA2].data);
    return true;

}

bool app_cmd_ntfsend()
{
    //AT+NTFDATA=0,10,0123456789
    uint8_t conIdx = getParameterValue(PARA0);

    if (app_cmd_check_contex_lesend(conIdx) == false)
    {
        APP_TRC_HIGH("CMD: %s, Contex error, can't NTFSEND\r\n", __FUNCTION__);
        app_cmd_print_error(ERR_FAIL);
        return false;
    }
    uint16_t dataLen = getParameterValue(PARA1);
    if (gCmdEnv.pInfo.count == 3) //Data is in AT command
    {
        APP_TRC("CMD: %s,conIdx=%d, dataLen=%d,data=%s\r\n", __FUNCTION__, conIdx, dataLen, gCmdEnv.pInfo.para[PARA1].data);
        app_ble_slave_send_data_via_ntf(conIdx, dataLen, gCmdEnv.pInfo.para[PARA2].data, true);
    }
    else //Data is not in AT commnad, send random data
    {
        APP_TRC("CMD: %s,conIdx=%d, dataLen=%d,data=RANDOM\r\n", __FUNCTION__, conIdx, dataLen);
        app_ble_slave_send_data_via_ntf(conIdx, dataLen, gInputBuffer, true);
    }

    return true;

}

bool app_cmd_data()//Todo
{
    //+DATA=0,10,0123456789
    return true;

}

bool app_cmd_ledisc() //Todo
{
    uint8_t idx = getParameterValue(PARA0);
    uint8_t reason = getParameterValue(PARA1);
    app_ble_disconnect(idx, reason);
    return true;
}

void app_cmd_ledisc_done(uint8_t conidx)
{
    if (gCmdEnv.cmdId != ID_LEDISC)
    {
        return;
    }

    uint8_t offset = app_cmd_make_response_header(ATRSP_GATTSTAT, ATRSP_LEN_GATTSTAT);
    uint8_t result[3] = {'0', ',', '1'};
    result[0] = conidx + '0';
    app_cmd_append_string(offset, result, 3, false);
    app_cmd_print_response(gOutputBuffer, offset + 3);
    gCmdEnv.cmdId = 0;

}


bool app_cmd_ntf()
{
    uint8_t conidx = getParameterValue(PARA0);
    uint8_t hdl = getParameterValue(PARA1);
    uint8_t value = getParameterValue(PARA2);

    if (value == TYPE_STOP)//Stop ntf
    {
        if (app_ble_master_turn_ntf(conidx, hdl, false))
        {
            app_cmd_print_ok();
        }
    }
    else if (value == TYPE_START)//Start ntf
    {
        if(app_ble_master_turn_ntf(conidx, hdl, true))
        {
            app_cmd_print_ok();
        }
    }
    return true;

}


bool app_cmd_ind()
{
    uint8_t conidx = getParameterValue(PARA0);
    uint8_t hdl = getParameterValue(PARA1);
    uint8_t value = getParameterValue(PARA2);
    if (value == 0)//Stop ind
    {
        if (app_ble_master_turn_ind(conidx, hdl, false))
        {
            app_cmd_print_ok();
        }
    }
    else if (value == 1)//Start ind
    {
        if(app_ble_master_turn_ind(conidx, hdl, true))
        {
            app_cmd_print_ok();
        }
    }
    return true;

}

bool app_cmd_adv()
{
    uint8_t adv = getParameterValue(PARA0);
    if (getParameterCount() == 2)
    {
        uint8_t scanResp = getParameterValue(PARA1);
        app_set_adv_scan_response_data_flag(scanResp);
    }
    if (adv == 0)//Stop adv
    {
        if (app_cmd_check_contex_adv(false) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't ADV\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        app_ble_stop_advertising();
    }
    else if (adv == 1)//Start adv
    {
        if (app_cmd_check_contex_adv(true) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't ADV\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        app_ble_config_legacy_advertising();
    }
    return true;

}


bool app_cmd_directadv()
{
    uint8_t adv = getParameterValue(PARA0);
    if (adv == 0)//Stop
    {
        if (app_cmd_check_contex_adv(false) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't ADV\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        app_ble_stop_advertising();
    }
    else if (adv == 1)//Start
    {
        if (app_cmd_check_contex_adv(true) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't ADV\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        if (getParameterCount() == 4)
        {
            uint8_t highDuty = getParameterValue(PARA1);
            uint8_t addrType = getParameterValue(PARA2);
            uint8_t *targetAddr = getParameter(PARA3);
            macChar2Value(targetAddr, gTargetAddr);
            app_ble_config_direct_legacy_advertising(highDuty, addrType, gTargetAddr);
        }
    }
    return true;

}

bool app_cmd_peroidadv()
{
    uint8_t adv = getParameterValue(PARA0);
    if (adv == 0)//Stop
    {
        if (app_cmd_check_contex_adv(false) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't ADV\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        app_ble_stop_advertising();
    }
    else if (adv == 1)//Start
    {
        if (app_cmd_check_contex_adv(true) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't ADV\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        app_ble_config_periodic_advertising();
    }
    return true;

}

bool app_cmd_peroidsync()
{
    uint8_t value = getParameterValue(PARA0);
    if (value == TYPE_STOP)//Stop
    {
        app_ble_stop_period_sync();
    }
    else if (value == TYPE_START)//Start
    {
        uint8_t *targetAddr = getParameter(PARA1);
        macChar2Value(targetAddr, gTargetAddr);
        APP_TRC("CMD: %s gTargetAddr=%02X %02X %02X %02X %02X %02X \r\n", __FUNCTION__,
                gTargetAddr[0],gTargetAddr[1],gTargetAddr[2],gTargetAddr[3],gTargetAddr[4],gTargetAddr[5]);
        //Period sync must be start in parallel with a scan, here we start a scan and then start a peroid sync
        //Steps: config scan->start scan->config period->start peroid;
        app_ble_config_scanning(SONATA_GAP_SCAN_TYPE_GEN_DISC, gTargetAddr);
        //app_ble_config_period_sync();
    }
    else if (value == TYPE_PEROIDSYNC_START_WITH_PAL)//Start with PAL
    {
        //Period sync must be start in parallel with a scan, here we start a scan and then start a peroid sync
        //Steps: config scan->start scan->config period->start peroid;
        app_ble_config_scanning(SONATA_GAP_SCAN_TYPE_GEN_DISC, NULL);
        //app_ble_config_period_sync();
    }
    return true;

}

bool app_cmd_extadv()
{
    uint8_t adv = getParameterValue(PARA0);
    if (adv == 0)//Stop ntf
    {
        if (app_cmd_check_contex_adv(false) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't ADV\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        app_ble_stop_advertising();
    }
    else if (adv == 1)//Start ntf
    {
        if (app_cmd_check_contex_adv(true) == false)
        {
            APP_TRC_HIGH("CMD: %s, Contex error, can't ADV\r\n", __FUNCTION__);
            app_cmd_print_error(ERR_FAIL);
            return false;
        }
        app_ble_config_extended_advertising();
    }
    return true;

}

void app_cmd_adv_done()
{
    if (gCmdEnv.cmdId != ID_ADV)
    {
        return;
    }
    app_cmd_print_ok();
    gCmdEnv.cmdId = 0;

}

void app_cmd_mtu()
{
    uint8_t conidx = getParameterValue(PARA0);
    sonata_ble_gatt_exchange_mtu(conidx);
    app_cmd_print_ok();

}


void app_cmd_connparam()
{
    uint8_t conidx = getParameterValue(PARA0);
    uint16_t intv_min = getParameterValue(PARA1);
    uint16_t intv_max = getParameterValue(PARA2);
    uint16_t latency = getParameterValue(PARA3);
    uint16_t time_out = getParameterValue(PARA4);
    uint16_t ce_len_min = getParameterValue(PARA5);
    uint16_t ce_len_max = getParameterValue(PARA6);
    APP_TRC_HIGH("APP: %s  . int=%d,lat=%d,timeout=%d, ceMin=%d, cdMax=%d\r\n", __FUNCTION__,
                 intv_max, latency, time_out, ce_len_min, ce_len_max);

    if (((time_out * 10) < ((1 + latency) * ((intv_max * 5) >> 1))))
    {
        APP_TRC_ERROR("ERR, (time_out * 10) should large than ((1 + latency) * ((intv_max * 5) >> 1))");
        app_cmd_print_error(ERR_INVALID);
        return;
    }
    uint16_t ret = sonata_ble_gap_update_connection_params(conidx, 0, intv_min, intv_max, latency, time_out, ce_len_min, ce_len_max);
    if (ret == API_SUCCESS)
    {
        app_cmd_print_ok();
    }
    else
    {
        app_cmd_print_error(ERR_INVALID);
    }

}

void app_cmd_change_conn_param_for_throughput()
{
    uint16_t intv_min = 6;
    uint16_t intv_max = 6;
    uint16_t time_out = 200;
    uint16_t latency = 0;
    APP_TRC_HIGH("APP: %s  . int=%d,lat=%d,timeout=%d\r\n", __FUNCTION__, intv_max, latency, time_out);
    sonata_ble_gap_update_connection_params(0, 0, intv_min, intv_max, latency, time_out, 0xFFFF, 0xFFFF);
}

void app_cmd_send_throughput_data()
{
    uint8_t *data = getAllTempBuffer(APP_MTU - 3);
    //APP_TRC("T@%d\r\n", gCount);
    if (gCount > 0)
    {
        data[0] = (uint8_t) (gCount >> 8);
        data[1] = (uint8_t) gCount;
        app_ble_slave_send_data_via_ntf(0, APP_MTU - 3, data, true);//-->>app_command_handle_ntf_complete_ind
        gCount--;
    }
    else if (gCount == 0)
    {
        uint8_t end[1] = {'-'};
        app_ble_slave_send_data_via_ntf(0, 1, end, true);//-->>app_command_handle_ntf_complete_ind
        gCount--;
    }
    else
    {
        APP_TRC_HIGH("CMD:Throuthput End\r\n");
    }
}

void app_cmd_send_throughput_start()
{
    uint8_t end[1] = {'+'};
    app_ble_slave_send_data_via_ntf(0, 1, end, true);//-->>app_command_handle_ntf_complete_ind
    gCount--;
}

void app_command_handle_ntf_complete_ind()
{
    if (gCmdEnv.cmdId == ID_THROUGHPUT)
    {
        app_cmd_send_throughput_data();
    }
    else
    {
        uint8_t ntfValue = app_nv_get_autontf();
        if (ntfValue == 3 || ntfValue == 0)
        {
            //Do nothing
        }
        else if (ntfValue == 1 || ntfValue == 2)
        {
            sonata_api_app_timer_set(APP_TIMER_NTF_DATA, app_nv_get_timer_value());
        }
    }

}


void app_command_handle_gatt_event(uint8_t conidx, uint16_t handle, uint16_t type, uint16_t length, uint8_t *value)
{

    if (app_nv_get_autontf() == 2)
    {
        uint16_t len = app_nv_get_length_value();
        uint8_t *data = getAllTempBuffer(length);
        if (data == NULL)
        {
            data = getTempBuffer(50);
            len = 50;
        }
        memset(data, 'M', len);
        app_ble_master_write_data(conidx, len, data);
        printf("->%d\r\n", conidx);
        return;
    }
}

void app_command_handle_param_updated()
{
    APP_TRC("CMD: %s, \r\n", __FUNCTION__);
    if (gCmdEnv.cmdId == ID_THROUGHPUT)
    {
        app_cmd_send_throughput_start();
    }
}

void app_command_handle_le_pkt_size()
{
    APP_TRC("CMD: %s, \r\n", __FUNCTION__);
    if (gCmdEnv.cmdId == ID_THROUGHPUT)
    {
        //app_cmd_send_throughput_start();
    }
}


bool app_cmd_discovery()
{
    uint8_t conidx = getParameterValue(PARA0);
    uint8_t discType = getParameterValue(PARA1);
    uint8_t uuid[2] = {0};
    if (gCmdEnv.pInfo.count == 3)
    {
        uuidChar2Value(getParameter(PARA2), uuid, true);
    }
    switch (discType)
    {
        case 1://Only servcie
            app_ble_discovery_svc(conidx, (gCmdEnv.pInfo.count == 3) ? uuid : NULL);
            break;
        case 2://Only char
            app_ble_discovery_char(conidx, (gCmdEnv.pInfo.count == 3) ? uuid : NULL);
            break;
        case 3://desc and char
            app_ble_discovery_desc(conidx);
            break;
        case 0: //All
            app_ble_discovery_svc(conidx, NULL);
            app_ble_discovery_char(conidx, NULL);
            app_ble_discovery_desc(conidx);
            break;
    }
    return true;

}

bool app_cmd_discovery_handler(uint8_t status)
{
    if (status == 0)
    {
        app_cmd_print_ok();
    }
    else
    {
        APP_TRC("CMD: %s, UUID not found\r\n", __FUNCTION__);
        uint8_t usedLen = app_cmd_make_response_header(ATCMD_DISCOVERY, ATCMD_LEN_DISCOVERY);
        uint8_t rsp[] = "Not Found";
        usedLen = app_cmd_append_string(usedLen,rsp, sizeof (rsp), false);
        app_cmd_print_response(gOutputBuffer, usedLen);
    }
    return true;
}

bool app_cmd_atthdl()
{
    if (gCmdEnv.pInfo.count == 0) //Print all DB
    {
        att_db_item *db = app_att_db_get();
        for (int i = 0; i < APP_ATT_DB_ITEM_MAX; ++i)
        {
            uint16_t usedLen = 0;
            if (db[i].uuid == 0)
            {
                continue;
            }
            else if (db[i].uuid == 0X2800) //Service
            {
                sprintf((char *) gOutputBuffer, "ATT: [%04d] UUID:%04X SVC:%04X START:%04d END:%04d\r\n",
                        i, db[i].uuid, db[i].svc_uuid, db[i].start, db[i].end);
                usedLen = 51;
            }
            else if (db[i].uuid == 0X2803) //Char
            {
                sprintf((char *) gOutputBuffer,"     [%04d] UUID:%04X PROP:%02X\r\n", i, db[i].uuid, db[i].prop);
                usedLen = 30;
            }
            else
            {
                sprintf((char *) gOutputBuffer,"     [%04d] UUID:%04X \r\n", i, db[i].uuid);
                usedLen = 22;
            }
            app_cmd_print_response(gOutputBuffer, usedLen);

        }
    }
    else if (gCmdEnv.pInfo.count == 1 || gCmdEnv.pInfo.count == 2) //Search Char's HDL
    {
        uint8_t uuid[2] = {0};
        uuidChar2Value(getParameter(PARA0), uuid, true);
        att_db_item *db = app_att_db_get();
        uint16_t uuidValue = (uuid[1] << 8) + uuid[0];
        uint16_t hdl = 0;
        for (int i = 0; i < APP_ATT_DB_ITEM_MAX; ++i)
        {
            if (db[i].uuid == uuidValue)
            {
                hdl = i;
            }
        }
        if (gCmdEnv.pInfo.count == 2)
        {
            for (int i = hdl; i < APP_ATT_DB_ITEM_MAX; ++i)
            {
                if (db[i].uuid == 0X2902)
                {
                    hdl = i;
                    break;
                }
            }
        }
        uint8_t usedLen = app_cmd_make_response_header(ATCMD_ATTHDL, ATCMD_LEN_ATTHDL);
        if (hdl == 0)
        {
            uint8_t rsp[] = "Not Found";
            usedLen = app_cmd_append_string(usedLen, rsp, sizeof(rsp), false);
        }
        else
        {
            usedLen = app_cmd_append_int(usedLen, hdl, false);
        }
        app_cmd_print_response(gOutputBuffer, usedLen);
    }

    return true;

}

bool app_cmd_dle()
{
    uint8_t conidx = getParameterValue(PARA0);
    sonata_ble_gap_set_le_pkt_size(conidx, 251, 2120);
    app_cmd_print_ok();
    return true;
}



bool app_cmd_log()
{
//    uint8_t value = getParameterValue(PARA0);
//    gLog = (value == 1);
//    app_cmd_print_ok();
//    gTimeStamp = sonata_get_sys_time();
//    sonata_api_app_timer_set(APP_TIMER_TEST, 5000);
    return true;
}


bool app_cmd_txpwr(bool isSet)
{
    //Support value are:
    //    -43,-38,-33,-31,-30, --->(-70~-90)
    //    -25,-20,             --->(-60~-63)
    //    -19,-16,-13,-10,     --->(-50~-56)
    //    -8,-6,-4,-5,-3,-1,0, --->(-43~-48)
    //    2,4,6,7,8,9,10,      --->(-28~-38)
    bool ret = false;
    int8_t supportValue[] = {-43, -38, -33, -31, -30, -25, -20, -19, -16, -13, -10, -8, -6, -4, -5, -3, -1, 0, 2, 4, 6, 7, 8, 9, 10};
    if (isSet)
    {

        int8_t value = getParameterValue(PARA0);
        for (int i = 0; i < sizeof(supportValue); ++i)
        {
            if (value == supportValue[i])
            {
                ret = sonata_ll_set_txpwr(value);
                break;
            }
        }
        APP_TRC("CMD: %s, value=%d, result=%d\r\n", __FUNCTION__, value, ret);
        if (ret == false)
        {
            app_cmd_print_error(ERR_INVALID);
        }
        return true;
    }
    return true;

}
bool app_cmd_bondclear()
{
    app_nv_set_bonded(false);
    #if (USE_APP_SEC)
    app_set_clear_bond_info();
    app_bond = false;
    #endif
    return true;
}

bool app_cmd_bond()
{
    #if (USE_APP_SEC)
    uint8_t conidx = getParameterValue(PARA0);
    app_sec_bond(conidx);
    #endif
    return true;
}

bool app_cmd_bondtype(uint8_t isSet)
{
    if (isSet)
    {
        uint8_t type = getParameterValue(PARA0);
        app_nv_set_bond_type(type);
    }
    {
        uint8_t type = app_nv_get_bond_type_value();
        uint8_t len = app_cmd_make_response_header(ATCMD_BONDTYPE, ATCMD_LEN_BONDTYPE);
        len = app_cmd_append_int(len, type, true);
        uint8_t rsp[] = "\n\t0:JustWork\n\t1:Pass Key MASTER_GEN_SLAVE_INPUT\n\t2:Pass Key MASTER_INPUT_SLAVE_GEN"
                        "\n\t3:Number compare\n\t4:OOB Legacy\n\t5:OOB Security\n\t6:OOB Security(No OOB Data)(Remote should set to Type 5)"
                        "\n\t7:JustWork SC\n\t10:Custom Test";
        len = app_cmd_append_string(len, rsp, sizeof(rsp), false);
        app_cmd_print_response(gOutputBuffer, len);
    }
    return true;
}


extern uint16_t sonata_get_chip_year(void);
extern uint8_t sonata_get_chip_month(void);
extern uint8_t sonata_get_chip_day(void);
bool app_cmd_chipinfo()
{
    #ifdef SONATA_CFG_EFUSE
    char *info = "Info:[Y:%04d,M:%02d,D:%02d,V:%02d]";
    sonata_efuse_init(EFUSE_LDO25_CLOSE);
    char *pTemp = getTempBuffer(40);
    sprintf(pTemp, info, sonata_get_chip_year(), sonata_get_chip_month(), sonata_get_chip_day(), sonata_get_chip_version(0));
    printf("\r\n>>>>>%s\r\n", pTemp);
    #endif
    return true;
}

bool app_cmd_efuseread(bool isSet)
{
    #ifdef SONATA_CFG_EFUSE
    sonata_efuse_init(EFUSE_LDO25_CLOSE);
    uint8_t addr = char2HexValue(getParameter(PARA0)[2]) * 16 + char2HexValue(getParameter(PARA0)[3]);
    uint8_t value = sonata_efuse_byte_read(addr);
    APP_TRC(">>>>>Addr:%02X, Value=0x%02X(X), %d(d)\r\n", addr, value, value);
    #endif
    return true;
}


bool app_cmd_efusewrite(bool isSet)
{
    if (isSet == false)
    {
        gCount = 0xEEEE;
        #ifdef SONATA_CFG_EFUSE
        sonata_efuse_init(EFUSE_LDO25_OPEN);
        #endif
        APP_TRC_HIGH(">>>>>EFUSE is open, use AT+EFUSEWRITE=0xXX,0xXX again\r\n");
        return true;
    }
    if (gCount == 0xEEEE)
    {
        #ifdef SONATA_CFG_EFUSE
        uint8_t addr = char2HexValue(getParameter(PARA0)[2]) * 16 + char2HexValue(getParameter(PARA0)[3]);
        uint8_t value =  char2HexValue(getParameter(PARA1)[2]) * 16 + char2HexValue(getParameter(PARA1)[3]);;
        APP_TRC(">>>>>Addr:%02X, Value=0x%02X(X), %d(d)\r\n", addr, value, value);
        sonata_efuse_byte_write(addr, value);
        gCount = 0;
        #endif
    }
    else
    {
        APP_TRC_HIGH(">>>>>To Keep Efuse safe, use AT+EFUSEWRITE at first\r\n");
    }
    return true;
}

bool app_cmd_appconfig(bool isSet)
{
//    if (isSet == false)
//    {
//        app_config_ext_t config = {0};
//        app_nv_get_app_config(&config);
//        APP_TRC("APP: Type=%d, Enable=%d, para=0x%04x(%d(D)), \r\n",
//                config.type, config.enable, (uint16_t) config.param.gatt_attr_cfg, (uint16_t) config.param.gatt_attr_cfg);
//    }
//    else
//    {
//        uint8_t type = getParameterValue(PARA0);
//        uint8_t enable = getParameterValue(PARA1);
//        uint32_t value = string2HexValue(getParameter(PARA2), getParameterLength(PARA2));
//        app_config_t config = {0};
//        switch (type)
//        {
//            case APP_CFG_NONE:
//                break;
//            case APP_CFG_COMPANY_ID:
//                config.company_id = value;
//                break;
//            case APP_CFG_MANUF_NAME:
//                config.manuf_name = value;
//                break;
//            case APP_CFG_GAP_ATTR_FLAG :
//                config.gap_attr_cfg = value;
//                break;
//            case APP_CFG_GATT_ATTR_FLAG:
//                config.gatt_attr_cfg = value;
//                break;
//            default:
//                return false;
//        }
//        app_nv_set_app_config(type, enable, &config);
//    }
    return true;
}

bool app_cmd_hltrans(bool isSet)
{
    uint8_t enable = 0;
    if (isSet == false)
    {
        enable = app_nv_get_hl_trans_value();
        APP_TRC_HIGH("APP: HL_TRANS=%d, \r\n",enable);
    }
    else
    {
        enable = getParameterValue(PARA0);
        if (app_nv_set_hl_trans(enable))
        {
            app_cmd_print_ok();
        }
        else
        {
            app_cmd_print_error(ERR_FAIL);
        }
    }
    return true;
}

bool app_cmd_addtarget(bool isSet)
{
    if (isSet == false)
    {
        for (int i = 0; i < MCONN_COUNT; ++i)
        {
            APP_TRC_HIGH("APP: conidx=%02X, act=%02X addr[%d]:%02X %02X %02X %02X %02X %02X\r\n",gMconn[i].conidx, gMconn[i].act, i,
                         gMconn[i].addr[0], gMconn[i].addr[1], gMconn[i].addr[2], gMconn[i].addr[3], gMconn[i].addr[4], gMconn[i].addr[5]);
        }
    }
    else
    {
        uint8_t opt = getParameterValue(PARA0);
        if (opt == 0)
        {
            //Clear Address
            memset(gMconn, 0, sizeof(gMconn));
            return true;
        }
        opt = opt > getParameterCount() - 1 ? getParameterCount() - 1 : opt;
        //Add address
        for (int i = 0; i < MCONN_COUNT; ++i)
        {
            if (gMconn[i].act == 0)
            {
                if (opt > MCONN_COUNT - i)
                {
                    opt = MCONN_COUNT - i;
                }
                for (int j = 0; j < opt; ++j)
                {
                    gMconn[i + j].conidx = U8_INVALID;
                    gMconn[i + j].act = U8_INVALID;
                    uint8_t *targetAddr = getParameter(PARA1 + j);
                    macChar2Value(targetAddr, gMconn[i + j].addr);//Change MAC form BN to LN
                }
                break;
            }
        }
        for (int i = 0; i < MCONN_COUNT; ++i)
        {
            APP_TRC_HIGH("APP: conidx=%02X, act=%02X addr[%d]:%02X %02X %02X %02X %02X %02X\r\n",gMconn[i].conidx, gMconn[i].act, i,
                   gMconn[i].addr[0], gMconn[i].addr[1], gMconn[i].addr[2], gMconn[i].addr[3], gMconn[i].addr[4], gMconn[i].addr[5]);
        }
    }
    return true;
}



bool app_cmd_reg(bool isSet)
{
//    #define APP_REG_RD(addr)              (*(volatile uint32_t *)(addr))
//    if (isSet)
//    {
//        uint8_t type = getParameterValue(PARA0);
//        if (type == 0) //Read -> AT+REG=0,62004C00
//        {
//            //uint32_t reg = strtoul((char *)getParameter(PARA1), 0, 16);
//            uint32_t reg = string2ValueU32(getParameter(PARA1),8);
//            util_reg_read(reg);
//        }
//
//        else if (type == 1) //Write -> AT+REG=1,62004C00,F3333390
//        {
//            //uint32_t reg = strtoul((char *)getParameter(PARA1), 0, 16);
//            //uint32_t value = strtoul((char *)getParameter(PARA2), 0, 16);
//            uint32_t reg = string2ValueU32(getParameter(PARA1),8);
//            uint32_t value = string2ValueU32(getParameter(PARA2),8);
//            util_reg_write(reg, value);
//        }
//    }
//    else
//    {
//        util_reg_write(0x60C0B03C,0xF3333390);
//        util_reg_write(0x40000908,0x20040208);
//        util_reg_write(0x62004C00,0x0008B200);
//    }
    return true;
}

void app_cmd_print_txtone_pattern(uint8_t ch, uint8_t patternType)
{
    printf("----Tone: CH = %d, PatternType=", ch);
    switch (patternType)
    {
        case SONATA_PAYL_PSEUDO_RAND_9:
            printf("SONATA_PAYL_PSEUDO_RAND_9----\r\n");
            break;
        case SONATA_PAYL_11110000:
            printf("SONATA_PAYL_11110000----\r\n");
            break;
        case SONATA_PAYL_10101010:
            printf("SONATA_PAYL_10101010----\r\n");
            break;
        case SONATA_PAYL_PSEUDO_RAND_15:
            printf("SONATA_PAYL_PSEUDO_RAND_15----\r\n");
            break;
        case SONATA_PAYL_ALL_1:
            printf("SONATA_PAYL_ALL_1----\r\n");
            break;
        case SONATA_PAYL_ALL_0:
            printf("SONATA_PAYL_ALL_0----\r\n");
            break;
        case SONATA_PAYL_00001111:
            printf("SONATA_PAYL_00001111----\r\n");
            break;
        case SONATA_PAYL_01010101:
            printf("SONATA_PAYL_01010101----\r\n");
            break;
    }
}

bool app_cmd_txtone(bool isSet)
{
    if (isSet)
    {
        uint8_t type = getParameterValue(PARA0);
        if (type == 0) //Stop
        {
            sonata_ll_rf_test_end();
            APP_TRC("APP: %s ,RF Test End \r\n", __FUNCTION__);
        }
        else if (type == 1) //Start, P1: Channel;
        {
            uint16_t ch = getParameterValueLong(PARA1) - 2400; //Should be 2402~2480
            printf("APP: %s ,ch=%d,\r\n", __FUNCTION__,ch);
            if ((ch <= 80) && (ch >= 02))
            {
                APP_TRC("----Tone: CH = %d---- \r\n", ch);
                sonata_ll_rf_tx_tone(ch);
            }
            else
            {
                APP_TRC("----Tone: CH Use default 37@2402---- \r\n");
                sonata_ll_rf_tx_tone(2);  //CH 37, 2402
            }
        }
        else if (type == 2) //Start, P1: Channel; P2: Pattern
        {
            uint16_t ch = getParameterValueLong(PARA1) - 2400; //Should be 2402~2480
            uint8_t patternType = getParameterValue(PARA2); //See @sonata_tone_pattern
            if ((ch <= 80) && (ch >= 02) && (patternType >= SONATA_PAYL_PSEUDO_RAND_9) && (patternType <= SONATA_PAYL_01010101))
            {
                app_cmd_print_txtone_pattern(ch, patternType);
                sonata_ll_rf_tx_test(ch, patternType);
            }
            else
            {
                APP_TRC("----Tone: CH Use default 2402, patternType=SONATA_PAYL_10101010---- \r\n");
                sonata_ll_rf_tx_test(2, 2);
            }
        }
    }
    else
    {
        for (int i = 0; i <= SONATA_PAYL_01010101; ++i)
        {
            app_cmd_print_txtone_pattern(0, i);
        }
    }
    return true;
}


bool app_cmd_gattread()
{
    uint8_t conidx = getParameterValue(PARA0);
    uint16_t hdl = getParameterValue(PARA1);
    app_ble_master_read_data(conidx, hdl);
    return true;

}


bool app_cmd_bondinfo()
{
    #if (USE_APP_SEC)
    app_sec_print_bond_info();
    #endif
    return true;

}

bool app_cmd_encrypt()
{
    #if (USE_APP_SEC)
    uint8_t addr[SONATA_GAP_BD_ADDR_LEN] = {0};
    uint8_t conidx = getParameterValue(PARA0);
    uint8_t *targetAddr = getParameter(PARA1);
    macChar2Value(targetAddr, addr);
    bonded_dev_info_t *bondedDevInfo = app_sec_get_bond_device_info(addr);
    if (bondedDevInfo != NULL)
    {
        uint16_t ediv = bondedDevInfo->ltk.ediv;
        uint8_t *random = bondedDevInfo->ltk.randnb;
        uint8_t key_size = SONATA_GAP_SMP_MAX_ENC_SIZE_LEN;
        uint8_t *ltk = bondedDevInfo->ltk.ltk;
        sonata_ble_gap_start_encrypt(conidx, ediv, random, key_size, ltk);
    }
    else
    {
        app_cmd_print_error(ERR_INVALID);
    }
    #endif
    return true;
}

bool app_cmd_ncompare()
{
    #if (USE_APP_SEC)
    uint8_t conidx = getParameterValue(PARA0);
    uint8_t value = getParameterValue(PARA1);
    app_sec_passkey_send_yesno(conidx, value);
    #endif
    return true;
}

bool app_cmd_genaddr()
{
    uint8_t type = getParameterValue(PARA0);
    uint8_t *randIn = getParameter(PARA1);
    switch (type)
    {
        case 0:
            type = SONATA_BD_ADDR_STATIC;
            break;
        case 1:
            type = SONATA_BD_ADDR_NON_RSLV;
            break;
        case 2:
            type = SONATA_BD_ADDR_RSLV;
            break;
        default:
            APP_TRC("CMD: %s,type=%d, Need to be 0/1/2\r\n", __FUNCTION__, type);

            return false;
    }

    uint8_t randValue[3] = {0};
    randValue[0] = char2HexValue((char)randIn[0]) * 16 + char2HexValue((char)randIn[1]);
    randValue[1] = char2HexValue((char)randIn[2]) * 16 + char2HexValue((char)randIn[3]);
    randValue[2] = char2HexValue((char)randIn[4]) * 16 + char2HexValue((char)randIn[5]);
    APP_TRC("CMD: %s,type=%02X[X], Rand:%02X,%02X,%02X\r\n", __FUNCTION__, type, randValue[0],randValue[1],randValue[2]);
    sonata_ble_gap_generate_random_address(type, randValue);
    return true;
}



/*!
 * @brief
 * Demo Addr and IRK
 *     Command: AT+RESOLVADDR=61:0E:00:4B:66:DE  [Input MAC following Android phone display order]
 *              [app_loc_irk should be {0x8D,0x8F,0x76,0x9A,0x7A,0x5B,0xB4,0xBD,0x4C,0xD9,0xA7,0x5D,0xF9,0x17,0x7D,0x80} ]
 *     Result:
 *        APP_CB: app_sec_address_resolved_callback, addr:DE 66 4B 00 0E 61
 *        APP_CB  app_sec_address_resolved_callback, Key:8D 8F 76 9A 7A 5B B4 BD 4C D9 A7 5D F9 17 7D 80
 * @return
 */
bool app_cmd_resolvaddr()
{
    #if (USE_APP_SEC)
    uint8_t inputAddr[SONATA_GAP_BD_ADDR_LEN] = {0};
    macChar2Value(getParameter(PARA0), inputAddr);
    if ((inputAddr[SONATA_BD_ADDR_LEN - 1] & SONATA_BD_ADDR_RND_ADDR_TYPE_MSK) != SONATA_BD_ADDR_RSLV)
    {
        APP_TRC_ERROR("APP: %s ,Addr can not resolve, \r\n", __FUNCTION__);
        return false;
    }
    app_sec_resolve_address(inputAddr);
    #endif
    return true;
}

bool app_cmd_setirk()
{
    #if (USE_APP_SEC)
    if (gCmdEnv.pInfo.count == 0)
    {
        app_sec_set_local_irk(NULL) ;//Use default IRK
        return true;
    }
    uint8_t *key = getParameter(PARA0);
    app_sec_set_local_irk(key);
    #endif
    return true;
}


bool app_cmd_rallist(bool isSet)
{
    if (isSet)
    {
        //APP_TRC("CMD: size should less than %d,\r\n", SONATA_CFG_RAL);
        uint8_t size = getParameterValue(PARA0);
        sonata_gap_ral_dev_info_t *info = getAllTempBuffer(size * sizeof(sonata_gap_ral_dev_info_t));
        if (info == NULL)
        {
            APP_TRC("ERR: %s, Buffer need %d\r\n", __FUNCTION__,size * sizeof(sonata_gap_ral_dev_info_t));
            app_cmd_print_error(ERR_FAIL);
            return true;
        }
        for (int i = 0; i < size; ++i)
        {
            if (i < getParameterCount()-1)
            {
                uint8_t *targetAddr = getParameter(i + 1);
                macChar2Value(targetAddr, info[i].addr.addr.addr);//Change MAC form BN to LN
                info[i].addr.addr_type = (info[i].addr.addr.addr[5] & 0xC0) ? SONATA_ADDR_RAND : SONATA_ADDR_PUBLIC;
                info[i].priv_mode = SONATA_PRIV_TYPE_NETWORK;
                gapKeyCopy(info[i].local_irk, app_sec_get_local_irk());
                gapKeyCopy(info[i].peer_irk, app_sec_get_local_irk());
            }
            else
            {
                memset(info[i].addr.addr.addr, '4', 6);
                info[i].addr.addr.addr[0] = util_rand_byte();
                info[i].addr.addr_type = SONATA_ADDR_PUBLIC;
                info[i].priv_mode = SONATA_PRIV_TYPE_NETWORK;
                gapKeyCopy(info[i].local_irk, app_sec_get_local_irk());
                gapKeyCopy(info[i].peer_irk, app_sec_get_local_irk());
            }
        }
        for (int i = 0; i < size; ++i)
        {
            APP_TRC("CMD:     %02d, addr:%02X %02X %02X %02X %02X %02X\r\n", i,
                    info[i].addr.addr.addr[0], info[i].addr.addr.addr[1],
                    info[i].addr.addr.addr[2], info[i].addr.addr.addr[3],
                    info[i].addr.addr.addr[4], info[i].addr.addr.addr[5]);
        }
        sonata_ble_gap_set_resolving_list(size, info);

    }
    else
    {
        APP_TRC("CMD: %s, Get Ral Size\r\n", __FUNCTION__);
        sonata_ble_gap_get_dev_info(SONATA_GET_RAL_SIZE);
    }
    return true;
}

bool app_cmd_getral()
{
    APP_TRC("CMD: %s, Get Ral Size\r\n", __FUNCTION__);
    sonata_ble_gap_get_dev_info(SONATA_GET_RAL_SIZE);
    uint8_t *addr = getParameter(PARA0);
    uint8_t inputAddr[SONATA_GAP_BD_ADDR_LEN] = {0};
    addrCopy(inputAddr, addr);
    macReverse(inputAddr);
    APP_TRC("CMD: %s, Get Local Ral address\r\n", __FUNCTION__);
    sonata_ble_gap_get_local_real_address(SONATA_ADDR_RAND, inputAddr);
    APP_TRC("CMD: %s, Get Peer Ral address\r\n", __FUNCTION__);
    sonata_ble_gap_get_peer_real_address(SONATA_ADDR_RAND, inputAddr);
    return true;
}

bool app_cmd_svc_change()
{
    uint8_t conidx = getParameterValue(PARA0);
    uint16_t handle = getParameterValue(PARA1);
    uint16_t start_handle = getParameterValue(PARA2);
    uint16_t end_handle = getParameterValue(PARA3);
    APP_TRC("CMD: %s, >sonata_ble_gatt_send_service_changed_event\r\n", __FUNCTION__);
    sonata_ble_gatt_send_service_changed_event(conidx, handle, start_handle, end_handle);
    return true;

}

bool app_cmd_throughput()
{
    #if APP_DBG
    APP_TRC(">>>>>>>>>>>>>>Log is on, throughput test result is not real<<<<<<<<<<<<<<<<<<<<<<\r\n");
    #endif
    app_nv_set_autontf(0);
    gCount = getParameterValue(PARA0);
    gCount += 1;//Will Send a Start flag
    APP_TRC_HIGH("CMD:Throuthput Start. Count=%ld,mtu=%d\r\n",gCount,dts_get_mtu());
    app_cmd_change_conn_param_for_throughput();//-->app_command_handle_param_updated()
    //app_cmd_send_throughput_start(); //--->app_cmd_send_throughput_data()
    return true;

}

CRITICAL_FUNC_SEG static void app_cmd_parse_parameters( app_cmd_id cmdId)
{
    //parse AT+XXX=YYY,ZZZ,WWW
    uint8_t index = app_cmd_get_index(cmdId);
    uint8_t start = ATCMD_LEN_AT_PLUS + cmdArray[index].cmdLen + ATCMD_LEN_EQUAL;
    uint16_t length = gInputCommandLength - (ATCMD_LEN_AT_PLUS + cmdArray[index].cmdLen + ATCMD_LEN_EQUAL);
    APP_TRC("CMD: %s, start=%d, length=%d\r\n", __FUNCTION__, start, length);
    parseParameter(&gInputBuffer[start], length, &gCmdEnv.pInfo);
    gCmdEnv.pOffset = start;
    printCommand();
}

CRITICAL_FUNC_SEG void app_cmd_data_adapter(uint16_t length, const uint8_t *value)
{
    APP_TRC_HIGH(">>>>>[%d]:", length);
    for (int i = 0; i < length; ++i){APP_TRC_HIGH("%c", value[i]);}APP_TRC_HIGH("\r\n");
    //1 Check basic AT command 1
    app_error err = app_cmd_is_at_command(length, value);
    if (err != ERR_NONE)
    {
        app_cmd_print_error(err);
        return;
    }
    if (length >= LEN_AT_INPUT)
    {
        APP_TRC_ERROR("APP: %s ,CMD length is %d, should less than %d, \r\n", __FUNCTION__, length, LEN_AT_INPUT);
        return;
    }
    //2 Back up
    app_cmd_backup(length, value);
    //3 Find ID
    gCmdEnv.cmdId = app_cmd_get_id(length);

    bool isSet = app_cmd_is_set(gCmdEnv.cmdId);
    //4 Parse parameters
    if (isSet)
    {
        app_cmd_parse_parameters(gCmdEnv.cmdId);
    }
    else
    {
        gCmdEnv.pOffset = 0;
        memset(&gCmdEnv.pInfo, 0, sizeof(param_info));
    }
    //5 Check Basic AT command  2
    err = app_cmd_check(gCmdEnv.cmdId,isSet);
    APP_TRC("CMD: %s, cmdID=%d,isSet=%d\r\n", __FUNCTION__, gCmdEnv.cmdId, isSet);
    if (err != ERR_NONE)
    {
        APP_TRC("CMD: %s, cmdID=%d,err=%d, FAIL\r\n", __FUNCTION__, gCmdEnv.cmdId, err);
        app_cmd_print_error(err);
        return;
    }
    switch (gCmdEnv.cmdId)
    {
        case ID_RESET:
            app_cmd_reset();
            break;
        case ID_REBOOT:
            app_cmd_reboot();
            break;
        case ID_VERSION:
            app_cmd_version();
            break;
        case ID_LADDR:
            app_cmd_laddr(isSet);
            break;
        case ID_NAME:
            app_cmd_name(isSet);
            break;
        case ID_BAUD:
            app_cmd_baud(isSet);
            break;
        case ID_UUID:
            app_cmd_uuid(isSet);
            break;
        case ID_PIN:
            app_cmd_pin(isSet);
            break;

        case ID_AUTOCONN:
            app_cmd_autoconn(isSet);
            break;
        case ID_SCAN:
            app_cmd_scan();
            break;
        case ID_CONN:
            app_cmd_conn();
            break;
        case ID_CHINFO:
            app_cmd_chinfo();
            break;
        case ID_LESEND:
            app_cmd_lesend();
            break;
        case ID_DATA:
            app_cmd_data();
            break;
        case ID_LEDISC:
            app_cmd_ledisc();
            break;
        case ID_ATTEST:
            app_cmd_print_ok();
            break;
        case ID_NTF:
            app_cmd_ntf();
            break;
        case ID_IND:
            app_cmd_ind();
            break;
        case ID_ADV:
            app_cmd_adv();
            break;
        case ID_THROUGHPUT:
            app_cmd_throughput();
            break;
        case ID_DISCOVERY:
            app_cmd_discovery();
            break;
        case ID_SVCCHANGE:
            app_cmd_svc_change();
            break;
        case ID_MCONN:
            app_cmd_mconn(isSet);
            break;
        case ID_NTFSEND:
            app_cmd_ntfsend();
            break;
        case ID_MTU:
            app_cmd_mtu();
            break;
        case ID_CONNPARAM:
            app_cmd_connparam();
            break;
        case ID_AUTOADV:
            app_cmd_autoadv(isSet);
            break;
        case ID_CLEARACTIVE:
            app_cmd_clearactive();
            break;
        case ID_EXTADV:
            app_cmd_extadv();
            break;
        case ID_ATTHDL:
            app_cmd_atthdl();
            break;
        case ID_DLE:
            app_cmd_dle();
            break;
        case ID_LOG:
            app_cmd_log();
            break;
        case ID_TXPWR:
            app_cmd_txpwr(isSet);
            break;
        case ID_BONDCLEAR:
            app_cmd_bondclear();
            break;
        case ID_BOND:
            app_cmd_bond();
            break;
        case ID_GATTREAD:
            app_cmd_gattread();
            break;
        case ID_BONDINFO:
            app_cmd_bondinfo();
            break;
        case ID_ENCRYPT:
            app_cmd_encrypt();
            break;
        case ID_NCOMPARE:
            app_cmd_ncompare();
            break;
        case ID_GENADDR:
            app_cmd_genaddr();
            break;
        case ID_RESOLVADDR:
            app_cmd_resolvaddr();
            break;
        case ID_SETIRK:
            app_cmd_setirk();
            break;
        case ID_RALLIST:
            app_cmd_rallist(isSet);
            break;
        case ID_GETRAL:
            app_cmd_getral();
            break;
        case ID_DIRECTADV:
            app_cmd_directadv();
            break;
        case ID_PERIODADV:
            app_cmd_peroidadv();
            break;
        case ID_PERIODSYNC:
            app_cmd_peroidsync();
            break;
        case ID_AUTONTF:
            app_cmd_autontf();
            break;
        case ID_ADVDATA:
            app_cmd_advdata();
            break;
        case ID_WLIST:
            app_cmd_wlist(isSet);
            break;
        case ID_PALLIST:
            app_cmd_pallist(isSet);
            break;
        case ID_DEVINFO:
            app_cmd_devinfo();
            break;
        case ID_TIMER:
            app_cmd_timer();
            break;
        case ID_OOBDATA:
            app_cmd_oobdata();
            break;
        case ID_BONDTYPE:
            app_cmd_bondtype(isSet);
            break;
        case ID_CHIPINFO:
            app_cmd_chipinfo();
            break;
        case ID_EFUSEREAD:
            app_cmd_efuseread(isSet);
            break;
        case ID_EFUSEWRITE:
            app_cmd_efusewrite(isSet);
            break;
        case ID_APPCONFIG:
            app_cmd_appconfig(isSet);
            break;
        case ID_HLTRANS:
            app_cmd_hltrans(isSet);
            break;
        case ID_ADDTARGET:
            app_cmd_addtarget(isSet);
            break;
        case ID_REG:
            app_cmd_reg(isSet);
            break;
        case ID_TXTONE:
            app_cmd_txtone(isSet);
            break;
        case ID_MODE://Not support
        default:
            APP_TRC("CMD: %s, No command to handle \r\n", __FUNCTION__);
            app_cmd_print_error(0);
            break;
    }
}



bool app_command_handle_peer_command( uint8_t *value, uint16_t length)
{
    #define PEER_CMD_HEADER "AT+PEER="
    #define PEER_CMD_HEADER_LEN    (8)
    if(strncmp((char *)value,PEER_CMD_HEADER,PEER_CMD_HEADER_LEN) == 0)
    {
        app_cmd_data_adapter(length - PEER_CMD_HEADER_LEN, value + PEER_CMD_HEADER_LEN);
        return true;
    }
    return false;

}

uint8_t app_command_get_cmdid()
{
    return gCmdEnv.cmdId;
}

uint8_t app_command_get_cmdtype()
{
    return getParameterValue(PARA0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                     At Command adapter for Combo platform
///////////////////////////////////////////////////////////////////////////////////////////////////

#if  defined(USE_LEGA_RTOS)

extern int init_ble_task(void);
extern int ble_close(void);
//static uint8_t gAtBle_open = 0;


uint16_t app_cmd_format_type1(int argc, char **argv)
{
    //ble_adv 1  TO: AT+ADV=1
    uint8_t offset = 0;
    memset(gFormatBuffer, 0, LEN_AT_DATA);
    gFormatBuffer[0] = 'A';
    gFormatBuffer[1] = 'T';
    gFormatBuffer[2] = '+';
    offset = 3;
    memcpy((gFormatBuffer + offset), (argv[0] + 4), (strlen(argv[0]) - 4));
    offset += (strlen(argv[0]) - 4);
    if (argc > 1)
    {
        gFormatBuffer[offset] = '=';
        offset += 1;
        for (int i = 1; i < argc; ++i)
        {
            memcpy(gFormatBuffer + offset, argv[i], strlen(argv[i]));
            offset += strlen(argv[i]);
            if (i + 1 != argc)
            {
                gFormatBuffer[offset] = ',';
                offset += 1;
            }
        }
    }
    APP_TRC_HIGH("CMD: %s ,Format:%s \r\n", __FUNCTION__, gFormatBuffer);
    return offset;

}

// Input "ble AT+AUTOADV=0", change to "AT+AUTOADV=0"
CRITICAL_FUNC_SEG uint16_t app_cmd_format_type2(int argc, char **argv)
{
    memset(gFormatBuffer, 0, LEN_AT_DATA);
    memcpy(gFormatBuffer , argv[1] , strlen(argv[1]));
    return strlen(argv[1]);
}

extern bool check_ble_task();
extern void atcmd_set_ble_open(uint8_t value);
extern uint8_t ble_open;  //@atcmd_ble.c


void app_cmd_set_ble_open(uint8_t value)
{
    ble_open = value;
}
static int app_cmd_ble_open(int argc, char **argv)
{
    if (check_ble_task() == false)
    {
        init_ble_task();
        atcmd_set_ble_open(1);
    }
    else
    {
        printf("CMD: ble is already open!\n");
    }
    return 0;
}
static int app_cmd_ble_close(int argc, char **argv)
{
    if (check_ble_task() == true)
    {
        atcmd_set_ble_open(0);
        ble_close();
    }
    else
    {
        printf("CMD: ble doesn't need to close!\n");
    }
    return 0;
}


CRITICAL_FUNC_SEG int app_cmd_ble(int argc, char **argv)
{
    if (argc == 2 && argv[1] != NULL)
    {
        if (check_ble_task() == false)
        {
            printf("***BLE Task Not Run***\r\n");
            return 0;
        }
        uint16_t length = strlen(argv[1]);
        memset(gFormatBuffer, 0, LEN_AT_DATA);
        memcpy(gFormatBuffer , argv[1] , length);
        if (0)
        {
            app_cmd_data_adapter(length, gFormatBuffer);
        }
        else
        {
            //Post a event to deal with the command
            gCmdUartData.length = length;
            gCmdUartData.data = gFormatBuffer;
            sonata_api_send_app_msg(APP_EVENT_UART_CMD, &gCmdUartData);
            //Next:app_uart_cmd_handler
        }
    }
    return 0;
}


//static int app_cmd_ble_adapter(int argc, char **argv)
//{
////    app_cmd_data_adapter(app_cmd_format_type1(argc, argv), gFormatBuffer);
//    app_cmd_data_adapter(app_cmd_format_type2(argc, argv), gFormatBuffer);
//    return 0;
//}

static app_cmd_entry at_ble_open  = {.name = "ble_open" , .function = app_cmd_ble_open , .help = "create ble task"   , };
static app_cmd_entry at_ble_close = {.name = "ble_close", .function = app_cmd_ble_close, .help = "close ble task"    , };
static app_cmd_entry at_ble       = {.name = "ble"      , .function = app_cmd_ble      , .help = "ble basic commands", };


void app_command_register_lega(void)
{
    printf("%s, SONATA AT Command on, \r\n",__FUNCTION__);
    app_at_cmd_register(&at_ble_open);
    app_at_cmd_register(&at_ble_close);
    app_at_cmd_register(&at_ble);
}

#endif //defined(USE_LEGA_RTOS)

