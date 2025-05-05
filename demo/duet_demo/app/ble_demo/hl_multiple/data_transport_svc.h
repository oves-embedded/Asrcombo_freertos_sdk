/**
 ****************************************************************************************
 *
 * @file data_transport_svc.h
 *
 * @brief Data transport service (DTS in short)
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */
#ifndef _DATA_TRANSPORT_SVC_H_
#define _DATA_TRANSPORT_SVC_H_
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_gatt_api.h"
#include "sonata_ble_api.h"

/*
 * MACRO DEFINES
 ****************************************************************************************
 */
#define DTS_DEMO_RSSI       (0)
#define UUID_DTS (0xFF50)

/*
 * ENUM DEFINITIONS
 ****************************************************************************************
 */

enum dts_att_db_handles
{
    DTS_NTF_CHAR,
    DTS_NTF_VAL,
    DTS_NTF_CFG,
    DTS_IND_CHAR,
    DTS_IND_VAL,
    DTS_IND_CFG,

    DTS_IDX_NB,
};


/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
/*!
 * @brief Add DTS service
 * @return
 */
uint16_t dts_add();
/*!
 * @brief DTS read request
 * @param connection_id connection index
 * @param localHandle custom handle id
 * @param handle   handle id in system
 * @return @see CBStatus
 */
uint16_t dts_read_request_handler(uint8_t connection_id, uint16_t handle);
/*!
 * @brief DTS write request
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 * @return
 */
uint16_t dts_write_request_handler(uint8_t connection_id, uint16_t handle,
                                   uint16_t offset, uint16_t length, uint8_t *value);

/*!
 * @brief Seav DTS start handle id
 * @param handle
 */
void dts_set_start_handle(uint16_t handle);

/*!
 * @brief Interface for sending data done
 * @param status
 */
void dts_data_send_done(uint8_t status);
/*!
 * @brief DTS should know MTU value, and  the real mtu is "app_mtu = mtu -3"
 * @param mtu
 */
void dts_set_mtu(uint16_t mtu);

uint16_t dts_get_mtu();
/*!
 * @brief get prefered MTU
 * @return
 */
uint16_t dts_get_preferred_mtu();

void dts_send_dec_data(uint8_t data);
uint8_t dts_timer_handler(uint16_t id);
void dts_send_data(uint8_t conidx, uint8_t *data, uint16_t dataLen, bool log);
void dts_send_indicate_data(uint8_t conidx, uint8_t *data, uint16_t dataLen);
void app_custom_svc_send_ind();
bool dts_tx_ready();
uint16_t dts_get_notify_handle();

#endif //_DATA_TRANSPORT_SVC_H_

