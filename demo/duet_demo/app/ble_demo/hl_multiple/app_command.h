
/**
 ****************************************************************************************
 *
 * @file (app_command.h)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */

#ifndef _APP_COMMAND_H
#define _APP_COMMAND_H
/*
 * INCLUDE FILES
 ****************************************************************************************
 */



#include <stdio.h>
#include "app.h"
#include "sonata_ble_api.h"

/*
 * MACRO DEFINES
 ****************************************************************************************
 */

//AT Command
#define ATCMD_AT              ("AT")
#define ATCMD_PLUS            ("+")
#define ATCMD_AT_PLUS         ("AT+")
#define ATCMD_OK              ("OK")
#define ATCMD_EQUAL           ("=")
#define ATCMD_RESET           ("RESET")       //AT+RESET
#define ATCMD_REBOOT          ("REBOOT")      //AT+REBOOT
#define ATCMD_VERSION         ("VERSION")     //AT+VERSION
#define ATCMD_LADDR           ("LADDR")       //AT+LADDR;    AT+LADDR=XX:XX:XX:XX:XX:XX
#define ATCMD_NAME            ("NAME")        //AT+NAME;     AT+NAME=XXXX (Not fixed length)
#define ATCMD_BAUD            ("BAUD")        //AT+BAUD;     AT+BAUD=X (0:4800; 1:9600; 2:19200; 3:24800; 4:57600; 5:115200; 6:230400 )
#define ATCMD_UUID            ("UUID")        //AT+UUID;     AT+UUID=XXXX
#define ATCMD_PIN             ("PIN")         //AT+PIN;      AT+PIN=XXXX  (Length is 4)
#define ATCMD_MODE            ("MODE")        //AT+MODE;     AT+MODE=X (0:Slave  1:Master)
#define ATCMD_AUTOCONN        ("AUTOCONN")    //AT+AUTOCONN; AT+AUTOCONN=X (0:Not Auto conn, 1:Autoconn
#define ATCMD_SCAN            ("SCAN")        //             AT+SCAN=X (0:Stop scan, 1:Start scan)(Rsp!!)
#define ATCMD_CONN            ("CONN")        //             AT+CONN=XXXXXXXXXXXXX (Addr+Type, Type: 0:static addr, 1:Radom addr)  (Rsp!!)
#define ATCMD_CHINFO          ("CHINFO")      //AT+CHINFO    (Rsp!!)
#define ATCMD_LESEND          ("LESEND")      //             AT+LESEND=X,XX,XXXX(Ch ID+Data Length+Data)
#define ATCMD_DATA            ("DATA")        //             AT+DATA=X,XX,XXXX(Ch ID+Data Length+Data)
#define ATCMD_LEDISC          ("LEDISC")      //             AT+LEDISC=X (Ch ID)  (RSP)
#define ATCMD_NTF             ("NTF")         //             AT+NTF=0,43,1
#define ATCMD_IND             ("IND")         //             AT+IND=0,44,1
#define ATCMD_ADV             ("ADV")         //             AT+ADV=X
#define ATCMD_THROUGHPUT      ("THROUGHPUT")  //             AT+THROUGHPUT=X
#define ATCMD_DISCOVERY       ("DISCOVERY")   //             AT+DISCOVERY
#define ATCMD_MTU             ("MTU")         //             AT+MTU
#define ATCMD_SVCCHANGE       ("SVCCHANGE")   //             AT+SVCCHANGE=0,0,FFFF,
#define ATCMD_MCONN           ("MCONN")       //             AT+CONN=XXXXXXXXXXXX,XXXXXXXXXXXX,...
#define ATCMD_NTFSEND         ("NTFSEND")     //             AT+NTFDATA=0,4,12345
#define ATCMD_CONNPARAM       ("CONNPARAM")   //             AT+CONNPARAM=0,6,6,44,200
#define ATCMD_AUTOADV         ("AUTOADV")     //             AT+AUTOADV=1
#define ATCMD_CLEARACTIVE     ("CLEARACTIVE") //             AT+CLEARACTIVE
#define ATCMD_EXTADV          ("EXTADV")      //             AT+EXTADV
#define ATCMD_ATTHDL          ("ATTHDL")      //             AT+EXTADV
#define ATCMD_DLE             ("DLE")         //             AT+DLE
#define ATCMD_LOG             ("LOG")         //             AT+LOG=1
#define ATCMD_TXPWR           ("TXPWR")       //             AT+TXPWR=10
#define ATCMD_BONDCLEAR       ("BONDCLEAR")   //             AT+BONDCLEAR
#define ATCMD_BOND            ("BOND")        //             AT+BOND=0
#define ATCMD_GATTREAD        ("GATTREAD")    //             AT+GATTREAD=0,43
#define ATCMD_BONDINFO        ("BONDINFO")    //             AT+BONDINFO
#define ATCMD_ENCRYPT         ("ENCRYPT")     //             AT+ENCRYPT=0
#define ATCMD_NCOMPARE        ("NCOMPARE")    //             AT+COMPARE=0,1
#define ATCMD_GENADDR         ("GENADDR")     //             AT+GENADDR=4,112233
#define ATCMD_RESOLVADDR      ("RESOLVADDR")  //             AT+GENADDR=4,112233
#define ATCMD_SETIRK          ("SETIRK")      //             AT+SETIRK
#define ATCMD_RALLIST         ("RALLIST")     //             AT+GENADDR=4,112233
#define ATCMD_GETRAL          ("GETRAL")      //             AT+GENADDR=4,112233
#define ATCMD_DIRECTADV       ("DIRECTADV")   //             AT+DIRECTADV=1
#define ATCMD_PERIODADV       ("PERIODADV")   //             AT+PERIODADV=1
#define ATCMD_PERIODSYNC      ("PERIODSYNC")  //            AT+PERIODSYNC=1
#define ATCMD_PRESSURE        ("PRESSURE")    //            AT+PERIODSYNC=1
#define ATCMD_AUTONTF         ("AUTONTF")     //            AT+AUTONTF=1
#define ATCMD_ADVDATA         ("ADVDATA")     //            AT+AUTONTF=1
#define ATCMD_WLIST           ("WLIST")       //            AT+WLIST=10,111100001111
#define ATCMD_PALLIST         ("PALLIST")     //            AT+PALLIST=10,111100001111
#define ATCMD_DEVINFO         ("DEVINFO")     //            AT+DEVINFO
#define ATCMD_TIMER           ("TIMER")       //            AT+DEVINFO
#define ATCMD_OOBDATA         ("OOBDATA")     //            AT+DEVINFO
#define ATCMD_BONDTYPE        ("BONDTYPE")    //            AT+BONDTYPE=0
#define ATCMD_CHIPINFO        ("CHIPINFO")    //            AT+CHIPINFO
#define ATCMD_EFUSEREAD       ("EFUSEREAD")   //            AT+EFUSEREAD=0x77
#define ATCMD_EFUSEWRITE      ("EFUSEWRITE")  //            AT+EFUSEWRITE=0x77,0x02
#define ATCMD_APPCONFIG       ("APPCONFIG")   //            AT+APPCONFIG=1,0
#define ATCMD_HLTRANS         ("HLTRANS")     //            AT+HL_TRANS=1,0
#define ATCMD_ADDTARGET       ("ADDTARGET")   //            AT+ADDTARGET=2,111100001111,222200002222
#define ATCMD_REG             ("REG")         //            AT+REG/AT+REG=1,62004C00,F333339
#define ATCMD_TXTONE          ("TXTONE")      //            AT+TXTONE

#define ATRSP_SCAN_START      ("+SCAN{")
#define ATRSP_SCAN_END        ("+SCAN}")
#define ATRSP_CHINFO_START    ("+CHINFO{")
#define ATRSP_CHINFO_END      ("+CHINFO}")
#define ATRSP_GATTSTAT        ("GATTSTAT")
//AT Command length

#define ATCMD_LEN_AT          (2)
#define ATCMD_LEN_PLUS        (1)
#define ATCMD_LEN_AT_PLUS     (3)   //For "AT+"
#define ATCMD_LEN_EQUAL       (1)
#define ATCMD_LEN_OK          (2)
#define ATCMD_LEN_RESET       (5)
#define ATCMD_LEN_REBOOT      (6)
#define ATCMD_LEN_VERSION     (7)
#define ATCMD_LEN_LADDR       (5)
#define ATCMD_LEN_NAME        (4)
#define ATCMD_LEN_BAUD        (4)
#define ATCMD_LEN_UUID        (4)
#define ATCMD_LEN_PIN         (3)
#define ATCMD_LEN_MODE        (4)
#define ATCMD_LEN_AUTOCONN    (8)
#define ATCMD_LEN_SCAN        (4)
#define ATCMD_LEN_CONN        (4)
#define ATCMD_LEN_CHINFO      (6)
#define ATCMD_LEN_LESEND      (6)
#define ATCMD_LEN_DATA        (4)
#define ATCMD_LEN_LEDISC      (6)
#define ATCMD_LEN_NTF         (3)
#define ATCMD_LEN_IND         (3)
#define ATCMD_LEN_ADV         (3)
#define ATCMD_LEN_THROUGHPUT  (10)
#define ATCMD_LEN_DISCOVERY   (9)
#define ATCMD_LEN_MTU         (3)
#define ATCMD_LEN_SVCCHANGE   (9)
#define ATCMD_LEN_MCONN       (5)
#define ATCMD_LEN_NTFSEND     (7)
#define ATCMD_LEN_CONNPARAM   (9)
#define ATCMD_LEN_AUTOADV     (7)
#define ATCMD_LEN_CLEARACTIVE (11)
#define ATCMD_LEN_EXTADV      (6)
#define ATCMD_LEN_ATTHDL      (6)
#define ATCMD_LEN_DLE         (3)
#define ATCMD_LEN_LOG         (3)
#define ATCMD_LEN_TXPWR       (5)
#define ATCMD_LEN_BONDCLEAR   (9)
#define ATCMD_LEN_BOND        (4)
#define ATCMD_LEN_GATTREAD    (8)
#define ATCMD_LEN_BONDINFO    (8)
#define ATCMD_LEN_ENCRYPT     (7)
#define ATCMD_LEN_NCOMPARE    (8)
#define ATCMD_LEN_GENADDR     (7)
#define ATCMD_LEN_RESOLVADDR  (10)
#define ATCMD_LEN_SETIRK      (6)
#define ATCMD_LEN_RALLIST     (7)
#define ATCMD_LEN_GETRAL      (6)
#define ATCMD_LEN_DIRECTADV   (9)
#define ATCMD_LEN_PERIODADV   (9)
#define ATCMD_LEN_PERIODSYNC  (10)
#define ATCMD_LEN_PRESSURE    (8)
#define ATCMD_LEN_AUTONTF     (7)
#define ATCMD_LEN_ADVDATA     (7)
#define ATCMD_LEN_WLIST       (5)
#define ATCMD_LEN_PALLIST     (7)
#define ATCMD_LEN_DEVINFO     (7)
#define ATCMD_LEN_TIMER       (5)
#define ATCMD_LEN_OOBDATA     (7)
#define ATCMD_LEN_BONDTYPE    (8)
#define ATCMD_LEN_CHIPINFO    (8)
#define ATCMD_LEN_EFUSEREAD   (9)
#define ATCMD_LEN_EFUSEWRITE  (10)
#define ATCMD_LEN_APPCONFIG   (9)
#define ATCMD_LEN_HLTRANS     (7)
#define ATCMD_LEN_ADDTARGET   (9)
#define ATCMD_LEN_REG         (3)
#define ATCMD_LEN_TXTONE      (6)


#define ATRSP_LEN_GATTSTAT    (8)

#define UUID_DATA_LEN         (19)
#define UUID_OFFSET_SVC       (0)
#define UUID_OFFSET_CHAR_READ (5)
#define UUID_OFFSET_CHAR_WRITE (10)
#define UUID_OFFSET_CHAR_NTF   (15)

#define MCONN_COUNT  (5)

#define U8_INVALID   (0xFF)


#define TYPE_STOP                   0
#define TYPE_START                  1

#define TYPE_CONN_WLIST                 2
#define TYPE_PEROIDSYNC_START_WITH_PAL  2
#define TYPE_SCAN_START_WLIST           2
#define TYPE_SCAN_START_WITH_WD_INTV    3
#define TYPE_SCAN_CONTINUOUSSCAN        4
#define TYPE_AUTONTF_PERIPHERAL_NTF     1  //Only send data to master
#define TYPE_AUTONTF_DUPLEX             2  //Master write NTF and then Slave NTF data

typedef enum
{
    ID_RESET          =0,
    ID_VERSION        =1,
    ID_LADDR          =2,
    ID_NAME           =3,
    ID_BAUD           =4,
    ID_UUID           =5,
    ID_PIN            =6,
    ID_MODE           =7,
    ID_AUTOCONN       =8,
    ID_SCAN           =9,
    ID_CONN           =10,
    ID_CHINFO         =11,
    ID_LESEND         =12,
    ID_DATA           =13,
    ID_LEDISC         =14,
    ID_ATTEST         =15,
    ID_REBOOT         =16,
    ID_NTF            =17,
    ID_IND            =18,
    ID_ADV            =19,
    ID_THROUGHPUT     =20,
    ID_DISCOVERY      =21,
    ID_MTU            =22,
    ID_SVCCHANGE      =23,
    ID_MCONN          =24,
    ID_NTFSEND        =25,
    ID_CONNPARAM      =26,
    ID_AUTOADV        =27,
    ID_CLEARACTIVE    =28,
    ID_EXTADV         =29,
    ID_ATTHDL         =30,
    ID_DLE            =31,
    ID_LOG            =32,
    ID_TXPWR          =33,
    ID_BONDCLEAR      =34,
    ID_BOND           =35,
    ID_GATTREAD       =36,
    ID_BONDINFO       =37,
    ID_ENCRYPT        =38,
    ID_NCOMPARE       =39,
    ID_GENADDR        =40,
    ID_RESOLVADDR     =41,
    ID_SETIRK         =42,
    ID_RALLIST        =43,
    ID_GETRAL         =44,
    ID_DIRECTADV      =45,
    ID_PERIODADV      =46,
    ID_PERIODSYNC     =47,
    ID_PRESSURE       =48,
    ID_AUTONTF        =49,
    ID_ADVDATA        =50,
    ID_WLIST          =51,
    ID_PALLIST        =53,
    ID_DEVINFO        =54,
    ID_TIMER          =55,
    ID_OOBDATA        =56,
    ID_BONDTYPE       =57,
    ID_CHIPINFO       =58,
    ID_EFUSEREAD      =59,
    ID_EFUSEWRITE     =60,
    ID_APPCONFIG      =61,
    ID_HLTRANS        =62,
    ID_ADDTARGET      =63,
    ID_REG            =64,
    ID_TXTONE         =65,

}app_cmd_id;

typedef enum
{
    ERR_NONE          =0,
    ERR_FAIL          =-1,
    ERR_INVALID       =-2,
    ERR_PARA_COUNT    =-3,

}app_error;

typedef struct app_cmd_info
{
    app_cmd_id id;
    uint8_t cmdLen;
    char * cmd;

}app_cmd_info_t;


#define PARAM_MAX (10)

typedef struct param_item_t{
    uint16_t length; //parameter's length
    uint16_t start;  //parameter's start index
    uint16_t end;    //parameter's end index
    uint8_t *data;  //point for parameter
}param_item;

typedef struct param_info_t{
    uint8_t count;
    param_item para[PARAM_MAX];
}param_info;

typedef struct mconn_item_t{
    uint8_t conidx;
    uint8_t act;
    uint8_t addr[SONATA_GAP_BD_ADDR_LEN];
}mconn_item;




/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
extern bool gLog;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

void app_cmd_data_adapter(uint16_t length, const uint8_t *value);

void app_cmd_scan_result_handler(uint8_t *addr, uint8_t addrType, uint8_t *report, uint16_t reportLen);

void app_cmd_scan_start();

void app_cmd_scan_end();

void app_cmd_update_scan_param(sonata_gap_scan_param_t * param)  ;

void app_cmd_conn_result_handler(uint8_t conidx);

void app_cmd_baud_apply();

void app_cmd_conn_start(uint8_t conIdx);

void app_cmd_conn_done(uint8_t conIdx);

void app_cmd_init_uart_at();

void app_cmd_print_raw_data(uint8_t *data, uint8_t length);

void app_cmd_print_end();

void app_cmd_print_at_raw_data(uint8_t id, uint8_t *data, uint8_t length);

bool app_cmd_check_param_mac(uint8_t *param);

void app_cmd_ledisc_done(uint8_t conidx);

void app_cmd_adv_done();

void app_cmd_send_throughput_data();

void app_command_handle_ntf_complete_ind();

void app_command_handle_gatt_event(uint8_t conidx, uint16_t handle, uint16_t type, uint16_t length, uint8_t *value);

void app_command_handle_param_updated();

void app_command_handle_le_pkt_size();

bool app_command_handle_peer_command(uint8_t *value, uint16_t length);

void app_cmd_mconn_update_status(uint8_t act, uint8_t conidx, uint8_t *addr);

bool app_cmd_mconn_continue();

void app_cmd_active_clear_handler(uint16_t opt);

bool app_cmd_discovery_handler(uint8_t status);
void app_cmd_ntf_switch_handler(uint8_t conidx, bool start)  ;
void app_cmd_ntf_timer_handler();

bool app_cmd_timer_handler(uint16_t id);

uint8_t app_command_get_cmdid();
uint8_t app_command_get_cmdtype();
void app_cmd_devinfo_done_handler(uint8_t currentInfo);

void *getTempBuffer(uint16_t size);
void *getAllTempBuffer(uint16_t size);
#endif //BLE_CODE_APP_COMMAND_H
