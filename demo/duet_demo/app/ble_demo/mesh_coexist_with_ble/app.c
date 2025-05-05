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
/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdio.h>
#include "arch.h"
#include "app.h"                     // Application Definition
#include "sonata_ble_api.h"
#include "sonata_gap_api.h"
#include "user_platform.h"
#include "sonata_ble_hook.h"
#include "sonata_mesh_api.h"
#include "sonata_error_api.h"
#include "sonata_ble_api.h"
#include "duet_timer.h"
#include "lega_rtos_api.h"

#include "sonata_log.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define DEMO_ADV  0

#define ELEMENT_INDEX 0
 /// Generic OnOff Set Unacknowledged
#define MESH_OPCODE_GEN_OO_SET_UNACK         (0x0382)
 /// vendor OnOff opcode
#define MESH_OPCODE_VENDOR_OO_SET_UNACK           (0x000917d1)
#define MESH_OPCODE_VENDOR_SET                    (0x000917d2)
#define MESH_OPCODE_VENDOR_STATUS                 (0x000917d3)
#define APP_RESET_TIMER_INTERVAL             (6000) //unit:ms
#define APP_TIMER_INTERVAL                   (1000) //unit:ms
#define APP_ATTATION_TIMER                   (3000) //unit:ms
#define APP_RESET_SWITCH_COUNT               (6)
#define APP_RESET_SAVE_TAG                   (APP_DATA_SAVE_TAG_FIRST+0)
#define APP_RESET_SAVE_LEN                   (1)
#define APP_UUID_MAC_OFFSET                  7

#define CFG_APP_MAX_CONNECTTION  5

#define ASR_SZ_COMP_ID                       0x0917

/*!
 * @brief peer device's address, this demo will connect them one by one
 */


static uint16_t demo_handle_id = 0X001E;
uint8_t g_next_write_conidx = 0;


typedef struct rx{
uint16_t addr;
uint16_t rx_num;
}rx_t;
static rx_t rx_count[5]={0};

static uint8_t exist_nb=0;

static uint16_t vendor_msg_count =0;



/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Type of activities that can be created
/*@TRACE*/
enum mesh_actv_type
{
    /// Advertising activity
    MESH_ACTV_TYPE_ADV = 0,
    /// Scanning activity
    MESH_ACTV_TYPE_SCAN,
    /// Initiating activity
    MESH_ACTV_TYPE_INIT,
    /// Periodic synchronization activity
    MESH_ACTV_TYPE_PER_SYNC,
};

typedef struct
{
    uint8_t onoff;
    uint8_t tid;
    uint8_t attr_parameter[];
} __attribute__((packed)) mesh_vendor_model_set_t;
/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/*
 * EXTERNAL FUNCTION DECLARATION
 ****************************************************************************************
 */

extern int __wrap_printf(const char* format, ...);
extern void ble_reset_cmp(void);
extern void sonata_mesh_api_init(void);



static uint16_t app_ble_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam);
static void app_ble_config_scanning(uint16_t intv, uint16_t wnd);
CRITICAL_FUNC_SEG void app_mesh_send_vendor_msg(bool res,uint8_t data_size);


/*!
 * @brief: app profile api init
 * @param none
 * @return none
 */
void app_prf_api_init(void)
{

    sonata_mesh_api_init();

#if DEMO_PRF_DISS
    sonata_prf_diss_init();
#endif
#if DEMO_PRF_BASS
    sonata_prf_bass_init();
#endif
}


/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

static mesh_prov_param_t init_prov_param = {
     .uuid  =  {
     //   0xA8, 0x01, //CID AliBaBa
     //   0xD1, //PID
     //   0x8A, 0x25, 0x00, 0x00, //ProductID: 9610
     //   0x57, 0x5D, 0x69, 0x63, 0xA7, 0xF8, //MAC:f8a763695d57
     //   0x00, //FeatureFlag
     //   0x00, 0x00//RFU
        0x17,0x09,0x1A,0x00,0x02,0x02,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x00,0x00,0x00 },
     .p_uri = NULL,
     .static_oob = 0,
    .static_oob_len = 0,
    .oob_info = 0,
    .output_size = 0,
    .output_actions = 0,
    .input_size = 0,
    .input_actions = 0,
    .cid = ASR_SZ_COMP_ID,
    .pid = 0x001A,
    .vid = 0x0001,
    .loc=  0x0100,
};

static uint8_t static_auth_val[MESH_PROVISION_AUTH_MAX_NUM]  = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
static uint32_t mesh_feature = MESH_FEAT_RELAY_NODE_SUP|MESH_FEAT_PROXY_NODE_SUP;
static duet_timer_dev_t mesh_timer;
static uint8_t reset_flag = 0xff;
static uint8_t irq=0;


static duet_timer_dev_t write_timer;
static duet_timer_dev_t vendor_packet_loss_timer;

static uint8_t g_connect_address[SONATA_GAP_BD_ADDR_LEN] = {0};
static uint8_t g_connection_id = 0xFF;
typedef struct
{
    uint8_t conidx;
    uint8_t address[6];
    bool is_connected;
    uint32_t write_cnt;
} st_conn_state_t;

static st_conn_state_t app_conn_state_list[CFG_APP_MAX_CONNECTTION];

struct
{
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
    bool is_cfg_valid;
} st_conn_cfg = {.is_cfg_valid = FALSE};

static struct {
    uint16_t interval;  // scan interval
    uint16_t window;   // scan window
}st_scan_cfg = {0};



sonata_ble_hook_t app_hook = {
    assert_err,
    assert_param,
    assert_warn,
    app_init,
    platform_reset,
    get_stack_usage,
    __wrap_printf,
    app_prf_api_init,
#ifdef SONATA_RTOS_SUPPORT
    (void *)lega_rtos_init_semaphore,
    (void *)lega_rtos_get_semaphore,
    (void *)lega_rtos_set_semaphore,
#endif
};

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


#if DEMO_ADV
static void app_ble_config_legacy_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);

    sonata_gap_directed_adv_create_param_t param = {0};
    param.disc_mode = SONATA_GAP_ADV_MODE_GEN_DISC;
    param.prop = SONATA_GAP_ADV_PROP_UNDIR_CONN_MASK;
    param.max_tx_pwr = 0xE2;
    param.filter_pol = SONATA_ADV_ALLOW_SCAN_ANY_CON_ANY;
    //msg->adv_param.adv_param.peer_addr.addr.addr:00
    param.addr_type = 0;
    param.adv_intv_min = 160;
    param.adv_intv_max = 160;
    param.chnl_map = 0x07;
    param.phy = SONATA_GAP_PHY_LE_1MBPS;
    //Next event:SONATA_GAP_CMP_ADVERTISING_CONFIG
    uint16_t ret = sonata_ble_config_legacy_advertising(SONATA_GAP_STATIC_ADDR, &param);
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}
#endif

/*!
 * @brief Config scanning
 */
static void app_ble_config_scanning(uint16_t intv, uint16_t wnd)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    if ((intv > 10000 )|| (wnd > 10000)||(wnd ==0)||(intv==0))
    {
        st_scan_cfg.interval = 0x0140;
        st_scan_cfg.window = 0x00A0;
    }
    else
    {
        st_scan_cfg.interval = intv;
        st_scan_cfg.window = wnd;
    }

    uint16_t ret = sonata_ble_config_scanning(SONATA_GAP_GEN_NON_RSLV_ADDR);
    //Next event:SONATA_GAP_CMP_SCANNING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}


/*!
 * @brief set advertising data
 */
static void app_ble_set_adv_data()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint8_t advData[] = {//Advertising data format
            8, SONATA_GAP_AD_TYPE_COMPLETE_NAME, 'A', 'S', 'R', '-', 'B', 'L', 'E'
    };
    uint16_t ret = sonata_ble_set_advertising_data(sizeof(advData), advData);
    //Next event:SONATA_GAP_CMP_SET_ADV_DATA

    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}
/*!
 * @brief Start advertising
 */
static void app_ble_start_advertising()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    uint16_t ret = sonata_ble_start_advertising(0, 0);
    //Next event:SONATA_GAP_CMP_ADVERTISING_START
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

/*!
 * @brief Start scanning
 */
static void app_ble_start_scanning()
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    sonata_gap_scan_param_t param = {0};
    param.type = SONATA_GAP_SCAN_TYPE_OBSERVER;//SONATA_GAP_SCAN_TYPE_GEN_DISC;
    // For continuous scan, use OBSERVER type, use duration to control scan timeer.
    // if duration=0, will scan for ever until sonata_ble_stop_scanning() called
    //param.type = SONATA_GAP_SCAN_TYPE_OBSERVER;
    param.prop = SONATA_GAP_SCAN_PROP_ACTIVE_1M_BIT | SONATA_GAP_SCAN_PROP_PHY_1M_BIT;//0x05
    param.dup_filt_pol = SONATA_GAP_DUP_FILT_DIS;//SONATA_GAP_DUP_FILT_EN;
    param.scan_param_1m.scan_intv = 0x0140;
    param.scan_param_1m.scan_wd = 0x00A0;
    param.scan_param_coded.scan_intv = 0x0140;
    param.scan_param_coded.scan_wd = 0x00A0;
    param.duration = 0;
    param.period = 0;
    uint16_t ret = sonata_ble_start_scanning(&param);
    //Scan result will show in app_gap_scan_result_callback()
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }

}

void app_ble_stop_scanning(void)
{
    sonata_ble_stop_scanning();
}

void app_mesh_stop_scan_cb(void)
{
    app_ble_config_scanning(st_scan_cfg.interval,st_scan_cfg.window);
}

void app_ble_start_scan_with_mesh(uint16_t intv, uint16_t wnd)
{
    st_scan_cfg.interval = intv;
    st_scan_cfg.window   = wnd;
    sonata_mesh_scan_pause(USER_APP_MODULE_ID,app_mesh_stop_scan_cb);
}

void app_mesh_resume_scan_cb(void)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
}

void app_ble_stop_scan_with_mesh(void)
{
    app_ble_stop_scanning();
}

/*!
 * @brief Check MAC address
 * @param address
 * @return
 */
static bool app_ble_check_address(uint8_t *address)
{
    uint8_t error_address[SONATA_GAP_BD_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (address == NULL)
    {
        return false;
    }
    if (memcmp(address, error_address, SONATA_GAP_BD_ADDR_LEN) == 0)
    {
        return false;
    }
    return true;


}
void app_ble_config_initiating(uint8_t *peer_address, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    memcpy(g_connect_address, peer_address, SONATA_GAP_BD_ADDR_LEN);
    st_conn_cfg.interval = interval;
    st_conn_cfg.latency = latency;
    st_conn_cfg.timeout = timeout;
    st_conn_cfg.is_cfg_valid = TRUE;
    //Call api to config init
    uint16_t ret = sonata_ble_config_initiating(SONATA_GAP_GEN_NON_RSLV_ADDR);
    //Next event:SONATA_GAP_CMP_INITIATING_CONFIG
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

/*!
 * @brief Start initiating
 */
static void app_ble_start_initiating(uint8_t *target)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    if (app_ble_check_address(target) == false)
    {
        APP_TRC("APP: %s, Target address is not right. Stop\r\n", __FUNCTION__);
        return;
    }
    sonata_gap_init_param_t param = {0};
    param.type = SONATA_GAP_INIT_TYPE_DIRECT_CONN_EST;
    param.prop = SONATA_GAP_INIT_PROP_1M_BIT | SONATA_GAP_INIT_PROP_2M_BIT | SONATA_GAP_INIT_PROP_CODED_BIT;
    param.conn_to = 0;
    param.peer_addr.addr_type = SONATA_GAP_STATIC_ADDR; //  Addr
    memcpy(param.peer_addr.addr.addr, target, SONATA_GAP_BD_ADDR_LEN);

    if (param.prop & SONATA_GAP_INIT_PROP_1M_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_1M_BIT \r\n", __FUNCTION__, param.prop);
        param.scan_param_1m.scan_intv = 0x0200;
        param.scan_param_1m.scan_wd = 0x0100;
        if (st_conn_cfg.is_cfg_valid)
        {
            param.conn_param_1m.conn_intv_min = st_conn_cfg.interval;
            param.conn_param_1m.conn_intv_max = st_conn_cfg.interval;
            param.conn_param_1m.conn_latency = st_conn_cfg.latency;
            param.conn_param_1m.supervision_to = st_conn_cfg.timeout;
            APP_TRC("connect params: %u, %u %u", st_conn_cfg.interval, st_conn_cfg.latency, st_conn_cfg.timeout);
        }
        else
        {
        param.conn_param_1m.conn_intv_min = 0x0028;
        param.conn_param_1m.conn_intv_max = 0x0028;
        param.conn_param_1m.conn_latency = 0;
        param.conn_param_1m.supervision_to = 0x02BC;
        }
        param.conn_param_1m.ce_len_min = 0x0008;
        param.conn_param_1m.ce_len_max = 0x0008;
    }
    if (param.prop & SONATA_GAP_INIT_PROP_2M_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_2M_BIT \r\n", __FUNCTION__, param.prop);

        if (st_conn_cfg.is_cfg_valid)
        {
            param.conn_param_2m.conn_intv_min = st_conn_cfg.interval;
            param.conn_param_2m.conn_intv_max = st_conn_cfg.interval;
            param.conn_param_2m.conn_latency = st_conn_cfg.latency;
            param.conn_param_2m.supervision_to = st_conn_cfg.timeout;
        }
        else
        {
            param.conn_param_2m.conn_intv_min = 0x0028;
            param.conn_param_2m.conn_intv_max = 0x0028;
            param.conn_param_2m.conn_latency = 0;
            param.conn_param_2m.supervision_to = 0x02BC;
        }

        param.conn_param_2m.ce_len_min = 0x0008;
        param.conn_param_2m.ce_len_max = 0x0008;
    }
    if (param.prop & SONATA_GAP_INIT_PROP_CODED_BIT)
    {
        APP_TRC("APP: %s  (%02X)set SONATA_GAP_INIT_PROP_CODED_BIT \r\n", __FUNCTION__, param.prop);
        if (st_conn_cfg.is_cfg_valid)
        {
            param.conn_param_coded.conn_intv_min = st_conn_cfg.interval;
            param.conn_param_coded.conn_intv_max = st_conn_cfg.interval;
            param.conn_param_coded.conn_latency = st_conn_cfg.latency;
            param.conn_param_coded.supervision_to = st_conn_cfg.timeout;
        }
        else {
            param.conn_param_coded.conn_intv_min = 0x0028;
            param.conn_param_coded.conn_intv_max = 0x0028;
            param.conn_param_coded.conn_latency = 0;
            param.conn_param_coded.supervision_to = 0x02BC;
        }
        param.scan_param_coded.scan_intv = 0x0200;
        param.scan_param_coded.scan_wd = 0x0100;

        param.conn_param_coded.ce_len_min = 0x0008;
        param.conn_param_coded.ce_len_max = 0x0008;
    }

    uint16_t ret = sonata_ble_start_initiating(&param);
    //Next event:If connected, SONATA_GAP_CMP_INITIATING_DELETE event will be received
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

uint16_t app_ble_stop_initiating(void)
{
    uint16_t val;
    val=sonata_ble_stop_initiating();
    return val;
}


static struct
{
    uint16_t handle;
    uint8_t conidx;
    uint8_t interval;
    uint8_t *data_buf;
    uint16_t data_len;
    bool is_enable;
    bool is_timeout;
} st_write_cfg = {.handle=0x00, .conidx=0xFF, .data_buf=NULL, .data_len=0, .is_enable=FALSE};

bool conn_wr_state_list[CFG_APP_MAX_CONNECTTION] = {FALSE};
static uint16_t cnt=0;
CRITICAL_FUNC_SEG void write_timer_cb(void *arg)
{
    if (st_write_cfg.is_enable && st_write_cfg.data_buf)
    {
        sonata_ble_gatt_write(st_write_cfg.conidx, st_write_cfg.handle, 0, 0, st_write_cfg.data_len, st_write_cfg.data_buf);
        cnt++;
    }
    APP_TRC("write event cnt=%u\r\n", cnt);
}

static void write_timer_init(uint8_t time)
{
    APP_TRC("app_timer_init\r\n");
    write_timer.port = DUET_TIMER2_INDEX;
    write_timer.config.period = time*1000000;
    write_timer.config.reload_mode = TIMER_RELOAD_AUTO;
    write_timer.config.cb = write_timer_cb;
    write_timer.config.arg = NULL;
    write_timer.priv = NULL;
    duet_timer_init(&write_timer);
    duet_timer_start(&write_timer);
}

void app_ble_config_write(uint8_t conidx, uint16_t peer_handle, uint8_t data_len, uint8_t interval)
{
    st_write_cfg.conidx = conidx;
    st_write_cfg.handle = peer_handle;
    st_write_cfg.interval = interval;
    st_write_cfg.is_enable = TRUE;
    st_write_cfg.data_len = data_len;
    write_timer_init(st_write_cfg.interval);
    if (NULL == st_write_cfg.data_buf)
    {
        st_write_cfg.data_buf = malloc(st_write_cfg.data_len);
        for (int i=0; i<st_write_cfg.data_len; i++)
        {
            st_write_cfg.data_buf[i] = i;
        }
    }
    sonata_ble_gatt_write(st_write_cfg.conidx, st_write_cfg.handle, 0, 0, st_write_cfg.data_len, st_write_cfg.data_buf);
    cnt++;
    APP_TRC("write event cnt=%u\r\n", cnt);
}

void app_ble_stop_write(void) {
    st_write_cfg.is_enable = FALSE;
    free(st_write_cfg.data_buf);
    st_write_cfg.data_buf = NULL;
    duet_timer_stop(&write_timer);
    cnt=0;
}

uint8_t app_ble_get_max_connection(void)
{
    return CFG_APP_MAX_CONNECTTION;
}

void app_ble_disconnect(uint8_t conidx)
{
    sonata_ble_gap_disconnect(conidx,0);
}
static void app_ble_on(void)
{
    APP_TRC("APP: %s  \r\n", __FUNCTION__);
    sonata_gap_set_dev_config_cmd cmd = {0};
    cmd.role = SONATA_GAP_ROLE_ALL;
    cmd.gap_start_hdl = 0;
    cmd.gatt_start_hdl = 0;
    cmd.renew_dur = 0x0096;
    cmd.privacy_cfg = 0;
    cmd.pairing_mode = SONATA_GAP_PAIRING_SEC_CON | SONATA_GAP_PAIRING_LEGACY;
    cmd.att_cfg = 0x0080;
    cmd.max_mtu = 0x02A0;
    cmd.max_mps = 0x02A0;
    cmd.max_nb_lecb = 0x0A;
    cmd.hl_trans_dbg = false;
    uint16_t ret = sonata_ble_on(&cmd);//app_ble_on_complete_callback
    if (ret != API_SUCCESS)
    {
        APP_TRC("APP: %s  ERROR:%02X\r\n", __FUNCTION__, ret);
    }
}

uint16_t gap_active_stopped_callback(uint8_t actv_idx, uint8_t type,uint8_t reason,uint8_t per_adv_stop)
{
    APP_TRC("APP_CB: %s, reason=0x%04X\r\n", __FUNCTION__,reason);
    if(MESH_ACTV_TYPE_ADV == type)
    {

    }
    if(MESH_ACTV_TYPE_SCAN == type)
    {
        return CB_DONE; //not delete scan instance
    }
    if(MESH_ACTV_TYPE_INIT == type)
    {
        return CB_REJECT; //delete init instance
    }
    return CB_DONE;
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
STATUS mesh_core_evt_ind(mesh_core_evt_ind_t evt ,mesh_core_evt_ind_params_t * p_param)
{

    switch(evt)
    {
        case (MESH_PUBLIC_KEY_REQUEST):
        {

        }
        break;
        case MESH_AUTH_OOB_IND:
        {
            sonata_mesh_send_prov_auth_data_rsp(true,MESH_PROVISION_AUTH_MAX_NUM,static_auth_val);
        }
        break;
        case MESH_ATTATION_IND:
        {

        }
        break;
        case MESH_APPKEY_UPDATE:
        {

        }
        break;
        case MESH_APPKEY_BINDED:
        {

        }
        break;
        case MESH_GRPADDR_SUBSED:
        {

        }
        break;
        case MESH_PROVISION_STATE:
        {
            APP_TRC("MESH_PROVISION_STATE %d\r\n",p_param->prov_state_ind.state);
            if(p_param->prov_state_ind.state) // true means be provisioned success
            {
                sonata_mesh_save_data();
            }
        }
        break;
        case MESH_APPKEY_BINDING:
        {
            APP_TRC("MESH_APPKEY_BINDING add %u :element %u, modelid 0x%x, index %u \r\n  ",p_param->appkey_bind.bind,
                p_param->appkey_bind.element ,(unsigned int )p_param->appkey_bind.modelid,p_param->appkey_bind.appkey_global_index);
            sonata_mesh_save_data();
        }
        break;
        default: break;
    }
    return MESH_ERROR_NO_ERROR;

}

static void mesh_print_hex(uint8_t *hex, uint8_t len )
{
    for(int i=0; i< len; i++)
    {
        printf("%x%x",hex[i]>>4,hex[i]&0xf);
    }
    printf("\r\n");
}

void  gens_onoff_cb(mesh_state_ind_t * p_state )
{
    if(p_state->type != MESH_STATE_GEN_ONOFF)
    {
        return;
    }
    if(p_state->state)
    {
        turn_on_led();
    }
    else
    {
        turn_off_led();
    }
}

void  venc_onoff_cb(mesh_api_model_msg_rx_t *msg_rx )
{
    if((msg_rx->opcode != MESH_OPCODE_VENDOR_OO_SET_UNACK)
        &&(msg_rx->opcode != MESH_OPCODE_VENDOR_SET)
        &&(msg_rx->opcode != MESH_OPCODE_VENDOR_STATUS))
    {
        return;
    }
    mesh_vendor_model_set_t *vendor_set = (mesh_vendor_model_set_t *)sonata_api_malloc(msg_rx->msg_len);
    memmove(vendor_set,msg_rx->msg,msg_rx->msg_len);
    printf("tid:%d\r\n",vendor_set->tid);

    int i;

    uint8_t is_inarry=0;

    for(i=0;i<5;i++)
    {
        if(rx_count[i].addr==msg_rx->src)
        {
            is_inarry=1;

            break;
        }
    }

    if((is_inarry)&&(i < 5))
    {
        rx_count[i].rx_num++;
    }
    else
    {
        if(exist_nb>4)
        {
            printf("arry is full\r\n");
            return;
        }
        rx_count[exist_nb].addr=msg_rx->src;
        rx_count[exist_nb].rx_num++;
        exist_nb++;
    }
    int j;

    for(j=0;j<exist_nb;j++)
    {
        printf("unicast address[%d]:vendor_rx_num:%d\r\n",rx_count[j].addr,rx_count[j].rx_num);
    }


    mesh_print_hex(vendor_set->attr_parameter,msg_rx->msg_len-2);
    if(msg_rx->opcode == MESH_OPCODE_VENDOR_SET)
    {
        app_mesh_send_vendor_msg(0,0);
    }
    vendor_msg_count++;
    printf("sever_vendor msg_count:%d\n",vendor_msg_count);

    if(vendor_set->onoff)
    {
        turn_on_led();    //test
    }
    else
    {
        turn_off_led();
    }
    sonata_api_free(vendor_set);
}

void started_cb(void)
{
    // user_load_data(); // user

     if(!sonata_mesh_is_provisioned())
     {
          APP_TRC("auto mesh_provision_service_broadcast\r\n");
     }
     else
     {

         APP_TRC("sonata_mesh_proxy_service_broadcast\r\n");
         sonata_mesh_proxy_service_broadcast(MESH_PROXY_ADV_START_NET);
     }
}



static void app_ble_on_callback(uint16_t status)
{
    APP_TRC("bluetooth on: 0x%X\r\n", status);
    mesh_set_params_t set_param;
    sonata_mesh_register_core_evt_ind(mesh_core_evt_ind);
    set_param.mesh_role.role = MESH_PROV_ROLE_DEVICE;
    sonata_mesh_param_set(MESH_ROLE,&set_param);

    set_param.addr_type.type = MESH_PRIVATE_STATIC_ADDR;
    sonata_mesh_param_set(MESH_ADDR_TYPE,&set_param);
    set_param.feature_param.feature = mesh_feature;
    sonata_mesh_param_set(MESH_FEATURE_SUPPORT,&set_param);

    // for mesh multi-node test
    mesh_get_params_t get_param;
    sonata_mesh_param_get(MESH_SYSTEM_ADDR,&get_param);
    memmove(init_prov_param.uuid+APP_UUID_MAC_OFFSET,get_param.addr.bd_addr,MESH_ADDR_LEN);
    set_param.prov_param = init_prov_param;
    sonata_mesh_param_set(MESH_PROV_PARAM,&set_param);
    sonata_mesh_model_register(MESH_MODELID_GENS_OO,0,1,gens_onoff_cb);
    mesh_vendor_model_register(MESH_MODELID_VENC_ASR,ELEMENT_INDEX,venc_onoff_cb);
    sonata_mesh_start(started_cb);
    app_hal_init();
}

STATUS publish_msg_sent(mesh_model_publish_param_t * p_param ,STATUS status)
{
    irq = 0;
    sonata_api_free(p_param);
    return API_SUCCESS;
}

static uint16_t send_count=0;

CRITICAL_FUNC_SEG void app_mesh_send_vendor_msg(bool res,uint8_t data_size)
{
    static uint8_t onoff = 1;
    static uint8_t tid = 1;
    if(irq == 1)
    {
         return;
    }
    irq = 1;
    APP_TRC("[APP_TRC]app_mesh_send_vendor_msg\r\n");
    //mesh_model_publish_param_t *msg_param = sonata_api_malloc(sizeof(mesh_model_publish_param_t) + sizeof(vendor_onoff_no_ack_t));
    //vendor_onoff_no_ack_t onoff_param;
    mesh_vendor_model_set_t *onoff_param = (mesh_vendor_model_set_t *)sonata_api_malloc(sizeof(mesh_vendor_model_set_t) + data_size);
    mesh_model_publish_param_t *msg_param = sonata_api_malloc(sizeof(mesh_model_publish_param_t) + sizeof(mesh_vendor_model_set_t) + data_size);
    msg_param->element = ELEMENT_INDEX;
    msg_param->modelid = MESH_MODELID_VENC_ASR;
    if(!res)
    {
        msg_param->opcode = MESH_OPCODE_VENDOR_OO_SET_UNACK;
    }
    else
    {
        msg_param->opcode = MESH_OPCODE_VENDOR_SET;
    }
    msg_param->len = sizeof(mesh_vendor_model_set_t) + data_size;
    onoff_param->onoff = onoff;
    send_count++;
    printf("tid:%d,send_count:%u\r\n",tid,send_count);
    onoff_param->tid = tid++;
    memset(onoff_param->attr_parameter,0x01, data_size);
    msg_param->trans_mic_64 = 0;
    memmove(msg_param->msg,onoff_param,sizeof(mesh_vendor_model_set_t)+data_size);
    sonata_api_free(onoff_param);
    mesh_msg_publish(msg_param,publish_msg_sent);
    onoff = 1 - onoff;
}

/*!
 * @brief GATT Read
 * @param connection_id
 */
static void app_gatt_read(uint8_t connection_id)
{
    sonata_ble_gatt_read_by_handle(connection_id, demo_handle_id);//app_disc_read_complete_callback //app_gatt_read_callback
}

static uint8_t gatt_read_cnt = 0;

static uint16_t app_ble_complete_event_handler(sonata_ble_complete_type opt_id, uint8_t status, uint16_t param, uint32_t dwparam)
{
    APP_TRC("APP_COMPLETE: %s  opt_id=%04X,status=%02X,param=%04X,dwparam=%lu\r\n", __FUNCTION__, opt_id, status, param, dwparam);
    switch (opt_id)
    {
        case SONATA_GAP_CMP_BLE_ON://0x0F01
            app_ble_on_callback(status);
            break;
        case SONATA_GAP_CMP_RESET:
            user_peri_deinit();
            ble_reset_cmp();
            break;
        case SONATA_GAP_CMP_ADVERTISING_CONFIG://0x0F02
            app_ble_set_adv_data();
            break;
        case SONATA_GAP_CMP_SCANNING_CONFIG ://0x0F03
            app_ble_start_scanning();
            break;
        case SONATA_GAP_CMP_SCANNING_STOP://0x0F0B
            sonata_mesh_scan_resume(USER_APP_MODULE_ID,app_mesh_resume_scan_cb);
            return CB_DONE; //not delete scan instance
        case SONATA_GAP_CMP_INITIATING_CONFIG ://0x0F04
            app_ble_start_initiating(g_connect_address);
            break;
        case SONATA_GAP_CMP_SCANNING_DELETE ://0x0F0F
            break;
        case SONATA_GAP_CMP_INITIATING_DELETE ://0x0F10
            APP_TRC("APP: %s  SONATA_GAP_CMP_INITIATING_DELETE\r\n", __FUNCTION__);
            sonata_ble_gatt_disc_all_characteristic(g_connection_id, 1, 0XFFFF);
            break;
        case SONATA_GAP_CMP_SET_ADV_DATA://0x01A9
            app_ble_start_advertising();
            break;
        case SONATA_GATT_CMP_DISC_ALL_SVC://0x0402
        {
            uint8_t connection_id = param;
            app_gatt_read(connection_id);
            break;
        }
        case SONATA_GATT_CMP_READ ://0x0408
            gatt_read_cnt++;
            if (gatt_read_cnt < 10)
            {
                sonata_ble_gatt_read_by_handle(param, demo_handle_id);
            }
            break;
        case SONATA_GATT_CMP_WRITE:
            //APP_TRC("write cmp event ,param=%04X,dwparam=%lu\r\n", param, dwparam);
            //APP_TRC("write cmp event, conidx=%u, cnt=%lu\r\n", param, dwparam);
            break;
        default:
            break;
    }

    return CB_DONE;

}

/*!
 * @brief GAP scan result callback
 * @param result
 */
static uint16_t app_gap_scan_result_callback(sonata_gap_ext_adv_report_ind_t *result)
{
    APP_TRC("APP_CB: %s  ", __FUNCTION__);
    APP_TRC("target_addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
    {
        APP_TRC("%02X ", result->target_addr.addr.addr[i]);
    }
    APP_TRC("  trans_addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i)
    {
        APP_TRC("%02X ", result->trans_addr.addr.addr[i]);
    }

    APP_TRC(" \r\n");
    return CB_DONE;
}


/*!
 * @brief Deal with peer device get local information request.
 * @param opt @see asr_gap_dev_info
 */
static uint16_t app_gap_peer_get_local_info_callback(uint8_t conidx, sonata_gap_dev_info opt)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    switch (opt)
    {
        case SONATA_GAP_DEV_NAME:
        {
            uint8_t dev_name[] = {'A', 'S', 'R', 0};
            sonata_ble_gap_send_get_dev_info_cfm_for_dev_name(conidx, 3, dev_name);
        }
            break;

        case SONATA_GAP_DEV_APPEARANCE:
        {
            uint16_t appearance = 0;
            sonata_ble_gap_send_get_dev_info_cfm_for_dev_appearance(conidx, appearance);

        }
            break;

        case SONATA_GAP_DEV_SLV_PREF_PARAMS:
        {
            sonata_ble_gap_send_get_dev_info_cfm_for_slv_pref_params(conidx, 8, 10, 0, 200);

        }
            break;

        default:
            break;
    }

    return CB_DONE;
}

/*!
 * @brief Disconnect indicate
 * @param conidx
 * @param conhdl
 * @param reason
 * @return
 */
static uint16_t app_gap_disconnect_ind_callback(uint8_t conidx, uint16_t conhdl, uint8_t reason)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    int i = 0;
    for (i = 0; i < CFG_APP_MAX_CONNECTTION; i++)
    {
        if (app_conn_state_list[i].conidx == conidx)
        {
            app_conn_state_list[i].is_connected = FALSE;
        }
    }
    return CB_DONE;
}


bool app_ble_get_connection_status(uint8_t conidx)
{
    for (int i=0; i<CFG_APP_MAX_CONNECTTION; i++)
    {
        if (app_conn_state_list[i].is_connected && app_conn_state_list[i].conidx == conidx)
        {
            return TRUE;
        }
    }
    return FALSE;
}

static uint16_t app_gap_connection_req_callback(uint8_t conidx, sonata_gap_connection_req_ind_t *req)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    APP_TRC("peer_addr:");
    for (int i = 0; i < SONATA_GAP_BD_ADDR_LEN; ++i) { APP_TRC("%02X ", req->peer_addr.addr[i]); }
    APP_TRC(" \r\n");
    g_connection_id = conidx;
    uint32_t i = 0;
    for (i=0; i<CFG_APP_MAX_CONNECTTION; i++)
    {
        if (app_conn_state_list[i].conidx == 0xFF)
        {
            app_conn_state_list[i].conidx = conidx;
            memcpy(app_conn_state_list[i].address, req->peer_addr.addr, SONATA_GAP_BD_ADDR_LEN);
            app_conn_state_list[i].is_connected = TRUE;
            break;
        }
        else if (app_conn_state_list[i].conidx == conidx)
        {
            app_conn_state_list[i].is_connected = TRUE;
            break;
        }
    }
    return CB_REJECT; //SDK will send connection confirm message
}


/*!
 * @brief Callback for get peer devices att information,
 * @param info
 */
static uint16_t app_gap_peer_att_info_callback(uint8_t conidx, sonata_gap_peer_att_info_ind_t *info)
{
    APP_TRC("APP_CB: %s  conidx=%02X\r\n", __FUNCTION__, conidx);
    sonata_gap_peer_att_info_ind_t *att_info = (sonata_gap_peer_att_info_ind_t *) info;
    APP_TRC("APP_CB: %s, req =0x%02X\r\n", __FUNCTION__, att_info->req);
    APP_TRC("APP_CB: %s, handle =0x%02X\r\n", __FUNCTION__, att_info->handle);
    switch (att_info->req)
    {
        case SONATA_GAP_DEV_NAME:
        {
            APP_TRC("APP  %s, Name[%d]:", __FUNCTION__, conidx);
            for (int i = 0; i < att_info->info.name.length; ++i)
            {
                APP_TRC("%c", att_info->info.name.value[i]);
            }
            APP_TRC("\r\n");
        }
            break;
        case SONATA_GAP_DEV_APPEARANCE:
        {
            APP_TRC("APP_CB: %s, appearance =0x%04X\r\n", __FUNCTION__, att_info->info.appearance);
        }
            break;
        case SONATA_GAP_DEV_SLV_PREF_PARAMS:
        {
            APP_TRC("APP_CB: %s, con_intv_min =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.con_intv_min);
            APP_TRC("APP_CB: %s, con_intv_max =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.con_intv_max);
            APP_TRC("APP_CB: %s, slave_latency =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.slave_latency);
            APP_TRC("APP_CB: %s, conn_timeout =0x%02X\r\n", __FUNCTION__, att_info->info.slv_pref_params.conn_timeout);
        }
            break;
        case SONATA_GAP_DEV_CTL_ADDR_RESOL:
        {
            APP_TRC("APP_CB: %s, ctl_addr_resol =0x%02X\r\n", __FUNCTION__, att_info->info.ctl_addr_resol);
        }
            break;

        default:
            break;

    }
    return CB_DONE;
}

/*!
 * @brief Callback for get peer devices information,
 * @param conidx
 * @param info
 * @return
 */
static uint16_t app_gap_peer_info_callback(uint8_t conidx, sonata_gap_peer_info_ind_t *info)
{
    APP_TRC("APP_CB: %s  conidx=%02X, info_type=%02X\r\n", __FUNCTION__, conidx, info->req);
    switch (info->req)
    {
        case SONATA_GET_PEER_VERSION:
            APP_TRC("APP_CB: SONATA_GET_PEER_VERSION, compid:%04X,lmp_subvers:%04X,lmp_vers:%02X,\r\n",
                    info->info.version.compid, info->info.version.lmp_subvers, info->info.version.lmp_vers);
            break;
        case SONATA_GET_PEER_FEATURES:
            APP_TRC("APP_CB: SONATA_GET_PEER_FEATURES, features:");
            for (int i = 0; i < SONATA_GAP_LE_FEATS_LEN; ++i)
            {
                APP_TRC("%02x ", info->info.features.features[i]);
            }
            APP_TRC("\r\n");
            break;
        case SONATA_GET_PEER_CON_RSSI:
            APP_TRC("APP_CB: SONATA_GET_PEER_CON_RSSI, rssi:%04X\r\n", info->info.rssi.rssi);

            break;
        case SONATA_GET_PEER_CON_CHANNEL_MAP:
            APP_TRC("APP_CB: SONATA_GET_PEER_CON_CHANNEL_MAP, map:");
            for (int i = 0; i < SONATA_GAP_LE_CHNL_MAP_LEN; ++i)
            {
                APP_TRC("%02x ", info->info.channel_map.ch_map.map[i]);
            }
            APP_TRC("\r\n");
            break;
        case SONATA_GET_LE_PING_TO:
            APP_TRC("APP_CB: SONATA_GET_LE_PING_TO, timeout:%04X,\r\n", info->info.ping_to_value.timeout);
            break;
        case SONATA_GET_PHY:
            APP_TRC("APP_CB: SONATA_GET_PHY, tx_phy:%02X, rx_phy:%02X\r\n", info->info.le_phy.tx_phy, info->info.le_phy.rx_phy);
            break;
        case SONATA_GET_CHAN_SEL_ALGO:
            APP_TRC("APP_CB: SONATA_GET_CHAN_SEL_ALGO, chan_sel_algo:%04X,\r\n", info->info.sel_algo.chan_sel_algo);
            break;
        default:
            break;
    }

    return CB_DONE;
}


/*!
 * @brief Discover service callback
 * @param connection_id
 * @param start_hdl
 * @param end_hdl
 * @param uuid_len
 * @param uuid
 */
static uint16_t app_gatt_disc_svc_callback(uint8_t connection_id, uint16_t start_hdl, uint16_t end_hdl, uint8_t uuid_len, uint8_t *uuid)
{
    APP_TRC("APP_CB: %s, start_hdl=0x%04X, end_hdl =0x%04X, uuid=", __FUNCTION__, start_hdl, end_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        APP_TRC("%02X", uuid[i]);
    }
    APP_TRC("\r\n");
    return CB_DONE;
}

/*!
 * @brief Discover characteristic callback
 * @param conidx
 * @param attr_hdl
 * @param pointer_hdl
 * @param prop
 * @param uuid_len
 * @param uuid
 * @return
 */
static uint16_t app_gatt_disc_char_callback(uint8_t conidx, uint16_t attr_hdl, uint16_t pointer_hdl, uint8_t prop, uint8_t uuid_len, uint8_t *uuid)
{
    APP_TRC("APP_CB: %s, attr_hdl=0x%04X, uuid=", __FUNCTION__, attr_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        APP_TRC("%02X", uuid[i]);
    }
    APP_TRC("\r\n");
    return CB_DONE;
}

/*!
 * @brief Discover description callback
 * @param conidx
 * @param attr_hdl
 * @param uuid_len
 * @param uuid
 * @return
 */
static uint16_t app_gatt_disc_desc_callback(uint8_t conidx, uint16_t attr_hdl, uint8_t uuid_len, uint8_t *uuid)
{
    APP_TRC("APP_CB: %s, attr_hdl=0x%04X, uuid=", __FUNCTION__, attr_hdl);
    for (int i = 0; i < uuid_len; ++i)
    {
        APP_TRC("%02X", uuid[i]);
    }
    APP_TRC("\r\n");
    return CB_DONE;
}

/*!
 * @brief
 * @param connection_id
 * @param handle
 * @param offset
 * @param length
 * @param value
 */
static uint16_t app_gatt_read_callback(uint8_t connection_id, uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    APP_TRC("APP_CB: %s, handle=0x%04X, offset=0x%04X, VALUE=", __FUNCTION__,
            handle, offset);
    for (int i = 0; i < length; ++i)
    {
        APP_TRC("%02x", value[i]);
    }
    APP_TRC("\r\n");
    return CB_DONE;
}

/*!
 * @brief Mtu changed
 * @param connection_id
 * @param mtu
 * @return
 */
static uint16_t app_gatt_mtu_changed_callback(uint8_t connection_id, uint16_t mtu)
{
    APP_TRC("APP_CB: %s, mtu=0x%04X\r\n", __FUNCTION__, mtu);
   return CB_DONE;
}
static uint16_t count =0;
uint16_t gatt_event_callback(uint8_t conidx, uint16_t handle, uint16_t type, uint16_t length, uint8_t *value)
{
    count++;
    printf("count:%d\r\n",count);
    return CB_DONE;
}


static ble_gap_callback ble_gap_callbacks =
{
    /*************** GAP Manager's callback ***************/
    //Must if use scan function, peer's information will show in this callback
    .gap_scan_result                    = app_gap_scan_result_callback,
    /*************** GAP Controller's callback  ***************/
    //Optional, used for get peer att information when call  sonata_ble_gap_get_peer_info()
    .gap_get_peer_info                  = app_gap_peer_info_callback,
    //Optional, used for get peer att information when call  sonata_ble_gap_get_peer_info()
    .gap_get_peer_att_info              = app_gap_peer_att_info_callback,
    //Optional, if peer device get local device's information, app can deal with it in this callback
    .gap_peer_get_local_info            = app_gap_peer_get_local_info_callback,
    //Optional
    .gap_disconnect_ind                 = app_gap_disconnect_ind_callback,
    .gap_connection_req                 = app_gap_connection_req_callback,
    .gap_active_stopped                 = gap_active_stopped_callback,
};

static ble_gatt_callback ble_gatt_callbacks =
{
    //Must if use discovery all servcie function
    .gatt_disc_svc                      = app_gatt_disc_svc_callback,
    //Must if use discovery all characteristic function
    .gatt_disc_char                     = app_gatt_disc_char_callback,
    //Must if use discovery all description function
    .gatt_disc_char_desc                = app_gatt_disc_desc_callback,
    //Must if use read attribute function
    .gatt_read                          = app_gatt_read_callback,
    //Optional, add this callback if app need to save changed mtu value
    .gatt_mtu_changed                   = app_gatt_mtu_changed_callback,
    .gatt_event                         = gatt_event_callback,
};


static ble_complete_callback ble_complete_callbacks =
{
    //Must, app can do next operation in this callback
    .ble_complete_event                 = app_ble_complete_event_handler,
};

void vendor_packet_loss_timer_cb(void *arg);
static uint16_t vendor_count=0;
static uint16_t vendor_nb=0;
static uint16_t data_len=0;

static bool response=0;

CRITICAL_FUNC_SEG void vendor_packet_loss_timer_cb(void *arg)
{
    vendor_count++;
    app_mesh_send_vendor_msg(response,data_len);
    if(vendor_count==vendor_nb)
    {
        duet_timer_stop(&vendor_packet_loss_timer);
        vendor_count=0;
    }
}


void vendor_packet_loss_test(int argc,int time,int number,uint8_t len)
{
    vendor_nb=number;
    response=argc;
    data_len=len;
    vendor_packet_loss_timer.config.arg=NULL;
    vendor_packet_loss_timer.config.cb=vendor_packet_loss_timer_cb;
    vendor_packet_loss_timer.config.period=time*1000000;
    vendor_packet_loss_timer.config.reload_mode=TIMER_RELOAD_AUTO;
    vendor_packet_loss_timer.port=DUET_TIMER1_INDEX;
    vendor_packet_loss_timer.priv=NULL;
    duet_timer_init(&vendor_packet_loss_timer);
    duet_timer_start(&vendor_packet_loss_timer);
}


static void app_clean_data_cb(void)
{
    platform_reset(0);
}

CRITICAL_FUNC_SEG void app_reset_timer_cb(void *arg)
{
    sonata_fs_len_t len = APP_RESET_SAVE_LEN;
    sonata_fs_tag_t tag = APP_RESET_SAVE_TAG;
    static uint32_t time_tick = 0;
    time_tick++;
    if(    reset_flag == 0xFE)
    {
        toggle_led();
        if(time_tick >= APP_ATTATION_TIMER/APP_TIMER_INTERVAL)
        {
            reset_flag = 0xFF;
            sonata_fs_write(tag,len,&reset_flag);
            sonata_mesh_clean_data(app_clean_data_cb);
        }
        return;
    }
    if(time_tick > APP_RESET_TIMER_INTERVAL/APP_TIMER_INTERVAL)
    {
       duet_timer_stop(&mesh_timer);
       reset_flag = 0xFF;
       sonata_fs_write(tag,len,&reset_flag);
       duet_timer_stop(&mesh_timer);
    }

}

static void app_timer_init(void)
{
    APP_TRC("app_timer_init\r\n");
    mesh_timer.port = DUET_TIMER1_INDEX;
    mesh_timer.config.period = APP_TIMER_INTERVAL*1000;
    mesh_timer.config.reload_mode = TIMER_RELOAD_AUTO;
    mesh_timer.config.cb = app_reset_timer_cb;
    mesh_timer.config.arg = NULL;
    mesh_timer.priv = NULL;
    duet_timer_init(&mesh_timer);
    duet_timer_start(&mesh_timer);
}


static void app_reset_process(void)
{
    sonata_fs_len_t len = APP_RESET_SAVE_LEN;
    sonata_fs_tag_t tag = APP_RESET_SAVE_TAG;
    uint8_t status = sonata_fs_read(tag,&len,&reset_flag);
    APP_TRC("reset_flag %d  %d\r\n",reset_flag,status);
    if(reset_flag == 0xff)
    {
        reset_flag = 1;
    }
    else
    {
        reset_flag ++;
    }
    len = APP_RESET_SAVE_LEN;
    if(reset_flag == APP_RESET_SWITCH_COUNT)
    {
        reset_flag = 0xFE;
    }
    else
    {
        sonata_fs_write(tag,len,&reset_flag);
    }
    app_timer_init();
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void app_init(void)
{
    APP_TRC("app_init\r\n");
    sonata_log_level_set(SONATA_LOG_VERBOSE);
    app_reset_process();
    sonata_ble_register_gap_callback(&ble_gap_callbacks);
    sonata_ble_register_gatt_callback(&ble_gatt_callbacks);
    sonata_ble_register_complete_callback(&ble_complete_callbacks);

    app_ble_on();

    for (int i=0; i<CFG_APP_MAX_CONNECTTION; i++)
    {
        app_conn_state_list[i].is_connected = FALSE;
        memset(app_conn_state_list[i].address, 0, SONATA_GAP_BD_ADDR_LEN);
        app_conn_state_list[i].conidx = 0xFF;
    }
}



/// @} APP
