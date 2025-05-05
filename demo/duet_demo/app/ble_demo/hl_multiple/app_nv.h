
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
#ifndef APP_NV_H
#define APP_NV_H

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdio.h>
#include "arch.h"
#include "app.h"                     // Application Definition
#include "sonata_ble_api.h"
#include "sonata_gap_api.h"
#include "sonata_gap_api.h"
#include "app_sec.h"
/*
 * MACRO DEFINES
 ****************************************************************************************
 */

//#define DEFAULT_NAME       ("ASR-BLE-M")
#define DEFAULT_NAME       ("ASR-5822S")
#define DEFAULT_BAUD       ("5")
#define DEFAULT_UUID       ("FF50,FF51,FF51,FF51")
#define DEFAULT_MODE       ("0")
#define DEFAULT_PIN        ("123456") //Should <=6
#define DEFAULT_AUTOCONN   ("0000000000000")
/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
typedef enum
{
    /// Peripheral Bonded
    APPNV_TAG_PERIPH_BONDED              = (APP_DATA_SAVE_TAG_FIRST+0),
    /// Mouse NTF Cfg
    APPNV_TAG_MOUSE_NTF_CFG,
    /// Mouse Timeout value
    APPNV_TAG_MOUSE_TIMEOUT,
    /// Peer Device BD Address
    APPNV_TAG_PEER_BD_ADDRESS,
    /// EDIV (2bytes), RAND NB (8bytes),  LTK (16 bytes), Key Size (1 byte)
    APPNV_TAG_LTK,
    /// app_ltk_key_in
    APPNV_TAG_LTK_IN,
    /// app irk addr
    APPNV_TAG_IRK_ADDR,
    /// app irk
    APPNV_TAG_IRK,
    /// device address
    APPNV_TAG_BD_ADDRESS,
    /// bonded dev info
    APPNV_TAG_BONDED_DEV_INFO,
    /// start pair on boot
    APPNV_TAG_PAIR_ON_BOOT,
    /// local devcie name
    APPNV_TAG_LOCAL_DEVICE_NAME,

    APPNV_TAG_DTS_UUID,
    APPNV_TAG_UART_RATE,
    APPNV_TAG_PIN,
    APPNV_TAG_MODE,
    APPNV_TAG_AUTOCONN,
    APPNV_TAG_AUTONTF,
    APPNV_TAG_TIMER,
    APPNV_TAG_LENGTH,
    APPNV_TAG_BOND_TYPE,
    APPNV_TAG_APP_CONFIG,
    APPNV_TAG_HL_TRANS,
    APPNV_TAG_AUTOADV,
}app_nv_tag;


typedef struct
{
    uint8_t type;
    uint8_t enable;
    //app_config_t param;
}app_config_ext_t;



#define APPNV_LEN_PEER_BD_ADDRESS            (6)
#define APPNV_LEN_LTK                        (27)
#define APPNV_LEN_LOCAL_DEVICE_NAME          (15)  //See  GAP_MAX_NAME_SIZE
#define APPNV_LEN_DTS_UUID                   (19)  //FF01,FF02,FF03,FF04
#define APPNV_LEN_PIN                        (6)
#define APPNV_LEN_AUTOCONN                   (13)  //1/0+MAC (1AABBCCDDEEFF)



typedef enum
{
    MODE_SLAVE = 0,
    MODE_MASTER = 1
}app_mode;

typedef enum
{
    CONN_NOTAUTO = 0,
    CONN_AUTO = 1,
}app_autoconn;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void app_nv_reset();

bool app_nv_set_pair_on_boot(bool pair);

bool app_nv_get_pair_on_boot();

bool app_nv_set_bonded(bool bond);

bool app_nv_get_bonded();

bool app_nv_set_peer_bt_address(uint8_t *addr);

bool app_nv_get_peer_bt_address(uint8_t *addr);

//bool app_nv_set_bonded_device_info(bonded_dev_info_list_t *dev_info);
//
//bool app_nv_get_bonded_device_info(bonded_dev_info_list_t *dev_info);

bool app_nv_set_local_device_name(uint8_t * name, uint8_t length);

bool app_nv_get_local_device_name(uint8_t *name, uint8_t bufLen, bool useDef);

bool app_nv_get_ltk(sonata_gap_ltk_t * key);

bool app_nv_set_ltk(sonata_gap_ltk_t * key);

bool app_nv_get_uuid(uint8_t *uuid, uint8_t bufLen, bool useDef);

uint16_t app_nv_get_dts_uuid_value();

bool app_nv_set_uuid(uint8_t *uuid, uint8_t bufLen);

bool app_nv_get_uart_rate(uint8_t *rateId, bool useDef);

bool app_nv_set_uart_rate(uint8_t *rateId);

uint32_t app_nv_get_uart_rate_value();

bool app_nv_get_pin(uint8_t *pin, uint8_t bufLen, bool useDef);

bool app_nv_set_pin(uint8_t *pin, uint8_t bufLen);

bool app_nv_get_mode(uint8_t *mode, bool useDef);

app_mode app_nv_get_mode_value();

bool app_nv_set_mode(uint8_t *mode);

bool app_nv_get_autoconn(uint8_t *autoconn, uint8_t bufLen);

bool app_nv_set_autoconn(uint8_t *autoconn, uint8_t bufLen);

void app_nv_get_uuid_value(app_uuids *uuids);

bool app_nv_set_autoadv(uint8_t autoadv);

uint8_t app_nv_get_autoadv();

bool app_nv_set_autontf(uint8_t autontf);

uint8_t app_nv_get_autontf();


bool app_nv_set_timer(uint16_t timer);

uint16_t app_nv_get_timer_value();

bool app_nv_set_length(uint16_t length);
uint16_t app_nv_get_length_value();

bool app_nv_set_bonded_device_info(bonded_dev_info_list_t *dev_info);

bool app_nv_get_bonded_device_info(bonded_dev_info_list_t *dev_info);

bool app_nv_set_bond_type(uint8_t type);

uint8_t app_nv_get_bond_type_value();

//bool app_nv_set_app_config(uint8_t type, uint8_t enable, app_config_t *para);
//bool app_nv_get_app_config(app_config_ext_t *config);

bool app_nv_set_hl_trans(uint8_t enable);

bool app_nv_get_hl_trans_value();

#endif //APP_NV_H
