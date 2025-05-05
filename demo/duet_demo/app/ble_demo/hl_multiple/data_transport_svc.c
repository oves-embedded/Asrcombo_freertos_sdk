/**
 ****************************************************************************************
 *
 * @file data_transport_svc.c
 *
 * @brief Data transport service
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
#include "data_transport_svc.h"
#include <stdio.h>
#include "sonata_gatt_api.h"
#include "app.h"
#include "sonata_utils_api.h"
#include "sonata_ble_api.h"
#include "app_command.h"
#include "app_nv.h"
#include "app_command.h"
#include "app_utils.h"
/*
 * MACRO DEFINES
 ****************************************************************************************
 */

#define DTSSVC "DTS"

#define DEMO_LOOP (0)


#define DTS_GATT_NORMAL
//#define DTS_GATT_AUTH

#define NUM_PKG_SIZE  (300)


#define NUM_READ_RSSI_TIMER (2000)

/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */
/// Possible values for setting client configuration characteristics
enum sonata_prf_cli_conf
{
    /// Stop notification/indication
    SONATA_PRF_CLI_STOP_NTFIND = 0x0000,
    /// Start notification
    SONATA_PRF_CLI_START_NTF,
    /// Start indication
    SONATA_PRF_CLI_START_IND,
};


/*
 * Type DEFINITIONS
 ****************************************************************************************
 */



/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
static uint16_t start_handle_id = 0;

//static uint8_t custom_value[NUM_PKG_SIZE] = {0};
static uint16_t app_mtu = APP_MTU;
static bool tx_ready = false;

///UUID defines
uint8_t dts_uuid[] = {0x50, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


///DB defines
#define UUID_CHARACTERISTIC {0X03, 0X28, 0}
#define UUID_CHAR_CFG       {0X02, 0X29, 0}
#define UUID_DTS_NTF_VAL    {0X51, 0XFF, 0}
#define UUID_DTS_IND_VAL    {0X52, 0XFF, 0}
#define PERM_DTS_NTF_VAL    (PNTF_NA | PRD_NA | PWR_NA | PWC_NA)
#define PERM_DTS_IND_VAL    (PNTF_NA | PRD_NA | PWR_NA | PWC_NA)
#define PERM_DTS_NTF_CFG    (PRD_NA | PWR_NA | PWC_NA)
#define PERM_DTS_IND_CFG    (PRD_NA | PWR_NA | PWC_NA)
#ifdef DTS_GATT_NORMAL
struct sonata_gatt_att_desc dts_atts[DTS_IDX_NB] = {
        // uuid,                                   perm,                    maxLen, ext_perm
        [DTS_NTF_CHAR] = {UUID_CHARACTERISTIC, PRD_NA          , 2           , 0},
        [DTS_NTF_VAL]  = {UUID_DTS_NTF_VAL   , PERM_DTS_NTF_VAL, NUM_PKG_SIZE, PRI},
        [DTS_NTF_CFG]  = {UUID_CHAR_CFG      , PERM_DTS_NTF_CFG, 2           , PRI},

        [DTS_IND_CHAR] = {UUID_CHARACTERISTIC, PRD_NA          , 2           , 0},
        [DTS_IND_VAL]  = {UUID_DTS_IND_VAL   , PERM_DTS_IND_VAL, NUM_PKG_SIZE, PRI},
        [DTS_IND_CFG]  = {UUID_CHAR_CFG      , PERM_DTS_IND_CFG, 2           , PRI},
};
#endif

#ifdef DTS_GATT_AUTH
struct sonata_gatt_att_desc dts_atts[DTS_IDX_NB] = {
        // uuid,                                   perm,                    maxLen, ext_perm
        [DTS_NTF_CHAR]    =   {UUID_CHARACTERISTIC, PRD_AU,                    2, 0},
        [DTS_NTF_VAL]     =   {UUID_DTS_NTF_VAL,    PNTF_NA | PRD_AU | PWR_NA, NUM_PKG_SIZE, PRI},  //Read need Auth
        [DTS_NTF_CFG]     =   {UUID_CHAR_CFG,       PRD_NA | PWR_NA,           2, PRI},
};
#endif




/*
* FUNCTION DEFINITIONS
****************************************************************************************
*/
/*!
 * @brief DTS should know MTU value, and  the real mtu is "app_mtu = mtu -3"
 * @param mtu
 */
void dts_set_mtu(uint16_t mtu)
{
    app_mtu = mtu - (SONATA_ATT_CODE_LEN + SONATA_ATT_HANDLE_LEN);
}

uint16_t dts_get_mtu()
{
    return app_mtu ;
}

void dts_send_dec_data(uint8_t data)
{
    uint8_t custom_value[10];
    custom_value[0] = data;
    uint8_t data1 = data / 100;
    uint8_t data2 = (data - data1 * 100) / 10;
    uint8_t data3 = (data % 100) % 10;
    custom_value[0] = data1;
    custom_value[1] = data2;
    custom_value[2] = data3;

    sonata_ble_gatt_send_notify_event(app_get_connection_id(), (start_handle_id + DTS_NTF_VAL + 1), 10, custom_value);
}


void dts_send_data(uint8_t conidx, uint8_t *data, uint16_t dataLen, bool log)
{
    if (dataLen > app_mtu)
    {
        APP_TRC_ERROR("ERR, %s, Mtu overflow. app_mtu=%d, dataLen=%d\r\n", __FUNCTION__, app_mtu, dataLen);
        return;
    }
    uint8_t id = app_command_get_cmdid();
    if (id != ID_THROUGHPUT)
    {
        if (log)
        {
            APP_TRC("APPSVC: %s, Send Data to %d[%d]:",__FUNCTION__ , conidx, dataLen);
            for (int i = 0; i < dataLen; ++i)
            {
                APP_TRC("%02X ", data[i]);
            }
            APP_TRC("\r\n");
        }
        gTimeStamp = sonata_get_sys_time();
    }
    sonata_ble_gatt_send_notify_event(conidx, (start_handle_id + DTS_NTF_VAL + 1), dataLen, data);
}


void dts_send_indicate_data(uint8_t conidx, uint8_t *data, uint16_t dataLen)
{
    sonata_ble_gatt_send_indicate_event(conidx, (start_handle_id + DTS_IND_VAL + 1), dataLen, data);

}



/*!
 * @brief Add DTS service
 * @return
 */
uint16_t dts_add()
{
    APP_TRC_HIGH("APPSVC: %s  DTS ADD\r\n", __FUNCTION__);
    uint16_t start_hdl = 0;//(0 = dynamically allocated)
    uint8_t perm = 0;
    uint8_t nb_att = DTS_IDX_NB;
    uint8_t *uuid = dts_uuid;
    struct sonata_gatt_att_desc *atts = dts_atts;
    uint16_t retval = sonata_ble_gatt_add_service(start_hdl, perm, uuid, nb_att, atts);


    //app_data_init();
    return retval;
}
/*!
 * @brief DTS read request
 * @param connection_id
 * @param handle
 * @return
 */
uint8_t rand_value = 0;
uint16_t dts_read_request_handler(uint8_t connection_id, uint16_t handle)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, localHandle=0x%04X,\r\n", __FUNCTION__, localHandle);
    switch (localHandle)
    {
        case DTS_NTF_VAL://43
        {
            APP_TRC("APPSVC: %s, DTS_NTF_VAL was read \r\n", __FUNCTION__);
            uint8_t localValue[] = "DTS NTF VAL";
            localValue[7] = rand_value + '0';
            rand_value++;
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case DTS_NTF_CFG: //44
        {
            uint8_t localValue[] = "DTS NTF config";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }

        case DTS_IND_CFG://47
        {
            uint8_t localValue[] = "DTS IND config";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case DTS_IND_VAL: //46
        {
            uint8_t localValue[] = "DTS IND Val";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }

        default:
            break;
    }
    return CB_REJECT;
}

/*!
 * @brief DTS write request
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 * @return
 */
uint16_t dts_write_request_handler(uint8_t connection_id, uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, localHandle=0x%04X,handle=0x%04X,offset=0x%04X,length=%d\r\n", __FUNCTION__, localHandle, handle, offset,length);
    switch (localHandle)
    {
        case DTS_NTF_VAL:
        {
            APP_TRC(" >>>App NTF Get Data[%05d]:", ((value[0] << 8) + value[1]));
//            for (int i = 0; i < length; ++i)
//            {
//                APP_TRC("%c", value[i]);
//            }
//            APP_TRC(">>\r\n");
//            for (int i = 0; i < length; ++i)
//            {
//                APP_TRC("%02X ", value[i]);
//            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
            app_command_handle_peer_command(value, length);

        }
            break;
        case DTS_IND_VAL:
            APP_TRC(" >>>App IND Get Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

            break;
        case DTS_NTF_CFG:
        {
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
            uint16_t ntf_cfg = value[0];
            if (ntf_cfg == SONATA_PRF_CLI_STOP_NTFIND)
            {
                APP_TRC("DTS: NTF Stopped\r\n");
                tx_ready = false;
                sonata_api_app_timer_clear(APP_TIMER_READ_RSSI);
                app_cmd_ntf_switch_handler(connection_id, false);

            }
            else if (ntf_cfg == SONATA_PRF_CLI_START_NTF)
            {
                APP_TRC("DTS: NTF Started. handle=%d\r\n", handle);
                tx_ready = true;
                #if DTS_DEMO_RSSI
                sonata_api_app_timer_set(APP_TIMER_READ_RSSI, NUM_READ_RSSI_TIMER);
                #endif
                app_cmd_ntf_switch_handler(connection_id, true);
            }
            break;
        }
        case DTS_IND_CFG:
        {
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
            uint16_t ind_cfg = value[0];
            if (ind_cfg == SONATA_PRF_CLI_STOP_NTFIND)
            {
                APP_TRC("DTS: IND Stopped\r\n");
            }
            else if (ind_cfg == SONATA_PRF_CLI_START_IND)
            {
                APP_TRC("DTS: IND Started. start_handle_id=%d\r\n",start_handle_id);
                //Use random value for example
                uint8_t localvalue[3] = {0};
                localvalue[0] = util_rand_byte();
                localvalue[1] = util_rand_byte();
                sonata_ble_gatt_send_indicate_event(connection_id, (start_handle_id + DTS_IND_VAL + 1), 2, localvalue);
            }
            break;
        }
        default:
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_ATT_ERR_ATTRIBUTE_NOT_FOUND);

            break;
    }


    return CB_DONE;
}


/*!
 * @brief Seav DTS start handle id
 * @param handle
 */
void dts_set_start_handle(uint16_t handle)
{
    start_handle_id = handle;
    printf("DTS: DTS Service  hdl=%d, \r\n", handle );
    printf("DTS: DTS_NTF_CHAR hdl=%d, \r\n", handle + 1);
    printf("DTS: DTS_NTF_VAL  hdl=%d, \r\n", handle + 2);
    printf("DTS: DTS_NTF_CFG  hdl=%d, \r\n", handle + 3);
    printf("DTS: DTS_IND_CHAR hdl=%d, \r\n", handle + 4);
    printf("DTS: DTS_IND_VAL  hdl=%d, \r\n", handle + 5);
    printf("DTS: DTS_IND_CFG  hdl=%d, \r\n", handle + 6);
}
/*!
 * @brief Interface for sending data done
 * @param status
 */
void dts_data_send_done(uint8_t status)
{
    #if DTS_DEMO_RSSI
    sonata_api_app_timer_set(APP_TIMER_READ_RSSI, NUM_READ_RSSI_TIMER);
    #endif
    if (app_command_get_cmdid() != ID_THROUGHPUT)
    {
        gTimeStamp = sonata_get_sys_time() - gTimeStamp;
        APP_TRC("DTS, Send data done in %lu half slot(*312.5us)", gTimeStamp);
    }
}


uint16_t dts_get_preferred_mtu()
{
    return NUM_PKG_SIZE;
}


uint8_t dts_timer_handler(uint16_t id)
{
    switch (id)
    {
        case APP_TIMER_READ_RSSI:
        {
            app_ble_get_peer_rssi();
            break;
        }
        default:
            break;

    }
    return 0;
}


bool dts_tx_ready()
{
    return tx_ready;
}

uint16_t dts_get_notify_handle()
{
    return (start_handle_id + DTS_NTF_VAL + 1);
}


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */








