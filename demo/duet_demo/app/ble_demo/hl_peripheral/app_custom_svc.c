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
 * @file \app_custom_svc.c
 *
 * @brief custom service db
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app_custom_svc.h"
#include <stdio.h>
#include "sonata_gatt_api.h"
#include "app.h"

#ifdef CLOUD_JOYLINK_SUPPORT
#include "joylink_sdk.h"
#include "joylink_adapter.h"
#include "joylink_utils.h"
#endif
#include "system_cm4.h"
#include "lega_rtos_api.h"



#include"gatt.h"
#include "elog.h"
#include"payg.h"
#include"eeprom.h"

/*
 * MACRO DEFINES
 ****************************************************************************************
 */
#define APPSVC "APP_SVC"
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
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
#ifdef CLOUD_JOYLINK_SUPPORT
uint8_t csvc_uuid[] = {0x70, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
struct sonata_gatt_att_desc csvc_atts[CSVC_IDX_NB] = {
        // uuid,                                   permission,              maxLen, ext_perm
        //ATT_DECL_CHARACTERISTIC (Read)
        [CSVC_IDX_1_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  0,      0},
        [CSVC_IDX_1_VAL]     =   {{0X71, 0XFE, 0}, PWR_NA,                  128,    PRI},

        //ATT_DECL_CHARACTERISTIC (Indicate,Read,Write)
        [CSVC_IDX_2_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  0,      0},
        [CSVC_IDX_2_IND_VAL] =   {{0X72, 0XFE, 0}, PIND_NA|PRD_NA|PWR_NA,   128,    PRI},
        [CSVC_IDX_2_CFG]    =    {{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},
};
#else
uint8_t csvc_uuid[] = {0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define CONST const
#define ATT_OV_UUID_SIZE 16

#define UUID_16BYTE_HEAD 0x47,0x9b,0xdd,0x05,0x6a,0xf8,0x62,0xbe,0x51,0x44,0xec,0xd1//, 

#define UUID_16BYTE_END 0x07,0x9b

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define OVATTPROFILE_SERV_UUID					0x1000
#define OVCMDPROFILE_SERV_UUID					0x2000
#define OVSTSPROFILE_SERV_UUID					0x3000
#define OVDTAPROFILE_SERV_UUID					0x4000
#define OVDIAPROFILE_SERV_UUID					0x5000




    
// Key Pressed UUID


#define UUID_16BYTE_ATT UUID_16BYTE_HEAD,LO_UINT16(OVATTPROFILE_SERV_UUID), HI_UINT16(OVATTPROFILE_SERV_UUID),UUID_16BYTE_END
#define UUID_16BYTE_CMD UUID_16BYTE_HEAD,LO_UINT16(OVCMDPROFILE_SERV_UUID), HI_UINT16(OVCMDPROFILE_SERV_UUID),UUID_16BYTE_END
#define UUID_16BYTE_STS UUID_16BYTE_HEAD,LO_UINT16(OVSTSPROFILE_SERV_UUID), HI_UINT16(OVSTSPROFILE_SERV_UUID),UUID_16BYTE_END
#define UUID_16BYTE_DTA UUID_16BYTE_HEAD,LO_UINT16(OVDTAPROFILE_SERV_UUID), HI_UINT16(OVDTAPROFILE_SERV_UUID),UUID_16BYTE_END
#define UUID_16BYTE_DIA UUID_16BYTE_HEAD,LO_UINT16(OVDIAPROFILE_SERV_UUID), HI_UINT16(OVDIAPROFILE_SERV_UUID),UUID_16BYTE_END

CONST uint8_t OvAttProfileServUUID[ATT_OV_UUID_SIZE] =
{ 
	UUID_16BYTE_HEAD,LO_UINT16(OVATTPROFILE_SERV_UUID), HI_UINT16(OVATTPROFILE_SERV_UUID),UUID_16BYTE_END
};
CONST uint8_t OvCmdProfileServUUID[ATT_OV_UUID_SIZE] =
{ 
	UUID_16BYTE_HEAD,LO_UINT16(OVCMDPROFILE_SERV_UUID), HI_UINT16(OVCMDPROFILE_SERV_UUID),UUID_16BYTE_END
};
CONST uint8_t OvStsProfileServUUID[ATT_OV_UUID_SIZE] =
{ 
	UUID_16BYTE_HEAD,LO_UINT16(OVSTSPROFILE_SERV_UUID), HI_UINT16(OVSTSPROFILE_SERV_UUID),UUID_16BYTE_END
};
CONST uint8_t OvDtaProfileServUUID[ATT_OV_UUID_SIZE] =
{ 
	UUID_16BYTE_HEAD,LO_UINT16(OVDTAPROFILE_SERV_UUID), HI_UINT16(OVDTAPROFILE_SERV_UUID),UUID_16BYTE_END
};
CONST uint8_t OvDiaProfileServUUID[ATT_OV_UUID_SIZE] =
{ 
	UUID_16BYTE_HEAD,LO_UINT16(OVDIAPROFILE_SERV_UUID), HI_UINT16(OVDIAPROFILE_SERV_UUID),UUID_16BYTE_END
};
	

//2901 Description
//2902 Chara Config
struct sonata_gatt_att_desc csvc_module_att[3] = {
        // uuid,            perm,                    maxLen, ext_perm
        //ATT_DECL_CHARACTERISTIC (Read)
        [0]={{0X03, 0X28, 0}, PRD_NA,                  0,      0},
        [1]={{UUID_16BYTE_ATT}, PRD_NA,                128,    PRI|0x4000},
        [2]={{0X01, 0X29, 0}, PRD_NA,           		2,      PRI},
	
};
struct sonata_gatt_att_desc csvc_module_cmd[3] = {
		// uuid,            perm,                    maxLen, ext_perm
		//ATT_DECL_CHARACTERISTIC (Read)
		[0]={{0X03, 0X28, 0}, PRD_NA,                  0,      0},
		[1]={{UUID_16BYTE_CMD}, PRD_NA|PWR_NA,         128,    PRI|0x4000},
		[2]={{0X01, 0X29, 0}, PRD_NA,           		2,      PRI},

};

struct sonata_gatt_att_desc csvc_module_sts[4] = {
		// uuid,             perm,                    maxLen, ext_perm
		//ATT_DECL_CHARACTERISTIC (Read)
        [0]={{0X03, 0X28, 0}, PRD_NA,                  2,      0},
        [1]={{UUID_16BYTE_STS},PRD_NA|PNTF_NA,        2,      PRI|0x4000},
        [2]={{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},
        [3]={{0X01, 0X29, 0}, PRD_NA,           	   2,      PRI},
};

struct sonata_gatt_att_desc csvc_module_dta[4] = {
		// uuid,            perm,                    maxLen, ext_perm
		//ATT_DECL_CHARACTERISTIC (Read)
		[0]={{0X03, 0X28, 0}, PRD_NA,                  2,      0},
        [1]={{UUID_16BYTE_DTA}, PIND_NA|PRD_NA|PNTF_NA,        2,      PRI|0x4000},
        [2]={{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},
        [3]={{0X01, 0X29, 0}, PRD_NA,           2,      PRI},

};

struct sonata_gatt_att_desc csvc_module_dia[4] = {
		// uuid,             perm,                    maxLen, ext_perm
		//ATT_DECL_CHARACTERISTIC (Read)
		[0]={{0X03, 0X28, 0}, PRD_NA,                  2,      0},
        [1]={{UUID_16BYTE_DIA}, PRD_NA|PNTF_NA,        2,    PRI|0x4000},
        [2]={{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},
        [3]={{0X01, 0X29, 0}, PRD_NA,           	   2,      PRI},

};



#define OVGATT_ATT_NUM  20
#define OVGATT_CMD_NUM  160
#define OVGATT_STS_NUM  160
#define OVGATT_DTA_NUM  160
#define OVGATT_DIA_NUM  200

struct sonata_gatt_att_desc csvc_ov_att[OVGATT_ATT_NUM];
struct sonata_gatt_att_desc csvc_ov_cmd[OVGATT_CMD_NUM];
struct sonata_gatt_att_desc csvc_ov_sts[OVGATT_STS_NUM];
struct sonata_gatt_att_desc csvc_ov_dta[OVGATT_DTA_NUM];
struct sonata_gatt_att_desc csvc_ov_dia[OVGATT_DIA_NUM];

uint16_t g_gatt_att_num,g_gatt_cmd_num,g_gatt_sts_num,g_gatt_dta_num,g_gatt_dia_num;
uint16_t g_gatt_list_start_handle[LIST_COUNT]={0};
uint8_t g_gatt_prop_list_id=LIST_ATT;
uint8_t g_gatt_prop_list_addr[LIST_COUNT]={0xff,0xff,0xff,0xff,0xff};

extern lega_semaphore_t g_ovapp_Semaphore;
//creat ble charac list
void app_custom_svc_gatt_create(void)
{
	gatt_prop_Typedef*proplist= gatt_get_prop_list();
	int i;
	
	g_gatt_att_num=0;
	g_gatt_cmd_num=0;
	g_gatt_sts_num=0;
	g_gatt_dta_num=0;
	g_gatt_dia_num=0;


	for(i=0;i<GATT_PROP_LIST_NUM&&proplist[i].prop!=NULL;i++)
	{
		
		switch(proplist[i].list_name)
		{
			case LIST_ATT:
				if(g_gatt_prop_list_addr[LIST_ATT]==0xFF)
					g_gatt_prop_list_addr[LIST_ATT]=i;
				
				csvc_module_att[1].uuid[12]++;
				memcpy((uint8_t*)&csvc_ov_att[g_gatt_att_num],(uint8_t*)csvc_module_att,sizeof(csvc_module_att));
				g_gatt_att_num+=sizeof(csvc_module_att)/sizeof(sonata_gatt_att_desc_t);

				if(g_gatt_att_num>OVGATT_ATT_NUM)
				    log_i(" g_gatt_att_num overlop!!!");
				break;
			case LIST_CMD:
				if(g_gatt_prop_list_addr[LIST_CMD]==0xFF)
					g_gatt_prop_list_addr[LIST_CMD]=i;
				
				csvc_module_cmd[1].uuid[12]++;
				memcpy((uint8_t*)&csvc_ov_cmd[g_gatt_cmd_num],(uint8_t*)csvc_module_cmd,sizeof(csvc_module_cmd));
				g_gatt_cmd_num+=sizeof(csvc_module_cmd)/sizeof(sonata_gatt_att_desc_t);
				if(g_gatt_cmd_num>OVGATT_CMD_NUM)
				    log_i(" g_gatt_cmd_num overlop!!!");
				break;
			case LIST_STS:
				if(g_gatt_prop_list_addr[LIST_STS]==0xFF)
					g_gatt_prop_list_addr[LIST_STS]=i;
				
				csvc_module_sts[1].uuid[12]++;
				memcpy((uint8_t*)&csvc_ov_sts[g_gatt_sts_num],(uint8_t*)csvc_module_sts,sizeof(csvc_module_sts));
				g_gatt_sts_num+=sizeof(csvc_module_sts)/sizeof(sonata_gatt_att_desc_t);
				if(g_gatt_sts_num>OVGATT_STS_NUM)
				    log_i(" g_gatt_sts_num overlop!!!");
				break;
			case LIST_DTA:
				if(g_gatt_prop_list_addr[LIST_DTA]==0xFF)
					g_gatt_prop_list_addr[LIST_DTA]=i;
				
				csvc_module_dta[1].uuid[12]++;
				memcpy((uint8_t*)&csvc_ov_dta[g_gatt_dta_num],(uint8_t*)csvc_module_dta,sizeof(csvc_module_dta));
				g_gatt_dta_num+=sizeof(csvc_module_dta)/sizeof(sonata_gatt_att_desc_t);
				if(g_gatt_dta_num>OVGATT_DTA_NUM)
				    log_i(" g_gatt_dta_num overlop!!!");
				break;
			case LIST_DIA:
				if(g_gatt_prop_list_addr[LIST_DIA]==0xFF)
					g_gatt_prop_list_addr[LIST_DIA]=i;
				
				csvc_module_dia[1].uuid[12]++;
				memcpy((uint8_t*)&csvc_ov_dia[g_gatt_dia_num],(uint8_t*)csvc_module_dia,sizeof(csvc_module_dia));
				g_gatt_dia_num+=sizeof(csvc_module_dia)/sizeof(sonata_gatt_att_desc_t);
				if(g_gatt_dia_num>OVGATT_DIA_NUM)
				    log_i(" g_gatt_dia_num overlop!!!");
				break;
			}

		}

	log_i("csvc ov att:%d cmd:%d sts:%d dta:%d dia:%d  \n",g_gatt_att_num,g_gatt_cmd_num,g_gatt_sts_num,g_gatt_dta_num,g_gatt_dia_num);

}


/*struct sonata_gatt_att_desc csvc_atts[CSVC_IDX_NB] = {
        // uuid,                                   perm,                    maxLen, ext_perm
        //ATT_DECL_CHARACTERISTIC (Read)
        [CSVC_IDX_1_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  2,      0},
        [CSVC_IDX_1_VAL]     =   {{0XF1, 0XFF, 0}, PRD_NA,                  128,    PRI|0x4000},
        [CSVC_IDX_3_CFG]    =    {{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},

        //ATT_DECL_CHARACTERISTIC (Read,Write)
        [CSVC_IDX_2_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  0,      0},
        [CSVC_IDX_2_VAL]     =   {{0XF2, 0XFF, 0}, PRD_NA|PWR_NA,           128,    PRI|0x4000},
        //ATT_DESC_CLIENT_CHAR_CFG (Notify)
        [CSVC_IDX_3_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  2,      0},
        [CSVC_IDX_3_NTF_VAL] =   {{0XAA, 0XAA, 0}, PNTF_NA|PRD_NA|PWR_NA,   2,      PRI|0x4000},
        [CSVC_IDX_3_CFG]    =    {{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},

        [CSVC_IDX_4_CHAR]    =   {{0X03, 0X28, 0}, PRD_NA,                  2,      0},
        [CSVC_IDX_4_IND_VAL] =   {{0XBB, 0XBB, 0}, PIND_NA|PRD_NA|PWR_NA,   2,      PRI|0x4000},
        [CSVC_IDX_4_CFG]    =    {{0X02, 0X29, 0}, PRD_NA|PWR_NA,           2,      PRI},

};*/
#endif

uint16_t start_handle_id = 0;
uint16_t start_handle_first_id = 0;

/*
* FUNCTION DEFINITIONS
****************************************************************************************
*/
#ifdef CLOUD_JOYLINK_SUPPORT
int app_jl_adapter_send_data(uint8_t*data, uint32_t data_len)//SDK: Call this interface to send data, indicate send function to achieve the interface
{
  APP_TRC(" >>>xxx_send_data len = %ld Data:", data_len);
  for (int i = 0; i < data_len; ++i)
  {
      APP_TRC("%02X ", data[i]);
  }
  APP_TRC("\r\n");

  uint8_t connect_idx = 0x00;
  int ret = sonata_ble_gatt_send_indicate_event(connect_idx, (start_handle_id + CSVC_IDX_2_IND_VAL + 1), data_len, data);

  APP_TRC("sonata_ble_gatt_send_indicate_event ret = %d\r\n", ret);

  return ret;
}
#endif
uint16_t  get_str_len(char *str,uint8_t end_tag)
{
	uint16_t ret=0,i=0;//,len=0;

	for(i=0;str[i]!=end_tag&&str[i];i++)
	{	
		ret++;
		}
	return ret;
}

uint8_t g_add_svc_state=0;
extern USER_SET_TypeDef g_UserSet;
uint16_t app_custom_svc_add_request(uint8_t start_handle)
{
    uint16_t start_hdl = 0;//start_handle;//(0 = dynamically allocated)
    uint8_t perm = 0|0x40;
    //uint8_t nb_att = CSVC_IDX_NB;
    uint8_t *uuid ;
    struct sonata_gatt_att_desc *atts ;
    uint16_t retval ;//= sonata_ble_gatt_add_service_request(start_hdl, perm, uuid, nb_att, atts);

	uuid = (uint8_t*)OvAttProfileServUUID;
	atts = csvc_ov_att;
	retval = sonata_ble_gatt_add_service_request(start_hdl, perm, uuid, g_gatt_att_num, atts);

	uuid = (uint8_t*)OvCmdProfileServUUID;
	atts = csvc_ov_cmd;
	retval = sonata_ble_gatt_add_service_request(start_hdl, perm, uuid, g_gatt_cmd_num, atts);

	uuid = (uint8_t*)OvStsProfileServUUID;
	atts = csvc_ov_sts;
	retval = sonata_ble_gatt_add_service_request(start_hdl, perm, uuid, g_gatt_sts_num, atts);
	
	uuid = (uint8_t*)OvDtaProfileServUUID;
	atts = csvc_ov_dta;
	retval = sonata_ble_gatt_add_service_request(start_hdl, perm, uuid, g_gatt_dta_num, atts);

	uuid = (uint8_t*)OvDiaProfileServUUID;
	atts = csvc_ov_dia;
	retval = sonata_ble_gatt_add_service_request(start_hdl, perm, uuid, g_gatt_dia_num, atts);

    return retval;
}

#ifdef CLOUD_JOYLINK_SUPPORT
uint16_t app_custom_svc_read_request_handler(uint8_t connection_id, uint16_t handle)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, localHandle=0x%04X,\r\n", __FUNCTION__, localHandle);
    switch (localHandle)
    {
        case CSVC_IDX_1_VAL:
        {
            uint8_t localValue[] = "C1 Value";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_2_IND_VAL:
        {
            uint8_t localValue[] = "C2 Value";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        default:
            break;
    }

    return CB_REJECT;
}

uint16_t app_custom_svc_write_request_handler(uint8_t connection_id,  uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    APP_TRC("APPSVC: %s, connection_id = 0x%02x, localHandle=0x%04X,handle=0x%04X,offset=0x%04X,\r\n", __FUNCTION__, connection_id, localHandle, handle, offset);
    switch (localHandle)
    {
        case CSVC_IDX_1_VAL:
        case CSVC_IDX_2_IND_VAL:
        case CSVC_IDX_2_CFG:
        {
            APP_TRC(" >>>App Get Data len = %d Data:", length);
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X ", value[i]);
            }
            APP_TRC("\r\n");

            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

            if (length > 2)
            {
              int32_t ret = jl_write_data((uint8_t*)value, length);
              if (ret) {
                  APP_TRC("jl_write_data failed !\n");
              }
            }
        }
            break;
        default:
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_ATT_ERR_ATTRIBUTE_NOT_FOUND);
            break;
    }

    return CB_DONE;
}
#else
uint16_t app_custom_svc_read_request_handler(uint8_t connection_id, uint16_t handle)
{
    uint16_t localHandle=0 ;//= handle - start_handle_id - 1;
    gatt_prop_Typedef*gattlist= gatt_get_prop_list();
	uint8_t list=0,chara_handle=3,desc=2;

	

	if(handle>g_gatt_list_start_handle[LIST_DIA])
	{
		start_handle_id=g_gatt_list_start_handle[LIST_DIA];
		list=LIST_DIA;
		chara_handle=4;
		desc=3;
		}
	else if(handle>g_gatt_list_start_handle[LIST_DTA])
	{
		start_handle_id=g_gatt_list_start_handle[LIST_DTA];
		list=LIST_DTA;
		chara_handle=4;
		desc=3;
		}
	else if(handle>g_gatt_list_start_handle[LIST_STS])
	{
		start_handle_id=g_gatt_list_start_handle[LIST_STS];
		list=LIST_STS;
		chara_handle=4;
		desc=3;
		}
	else if(handle>g_gatt_list_start_handle[LIST_CMD])
	{
		start_handle_id=g_gatt_list_start_handle[LIST_CMD];
        list=LIST_CMD;
		chara_handle=3;
		desc=2;
		}
	else
	{
		list=LIST_ATT;
		start_handle_id=g_gatt_list_start_handle[LIST_ATT];
		chara_handle=3;
		desc=2;
		}

	localHandle = handle - start_handle_id - 1	;

	APP_TRC("APPSVC: %s, localHandle=0x%04X,handle=0x%04X,\r\n", __FUNCTION__, localHandle,handle);

	if(localHandle%chara_handle==1)
	{	lega_rtos_get_semaphore(&g_ovapp_Semaphore, 20000);
		gattlist=&gattlist[g_gatt_prop_list_addr[list]+localHandle/chara_handle];
		sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, gattlist->len, gattlist->value);
		lega_rtos_set_semaphore(&g_ovapp_Semaphore);
		return CB_DONE;
		}
	else
	{	
		if(localHandle%chara_handle==desc)
		{
			lega_rtos_get_semaphore(&g_ovapp_Semaphore, 20000);
			gattlist=&gattlist[g_gatt_prop_list_addr[list]+localHandle/chara_handle];
			sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, strlen(gattlist->meta),(uint8_t*)gattlist->meta);
			lega_rtos_set_semaphore(&g_ovapp_Semaphore);
			return CB_DONE;
			}
		
		return CB_REJECT;
		}
	
		
    
    switch (localHandle)
    {
        case CSVC_IDX_1_VAL:
        {
            uint8_t localValue[] = "C1 Value";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_2_VAL:
        {
            uint8_t localValue[] = "C2 Value";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_3_NTF_VAL:
        {
            uint8_t localValue[] = "C3 Notify";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_3_CFG:
        {
            uint8_t localValue[] = "C3 config";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_4_IND_VAL:
        {
            uint8_t localValue[] = "C4 Indicate";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        case CSVC_IDX_4_CFG:
        {
            uint8_t localValue[] = "C4 Config";
            sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, sizeof(localValue) - 1, localValue);
            return CB_DONE;
        }
        default:
            break;
    }


    return CB_REJECT;
}

uint16_t app_custom_svc_write_request_handler(uint8_t connection_id,  uint16_t handle, uint16_t offset, uint16_t length, uint8_t *value)
{
    uint16_t localHandle = handle - start_handle_id - 1;
    gatt_prop_Typedef*gattlist= gatt_get_prop_list();
	uint8_t list=0,chara_handle=3,notify=2;
	uint16_t len=0;

	

	if(handle>g_gatt_list_start_handle[LIST_DIA])
	{
		start_handle_id=g_gatt_list_start_handle[LIST_DIA];
		list=LIST_DIA;
		chara_handle=4;
		notify=2;
		}
	else if(handle>g_gatt_list_start_handle[LIST_DTA])
	{
		start_handle_id=g_gatt_list_start_handle[LIST_DTA];
		list=LIST_DTA;
		chara_handle=4;
		notify=2;
		}
	else if(handle>g_gatt_list_start_handle[LIST_STS])
	{
		start_handle_id=g_gatt_list_start_handle[LIST_STS];
		list=LIST_STS;
		chara_handle=4;
		notify=2;
		}
	else if(handle>g_gatt_list_start_handle[LIST_CMD])
	{
		start_handle_id=g_gatt_list_start_handle[LIST_CMD];
        list=LIST_CMD;
		chara_handle=3;
		notify=2;
		}
	else
	{
		list=LIST_ATT;
		start_handle_id=g_gatt_list_start_handle[LIST_ATT];
		chara_handle=3;
		notify=2;
		}
	localHandle = handle - start_handle_id - 1	;
	
    APP_TRC("APPSVC: %s, localHandle=0x%04X,handle=0x%04X,offset=0x%04X,\r\n", __FUNCTION__, localHandle, handle, offset);

	if(localHandle%chara_handle==1)
	{	
		APP_TRC(" >>>App Get Data:");
        for (int i = 0; i < length; ++i)
        {
            APP_TRC("%02X[%c]", value[i], value[i]);
        }
        APP_TRC("\r\n");
		APP_TRC("Write:%s \r\n",value);

		lega_rtos_get_semaphore(&g_ovapp_Semaphore, 2000);
		
		gattlist=&gattlist[g_gatt_prop_list_addr[list]+localHandle/chara_handle];

		
		if(strstr(gattlist->prop,"wifi\0")!=NULL)
		{
			len=get_str_len((char*)value,',');
			memcpy(g_UserSet.wifi_ssid,(char*)value,len);
			memcpy(g_UserSet.wifi_password,(char*)&value[len+1],strlen((char*)&value[len+1]));

			//memcpy(gattlist->value,value,len);
			APP_TRC("ssid:%s key:%s\r\n",g_UserSet.wifi_ssid,g_UserSet.wifi_password);
			EEpUpdateEnable();
			EEpProcess();
			lega_rtos_delay_milliseconds(10);
			APP_TRC("ssid:%s key:%s\r\n",g_UserSet.wifi_ssid,g_UserSet.wifi_password);
			duet_system_reset();
			}
		if(strstr(gattlist->prop,"pubk\0")!=NULL)
		{
			uint32_t token_hi,token_lo;
			uint8_t token[32];
		    uint8_t *p,*p_target;
			uint8_t i,value_len=0,token_len=0;

			p_target=token;


			p=strstr(value,"*0");

			if(p==NULL)
			{
				return CB_REJECT;
				}
			p+=2;

			if(strstr(value,"#")==NULL)
			{
				return CB_REJECT;
				}

			value_len=strstr((char*)value,"#")-(char*)p;
			TokenParse(p-2,"*0");
			}

		lega_rtos_set_semaphore(&g_ovapp_Semaphore);

		APP_TRC("token process !!!\n");
		
		sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
		return CB_DONE;
		}
	else
	{	
		if(localHandle%chara_handle==notify)
		{
			//gattlist=&gattlist[g_gatt_prop_list_addr[list]+localHandle/chara_handle];
			//sonata_ble_gatt_send_read_confirm(connection_id, handle, 0, strlen(gattlist->meta),gattlist->meta);
			 sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
            uint16_t ntf_cfg = value[0];
            if (ntf_cfg == SONATA_PRF_CLI_STOP_NTFIND)
            {
                APP_TRC("APP_SVC: NTF Stopped\r\n");
            }
            else if (ntf_cfg == SONATA_PRF_CLI_START_NTF)
            {
                APP_TRC("APP_SVC: NTF Started. start_handle_id=%d\r\n",start_handle_id);
                //Use random value for example
                uint8_t localvalue[3] = {0};
                localvalue[0] = util_rand_byte();
                localvalue[1] = util_rand_byte();
                //sonata_ble_gatt_send_notify_event(connection_id, handle-(CSVC_IDX_3_VAL-CSVC_IDX_3_NTF_CFG), 2, localvalue);
                sonata_ble_gatt_send_notify_event(connection_id, (start_handle_id + CSVC_IDX_3_NTF_VAL + 1), 2, localvalue);
            }
			return CB_DONE;
			}
		
		return CB_REJECT;
		}

	#if 0
    switch (localHandle)
    {
        case CSVC_IDX_1_VAL:
        case CSVC_IDX_2_VAL:
        case CSVC_IDX_3_NTF_VAL:
        case CSVC_IDX_4_IND_VAL:
        {
            APP_TRC(" >>>App Get Data:");
            for (int i = 0; i < length; ++i)
            {
                APP_TRC("%02X[%c]", value[i], value[i]);
            }
            APP_TRC("\r\n");
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);

        }
            break;
        case CSVC_IDX_3_CFG:
        {
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
            uint16_t ntf_cfg = value[0];
            if (ntf_cfg == SONATA_PRF_CLI_STOP_NTFIND)
            {
                APP_TRC("APP_SVC: NTF Stopped\r\n");
            }
            else if (ntf_cfg == SONATA_PRF_CLI_START_NTF)
            {
                APP_TRC("APP_SVC: NTF Started. start_handle_id=%d\r\n",start_handle_id);
                //Use random value for example
                uint8_t localvalue[3] = {0};
                localvalue[0] = util_rand_byte();
                localvalue[1] = util_rand_byte();
                //sonata_ble_gatt_send_notify_event(connection_id, handle-(CSVC_IDX_3_VAL-CSVC_IDX_3_NTF_CFG), 2, localvalue);
                sonata_ble_gatt_send_notify_event(connection_id, (start_handle_id + CSVC_IDX_3_NTF_VAL + 1), 2, localvalue);
            }
            break;
        }
        case CSVC_IDX_4_CFG:
        {
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_GAP_ERR_NO_ERROR);
            uint16_t ind_cfg = value[0];
            if (ind_cfg == SONATA_PRF_CLI_STOP_NTFIND)
            {
                APP_TRC("APP_SVC: IND Stopped\r\n");
            }
            else if (ind_cfg == SONATA_PRF_CLI_START_IND)
            {
                APP_TRC("APP_SVC: IND Started. start_handle_id=%d\r\n",start_handle_id);
                //Use random value for example
                uint8_t localvalue[3] = {0};
                localvalue[0] = util_rand_byte();
                localvalue[1] = util_rand_byte();
                sonata_ble_gatt_send_indicate_event(connection_id, (start_handle_id + CSVC_IDX_4_IND_VAL + 1), 2, localvalue);
            }
            break;
        }
        default:
            sonata_ble_gatt_send_write_confirm(connection_id, handle, SONATA_HL_ATT_ERR_ATTRIBUTE_NOT_FOUND);

            break;
    }


    return CB_DONE;
	#endif
}
#endif


void app_custom_svc_set_start_handle(uint16_t handle)
{
	if(start_handle_first_id==0)
		start_handle_first_id=handle;
	
    start_handle_id = start_handle_first_id;

	g_gatt_list_start_handle[g_gatt_prop_list_id++]=handle;

	
	APP_TRC("APP_SVC: start_handle_id %d\r\n" ,handle);
	APP_TRC("APP_SVC: start handle(att: %d cmd: %d sts: %d dta: %d dia: %d)\r\n" ,g_gatt_list_start_handle[LIST_ATT],g_gatt_list_start_handle[LIST_CMD],\
		g_gatt_list_start_handle[LIST_STS],g_gatt_list_start_handle[LIST_DTA],g_gatt_list_start_handle[LIST_DIA]);
}

