
/**
 ****************************************************************************************
 *
 * @file (app_sec.h)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */

#ifndef APP_SEC_H_
#define APP_SEC_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include "arch.h"
#include "app.h"                     // Application Definition
#include "sonata_ble_api.h"
#include "sonata_gap_api.h"
/*
 * MACRO DEFINES
 ****************************************************************************************
 */
#define MAX_BONDED_DEV_NUM   (3)
#define MAX_BONDED_DEV_INDEX (MAX_BONDED_DEV_NUM - 1)
#define INVALID_BONDED_INDEX (-1)
#define APP_GAP_KEY_LEN      (0x10)
#define APP_GAP_RAND_NB_LEN  (0x08)
#define APP_BD_ADDR_LEN      (6)


/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
extern bool app_bond;

/// Short Term Key information
typedef struct app_sonata_gap_irk
{
    /// Short Term Key
    uint8_t irk[APP_GAP_KEY_LEN];
    /// irk addr
    uint8_t irk_addr[APP_BD_ADDR_LEN];
}app_sonata_gap_irk_t;


typedef struct app_sonata_gap_ltk
{
    /// Long Term Key
    uint8_t ltk[APP_GAP_KEY_LEN];
    /// Encryption Diversifier
    uint16_t ediv;
    /// Random Number
    uint8_t randnb[APP_GAP_RAND_NB_LEN];
}app_sonata_gap_ltk_t;

typedef struct bonded_dev_info
{
    uint8_t              peer_addr[APP_BD_ADDR_LEN];
    app_sonata_gap_ltk_t ltk;
    app_sonata_gap_irk_t irk;
    uint8_t              periph_bond;
}bonded_dev_info_t;

typedef struct bonded_dev_info_list
{
    uint8_t           total_dev;
    uint8_t           curIdx;
    bonded_dev_info_t devInfo[MAX_BONDED_DEV_NUM];
}bonded_dev_info_list_t;

typedef struct resolve_info
{
    uint8_t conidx;
    uint8_t peer_addr[APP_BD_ADDR_LEN];
}resolve_info_t;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

uint16_t app_sec_gap_bond_req_callback(uint8_t conidx, struct sonata_gap_bond_req_ind *request);

uint16_t app_sec_gap_bond_callback(uint8_t conidx, struct sonata_gap_bond_ind *ind);

uint16_t app_sec_gap_encrypt_req_callback(uint8_t conidx, uint16_t in_ediv, uint8_t *in_nb);

uint16_t app_sec_gap_gen_random_number_callback(uint8_t *number);

uint16_t app_sec_gap_security_callback(uint8_t conidx, uint8_t auth_level);

uint16_t app_sec_gap_encrypt_callback(uint8_t conidx, uint8_t auth_level);

uint16_t app_sec_gap_resolving_address_callback(uint8_t operation, uint8_t addr_type, uint8_t *addr);

uint16_t app_sec_address_resolved_callback(uint8_t *addr, uint8_t *key);


void app_sec_user_send_pin_code(uint32_t pin_code);

void app_sec_make_connection_confirm_parameter(sonata_gap_connection_req_ind_t *req, sonata_gap_connection_cfm_t *cfm);

uint16_t app_sec_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam);

void app_sec_set_local_irk(uint8_t *key);

uint8_t * app_sec_get_local_irk();

void app_sec_add_resolve_list(uint8_t *addr, uint8_t * localIrk, uint8_t * peerIrk);

void app_sec_bond(uint8_t conidx);

void app_sec_start_security(uint8_t conidx, uint8_t auth_level);

void app_sec_print_bond_info();

void app_sec_start_encrypt(uint8_t conidx, sonata_gap_connection_req_ind_t *req);

bonded_dev_info_t *app_sec_get_bond_device_info(uint8_t *address);

bonded_dev_info_t * app_sec_get_current_bond_device_info() ;

void app_sec_resolve_address(uint8_t * addr);

void app_sec_set_peer_addr(uint8_t *addr);

void app_set_clear_bond_info();

sonata_gap_pairing_mode app_sec_get_pairing_mode();

void app_sec_set_oobdata(uint8_t * oobC, uint8_t* oobR);

void app_sec_send_oob_bond_cfm(uint8_t conidx)  ;
void app_set_send_oob_via_dts(uint8_t conidx);



#endif //APP_SEC_H_
