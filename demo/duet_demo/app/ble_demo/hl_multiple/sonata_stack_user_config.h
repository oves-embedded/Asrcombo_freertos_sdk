#ifndef SONATA_STACK_USER_CONFIG_H_
#define SONATA_STACK_USER_CONFIG_H_

/*
 * DEFINES
 ****************************************************************************************
 */
#define SONATA_CFG_CONTROLLER

#define SONATA_CFG_HOST

#define SONATA_CFG_APP

#ifdef CFG_MRFOTA_TEST
#define SONATA_CFG_BLE_TRANSPORT
#endif
//#define SONATA_CFG_BLE_DIRECTION_FINDING

//#define SONATA_CFG_BLE_WLAN_COEX

//#define SONATA_CFG_MESH

#define SONATA_CFG_DIS

//#define SONATA_CFG_BASS

//#define SONATA_CFG_OTAS

//#define SONATA_CFG_DBG

//#define SONATA_CFG_MESH_DBG

//#define SONATA_CFG_TRC

//#define SONATA_CFG_DISPLAY

#define SONATA_CFG_NVDS


#define APP_BLE_ROLE SONATA_GAP_ROLE_ALL
#endif /*SONATA_STACK_USER_CONFIG_H_*/

