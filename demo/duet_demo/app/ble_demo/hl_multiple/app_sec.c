
/**
 ****************************************************************************************
 *
 * @file (app_sec.c)
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
#include "app_sec.h"
#include <stdio.h>
#include "sonata_ble_api.h"
#include "sonata_gap_api.h"
#include "app_nv.h"
#include "app_utils.h"
#include "app_command.h"


/*
 * MACRO DEFINES
 ****************************************************************************************
 */
//Bond Macro.
typedef enum
{
    BOND_JUSTWORK                       = 0,
    BOND_PASSKEY_MASTER_GEN_SLAVE_INPUT = 1, //Master generate PIN Slave input PIN
    BOND_PASSKEY_MASTER_INPUT_SLAVE_GEN = 2, //Slave generate PIN Master input Pin
    BOND_NUMBER_COMPARE                 = 3, //Master show PIN, slave shows same PIN , then press YES/NO
    BOND_OOB_LEGACY                     = 4, //  same temp key transmited by OOB
    BOND_OOB_SC                         = 5,
    BOND_OOB_SC_NO_DATA                 = 6,
    BOND_JUSTWORK_SC                    = 7,
    BOND_CUSTOMER                       = 10,

}app_bond_type;



bool app_bond = false;
//Get in app_sec_gap_gen_random_number_callback()
static uint8_t app_rand_cnt = 0;
static uint8_t app_loc_irk[SONATA_KEY_LEN]= {0x8D,0x8F,0x76,0x9A,0x7A,0x5B,0xB4,0xBD,0x4C,0xD9,0xA7,0x5D,0xF9,0x17,0x7D,0x80};
static uint8_t app_peer_irk[SONATA_KEY_LEN]= {0x0B, 0x53, 0xA7, 0xF1, 0x57, 0x3B, 0x66, 0xE4, 0xB5, 0xAA, 0x65, 0x30, 0x53, 0x7A, 0x6D, 0xA9};
static uint8_t app_oob_tk[SONATA_KEY_LEN]= {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
static uint32_t app_passkey_pincode = 0;

uint8_t app_csrk[SONATA_GAP_KEY_LEN] = {0x00,0x01,0x02,0x03,0x04,0x05,0};

bonded_dev_info_list_t bondList = {0};
resolve_info_t gResolveInfo = {0};
uint8_t peer_dbaddr[APP_BD_ADDR_LEN] = {0};
uint8_t gPeerAddr[APP_BD_ADDR_LEN] = {0};

uint8_t gOobConf[SONATA_GAP_KEY_LEN] = {0};
uint8_t gOobRand[SONATA_GAP_KEY_LEN] = {0};

//uint8_t app_irk_addr[SONATA_GAP_BD_ADDR_LEN] = {0};
//uint8_t app_irk_key[SONATA_GAP_KEY_LEN] = {0}
//uint8_t app_loc_addr[SONATA_GAP_BD_ADDR_LEN] = {0x0C,0x20,0x18,0xA7,0x9F,0xDD};
/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
sonata_gap_pairing_mode app_sec_get_pairing_mode()
{
    #ifdef BOND_NUMBER_COMPARE
    return SONATA_GAP_PAIRING_SEC_CON; //Number compare should enabel secure connection
    #endif
    return SONATA_GAP_PAIRING_LEGACY;
}


void app_set_clear_bond_info()
{
    memset(&bondList, 0, sizeof(bonded_dev_info_list_t));
    app_nv_set_bonded_device_info(&bondList);

}
void app_sec_set_peer_addr(uint8_t * addr)
{
    addrCopy(peer_dbaddr, addr);
    addrCopy(gPeerAddr, addr);
}

bonded_dev_info_t * app_sec_get_bond_device_info(uint8_t * address)
{
    for (int i = 0; i < MAX_BONDED_DEV_NUM; i++)
    {
        if (memcmp(bondList.devInfo[i].peer_addr, address, SONATA_BD_ADDR_LEN) == 0)
        {
            return &bondList.devInfo[i];
        }
    }
    return NULL;
}

bonded_dev_info_t * app_sec_get_current_bond_device_info()
{
    return &bondList.devInfo[bondList.curIdx];
}

void app_sec_resolve_address(uint8_t * addr)
{
    struct sonata_gap_sec_key irk = {0};
//    memcpy(irk.key, bondList.devInfo[bondList.curIdx].irk.irk, SONATA_GAP_KEY_LEN);
    printf("APP: %s, Use IRK:", __FUNCTION__);
    for (int i = 0; i < SONATA_KEY_LEN; ++i)
    {
        printf("%02X ", app_peer_irk[i]);
    }
    printf("\r\n");
    gapKeyCopy(irk.key, app_peer_irk);
    sonata_ble_gap_resolve_address(addr, 1, &irk);
}

bool app_sec_check_ltk(uint8_t * ltk)
{
    uint8_t ltkTemp[SONATA_GAP_KEY_LEN] = {0};
    if (memcmp(ltk, ltkTemp, SONATA_GAP_KEY_LEN) == 0)
    {
        return false;
    }
    return true;
}



void app_sec_add_bonded_dev(uint8_t *peerAddr, sonata_gap_ltk_t * data_ltk, struct  sonata_gap_irk * data_irk)
{
//    APP_TRC("APP_SEC: %s  ,", __FUNCTION__);
    bonded_dev_info_t *savedInfo = app_sec_get_bond_device_info(peerAddr);
    if (savedInfo == NULL) //Add as new device
    {
//        APP_TRC("Add as new device\r\n");
        if (bondList.curIdx == MAX_BONDED_DEV_INDEX)
        {
            bondList.curIdx = 0;
        }
        savedInfo = &bondList.devInfo[bondList.curIdx];
//        APP_TRC(" bondList.curIdx = %d \r\n", bondList.curIdx);
        addrCopy(savedInfo->peer_addr, peerAddr);
        if (data_ltk != NULL)
        {
            gapKeyCopy(savedInfo->ltk.ltk, data_ltk->ltk.key);
            memcpy(savedInfo->ltk.randnb, data_ltk->randnb.nb, 8);
            savedInfo->ltk.ediv = data_ltk->ediv;
        }
        if (data_irk != NULL)
        {
            addrCopy(savedInfo->irk.irk_addr, data_irk->addr.addr.addr);
            gapKeyCopy(savedInfo->irk.irk, data_irk->irk.key);
        }
        app_nv_set_bonded_device_info(&bondList);
        bondList.curIdx++;
    }
    else  //Update old device info
    {
//        APP_TRC(" Update old device info\r\n");

        if (data_ltk != NULL)
        {
            gapKeyCopy(savedInfo->ltk.ltk, data_ltk->ltk.key);
            memcpy(savedInfo->ltk.randnb, data_ltk->randnb.nb, APP_GAP_RAND_NB_LEN);
            savedInfo->ltk.ediv = data_ltk->ediv;
        }
        if (data_irk != NULL)
        {
            addrCopy(savedInfo->irk.irk_addr, data_irk->addr.addr.addr);
            gapKeyCopy(savedInfo->irk.irk, data_irk->irk.key);
        }
        app_nv_set_bonded_device_info(&bondList);
    }
}

void app_sec_print_bond_info()
{
    //bonded_dev_info_list_t info = {0};
    //app_nv_get_bonded_device_info(&info);
    bonded_dev_info_list_t *infoList = &bondList;
    printf("APP_SEC: %s  \r\n", __FUNCTION__);
    printf("    total_dev=%d\r\n", infoList->total_dev);
    printf("    curIdx=%d\r\n", infoList->curIdx);
    for (int i = 0; i < MAX_BONDED_DEV_NUM; ++i)
    {
        if (addrValid(infoList->devInfo[i].peer_addr)) {
            printf("   ----%d----\r\n", i);
            uint8_t *ptr = infoList->devInfo[i].peer_addr;
            printf("    peer_addr   :%02X %02X %02X %02X %02X %02X \r\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
            printf("    Ltk.ltk     :" );for (int k = 0; k < APP_GAP_KEY_LEN; ++k){ printf("%02X ", infoList->devInfo[i].ltk.ltk[k]);}printf("\r\n");
            printf("    Ltk.randnb  :" ); for (int k = 0; k < APP_GAP_RAND_NB_LEN; ++k){printf("%02X ", infoList->devInfo[i].ltk.randnb[k]);}printf("\r\n");
            printf("    Ltk.ediv    :%04X\r\n",infoList->devInfo[i].ltk.ediv);
            printf("    Irk.irk     :" );for (int k = 0; k < APP_GAP_KEY_LEN; ++k){printf("%02X ", infoList->devInfo[i].irk.irk[k]);}printf("\r\n");
            ptr = infoList->devInfo[i].irk.irk_addr;
            printf("    Irk.irk_addr:%02X %02X %02X %02X %02X %02X \r\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
        }
    }
}

static void print_peer_bond_request(struct sonata_gap_bond_req_ind *request)
{
    switch (request->request)
    {
        case SONATA_GAP_PAIRING_REQ:
            APP_TRC("PEER_PAIR: SONATA_GAP_PAIRING_REQ,auth_req=%02X(X)", request->data.auth_req);
            switch (request->data.auth_req)
            {
                case SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND: APP_TRC(" (SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND)\r\n");break;
                case SONATA_GAP_AUTH_REQ_NO_MITM_BOND   : APP_TRC(" (SONATA_GAP_AUTH_REQ_NO_MITM_BOND)\r\n");break;
                case SONATA_GAP_AUTH_REQ_MITM_NO_BOND   : APP_TRC(" (SONATA_GAP_AUTH_REQ_MITM_NO_BOND)\r\n");break;
                case SONATA_GAP_AUTH_REQ_MITM_BOND      : APP_TRC(" (SONATA_GAP_AUTH_REQ_MITM_BOND)\r\n");break;
                case SONATA_GAP_AUTH_REQ_SEC_CON_NO_BOND: APP_TRC(" (SONATA_GAP_AUTH_REQ_SEC_CON_NO_BOND)\r\n");break;
                case SONATA_GAP_AUTH_REQ_SEC_CON_BOND   : APP_TRC(" (SONATA_GAP_AUTH_REQ_SEC_CON_BOND)\r\n");break;
                default                          : APP_TRC(" (Default)\r\n"); break;

            }
            break;
        case SONATA_GAP_TK_EXCH:
            APP_TRC("PEER_PAIR: SONATA_GAP_TK_EXCH,tk_type=%02X(X) ", request->data.tk_type);
            switch (request->data.tk_type)
            {
                case SONATA_GAP_TK_OOB      : APP_TRC(" (SONATA_GAP_TK_OOB)\r\n");break;
                case SONATA_GAP_TK_DISPLAY  : APP_TRC(" (SONATA_GAP_TK_DISPLAY)\r\n"); break;
                case SONATA_GAP_TK_KEY_ENTRY: APP_TRC(" (SONATA_GAP_TK_KEY_ENTRY)\r\n");break;
                default              : APP_TRC(" (Default)\r\n");break;
            }
            break;
        case SONATA_GAP_LTK_EXCH:
            APP_TRC("PEER_PAIR: SONATA_GAP_LTK_EXCH,key_size=%02X(X)\r\n", request->data.key_size);
            break;
        case SONATA_GAP_OOB_EXCH:
            APP_TRC("PEER_PAIR: \r\n");
            break;
        case SONATA_GAP_NC_EXCH:
            APP_TRC("PEER_PAIR: SONATA_GAP_NC_EXCH,NC Value:%02X %02X %02X %02X\r\n", request->data.nc_data.value[0], request->data.nc_data.value[1],
                    request->data.nc_data.value[2], request->data.nc_data.value[3]);
            break;

    }
}

void app_sec_make_connection_confirm_parameter(sonata_gap_connection_req_ind_t *req, sonata_gap_connection_cfm_t *connectionCfm)
{
    APP_TRC("ACB: %s, role=%d\r\n", __FUNCTION__, req->role);
    if (connectionCfm == NULL)
    {
        return;
    }
    bonded_dev_info_t *bondedDevInfo = app_sec_get_bond_device_info(req->peer_addr.addr);
    if (req->role == SONATA_ROLE_MASTER)
    {
        if (bondedDevInfo != NULL)
        {
            connectionCfm->ltk_present = true;
            uint8_t bondType = app_nv_get_bond_type_value();
            switch (bondType)
            {
                case BOND_JUSTWORK:
                    break;
                case BOND_JUSTWORK_SC:
                    break;
                case BOND_PASSKEY_MASTER_GEN_SLAVE_INPUT:
                    APP_TRC("ACB: %s, send SONATA_GAP_AUTH_REQ_MITM_BOND\r\n", __FUNCTION__);
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_MITM_BOND;
                    break;
                case BOND_PASSKEY_MASTER_INPUT_SLAVE_GEN:
                    APP_TRC("ACB: %s, send SONATA_GAP_AUTH_REQ_MITM_BOND\r\n", __FUNCTION__);
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_MITM_BOND;
                    break;
                case BOND_NUMBER_COMPARE:
                    APP_TRC("ACB: %s, send SONATA_GAP_AUTH_REQ_SEC_CON_BOND\r\n", __FUNCTION__);
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_SEC_CON_BOND;
                    break;
                case BOND_OOB_LEGACY:
                    APP_TRC("ACB: %s, send SONATA_GAP_AUTH_REQ_SEC_CON_BOND\r\n", __FUNCTION__);
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_SEC_CON_BOND;
                    break;
                case BOND_OOB_SC:
                    APP_TRC("ACB: %s, send SONATA_GAP_AUTH_REQ_SEC_CON_BOND\r\n", __FUNCTION__);
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_SEC_CON_BOND;
                    break;
                case BOND_OOB_SC_NO_DATA:
                    APP_TRC("ACB: %s, send SONATA_GAP_AUTH_REQ_SEC_CON_BOND\r\n", __FUNCTION__);
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_SEC_CON_BOND;
                    break;
                case BOND_CUSTOMER:
                    break;
            }
        }
        else
        {
            APP_TRC("ACB: %s, not found, send SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND confirm\r\n", __FUNCTION__);
            connectionCfm->auth = SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND;
        }
    }
    else
    {
        if (app_bond == true)
        {
            uint8_t bondType = app_nv_get_bond_type_value();
            switch (bondType)
            {
                case BOND_JUSTWORK:
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_NO_MITM_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_NO_MITM_BOND\r\n", __FUNCTION__);
                    break;
                case BOND_JUSTWORK_SC:
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_NO_MITM_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_NO_MITM_BOND\r\n", __FUNCTION__);
                    break;
                case BOND_PASSKEY_MASTER_GEN_SLAVE_INPUT:
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_MITM_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_MITM_BOND\r\n", __FUNCTION__);
                    break;
                case BOND_PASSKEY_MASTER_INPUT_SLAVE_GEN :
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_MITM_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_MITM_BOND\r\n", __FUNCTION__);
                    break;
                case BOND_NUMBER_COMPARE:
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_SEC_CON_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_MITM_BOND\r\n", __FUNCTION__);
                    break;
                case BOND_OOB_LEGACY:
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_SEC_CON_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_MITM_BOND\r\n", __FUNCTION__);
                    break;
                case BOND_OOB_SC:
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_SEC_CON_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_MITM_BOND\r\n", __FUNCTION__);
                    break;
                case BOND_OOB_SC_NO_DATA:
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_SEC_CON_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_MITM_BOND\r\n", __FUNCTION__);
                    break;
                case BOND_CUSTOMER:
                    connectionCfm->auth = SONATA_GAP_AUTH_REQ_NO_MITM_BOND;
                    APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_NO_MITM_BOND\r\n", __FUNCTION__);
                    break;
            }
        }
        else
        {
            connectionCfm->auth = SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND;
            APP_TRC("APP: %s  connectionCfm.auth=SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND\r\n", __FUNCTION__);
        }
    }
}



void app_sec_gen_pin_code_send_tk(uint8_t conidx)
{
    // Generate a PIN Code- (Between 100000 and 999999)
    uint32_t pin_code = (100000 + (util_rand_word()%900000));
    pin_code = 123456;
    APP_TRC("APP: %s  ********************************pin_code=%lu\r\n", __FUNCTION__, pin_code);

    uint8_t tKey[SONATA_GAP_KEY_LEN] = {0};
    tKey[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
    tKey[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
    tKey[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
    tKey[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);
    sonata_ble_gap_send_bond_cfm_for_tk_exchange(conidx, true, tKey);
}


void app_sec_start_security(uint8_t conidx, uint8_t auth_level)
{
    sonata_ble_gap_start_security(conidx, auth_level);
}

void app_sec_start_encrypt(uint8_t conidx, sonata_gap_connection_req_ind_t *req)
{
    if (req->role == SONATA_ROLE_MASTER)
    {
        uint8_t bondType = app_nv_get_bond_type_value();
        if (bondType != 0)
        {
            bonded_dev_info_t *bondedDevInfo = app_sec_get_bond_device_info(req->peer_addr.addr);
            if (bondedDevInfo != NULL && app_sec_check_ltk(bondedDevInfo->ltk.ltk))
            {
                APP_TRC("ACB: %s  Start Encrypt as Master\r\n", __FUNCTION__);
                uint16_t ediv = bondedDevInfo->ltk.ediv;
                uint8_t *random = bondedDevInfo->ltk.randnb;
                uint8_t key_size = SONATA_GAP_SMP_MAX_ENC_SIZE_LEN;
                uint8_t *ltk = bondedDevInfo->ltk.ltk;
                sonata_ble_gap_start_encrypt(conidx, ediv, random, key_size, ltk);
            }
            else
            {
                APP_TRC("ACB: %s LTK missing, can't start encrypt \r\n", __FUNCTION__);
            }
        }
    }
    else
    {
        APP_TRC("ACB: %s  No need to start encrypt\r\n", __FUNCTION__);
    }
}

void app_sec_generate_ltk(sonata_gap_ltk_t *data_ltk)
{
    uint8_t counter = 0;
    data_ltk->ediv = (uint16_t)util_rand_word();
    APP_TRC_HIGH("APP: %s, app_ltk_ediv :%04X\r\n",__FUNCTION__, data_ltk->ediv );
    for (counter = 0; counter < SONATA_RAND_NB_LEN; counter++)
    {
        data_ltk->ltk.key[counter]    = (uint8_t)util_rand_word();
        data_ltk->randnb.nb[counter] = (uint8_t)util_rand_word();
    }
    for (counter = SONATA_RAND_NB_LEN; counter < SONATA_KEY_LEN; counter++)
    {
        data_ltk->ltk.key[counter]    = (uint8_t)util_rand_word();
    }
    APP_TRC_HIGH("APP: app_ltk_key  :" );for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i) {APP_TRC_HIGH("%02X ", data_ltk->ltk.key[i]);}APP_TRC_HIGH("\r\n");
    APP_TRC_HIGH("APP: app_randnb_nb:");for (int i = 0; i < SONATA_GAP_RAND_NB_LEN; ++i){APP_TRC_HIGH("%02X ", data_ltk->randnb.nb[i]);}APP_TRC_HIGH("\r\n");
}

void app_sec_add_resolve_list(uint8_t *addr, uint8_t * localIrk, uint8_t * peerIrk)
{
    struct sonata_gap_ral_dev_info info = {0};
    info.priv_mode = SONATA_PRIV_TYPE_NETWORK;//PRIV_TYPE_DEVICE;
    info.addr.addr_type = (addr[5] & 0xC0) ? SONATA_ADDR_RAND : SONATA_ADDR_PUBLIC;
    addrCopy(info.addr.addr.addr, addr);
    if (localIrk != NULL)
    {
        gapKeyCopy(info.local_irk, localIrk);
    }
    if (peerIrk != NULL)
    {
        gapKeyCopy(info.peer_irk, peerIrk);
    }
    sonata_ble_gap_set_resolving_list(1, &info);
}

void app_set_send_oob_via_dts(uint8_t conidx)
{
    uint8_t localValue[] = "AT+PEER=AT+OOBDATA=3,";
    uint8_t *oob = getAllTempBuffer(100);
    uint8_t offset = strlen((char *) localValue);
    memcpy(oob, localValue, offset);
    offset = strlen((char *) oob);
    for (int i = 0; i < 16; ++i)
    {
        hexValue2Char(gOobConf[i], &oob[offset + i * 2]);
    }
    offset = strlen((char *) oob);
    oob[offset] = ',';
    offset = strlen((char *) oob);
    for (int i = 0; i < 16; ++i)
    {
        hexValue2Char(gOobRand[i], &oob[offset + i * 2]);
    }
    offset = strlen((char *) oob);
    APP_TRC("APP: Send OOB Via DTS:[%d]", offset);
    for (int i = 0; i < offset; ++i)
    {
        APP_TRC("%c", oob[i]);
    }
    APP_TRC("\r\n");
    if (sonata_ble_get_role(conidx) == SONATA_ROLE_MASTER)
    {
        app_ble_master_write_data(conidx, offset, oob);
    }
    else
    {
        app_ble_slave_send_data_via_ntf(conidx, offset, oob, true);
    }
}

uint16_t app_sec_gap_bond_req_callback(uint8_t conidx, struct sonata_gap_bond_req_ind *request)
{
    APP_TRC_HIGH("ACB: %s  request->request=%d\r\n", __FUNCTION__, request->request);
    //sonata_api_app_timer_clear(APP_TIMER_ADV_END);
    print_peer_bond_request(request);
    switch (request->request)
    {
        case SONATA_GAP_PAIRING_REQ:
        {
            APP_TRC_HIGH("ACB: %s  SONATA_GAP_PAIRING_REQ,request->data.auth_req=%02X(x)\r\n", __FUNCTION__,request->data.auth_req);
            uint8_t accept = true;
            uint8_t key_size      = SONATA_GAP_SMP_MAX_ENC_SIZE_LEN;
            enum sonata_gap_io_cap       iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
            enum sonata_gap_oob_auth     oob        = SONATA_GAP_OOB_AUTH_DATA_NOT_PRESENT;
            enum sonata_gap_auth         auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND;
            enum sonata_gap_sec_req      sec_req    = SONATA_GAP_NO_SEC;
            enum sonata_gap_kdist        ikey_dist  = SONATA_GAP_KDIST_NONE;//Initiator key distribution
            enum sonata_gap_kdist        rkey_dist  = SONATA_GAP_KDIST_IDKEY;//Responder key distribution
            uint8_t bondType = app_nv_get_bond_type_value();
            switch (bondType)
            {
                case BOND_JUSTWORK:  //************************************************************************
                    APP_TRC_HIGH("APP: -----------------------BOND_JUSTWORK--------------------\r\n");
                    iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
                    ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;  //Initiator key distribution
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;  //Responder key distribution
                    auth       = SONATA_GAP_AUTH_BOND;
                    sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
                    break;
                case BOND_JUSTWORK_SC:
                    APP_TRC_HIGH("APP: -----------------------BOND_JUSTWORK_SC--------------------\r\n");
                    iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
                    ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Responder key distribution
                    auth       = SONATA_GAP_AUTH_SEC_CON | SONATA_GAP_AUTH_BOND;
                    sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
                    break;
                    break;
                case BOND_PASSKEY_MASTER_GEN_SLAVE_INPUT: //************************************************************************
                    APP_TRC_HIGH("APP: -----------------------BOND_PASSKEY_MASTER_GEN_SLAVE_INPUT--------------------\r\n");
                    iocap      = SONATA_GAP_IO_CAP_KB_ONLY;
                    ikey_dist  = SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;  //Responder key distribution
                    auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND;
                    sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC; //SONATA_GAP_NO_SEC;//SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
                    break;
                case BOND_PASSKEY_MASTER_INPUT_SLAVE_GEN ://************************************************************************
                    APP_TRC_HIGH("APP: -----------------------BOND_PASSKEY_MASTER_INPUT_SLAVE_GEN--------------------\r\n");
                    iocap      = SONATA_GAP_IO_CAP_DISPLAY_YES_NO;
                    ikey_dist  = SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;  //Responder key distribution
                    auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND;
                    sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC; //SONATA_GAP_NO_SEC;//SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
                    break;
                case BOND_NUMBER_COMPARE: //************************************************************************
                    APP_TRC_HIGH("APP: -----------------------BOND_NUMBER_COMPARE--------------------\r\n");
                    iocap      = SONATA_GAP_IO_CAP_DISPLAY_YES_NO;
                    ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;
                    auth       = SONATA_GAP_AUTH_SEC_CON |SONATA_GAP_AUTH_MITM |SONATA_GAP_AUTH_BOND;
                    sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
                    break;
                case  BOND_OOB_LEGACY:
                    APP_TRC_HIGH("APP: -----------------------BOND_OOB_LEGACY--------------------\r\n");
                    oob        = SONATA_GAP_OOB_AUTH_DATA_PRESENT;
                    iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
                    ikey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY; //Initiator key distribution
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY ;  //Responder key distribution
                    auth       =  SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND;                               //SONATA_GAP_AUTH_SEC_CON |
                    sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC;
                    break;
                case BOND_OOB_SC: //************************************************************************
                    APP_TRC_HIGH("APP: -----------------------BOND_OOB_SC--------------------\r\n");
                    oob        = SONATA_GAP_OOB_AUTH_DATA_PRESENT;
                    iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
                    ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;
                    auth       = SONATA_GAP_AUTH_SEC_CON |SONATA_GAP_AUTH_MITM |SONATA_GAP_AUTH_BOND;
                    sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
                    break;
                case BOND_OOB_SC_NO_DATA: //************************************************************************
                    APP_TRC_HIGH("APP: -----------------------BOND_OOB_SC_NO_DATA--------------------\r\n");
                    oob        = SONATA_GAP_OOB_AUTH_DATA_NOT_PRESENT;
                    iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
                    ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;
                    auth       = SONATA_GAP_AUTH_SEC_CON |SONATA_GAP_AUTH_MITM |SONATA_GAP_AUTH_BOND;
                    sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
                    break;
                case BOND_CUSTOMER:
                    APP_TRC_HIGH("APP: -----------------------BOND_CUSTOMER--------------------\r\n");
                    iocap      = SONATA_GAP_IO_CAP_DISPLAY_YES_NO;
                    auth       = SONATA_GAP_AUTH_SEC_CON | SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND;
                    ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY | SONATA_GAP_KDIST_LINKKEY;
                    rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY;
                    sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
                    oob        = SONATA_GAP_OOB_AUTH_DATA_NOT_PRESENT;
                    break;
                default:
                    APP_TRC_HIGH("APP: -----------------------BondType Not Support--------------------\r\n");
                    return CB_DONE;
            }

            if (request->data.auth_req & SONATA_GAP_AUTH_SEC_CON)
            {
                APP_TRC("ACB: %s  sec_con_enabled\r\n", __FUNCTION__);
            }
            sonata_ble_gap_send_bond_cfm_for_pairing_req(conidx, request, accept,
                                                         iocap, oob, auth, key_size, ikey_dist, rkey_dist, sec_req);
        }
            break;

        case SONATA_GAP_LTK_EXCH:
        {
            APP_TRC_HIGH("ACB: %s  SONATA_GAP_LTK_EXCH, Generate LTK and send to peer.\r\n", __FUNCTION__);
            sonata_gap_ltk_t data_ltk = {0};
            app_sec_generate_ltk(&data_ltk);
            sonata_ble_gap_send_bond_cfm_for_ltk_exchange(conidx, true, data_ltk.ediv, data_ltk.randnb.nb, SONATA_GAP_KEY_LEN, data_ltk.ltk.key);
            app_sec_add_bonded_dev(peer_dbaddr, &data_ltk, NULL);
        }
            break;
        case SONATA_GAP_IRK_EXCH:
        {
            APP_TRC_HIGH("ACB: %s  SONATA_GAP_IRK_EXCH, Send IRK to peer \r\n", __FUNCTION__);
            uint8_t accept = true;
            uint8_t *local_addr = sonata_get_bt_address();
            uint8_t addr_type = (local_addr[5] & 0xC0) ? SONATA_ADDR_RAND : SONATA_ADDR_PUBLIC;
            sonata_ble_gap_send_bond_cfm_for_irk_exchange(conidx, accept, app_loc_irk, addr_type, local_addr);
        }
            break;
        case SONATA_GAP_TK_EXCH:
        {
            switch (request->data.tk_type )
            {
                case SONATA_GAP_TK_OOB:
                    APP_TRC_HIGH("ACB: %s  SONATA_GAP_TK_EXCH, Wait for Sending OOB\r\n", __FUNCTION__);
                    sonata_ble_gap_send_bond_cfm_for_tk_exchange(conidx, true, app_oob_tk);// master and slave use the same temp key
                    break;
                case SONATA_GAP_TK_DISPLAY:
                    APP_TRC_HIGH("ACB: Bond Request: SONATA_GAP_TK_EXCH, SONATA_GAP_TK_DISPLAY, Wait Master to input Pin and\r\n");
                    app_sec_gen_pin_code_send_tk(conidx);

                    break;
                case SONATA_GAP_TK_KEY_ENTRY:
                    if (sonata_ble_get_role(conidx) == SONATA_ROLE_MASTER)
                    {
                        uint8_t bondType = app_nv_get_bond_type_value();
                        switch (bondType)
                        {
                            case BOND_PASSKEY_MASTER_GEN_SLAVE_INPUT:
                                APP_TRC_HIGH("ACB: %s  SONATA_GAP_TK_EXCH, Make Pin Code for slave\r\n", __FUNCTION__);
                                app_sec_gen_pin_code_send_tk(conidx);
                                break;
                            case BOND_PASSKEY_MASTER_INPUT_SLAVE_GEN:
                                APP_TRC_HIGH("************************************************************************\r\n");
                                APP_TRC_HIGH("*              SONATA_GAP_TK_EXCH, Input PIN code for slave            *\r\n" );
                                APP_TRC_HIGH("************************************************************************\r\n");

                                //Call sonata_ble_gap_send_bond_cfm_for_tk_exchange() to send the input PIN (eg. AT+PIN=123456)
                                break;
                        }
                    }
                    else
                    {
                        APP_TRC_HIGH("************************************************************************\r\n");
                        APP_TRC_HIGH("*        SONATA_GAP_TK_KEY_ENTRY, Wait for Sending PIN code            *\r\n" );
                        APP_TRC_HIGH("*                        USE AT+PIN=XXXXXX                             *\r\n" );
                        APP_TRC_HIGH("************************************************************************\r\n");
                    }
                    break;
                default:
                    APP_TRC(" (Default)\r\n");
                    break;
            }

        }
            break;
        case SONATA_GAP_NC_EXCH:
        {
            APP_TRC_HIGH("ACB: %s  SONATA_GAP_NC_EXCH ", __FUNCTION__);
            app_passkey_pincode = sonata_passkey_byte_to_pincode(request->data.nc_data.value);
            APP_TRC_HIGH(" Pin:%06lu, Wait to send YES/NO\r\n", app_passkey_pincode);
            //app_gap_send_security_req(0);
            app_sec_passkey_send_yesno(conidx,true);
        }
            break;
        case SONATA_GAP_CSRK_EXCH:
            APP_TRC_HIGH("ACB: %s  SONATA_GAP_CSRK_EXCH\r\n", __FUNCTION__);
            uint8_t accept  = true;
            sonata_ble_gap_send_bond_cfm_for_csrk_exchange(conidx, accept, app_csrk);
            break;
        case SONATA_GAP_OOB_EXCH:
            if (sonata_ble_get_role(conidx) == SONATA_ROLE_MASTER)
            {
                APP_TRC_HIGH("ACB: 1. Input this command 1&2 OR 3 at Peripheral Device(in 30S).............\r\n\r\n");
            }
            else
            {
                APP_TRC_HIGH("ACB: 2 .Input this command 1&2 OR 3 at Master Device(in 30S).............\r\n\r\n");
            }
            APP_TRC_HIGH("\r\n\r\n\r\n---------------------------------------------------------\r\n");
            APP_TRC_HIGH("AT+OOBDATA=1,");
            for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i)
            {
                APP_TRC_HIGH("%02X", request->data.oob_data.conf[i]);
            }
            APP_TRC_HIGH("\r\n");
            APP_TRC_HIGH("AT+OOBDATA=2,");
            for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i)
            {
                APP_TRC_HIGH("%02X", request->data.oob_data.rand[i]);
            }
            APP_TRC_HIGH("\r\n");

            APP_TRC_HIGH("AT+OOBDATA=3,");
            for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i)
            {
                APP_TRC_HIGH("%02X", request->data.oob_data.conf[i]);
            }
            APP_TRC_HIGH(",");
            for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i)
            {
                APP_TRC_HIGH("%02X", request->data.oob_data.rand[i]);
            }
            APP_TRC_HIGH("\r\n---------------------------------------------------------\r\n\r\n\r\n");

            break;
    }
    return CB_DONE;
}


uint16_t app_sec_gap_bond_callback(uint8_t conidx, struct sonata_gap_bond_ind *ind)
{
    APP_TRC("ACB: %s  \r\n", __FUNCTION__);
    //sonata_api_app_timer_clear(APP_TIMER_ADV_END);
    switch (ind->info)
    {
        case SONATA_GAP_PAIRING_SUCCEED:
            APP_TRC_HIGH("ACB: %s  SONATA_GAP_PAIRING_SUCCEED,\r\n", __FUNCTION__);
            APP_TRC(" app_bond = true,");
            app_bond = true;
            app_nv_set_bonded(app_bond);
            //// Set the BD Address of the peer device in FS
            struct sonata_gap_bdaddr* bdaddr = sonata_ble_gap_get_bdaddr(conidx, SONATA_GAP_SMP_INFO_PEER);
            if (sonata_fs_write(APPNV_TAG_PEER_BD_ADDRESS, SONATA_GAP_BD_ADDR_LEN, bdaddr->addr.addr) != SONATA_FS_OK)
            {
                // An error has occurred during access to the FS
                //ASSERT_ERR(3694, 0);
            }
            else
            {
                printf("\r\n\r\n----BLE Bond Success ----\r\n");
                APP_TRC("----Peer_addr:");
                for (int i = SONATA_GAP_BD_ADDR_LEN-1; i >=0; --i)
                {
                    APP_TRC("%02X ", bdaddr->addr.addr[i]);
                }
                APP_TRC(" ----\r\n\r\n");
                app_sec_set_peer_addr(bdaddr->addr.addr);
                //app_sec_print_bond_info();
            }

            switch (ind->data.auth.info)
            {
                case SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND:
                    APP_TRC("Auth:SONATA_GAP_AUTH_REQ_NO_MITM_NO_BOND \r\n");
                    break;
                case SONATA_GAP_AUTH_REQ_NO_MITM_BOND:
                    APP_TRC("Auth:SONATA_GAP_AUTH_REQ_NO_MITM_BOND \r\n");
                    break;
                case SONATA_GAP_AUTH_REQ_MITM_NO_BOND:
                    APP_TRC("Auth:SONATA_GAP_AUTH_REQ_MITM_NO_BOND \r\n");
                    break;
                case SONATA_GAP_AUTH_REQ_MITM_BOND:
                    APP_TRC("Auth:SONATA_GAP_AUTH_REQ_MITM_BOND \r\n");
                    break;
                case SONATA_GAP_AUTH_REQ_SEC_CON_NO_BOND:
                    APP_TRC("Auth:SONATA_GAP_AUTH_REQ_SEC_CON_NO_BOND \r\n");
                    break;
                case SONATA_GAP_AUTH_REQ_SEC_CON_BOND:
                    APP_TRC("Auth:SONATA_GAP_AUTH_REQ_SEC_CON_BOND \r\n");
                    break;
            }
            break;
        case SONATA_GAP_PAIRING_FAILED:
            app_bond = false;
            //Reason see (0X70|smp_pair_fail_reason)
            APP_TRC_HIGH("ACB: %s  SONATA_GAP_PAIRING_FAILED,Reason:%02X(X)\r\n", __FUNCTION__,ind->data.reason);
            sonata_ble_gap_start_security(conidx, SONATA_GAP_AUTH_REQ_MITM_BOND);

            break;
        case SONATA_GAP_LTK_EXCH:
            APP_TRC("ACB: %s  SONATA_GAP_LTK_EXCH\r\n", __FUNCTION__);
            app_sec_add_bonded_dev(peer_dbaddr, &ind->data.ltk, NULL);

            APP_TRC("ACB: Peer LTK Saved in ltk:");
            for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i)
            {
                APP_TRC("%02X ", ind->data.ltk.ltk.key[i]);
            }
            APP_TRC("\r\n");

            break;
        case SONATA_GAP_IRK_EXCH:
            APP_TRC("ACB: %s  SONATA_GAP_IRK_EXCH\r\n", __FUNCTION__);
            APP_TRC_HIGH("ACB: Peer IRK Saved in irk.irk:");
            for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i)
            {
                APP_TRC_HIGH("%02X ", ind->data.irk.irk.key[i]);
            }
            APP_TRC_HIGH("\r\n");
            APP_TRC_HIGH("ACB: Peer Addr In Irk[type=%d]:", ind->data.irk.addr.addr_type);
            for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
            {
                APP_TRC_HIGH("%02X ", ind->data.irk.addr.addr.addr[i]);
            }
            APP_TRC_HIGH("\r\n");
            app_sec_add_bonded_dev(peer_dbaddr, NULL, &ind->data.irk);
            app_sec_add_resolve_list(ind->data.irk.addr.addr.addr, app_sec_get_local_irk(), ind->data.irk.irk.key);
            break;
        case SONATA_GAP_CSRK_EXCH:
            APP_TRC("ACB: %s  SONATA_GAP_CSRK_EXCH\r\n", __FUNCTION__);
            break;
        case SONATA_GAP_REPEATED_ATTEMPT:
            APP_TRC("ACB: %s  SONATA_GAP_REPEATED_ATTEMPT\r\n", __FUNCTION__);
            sonata_ble_gap_disconnect(conidx, SONATA_CO_ERROR_REMOTE_USER_TERM_CON);
            break;
    }
    return CB_DONE;
}


void app_sec_bond(uint8_t conidx)
{
    uint8_t key_size                 = SONATA_GAP_SMP_MAX_ENC_SIZE_LEN;
    enum sonata_gap_oob_auth     oob        = SONATA_GAP_OOB_AUTH_DATA_NOT_PRESENT;
    enum sonata_gap_io_cap       iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    enum sonata_gap_kdist        ikey_dist  = SONATA_GAP_KDIST_NONE;//Initiator key distribution
    enum sonata_gap_kdist        rkey_dist  = SONATA_GAP_KDIST_IDKEY;//Responder key distribution
    enum sonata_gap_auth         auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND;
    enum sonata_gap_sec_req      sec_req    = SONATA_GAP_NO_SEC;
    uint8_t bondType = app_nv_get_bond_type_value();
    switch (bondType)
    {
        case BOND_JUSTWORK:  //************************************************************************
            APP_TRC_HIGH("APP: -----------------------BOND_JUSTWORK--------------------\r\n");
            iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
            ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY;  //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY;  //Responder key distribution
            auth       = SONATA_GAP_AUTH_BOND;
            sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
            break;
        case BOND_JUSTWORK_SC:  //************************************************************************
            APP_TRC_HIGH("APP: -----------------------BOND_JUSTWORK_SC--------------------\r\n");
            iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
            ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Responder key distribution
            auth       = SONATA_GAP_AUTH_SEC_CON | SONATA_GAP_AUTH_BOND;
            sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
            break;
        case BOND_PASSKEY_MASTER_GEN_SLAVE_INPUT: //****************************************************
            APP_TRC_HIGH("APP: -----------------------BOND_PASSKEY_MASTER_GEN_SLAVE_INPUT--------------------\r\n");
            iocap      = SONATA_GAP_IO_CAP_KB_ONLY;
            ikey_dist  = SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;  //Responder key distribution
            auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND;
            sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC;
            break;
        case BOND_PASSKEY_MASTER_INPUT_SLAVE_GEN: //****************************************************
            APP_TRC_HIGH("APP: -----------------------BOND_PASSKEY_MASTER_INPUT_SLAVE_GEN--------------------\r\n");
            iocap      = SONATA_GAP_IO_CAP_KB_DISPLAY;
            ikey_dist  = SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY ;  //Responder key distribution
            auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND;
            sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC;
            break;
        case BOND_NUMBER_COMPARE: //************************************************************
            APP_TRC_HIGH("APP: -----------------------BOND_NUMBER_COMPARE--------------------\r\n");
            iocap      = SONATA_GAP_IO_CAP_KB_DISPLAY;
            ikey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY ;  //Responder key distribution
            auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND |SONATA_GAP_AUTH_SEC_CON;
            sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC;
            break;
        case BOND_OOB_LEGACY:
            APP_TRC_HIGH("APP: -----------------------BOND_OOB_LEGACY--------------------\r\n");
            oob        = SONATA_GAP_OOB_AUTH_DATA_PRESENT;
            iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
            ikey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;   //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY ;  //Responder key distribution
            auth       =  SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND ;
            sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC;
            break;
        case BOND_OOB_SC: //************************************************************
            APP_TRC_HIGH("APP: -----------------------BOND_OOB_SC--------------------\r\n");
            oob        = SONATA_GAP_OOB_AUTH_DATA_PRESENT;
            iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
            ikey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY ;  //Responder key distribution
            auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND |SONATA_GAP_AUTH_SEC_CON;
            sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC;
            break;
        case BOND_OOB_SC_NO_DATA: //************************************************************
            APP_TRC_HIGH("APP: -----------------------BOND_OOB_SC_NO_DATA--------------------\r\n");
            oob        = SONATA_GAP_OOB_AUTH_DATA_NOT_PRESENT;
            iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
            ikey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY;  //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY|SONATA_GAP_KDIST_IDKEY |SONATA_GAP_KDIST_LINKKEY ;  //Responder key distribution
            auth       = SONATA_GAP_AUTH_MITM | SONATA_GAP_AUTH_BOND |SONATA_GAP_AUTH_SEC_CON;
            sec_req    = SONATA_GAP_SEC1_AUTH_PAIR_ENC;
            break;
        case BOND_CUSTOMER: //************************************************************
            APP_TRC_HIGH("APP: -----------------------BOND_CUSTOMER--------------------\r\n");
            iocap      = SONATA_GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
            ikey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY;  //Initiator key distribution
            rkey_dist  = SONATA_GAP_KDIST_ENCKEY | SONATA_GAP_KDIST_IDKEY;  //Responder key distribution
            auth       = SONATA_GAP_AUTH_BOND;
            sec_req    = SONATA_GAP_SEC1_NOAUTH_PAIR_ENC;
            break;
        default:
            APP_TRC("APP: -----------------------BondType Not Support--------------------\r\n");
            return ;
    }
    sonata_ble_gap_bond(conidx, iocap, oob, auth, key_size, ikey_dist, rkey_dist, sec_req);
}

int8_t app_sec_check_device_isbonded(uint16_t in_ediv, uint8_t *in_nb)
{
    APP_TRC("ACB: %s \r\n", __FUNCTION__);
    for (int i = 0; i < MAX_BONDED_DEV_NUM; i++)
    {
        if (in_ediv == bondList.devInfo[i].ltk.ediv &&
            !memcmp(&in_nb[0], bondList.devInfo[i].ltk.randnb, SONATA_RAND_NB_LEN))
        {
            APP_TRC("ACB: %s i = %d\r\n", __FUNCTION__, i);
            return i;
        }
    }
    return INVALID_BONDED_INDEX;
}

void app_sec_resolve_connected_device_addr(uint8_t * addr)
{
    struct sonata_gap_sec_key irk[MAX_BONDED_DEV_NUM] = {0};
    printf("APP: Resolve addr:%02X %02X %02X %02X %02X %02X\r\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    for (int i = 0; i < MAX_BONDED_DEV_NUM; ++i)
    {
        if (addrValid(bondList.devInfo[i].peer_addr)) {
            memcpy(irk[i].key, bondList.devInfo[i].irk.irk, SONATA_GAP_KEY_LEN);
            bondList.total_dev++;
            printf("APP:      Use IRK:");
            for (int j = 0; j < SONATA_KEY_LEN; ++j)        {            printf("%02X ", irk[i].key[j]);          }       printf("\r\n");
        }
    }
    sonata_ble_gap_resolve_address(addr, bondList.total_dev, irk);
}



uint16_t app_sec_gap_encrypt_req_callback(uint8_t conidx, uint16_t in_ediv, uint8_t *in_nb)
{
    APP_TRC("ACB: %s in_ediv=%X(x), in_nb:", __FUNCTION__, in_ediv);
    for (int i = 0; i < SONATA_GAP_RAND_NB_LEN; ++i)
    {
        APP_TRC("%02X ", in_nb[i]);
    }
    APP_TRC("\r\n");
    app_bond = app_nv_get_bonded();
    if (app_nv_get_bonded())
    {
        uint8_t found = false;
        uint16_t keySize = SONATA_GAP_KEY_LEN;
        if (app_nv_get_bonded_device_info(&bondList))
        {
            app_sec_print_bond_info();
            if (in_ediv == 0)
            {
                //Try resolve peer address
                gResolveInfo.conidx = conidx;
                addrCopy(gResolveInfo.peer_addr, gPeerAddr);
                app_sec_resolve_connected_device_addr(gPeerAddr);
            }
            else
            {
                // Check if the provided EDIV and Rand Nb values match with the stored values
                int8_t index = app_sec_check_device_isbonded(in_ediv, in_nb);
                if (index > INVALID_BONDED_INDEX)
                {
                    APP_TRC_HIGH("ACB: %s, found at %d, send encrypt confirm, key:", __FUNCTION__, index);
                    for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i){ APP_TRC_HIGH("%02X ", bondList.devInfo[index].ltk.ltk[i]); }APP_TRC_HIGH("\r\n");
                    found = true;
                    sonata_ble_gap_send_encrypt_cfm(conidx, found, keySize, bondList.devInfo[index].ltk.ltk);
                }
                else
                {
                    APP_TRC_HIGH("ACB: %s, not found, send encrypt confirm\r\n", __FUNCTION__);
                    uint8_t app_ltk_key_zero[SONATA_GAP_KEY_LEN] = {0};
                    sonata_ble_gap_send_encrypt_cfm(conidx, found, keySize, app_ltk_key_zero);
                }
            }
        }
        else
        {
            APP_TRC("Error when read LTK in FS!!!\r\n");
        }
    }
    else
    {
        APP_TRC("ACB: %s, not found, send encrypt confirm\r\n", __FUNCTION__);
        uint8_t app_ltk_key_zero[SONATA_GAP_KEY_LEN] = {0};
        sonata_ble_gap_send_encrypt_cfm(conidx, false, 0, app_ltk_key_zero);
    }

    return CB_DONE;
}

uint16_t app_sec_gap_gen_random_number_callback(uint8_t *number)
{
    APP_TRC("ACB: %s  \r\n", __FUNCTION__);
    if (app_rand_cnt == 1)      // First part of IRK
    {
        memcpy(&app_loc_irk[0], number, 8);
    }
    else if (app_rand_cnt == 2) // Second part of IRK
    {
        memcpy(&app_loc_irk[8], number, 8);
        APP_TRC("app_loc_irk :");
        for (int i = 0; i < SONATA_KEY_LEN; ++i)
        {
            APP_TRC("%02X ", app_loc_irk[i]);
        }
        APP_TRC("\r\n");
    }
    return CB_DONE;
}

uint16_t app_sec_gap_security_callback(uint8_t conidx, uint8_t auth_level)
{
    APP_TRC("ACB: %s  auth_level=%02X(x)\r\n", __FUNCTION__, auth_level);

    return CB_DONE;

}

uint16_t app_sec_gap_encrypt_callback(uint8_t conidx, uint8_t auth_level)
{
    APP_TRC_HIGH("ACB: %s  auth_level=%02X(x)\r\n", __FUNCTION__, auth_level);
    return CB_DONE;
}


uint16_t app_sec_gap_resolving_address_callback(uint8_t operation, uint8_t addr_type, uint8_t *addr)
{
    APP_TRC("ACB: %s  operation=%d(d), addr_type=%d(d)\r\n", __FUNCTION__, operation, addr_type);

    return CB_DONE;
}


uint16_t app_sec_address_resolved_callback(uint8_t *addr, uint8_t *key)
{
    APP_TRC("ACB: %s, addr:%02X %02X %02X %02X %02X %02X\r\n", __FUNCTION__,
           addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    APP_TRC("ACB: %s, Key:", __FUNCTION__);
    for (int i = 0; i < SONATA_GAP_KEY_LEN; ++i)
    {
        APP_TRC("%02X ", key[i]);
    }
    APP_TRC("\r\n");
    if (addrCompare(gResolveInfo.peer_addr, true, addr, true)) {
        APP_TRC("APP: Addr Resolved, send confirm to conidx %d\r\n",gResolveInfo.conidx);
        sonata_ble_gap_send_encrypt_cfm(gResolveInfo.conidx, true, SONATA_GAP_KEY_LEN, bondList.devInfo[0].ltk.ltk);
        memset(&gResolveInfo, 0xFF, sizeof(resolve_info_t));
    }

    return CB_DONE;
}



void app_sec_user_send_pin_code(uint32_t pin_code)
{
    uint8_t data_tk[SONATA_KEY_LEN] = {0};
    APP_TRC_HIGH("APP_SEC: %s  pin_code=%lu,", __FUNCTION__, pin_code);
    sonata_passkey_pincode_to_byte(pin_code, data_tk, SONATA_KEY_LEN);
    APP_TRC_HIGH(" TKey:"); for (int i = 0; i < SONATA_KEY_LEN; ++i){APP_TRC("%02X ", data_tk[i]); } APP_TRC("\r\n");
    sonata_ble_gap_send_bond_cfm_for_tk_exchange(0, true, data_tk);
}

void app_sec_passkey_send_yesno(uint8_t conidx, bool accept)
{
    APP_TRC_HIGH("APP_SEC: %s  accept=%d,", __FUNCTION__, accept);

    //uint8_t data_tk[SONATA_KEY_LEN] = {0};
    //sonata_passkey_pincode_to_byte(app_passkey_pincode, data_tk, SONATA_KEY_LEN);
    //sonata_ble_gap_send_bond_cfm_for_tk_exchange(0, accept, data_tk);
    sonata_ble_gap_send_bond_cfm_for_nc_exchange(conidx, accept);
}

uint8_t * app_sec_get_local_irk()
{
    return app_loc_irk;
}

void app_sec_set_local_irk(uint8_t *key)
{
    if (key == NULL)
    {
        sonata_ble_gap_set_irk(app_loc_irk);
    }
    else
    {
        sonata_ble_gap_set_irk(key);
    }
}

uint16_t app_sec_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam)
{
    switch (opt_id)
    {
        case SONATA_GAP_CMP_PROFILE_TASK_ADD://0x011B
        {

            APP_TRC("APP_COMPLETE: %s  Profile add done. app_bond=%d\r\n", __FUNCTION__, app_bond);
            if (app_bond == true)
            {
                sonata_ble_gap_set_irk(app_loc_irk);
            }
            else
            {
                app_rand_cnt = 1;
                sonata_ble_gap_generate_random_number();//app_sec_gap_gen_random_number_callback
            }
        }
        case SONATA_GAP_CMP_GEN_RAND_NB:
            APP_TRC("APP_COMPLETE: %s  SONATA_GAP_CMP_GEN_RAND_NB,  \r\n", __FUNCTION__);

            if (app_rand_cnt == 1)
            {
                // Generate a second random number
                app_rand_cnt++;
                sonata_ble_gap_generate_random_number();
            }
            else
            {
                app_rand_cnt = 0;
                sonata_ble_gap_set_irk(app_loc_irk);
            }
            break;
        case SONATA_GAP_CMP_SET_IRK:
            APP_TRC("APP_COMPLETE: %s  SONATA_GAP_CMP_SET_IRK,  \r\n", __FUNCTION__);
            app_rand_cnt = 0;

            break;
        default:
            break;
    }
    return 0;

}

void app_sec_set_oobdata(uint8_t * oobC, uint8_t* oobR)
{
    if (oobC != NULL)
    {
        memcpy(gOobConf, oobC, 16);
    }
    if (oobR != NULL)
    {
        memcpy(gOobRand, oobR, 16);
    }
}

void app_sec_send_oob_bond_cfm(uint8_t conidx)
{
    APP_TRC("APP  gOobConf:");
    for (int i = 0; i < 16; ++i)
    {
        APP_TRC("%02X", gOobConf[i]);
    }
    APP_TRC("\r\n");
    APP_TRC("APP  gOobRand:");
    for (int i = 0; i < 16; ++i)
    {
        APP_TRC("%02X", gOobRand[i]);
    }
    APP_TRC("\r\n");
    APP_TRC_HIGH("App: Send oob exchange............ \r\n");
    sonata_ble_gap_send_bond_cfm_for_oob_exchange(conidx, 1, gOobConf, gOobRand);
}








