
/**
 ****************************************************************************************
 *
 * @file (app_nv.h)
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
#include "app_nv.h"
#include "app_utils.h"
#include "app_platform.h"


/*
 * MACRO DEFINES
 ****************************************************************************************
 */



/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void app_nv_load_default()
{
    app_nv_set_local_device_name((uint8_t *)DEFAULT_NAME, sizeof(DEFAULT_NAME));
    app_nv_set_uart_rate((uint8_t *)DEFAULT_BAUD);
    app_nv_set_uuid((uint8_t *) DEFAULT_UUID, strlen(DEFAULT_UUID));
    app_nv_set_mode((uint8_t *) DEFAULT_MODE);
    app_nv_set_pin((uint8_t *) DEFAULT_PIN, strlen(DEFAULT_PIN));
    app_nv_set_autoconn((uint8_t *) DEFAULT_AUTOCONN, strlen(DEFAULT_AUTOCONN));
}
void app_nv_reset()
{
    sonata_fs_erase(APPNV_TAG_PERIPH_BONDED);
    sonata_fs_erase(APPNV_TAG_MOUSE_NTF_CFG);
    sonata_fs_erase(APPNV_TAG_MOUSE_TIMEOUT);
    sonata_fs_erase(APPNV_TAG_PEER_BD_ADDRESS);
    sonata_fs_erase(APPNV_TAG_LTK);
    sonata_fs_erase(APPNV_TAG_LTK_IN);
    sonata_fs_erase(APPNV_TAG_IRK_ADDR);
    sonata_fs_erase(APPNV_TAG_IRK);
    sonata_fs_erase(APPNV_TAG_BONDED_DEV_INFO);
    sonata_fs_erase(APPNV_TAG_LOCAL_DEVICE_NAME);
    sonata_fs_erase(APPNV_TAG_DTS_UUID);
    sonata_fs_erase(APPNV_TAG_UART_RATE);
    sonata_fs_erase(APPNV_TAG_PIN);
    sonata_fs_erase(APPNV_TAG_MODE);
    sonata_fs_erase(APPNV_TAG_AUTOCONN);
    //app_nv_load_default();
}





bool app_nv_set_pair_on_boot(bool pair)
{
    uint8_t pairOnBoot = pair;
    if (sonata_fs_write(APPNV_TAG_PAIR_ON_BOOT, sizeof (uint8_t), (uint8_t *)&pairOnBoot) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_pair_on_boot()
{
    uint8_t pairOnBoot = 0;
    uint8_t length = sizeof (uint8_t);
    if (sonata_fs_read(APPNV_TAG_PAIR_ON_BOOT, &length, (uint8_t *) &pairOnBoot) == SONATA_FS_OK)
    {
        return pairOnBoot;
    }
    return false;
}


bool app_nv_set_bonded(bool bond)
{
    uint8_t value = bond;
    if (sonata_fs_write(APPNV_TAG_PERIPH_BONDED, sizeof (uint8_t), (uint8_t *) &value) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_bonded()
{
    uint8_t value = 0;
    uint8_t length = sizeof (uint8_t);

    if (sonata_fs_read(APPNV_TAG_PERIPH_BONDED, &length, (uint8_t *) &value) == SONATA_FS_OK)
    {
        return value;
    }
    return false;
}


bool app_nv_set_peer_bt_address(uint8_t *  addr)
{
    if (addr == NULL)
    {
        return false;
    }
    uint8_t value[SONATA_GAP_BD_ADDR_LEN];
    addrCopy(value, addr);

    if (sonata_fs_write(APPNV_TAG_PEER_BD_ADDRESS, APPNV_LEN_PEER_BD_ADDRESS, value) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_peer_bt_address(uint8_t *  addr)
{
    if (addr == NULL)
    {
        return false;
    }
    uint8_t length = APPNV_LEN_PEER_BD_ADDRESS;
    if (sonata_fs_read(APPNV_TAG_PEER_BD_ADDRESS, &length, addr) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}


bool app_nv_set_bonded_device_info(bonded_dev_info_list_t *dev_info)
{
    if (dev_info == NULL)
    {
        return false;
    }
    if (sonata_fs_write(APPNV_TAG_BONDED_DEV_INFO, sizeof (bonded_dev_info_list_t), (uint8_t *) dev_info) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_bonded_device_info(bonded_dev_info_list_t *dev_info)
{
    if (dev_info == NULL)
    {
        return false;
    }
    uint8_t length = sizeof (bonded_dev_info_list_t);
    if (sonata_fs_read(APPNV_TAG_BONDED_DEV_INFO, &length, (uint8_t *)dev_info) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}


bool app_nv_get_local_device_name(uint8_t *name, uint8_t bufLen, bool useDef)
{
    if (name == NULL)
    {
        return false;
    }
    if (bufLen != APPNV_LEN_LOCAL_DEVICE_NAME)
    {
        return false;
    }
    uint8_t length = APPNV_LEN_LOCAL_DEVICE_NAME;
    if (sonata_fs_read(APPNV_TAG_LOCAL_DEVICE_NAME, &length, name) == SONATA_FS_OK)
    {
        return true;
    }
    else
    {
        if (useDef)
        {
            uint8_t defLen = strlen(DEFAULT_NAME);
            if (defLen > (APPNV_LEN_LOCAL_DEVICE_NAME - 1))
            {
                defLen = APPNV_LEN_LOCAL_DEVICE_NAME - 1;
            }
            name[0] = defLen;
            memcpy(name + 1, DEFAULT_NAME, defLen);
            app_nv_set_local_device_name(name + 1, name[0]);
            return true;
        }
    }
    return false;
}

bool app_nv_set_local_device_name(uint8_t * name, uint8_t length)
{
    if (name == NULL)
    {
        return false;
    }
    if (length >= APPNV_LEN_LOCAL_DEVICE_NAME)
    {
        return false;
    }
    uint8_t newName[APPNV_LEN_LOCAL_DEVICE_NAME] = {0};
    newName[0] = length;
    memcpy(newName + 1, name, length);
    if (sonata_fs_write(APPNV_TAG_LOCAL_DEVICE_NAME, APPNV_LEN_LOCAL_DEVICE_NAME, newName) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_ltk(sonata_gap_ltk_t * key)
{
    if (key == NULL)
    {
        return false;
    }
    uint8_t length = APPNV_LEN_LTK;
    if (sonata_fs_read(APPNV_TAG_LTK, &length, (uint8_t*)key) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_set_ltk(sonata_gap_ltk_t * key)
{
    if (key == NULL)
    {
        return false;
    }
    if (sonata_fs_write(APPNV_TAG_LTK, APPNV_LEN_LTK, (uint8_t *)key) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_uuid(uint8_t *uuid, uint8_t bufLen, bool useDef)
{
    if (uuid == NULL)
    {
        return false;
    }
    if (bufLen != APPNV_LEN_DTS_UUID)
    {
        return false;
    }
    uint8_t length = APPNV_LEN_DTS_UUID;
    if (sonata_fs_read(APPNV_TAG_DTS_UUID, &length, (uint8_t*)uuid) == SONATA_FS_OK)
    {
        return true;
    }
    else
    {
        if (useDef)
        {
            memcpy(uuid, DEFAULT_UUID, sizeof(DEFAULT_UUID));
            return true;
        }
    }
    return false;
}

void app_nv_get_uuid_value(app_uuids *uuids)
{
    uint8_t customUuid[APPNV_LEN_DTS_UUID] = {0};
    app_nv_get_uuid(customUuid, APPNV_LEN_DTS_UUID, true);
    uint16_t d1 = char2HexValue(customUuid[0]) * 16 + char2HexValue(customUuid[1]);
    uint16_t d0 = char2HexValue(customUuid[2]) * 16 + char2HexValue(customUuid[3]);
    uuids->service = ((d1 * 0X100) + d0);

    d1 = char2HexValue(customUuid[5]) * 16 + char2HexValue(customUuid[6]);
    d0 = char2HexValue(customUuid[7]) * 16 + char2HexValue(customUuid[8]);
    uuids->read = ((d1 * 0X100) + d0);

    d1 = char2HexValue(customUuid[10]) * 16 + char2HexValue(customUuid[11]);
    d0 = char2HexValue(customUuid[12]) * 16 + char2HexValue(customUuid[13]);
    uuids->write = ((d1 * 0X100) + d0);

    d1 = char2HexValue(customUuid[15]) * 16 + char2HexValue(customUuid[16]);
    d0 = char2HexValue(customUuid[17]) * 16 + char2HexValue(customUuid[18]);
    uuids->ntf = ((d1 * 0X100) + d0);

}

uint16_t app_nv_get_dts_uuid_value()
{
    uint8_t customUuid[APPNV_LEN_DTS_UUID] = {0};
    app_nv_get_uuid(customUuid, APPNV_LEN_DTS_UUID, true);
    uint16_t d1 = char2HexValue(customUuid[0]) * 16 + char2HexValue(customUuid[1]);
    uint16_t d0 = char2HexValue(customUuid[2]) * 16 + char2HexValue(customUuid[3]);
    APP_TRC("NV :%s, d0=%02X, d1=%02X\r\n", __FUNCTION__, d0, d1);
    return ((d1 * 0X100) + d0);
}

bool app_nv_set_uuid(uint8_t *uuid, uint8_t bufLen)
{
    if (uuid == NULL)
    {
        return false;
    }
    if (bufLen != APPNV_LEN_DTS_UUID)
    {
        return false;
    }
    if (sonata_fs_write(APPNV_TAG_DTS_UUID, APPNV_LEN_DTS_UUID, (uint8_t *)uuid) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_uart_rate(uint8_t *rateId, bool useDef)
{
    if (rateId == NULL)
    {
        return false;
    }
    uint8_t length = sizeof (uint8_t);
    if (sonata_fs_read(APPNV_TAG_UART_RATE, &length, (uint8_t*)rateId) == SONATA_FS_OK)
    {
        return true;
    }
    else
    {
        if (useDef)
        {
            memcpy(rateId, DEFAULT_BAUD, sizeof (uint8_t));
            return true;
        }
    }
    return false;
}

uint32_t app_nv_get_uart_rate_value()
{
    uint8_t baud[1] = {'5'};
    app_nv_get_uart_rate(baud, true);

    uint8_t baudId = baud[0] - '0';
    uint32_t baudValue = 115200;
    switch (baudId)
    {
        case 0:
            baudValue = UART_BAUDRATE_4800;
            break;
        case 1:
            baudValue = UART_BAUDRATE_9600;
            break;
        case 2:
            baudValue = UART_BAUDRATE_19200;
            break;
        case 3:
            baudValue = UART_BAUDRATE_38400;
            break;
        case 4:
            baudValue = UART_BAUDRATE_57600;
            break;
        case 5:
            baudValue = UART_BAUDRATE_115200;
            break;
        case 6:
            baudValue = UART_BAUDRATE_230400;
            break;
        default:
            baudValue = UART_BAUDRATE_115200;
            break;
    }
    return baudValue;
}

bool app_nv_set_uart_rate(uint8_t *rateId)
{
    if (rateId == NULL)
    {
        return false;
    }
    if (sonata_fs_write(APPNV_TAG_UART_RATE, sizeof (uint8_t), (uint8_t *)rateId) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_pin(uint8_t *pin, uint8_t bufLen, bool useDef)
{
    if (pin == NULL)
    {
        return false;
    }
    if (bufLen != APPNV_LEN_PIN)
    {
        return false;
    }
    uint8_t length = APPNV_LEN_PIN;
    if (sonata_fs_read(APPNV_TAG_PIN, &length, (uint8_t*)pin) == SONATA_FS_OK)
    {
        return true;
    }
    else
    {
        if (useDef)
        {
            memcpy(pin, DEFAULT_PIN, strlen(DEFAULT_PIN));
            return true;
        }
    }
    return false;
}

bool app_nv_set_pin(uint8_t *pin, uint8_t bufLen)
{
    if (pin == NULL)
    {
        return false;
    }
    if (bufLen != APPNV_LEN_PIN)
    {
        return false;
    }
    if (sonata_fs_write(APPNV_TAG_PIN, APPNV_LEN_PIN, (uint8_t *)pin) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_mode(uint8_t *mode, bool useDef)
{
    if (mode == NULL)
    {
        return false;
    }
    uint8_t length = sizeof (uint8_t);
    if (sonata_fs_read(APPNV_TAG_MODE, &length, (uint8_t*)mode) == SONATA_FS_OK)
    {
        return true;
    }
    else
    {
        if (useDef)
        {
            memcpy(mode, DEFAULT_MODE, sizeof (uint8_t));
            return true;
        }
    }
    return false;
}

app_mode app_nv_get_mode_value()
{
    uint8_t mode[1] = {'0'};
    if (app_nv_get_mode(mode, true))
    {
        if (mode[0] == '1')
        {
            return MODE_MASTER;
        }
    }
    return MODE_SLAVE;
}

bool app_nv_set_mode(uint8_t *mode)
{
    if (mode == NULL)
    {
        return false;
    }
    if (sonata_fs_write(APPNV_TAG_MODE, sizeof (uint8_t), (uint8_t *)mode) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_autoconn(uint8_t *autoconn, uint8_t bufLen)
{
    if (autoconn == NULL)
    {
        return false;
    }
    if (bufLen != APPNV_LEN_AUTOCONN)
    {
        return false;
    }
    uint8_t length = APPNV_LEN_AUTOCONN;
    if (sonata_fs_read(APPNV_TAG_AUTOCONN, &length, (uint8_t*)autoconn) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}


bool app_nv_set_autoconn(uint8_t *autoconn, uint8_t bufLen)
{
    if (autoconn == NULL)
    {
        return false;
    }
    if (bufLen != APPNV_LEN_AUTOCONN)
    {
        return false;
    }
    if (sonata_fs_write(APPNV_TAG_AUTOCONN, APPNV_LEN_AUTOCONN, (uint8_t *)autoconn) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_set_autoadv(uint8_t autoadv)
{
    uint8_t value[1] = {0};
    value[0] = autoadv;
    if (sonata_fs_write(APPNV_TAG_AUTOADV, sizeof (uint8_t), (uint8_t *)value) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

uint8_t app_nv_get_autoadv()
{
    uint8_t value[1] = {0};
    uint8_t length = sizeof (uint8_t);
    if (sonata_fs_read(APPNV_TAG_AUTOADV, &length, (uint8_t*)value) == SONATA_FS_OK)
    {
        return value[0];
    }
    return 0;
}

bool app_nv_set_autontf(uint8_t autontf)
{
    uint8_t value[1] = {0};
    value[0] = autontf;
    if (sonata_fs_write(APPNV_TAG_AUTONTF, sizeof (uint8_t), (uint8_t *)value) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

uint8_t app_nv_get_autontf()
{
    uint8_t value[1] = {0};
    uint8_t length = sizeof (uint8_t);
    if (sonata_fs_read(APPNV_TAG_AUTONTF, &length, (uint8_t*)value) == SONATA_FS_OK)
    {
        return value[0];
    }
    return 0;
}



bool app_nv_set_timer(uint16_t timer)
{
    uint8_t value[2] = {0};
    value[0] = timer;
    value[1] = timer >> 8;
    if (sonata_fs_write(APPNV_TAG_TIMER, sizeof (uint16_t), (uint8_t *)value) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

uint16_t app_nv_get_timer_value()
{
    uint8_t value[2] = {0};
    uint8_t length = sizeof (uint16_t);
    if (sonata_fs_read(APPNV_TAG_TIMER, &length, (uint8_t*)value) == SONATA_FS_OK)
    {
        return value[0] + (value[1] << 8);
    }
    return 0;
}


bool app_nv_set_length(uint16_t length)
{
    uint8_t value[2] = {0};
    value[0] = length;
    value[1] = length >> 8;
    if (sonata_fs_write(APPNV_TAG_LENGTH, sizeof (uint16_t), (uint8_t *)value) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

uint16_t app_nv_get_length_value()
{
    uint8_t value[2] = {0};
    uint8_t length = sizeof (uint16_t);
    if (sonata_fs_read(APPNV_TAG_LENGTH, &length, (uint8_t*)value) == SONATA_FS_OK)
    {
        return value[0] + (value[1] << 8);
    }
    return 0;
}

bool app_nv_set_bond_type(uint8_t type)
{
    uint8_t value[1] = {0};
    value[0] = type;
    if (sonata_fs_write(APPNV_TAG_BOND_TYPE, sizeof (uint8_t), (uint8_t *)value) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

uint8_t app_nv_get_bond_type_value()
{
    uint8_t value[1] = {0};
    uint8_t length = sizeof (uint8_t);
    if (sonata_fs_read(APPNV_TAG_BOND_TYPE, &length, (uint8_t*)value) == SONATA_FS_OK)
    {
        return value[0];
    }
    return 0;
}

//bool app_nv_set_app_config(uint8_t type, uint8_t enable, app_config_t *para)
//{
//    app_config_ext_t configExt = {0};
//    configExt.type = type;
//    configExt.enable = enable;
//    memcpy(&configExt.param, para, sizeof(app_config_t));
//    printf("AAA: %s ,configExt size=%d, \r\n", __FUNCTION__, sizeof(app_config_ext_t));
//
//    if (sonata_fs_write(APPNV_TAG_APP_CONFIG, APPNV_LEN_APP_CONFIG, (uint8_t *)&configExt) == SONATA_FS_OK)
//    {
//        return true;
//    }
//    return false;
//}
//
//bool app_nv_get_app_config(app_config_ext_t *config)
//{
//    uint8_t length = APPNV_LEN_APP_CONFIG;
//    if (sonata_fs_read(APPNV_TAG_APP_CONFIG, &length, (uint8_t*)config) == SONATA_FS_OK)
//    {
//        return true;
//    }
//    return false;
//
//}


bool app_nv_set_hl_trans(uint8_t enable)
{
    uint8_t value[1] = {0};
    value[0] = enable;
    if (sonata_fs_write(APPNV_TAG_HL_TRANS, sizeof (uint8_t), (uint8_t *)value) == SONATA_FS_OK)
    {
        return true;
    }
    return false;
}

bool app_nv_get_hl_trans_value()
{
    uint8_t value[1] = {0};
    uint8_t length = sizeof (uint8_t);
    if (sonata_fs_read(APPNV_TAG_HL_TRANS, &length, (uint8_t*)value) == SONATA_FS_OK)
    {
        return value[0];
    }
    return 0;
}







