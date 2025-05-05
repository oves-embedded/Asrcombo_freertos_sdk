
/**
 ****************************************************************************************
 *
 * @file (app_ota.h)
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
#include "app_ota.h"
#include "arch.h"
#include "sonata_ble_api.h"
#include "app.h"
#include "app_nv.h"
#include "sonata_prf_otas_api.h"
/*
 * MACRO DEFINES
 ****************************************************************************************
 */


/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
static bool gOtaStart = false;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

bool app_ota_is_start()
{
    return (gOtaStart == true);
}


void app_ota_disable(bool disable)
{
    APP_TRC("APP: %s  . disable=%d\r\n", __FUNCTION__,disable);
    //sonata_prf_otas_disable(disable);
}

uint8_t app_ota_start_callback(uint8_t length, uint8_t *version)
{

    gOtaStart = true;

    return SONATA_OTAS_STS_OK;
}

void app_ota_before_start_callback(uint8_t conidx)
{
    bool ota = true;
    APP_TRC_HIGH("APP: %s  . \r\n", __FUNCTION__);
    if(ota == true)
    {
        app_ota_disable(false);
        uint8_t con_idx = conidx;
        uint8_t pkt_id = 0;
        uint16_t intv_min = 6; //7.5ms
        uint16_t intv_max = 6; //7.5ms
        uint16_t latency  = 0;
        uint16_t time_out = 500;
        uint16_t ce_len_min = 0xFFFF;
        uint16_t ce_len_max = 0xFFFF;
        sonata_ble_gap_update_connection_params(con_idx, pkt_id, intv_min, intv_max, latency, time_out, ce_len_min, ce_len_max);
        //sonata_api_app_timer_set(APP_TIMER_ADV_END, 500);
    }
    else
    {
        APP_TRC_ERROR("APP: %s  . Battery low, OTA stopped\r\n", __FUNCTION__);
        app_ota_disable(true);
    }
}
void app_ota_stop_callback()
{
    APP_TRC("APP: %s  . \r\n", __FUNCTION__);
    gOtaStart = false;
    //System will reset
    //app_hid_change_conn_params();
    app_nv_set_pair_on_boot(true);
    #ifdef SONATA_CFG_LOW_POWER
    sonata_ble_prevent_sleep_clear(APP_WAKE);
    sonata_sleep_cleanup(ALL_WKUP_SRC_CLR);
    #endif
}

void app_ota_fail_callback()
{
    APP_TRC_ERROR("APP: %s  . \r\n", __FUNCTION__);
    gOtaStart = false;
    #ifdef SONATA_CFG_LOW_POWER
    sonata_ble_prevent_sleep_clear(APP_WAKE);

    #endif
}



