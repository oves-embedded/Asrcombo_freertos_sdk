
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "lega_wlan_api.h"
#include "tcpip.h"
#include "lega_at_api.h"
#include "soc_init.h"
#include "lwip_app_iperf.h"
#include "duet_version.h"
#include "duet_uart.h"
#include "duet_pinmux.h"
#include "duet_flash_kv.h"
#include "elog.h"
#include "payg.h"

#include "eeprom.h"

#include "gatt.h"


__IO uint8_t g_pub_json[JSON_LEN];


uint8_t g_gatt_data_ram[GATT_RAM_SIZE];
gatt_data_ram_TypeDef g_gatt_data_ram_mng;

gatt_prop_Typedef g_gatt_prop_list[GATT_PROP_LIST_NUM];

extern USER_SET_TypeDef g_UserSet;


const char *g_attlist[]={"opid\0","ppid\0","flid\0","frmv\0"};
const char *g_cmdlist[]={"pubk\0","wifi\0","gstw\0","gctw\0","napn\0","swch\0","read\0","rptm\0","hbfq\0",\
	                     "sfid\0","aovs\0","cbte\0","aoss\0","bcvc\0","mxcc\0","sacc\0"};
const char *g_stslist[]={"sstc\0","crtm\0","udtm\0","time\0","plat\0","plon\0","gpss\0","gpfc\0",\
						 "rcrd\0","trhd\0","tpgd\0","pgst\0","tkre\0","ocst\0","acos\0","accs\0","mpcs\0"};
const char *g_dtalist[]={"batp\0","inpp\0","outp\0","aeng\0","pckv\0","pckc\0","rsoc\0","rcap\0","fccp\0",\
					     "pckt\0","acyc\0","pviv\0","pvic\0","pvie\0","aciv\0","acov\0","acfr\0","accu\0",\
					     "acip\0","acop\0","acie\0","acoe\0","iite\0","rsoh\0","rdnt\0","rcht\0"};
const char *g_dialist[]={"cv01\0","cv02\0","cv03\0","cv04\0","cv05\0","cv06\0","cv07\0","cv08\0","cv09\0","cv10\0",\
						 "cv11\0","cv12\0","cv13\0","cv14\0","cv15\0","cv16\0","cv17\0","cv18\0","cv19\0","cv20\0",\
						 "cv21\0","cv22\0","cv23\0","pkt1\0","pkt2\0","pkt3\0","pkt4\0","pkt5\0","pkt6\0"};






const gatt_attri_ovitem_TypeDef g_gatt_attri_ovitem_list[]=
{
	{"opid\0",TYPE_STRS,MEM_SIZE_OPID,"opid:5:OEM Device ID. Factory set.\0"},
	{"ppid\0",TYPE_STRS,MEM_SIZE_PPID,"ppid:5:PAYG Device ID. Factory set. Field override options.\0"},
	{"flid\0",TYPE_STRS,MEM_SIZE_FLID,"flid:5:Fleet ID. Factory set. Field override options.\0"},
	{"ccid\0",TYPE_STRS,MEM_SIZE_CCID,"ccid:5: iccid \0"},
	{"frmv\0",TYPE_STRS,MEM_SIZE_FRMV,"frmv:5: Firmware Version. Factory set.\0"},

	{"pubk\0",TYPE_STRS,MEM_SIZE_PUBK,"pubk:5:Public Key / Last Code\0"},
	{"gstw\0",TYPE_UINT16,MEM_SIZE_GSTW,"gstw:0:GPRS Sleep Time Window in mins\0"},
	{"gctw\0",TYPE_UINT16,MEM_SIZE_GCTW,"gctw:0:GPRS Connect Time Window in mins\0"},
	{"napn\0",TYPE_STRS,MEM_SIZE_NAPN,"napn:5:GPRS Carrier APN Name\0"},
	{"swch\0",TYPE_UINT16,MEM_SIZE_SWCH,"swch:0:Generic device switch swch = ON | OFF \0"},
	{"read\0",TYPE_UINT16,MEM_SIZE_READ,"read:0:Last read request of ANY GATT <data> = [opid|...|]\0"},
	{"rptm\0",TYPE_UINT16,MEM_SIZE_RPTM,"rptm:0:Set notification request mode = [0|1|2|3|4], 0: Auto-Periodic | 1 = Request-Response | 2 = Change-Notification\0"},
	{"raml\0",TYPE_STRS,MEM_SIZE_RAML,"raml:5:raml list set\0"},
	{"rtmd\0",TYPE_STRS,MEM_SIZE_RTMD,"rtmd:5:Total adjusted mileage\0"},
	{"hbfq\0",TYPE_UINT16,MEM_SIZE_HBFQ,"hbfq:0:Heart Beat Interval Minutes\0"},
	{"wifi\0",TYPE_STRS,MEM_SIZE_WIFI,"wifi:5: wifi station ,wifi password\0"},

	{"sfid\0",TYPE_STRS,MEM_SIZE_SFID,"sfid:5:Setting the fleet ID\0"},																				
	//{"aovs\0",MEM_SIZE_AOVS,"aovs:0:AC Output Voltage Selection ( V)\0",TYPE_UINT16},																		
	//{"aofs\0",MEM_SIZE_AOFS,"aofs:0:Output Frequency Selection (Hz)\0",TYPE_UINT16},																		
	{"cbte\0",TYPE_UINT16,MEM_SIZE_CBTE,"cbte:0:Current battery type 0:lead-acid 1:  water-filled 2:foIon battery 3:for customised mode\0"},
	//{"aoss\0",MEM_SIZE_AOSS,"aoss:0:AC Output Source Selectoion Priority  00-UTI , 01-SUB , 02-SBU \0",TYPE_UINT16},										
	{"bcvc\0",TYPE_UINT16,MEM_SIZE_BCVC,"bcvc:0:Battery Constant Voltage Charge Voltage (V)\0"},															
	{"bcvf\0",TYPE_UINT16,MEM_SIZE_BCVF,"bcvf:0:Battery Float Charge Voltage (V)\0"},																		
	//{"mxcc\0",MEM_SIZE_MXCC,"mxcc:0:Max Charge Current Setting ( A)\0",TYPE_UINT16},																		
	{"sacc\0",TYPE_UINT16,MEM_SIZE_SACC,"sacc:0:Setting the AC charging current(A)\0"},	

	{"acos\0",TYPE_UINT16,MEM_SIZE_ACOS,"acos:0:AC Output Status\0"},
	{"mpcs\0",TYPE_UINT16,MEM_SIZE_MPCS,"mpcs:0:MPPT charging status\0"},

	{"aoct\0",TYPE_UINT16,MEM_SIZE_AOCT,"aoct:0:AC Output Control { [x00-- / x01-- | x--01 / x--01] = [Output Enable On/Off | Standby On/Off]}\0"},
	{"aoss\0",TYPE_UINT16,MEM_SIZE_AOSS,"aoss:0:AC Output Source Selectoion Priority {0:BAT | 1:PV | 2:UTL | 3:PV & UTL}\0"},
	{"bcss\0",TYPE_UINT16,MEM_SIZE_BCSS,"bcss:0:Battery Charge Source Selectoion Priority {0:PV First | 1:PV@UTL | 2:PV Only}\0"},
	{"uost\0",TYPE_UINT16,MEM_SIZE_UOST,"uost:0:Utility Output Start Time\0"},
	{"uoet\0",TYPE_UINT16,MEM_SIZE_UOET,"uoet:0:Utility Output End Time\0"},
	{"ucst\0",TYPE_UINT16,MEM_SIZE_UCST,"ucst:0:Utility Charge Start Time\0"},
	{"ucet\0",TYPE_UINT16,MEM_SIZE_UCET,"ucet:0:Utility Charge End Time\0"},
	{"pimd\0",TYPE_UINT16,MEM_SIZE_PIMD,"pimd:0:PV Input Mode {0：Independent | 1：Parallel}\0"},
	{"aimd\0",TYPE_UINT16,MEM_SIZE_AIMD,"aimd:0:PV Input Mode {0：APL,90-280VAC | 1：UPS,170-280VAC | 2：GEN}\0"},
	{"lngs\0",TYPE_UINT16,MEM_SIZE_LNGS,"lngs:0:Language Selection {0：Chinese | 1：English}\0"},
	{"aovs\0",TYPE_UINT16,MEM_SIZE_AOVS,"aovs:0:AC Output Voltage Selection {0:208 | 1:230 | 2:240 | 3:220 4:100 | 5:110 | 6:120}\0"}, 
	{"aofs\0",TYPE_UINT16,MEM_SIZE_AOFS,"aofs:0:Output Frequency Selection {0:50 | 1:60}\0"},  
	{"olrs\0",TYPE_UINT16,MEM_SIZE_OLRS,"olrs:0:Overload Restart Mode {0:Yes | 1:No | 2:UTL}\0"},  
	{"otrs\0",TYPE_UINT16,MEM_SIZE_OTRS,"bzon:0:Over Temperature Restart Mode {0:Yes | 1:No }\0"},  
	{"bzon\0",TYPE_UINT16,MEM_SIZE_BZON,"bzon:0:Buzzer Enable On/Off\0"}, 
	{"cass\0",TYPE_UINT16,MEM_SIZE_CASS,"cass:0:Communication Address SS?\0"},
	{"mxcc\0",TYPE_UINT16,MEM_SIZE_MXCC,"mxcc:0:Max Charge Current Setting\0"},
	{"bbcv\0",TYPE_UINT16,MEM_SIZE_BBCV,"bbcv:0:Bulk Charge Phase Voltage Setting\0"},
	{"bfcv\0",TYPE_UINT16,MEM_SIZE_BFCV,"bfcv:0:Float Charge Phase Voltage Setting\0"},
	{"lbus\0",TYPE_UINT16,MEM_SIZE_LBUS,"lbus:0:Low Battery Switch To Utility Voltage Threshold\0"},
	{"accs\0",TYPE_UINT16,MEM_SIZE_ACCS,"accs:0:AC Charge Current Setting\0"},
	{"agmd\0",TYPE_UINT16,MEM_SIZE_AGMD,"agmd:0:Aging Mode?\0"},
	{"stys\0",TYPE_UINT16,MEM_SIZE_STYS,"stys:0:System Time Year Set\0"},
	{"stms\0",TYPE_UINT16,MEM_SIZE_STMS,"stms:0:System Time Month Set\0"},
	{"stds\0",TYPE_UINT16,MEM_SIZE_STDS,"stds:0:System Time Day Set\0"},
	{"sths\0",TYPE_UINT16,MEM_SIZE_STHS,"sths:0:System Time Hour Set\0"},
	{"stmm\0",TYPE_UINT16,MEM_SIZE_STMM,"stmm:0:System Time Minute Set\0"},
	{"stss\0",TYPE_UINT16,MEM_SIZE_STSS,"stss:0:System Time Second Set" },  
	{"batp\0",TYPE_UINT16,MEM_SIZE_BATP,"batp:0:Battery Net Charging Power W\0"},
	{"inpp\0",TYPE_UINT16,MEM_SIZE_INPP,"inpp:0:Total Input Power W\0"},
	{"outp\0",TYPE_UINT16,MEM_SIZE_OUTP,"outp:0:Output Power W,\0"},
	{"aeng\0",TYPE_UINT16,MEM_SIZE_AENG,"aeng:0:Accumulated Energy Output in Whs\0"},
	{"pckv\0",TYPE_UINT32,MEM_SIZE_PCKV,"pckv:2:Pack Voltage mV\0"},
	{"pckc\0",TYPE_INT,MEM_SIZE_PCKC,"pckc:1:Pack Current in A\0"},
	{"rsoc\0",TYPE_UINT16,MEM_SIZE_RSOC,"rsoc:0:Relative State of Charge\0"},
	{"rcap\0",TYPE_UINT16,MEM_SIZE_RCAP,"rcap:0:Remaining Capacity in Whs\0"},
	{"fccp\0",TYPE_UINT16,MEM_SIZE_FCCP,"fccp:0:Full Charge Capacity in Whs\0"},
	{"pckt\0",TYPE_INT,MEM_SIZE_PCKT,"pckt:1:Pack Temperature in Celsius\0"},
	{"acyc\0",TYPE_UINT16,MEM_SIZE_ACYC,"acyc:0:Accumulated Cycles\0"},

	{"sstc\0",TYPE_UINT16,MEM_SIZE_SSTC,"sstc:0:System Status Code\0"},
	{"crtm\0",TYPE_STRS,MEM_SIZE_CRTM,"crtm:5:created at time\0"},
	{"udtm\0",TYPE_STRS,MEM_SIZE_UDTM,"udtm:5:update at time\0"},
	{"time\0",TYPE_STRS,MEM_SIZE_TIME,"time:5:Device internal timer readout at last communication\0" },
	{"plat\0",TYPE_STRS,MEM_SIZE_PLAT,"plat:5:Device GPS Latitude\0"},
	{"plon\0",TYPE_STRS,MEM_SIZE_PLON,"plon:5:Device GPS Longitude\0"},
	{"gpss\0",TYPE_UINT16,MEM_SIZE_GPSS,"gpss:0:GPS Signal Strength in db\0"},
	{"gpfc\0",TYPE_UINT16,MEM_SIZE_GPFC,"gpfc:0:GPRS Connect Failure Counts\0"},
	{"rcrd\0",TYPE_UINT16,MEM_SIZE_RCRD,"rcrd:0:Remaining Credit Days\0"},
	{"trhd\0",TYPE_UINT16,MEM_SIZE_TRHD,"trhd:0:Total Running History Days\0"},
	{"tpgd\0",TYPE_UINT16,MEM_SIZE_TPGD,"tpgd:0:PAYG Running Days\0"},
	{"pgst\0",TYPE_UINT16,MEM_SIZE_PGST,"pgst:0:PAYG State = PAYG | FREE\0"},
	{"tkre\0",TYPE_UINT16,MEM_SIZE_TKRE,"tkre:0:token response = NEW | REPEAT | ERROR\0"},
	{"ocst\0",TYPE_UINT16,MEM_SIZE_OCST,"ocst:0:Device Output State \0"},

	{"slon\0",TYPE_STRS,MEM_SIZE_SLON,"slon:5:Satellite Longitude in DD (decimal degrees)\0"},
	{"slat\0",TYPE_STRS,MEM_SIZE_SLAT,"slat:5:Satellite Latitude in DD (decimal degrees)\0"},
	{"salt\0",TYPE_INT,MEM_SIZE_SALT,"salt:1:Satellite Altitude in Metres\0"},
	{"sstm\0",TYPE_STRS,MEM_SIZE_SSTM,"sstm:5:Satellite-Synchronized Network Clock time in ISO 8601\0"},
	{"sgps\0",TYPE_UINT16,MEM_SIZE_SGPS,"sgps:0:Satellite GPS Strength\0"},
	{"sspe\0",TYPE_UINT16,MEM_SIZE_SSPE,"sspe:0:Satellite Speed in kM/h\0"},
	{"ppst\0",TYPE_UINT16,MEM_SIZE_PPST,"ppst:0:PACK protection status\0"},
	{"pmcs\0",TYPE_UINT16,MEM_SIZE_PMCS,"pmcs:0:PACK MOSFET control status\0"},
	{"cmos\0",TYPE_UINT16,MEM_SIZE_CMOS,"cmos:0:Charging MOS Status, 0 for off, 1 for on\0"}, 
	{"dmos\0",TYPE_UINT16,MEM_SIZE_DMOS,"dmos:0:Discharging MOS Status, 0 for off, 1 for on\0"}, 
	{"ctmp\0",TYPE_INT,MEM_SIZE_CTMP,"ctmp:1:Controller temperature (uint8_t, 1 Celsius) - Offset: 40, Min: -40\0"}, 
	{"mtpm\0",TYPE_INT,MEM_SIZE_MTPM,"mtpm:1:Motor temperature (uint8_t, 1 Celsius) - Offset: 40, Min: -40, Max: 200\0"}, 
	{"mtrd\0",TYPE_UINT16,MEM_SIZE_MTRD,"mtrd:0:Motor speed (uint16_t, 1RPM)\0"}, 
	{"tspd\0",TYPE_UINT16,MEM_SIZE_TSPD,"tspd:0:Tyre speed (uint16_t, 1RPM)\0"}, 
	{"rvlt\0",TYPE_UINT16,MEM_SIZE_RVLT,"rvlt:0:Real-time voltage \0"}, 
	{"rcur\0",TYPE_INT,MEM_SIZE_RCUR,"rcur:1:Real-time current\0"},
	{"rmax\0",TYPE_UINT16,MEM_SIZE_RMAX,"rmax:0:Rated maximum input current \0"}, 
	{"cmxs\0",TYPE_UINT16,MEM_SIZE_CMXS,"cmxs:0:Current set maximum speed (uint16_t, 1RPM) - Min: 0, Max: 20000\0"}, 
	{"cmxc\0",TYPE_UINT16,MEM_SIZE_CMXC,"cmxc:0:Current set maximum input current (uint16_t, 1A) - Min: 0, Max: 2000\0"},
	{"tomd\0",TYPE_STRS,MEM_SIZE_TOMD,"tomd:5:Total mileage driven,Km\0"},
	{"cumd\0",TYPE_STRS,MEM_SIZE_CUMD,"cumd:5:Current mileage driven,Km\0"},


	{"pviv\0",TYPE_UINT16,MEM_SIZE_PVIV,"cv01:0:pviv:0:Photovoltaic (PV) Input Voltage - Unit V\0"},		 
	{"pvic\0",TYPE_UINT16,MEM_SIZE_PVIC,"cv01:0:pvic:0:Photovoltaic (PV) Input Current - Unit A\0"},			 
	{"pvip\0",TYPE_UINT16,MEM_SIZE_PVIP,"cv01:0:pvip:0:Photovoltaic (PV) Input Power - Unit W\0"},				 
	{"pvie\0",TYPE_UINT32,MEM_SIZE_PVIE,"cv01:0:pvie:2:Photovoltaic (PV) Cumulative Input Energy - Unit Wh\0"}, 						 
	{"aciv\0",TYPE_UINT16,MEM_SIZE_ACIV,"cv01:0:aciv:0:AC Input Voltage - Unit V\0"},							 
	{"acov\0",TYPE_UINT16,MEM_SIZE_ACOV,"cv01:0:acov:0:AC Output Voltage - Unit V\0"},							 
	{"acfr\0",TYPE_UINT16,MEM_SIZE_ACFR,"cv01:0:acfr:0:AC Frequency - Unit: Hz\0"},								 
	{"accu\0",TYPE_INT,  MEM_SIZE_ACCU,"cv01:0:AC Input/Output Current - Unit A\0"},					 
	{"acip\0",TYPE_UINT16,MEM_SIZE_ACIP,"cv01:0:AC Input Power - Unit: W\0"}, 							 
	{"acop\0",TYPE_UINT16,MEM_SIZE_ACOP,"cv01:0:AC Output Power - Unit: W\0"},						 
	{"acie\0",TYPE_UINT32,MEM_SIZE_ACIE,"cv01:0:AC Cumulative Input Energy - Unit Wh\0"},			 
	{"acoe\0",TYPE_UINT32,MEM_SIZE_ACOE,"cv01:0:AC Cumulative Output Energy - Unit Wh\0"},			 
	{"iite\0",TYPE_UINT16,MEM_SIZE_IITE,"cv01:0:Inverter Internal Temperature - Unit °C\0"},
	{"rsoh\0",TYPE_UINT16,MEM_SIZE_RSOH,"cv01:0:State of Health\0"},
	{"rdnt\0",TYPE_UINT16,MEM_SIZE_RDNT,"cv01:0:Remaining Discharge Time- Unit 0.1H\0"},
	{"rcht\0",TYPE_UINT16,MEM_SIZE_RCHT,"cv01:0:Remaining Charge Time- Unit 0.1H\0"},
	
	{"cv01\0",TYPE_UINT16,MEM_SIZE_CV01,"cv01:0:Cell 01 Voltage mV\0"},
	{"cv02\0",TYPE_UINT16,MEM_SIZE_CV02,"cv02:0:Cell 02 Voltage mV\0"},
	{"cv03\0",TYPE_UINT16,MEM_SIZE_CV03,"cv03:0:Cell 03 Voltage mV\0"},
	{"cv04\0",TYPE_UINT16,MEM_SIZE_CV04,"cv04:0:Cell 04 Voltage mV\0"},
	{"cv05\0",TYPE_UINT16,MEM_SIZE_CV05,"cv05:0:Cell 05 Voltage mV\0"},
	{"cv06\0",TYPE_UINT16,MEM_SIZE_CV06,"cv06:0:Cell 06 Voltage mV\0"},
	{"cv07\0",TYPE_UINT16,MEM_SIZE_CV07,"cv07:0:Cell 07 Voltage mV\0"},
	{"cv08\0",TYPE_UINT16,MEM_SIZE_CV08,"cv08:0:Cell 08 Voltage mV\0"},
	{"cv09\0",TYPE_UINT16,MEM_SIZE_CV09,"cv09:0:Cell 09 Voltage mV\0"},
	{"cv10\0",TYPE_UINT16,MEM_SIZE_CV10,"cv10:0:Cell 10 Voltage mV\0"},
	{"cv11\0",TYPE_UINT16,MEM_SIZE_CV11,"cv11:0:Cell 11 Voltage mV\0"},
	{"cv12\0",TYPE_UINT16,MEM_SIZE_CV12,"cv12:0:Cell 12 Voltage mV\0"},
	{"cv13\0",TYPE_UINT16,MEM_SIZE_CV13,"cv13:0:Cell 13 Voltage mV\0"},
	{"cv14\0",TYPE_UINT16,MEM_SIZE_CV14,"cv14:0:Cell 14 Voltage mV\0"},
	{"cv15\0",TYPE_UINT16,MEM_SIZE_CV15,"cv15:0:Cell 15 Voltage mV\0"},
	{"cv16\0",TYPE_UINT16,MEM_SIZE_CV16,"cv16:0:Cell 16 Voltage mV\0"},
	{"cv17\0",TYPE_UINT16,MEM_SIZE_CV17,"cv17:0:Cell 17 Voltage mV\0"},
	{"cv18\0",TYPE_UINT16,MEM_SIZE_CV18,"cv18:0:Cell 18 Voltage mV\0"},
	{"cv19\0",TYPE_UINT16,MEM_SIZE_CV19,"cv19:0:Cell 19 Voltage mV\0"},
	{"cv20\0",TYPE_UINT16,MEM_SIZE_CV20,"cv20:0:Cell 20 Voltage mV\0"},
	{"cv21\0",TYPE_UINT16,MEM_SIZE_CV21,"cv21:0:Cell 21 Voltage mV\0"},
	{"cv22\0",TYPE_UINT16,MEM_SIZE_CV22,"cv22:0:Cell 22 Voltage mV\0"},
	{"cv23\0",TYPE_UINT16,MEM_SIZE_CV23,"cv23:0:Cell 21 Voltage mV\0"},
	{"pkt1\0",TYPE_INT,MEM_SIZE_TEMP1,"pkt1:1:Pack Temperature-1 Celsius\0"},
	{"pkt2\0",TYPE_INT,MEM_SIZE_TEMP1,"pkt2:1:Pack Temperature-2 Celsius\0"},
	{"pkt3\0",TYPE_INT,MEM_SIZE_TEMP1,"pkt3:1:Pack Temperature-3 Celsius\0"},
	{"pkt4\0",TYPE_INT,MEM_SIZE_TEMP1,"pkt4:1:Pack Temperature-4 Celsius\0"},
	{"pkt5\0",TYPE_INT,MEM_SIZE_TEMP1,"pkt5:1:Pack Temperature-5 Celsius\0"},
	{"pkt6\0",TYPE_INT,MEM_SIZE_TEMP1,"pkt6:1:Pack Temperature-6 Celsius\0"}

};


#define TOPIC_LEN    128

char MQTT_ClienID[60];
char MQTT_topic_ppid[TOPIC_LEN];
//char MQTT_topic_ppid_ext[20];

char MQTT_topic_ppid_Subscribe[60];

const uint8_t g_MqttPubFixedPath[]="dt/V01/GPRSV2/\0";
const uint8_t g_MqttSubFixedPath[]="cmd/V01/GPRSV2/+/BATT/\0";
const uint8_t g_MqttComparePath[]="cmd/V01/GPRSV2/\0";
const uint8_t g_MqttProductName[]="/BATT/\0";

extern PAYG_TypeDef payg;
extern USER_SET_TypeDef g_UserSet;


uint8_t* AtGetPubTopicId(uint8_t * ext_topic)
{

	uint8_t tempstr[128]={0};
	//Creat Client ID
	memset(MQTT_ClienID,0x00,20);
	#ifdef PUMP_PROJECT
	sprintf((char*)MQTT_ClienID,"ovPump%s",payg.oem_id);
	#elif defined(CAMP_PROJECT)
	//memcpy(MQTT_ClienID, GattGetPpid(), 14);
	GattGetPpid((uint8_t*)MQTT_ClienID);
	#elif defined(BMS_CAMP_SUPPORT)
	GattGetData( LIST_ATT, ATT_OPID, MQTT_ClienID);
	#elif defined(BLE_MASTER_ENABLE)
	GattGetData( LIST_ATT, ATT_OPID, MQTT_ClienID);
	#else
	memcpy(MQTT_ClienID,payg.oem_id/*json+ 32"99AH1712099999"*/, strlen((char*)payg.oem_id));
	#endif
	//Creat Topic
	memset(MQTT_topic_ppid, 0x00,TOPIC_LEN);

	if(strlen((char*)g_UserSet.fleed)>0&&strlen((char*)g_UserSet.fleed)<40)
	{
		sprintf(tempstr,"%s%s%s",g_MqttPubFixedPath,g_UserSet.fleed,g_MqttProductName);
		memcpy(MQTT_topic_ppid, tempstr, strlen((char*)tempstr));
		}
	else
	{	sprintf(tempstr,"%s%s",g_MqttPubFixedPath,g_MqttProductName);
		memcpy(MQTT_topic_ppid, tempstr, strlen((char*)tempstr));
		} 
	memcpy(MQTT_topic_ppid + strlen((char*)MQTT_topic_ppid), MQTT_ClienID, strlen((char*)MQTT_ClienID));

	if(ext_topic!=NULL)
	{
		sprintf(MQTT_topic_ppid + strlen(MQTT_topic_ppid),"%s",ext_topic);
		}

	return MQTT_topic_ppid;
}

uint8_t*  AtGetSubscribeId(void)
{
	#ifdef PUMP_PROJECT
	//sprintf((char*)MQTT_ClienID,"ovPump%s",g_UserSet.Payg.oem_id);
	sprintf((char*)MQTT_ClienID,"ovPump%s",payg.oem_id);
	#elif defined(CAMP_PROJECT)
	//memcpy(MQTT_ClienID, GattGetPpid(), 14);
	GattGetPpid(MQTT_ClienID);
	#elif defined(BMS_CAMP_SUPPORT)
	GattGetData( LIST_ATT, ATT_OPID, MQTT_ClienID);
	#elif defined(BLE_MASTER_ENABLE)
	GattGetData( LIST_ATT, ATT_OPID, MQTT_ClienID);
	#else
	memcpy(MQTT_ClienID, payg.oem_id, strlen((char*)payg.oem_id));
	#endif
	
	memset(MQTT_topic_ppid_Subscribe, 0x00,60);
	memcpy(MQTT_topic_ppid_Subscribe, g_MqttSubFixedPath, strlen((char*)g_MqttSubFixedPath));
	memcpy(MQTT_topic_ppid_Subscribe + strlen(MQTT_topic_ppid_Subscribe),MQTT_ClienID, strlen(MQTT_ClienID));
	memcpy(MQTT_topic_ppid_Subscribe + strlen(MQTT_topic_ppid_Subscribe),"/#", 2);

	return MQTT_topic_ppid_Subscribe;
	
}


uint8_t  gatt_attri_list_search(char*prop,gatt_attri_ovitem_TypeDef*pitem)
{
	int i;

	for(i=0;i<sizeof(g_gatt_attri_ovitem_list)/sizeof(gatt_attri_ovitem_TypeDef);i++)
	{
		if(strstr(g_gatt_attri_ovitem_list[i].prop ,prop)!=NULL)
		{
			memcpy(pitem,&g_gatt_attri_ovitem_list[i],sizeof(gatt_attri_ovitem_TypeDef));

			//log_i("gatt database :%s  len:%d \n",g_gatt_attri_ovitem_list[i].prop,g_gatt_attri_ovitem_list[i].len );
			
			return TRUE;
			}
		}

	return FALSE;
}
int8_t gatt_prop_list_add(uint8_t name,const char *list[],uint16_t len,gatt_prop_Typedef*root)
{
	gatt_attri_ovitem_TypeDef temp;
	uint16_t i;

	for(i=0;i<len;i++)
	{
		if(gatt_attri_list_search((char*)list[i],&temp))
		{
			root[i].prop=temp.prop;
			root[i].meta=temp.meta;
			root[i].data_type=temp.type;
			root[i].len=temp.len;
			root[i].list_name=name;
			//log_i("list:%d  len=%d\n",sizeof(g_attlist),sizeof(char*));
			
			
			if(g_gatt_data_ram_mng.pos+temp.len<sizeof(g_gatt_data_ram))
			{	root[i].value=&g_gatt_data_ram_mng.ram[g_gatt_data_ram_mng.pos];
				g_gatt_data_ram_mng.pos+=temp.len;

				//log_i("added succ %d...!!!\n",i);
				}
			else
			{   log_i("gatt searched :%s--%s len %d overflow...!!!\n",list[i],temp.prop,temp.len);
				return -1;
				}
			}
		else
		{
			log_i("gatt searched :%s--%d fail!!!\n",list[i],i);
			}
		}
	return i; 
}
void gatt_list_create(void)
{
	int pos=0,ret=0;

	g_gatt_data_ram_mng.pos=0;
	g_gatt_data_ram_mng.ram=g_gatt_data_ram;
	
	memset(g_gatt_data_ram,0x00,sizeof(g_gatt_data_ram));
	memset(g_gatt_prop_list,0x00,sizeof(g_gatt_prop_list));
	memset(g_gatt_prop_list,0x00,sizeof(g_gatt_prop_list));

	log_i("gatt att items:%d  \n",sizeof(g_attlist)/4);
	log_i("gatt cmd items:%d  \n",sizeof(g_cmdlist)/4);
	log_i("gatt sts items:%d  \n",sizeof(g_stslist)/4);
	log_i("gatt dta items:%d  \n",sizeof(g_dtalist)/4);
	log_i("gatt dia items:%d  \n",sizeof(g_dialist)/4);

	ret=gatt_prop_list_add(LIST_ATT,g_attlist,sizeof(g_attlist)/4,g_gatt_prop_list);
	if(ret>=0)
		pos+=ret;
	ret=gatt_prop_list_add(LIST_CMD,g_cmdlist,sizeof(g_cmdlist)/4,&g_gatt_prop_list[pos]);
	if(ret>=0)
		pos+=ret;
	ret=gatt_prop_list_add(LIST_STS,g_stslist,sizeof(g_stslist)/4,&g_gatt_prop_list[pos]);
	if(ret>=0)
		pos+=ret;
	ret=gatt_prop_list_add(LIST_DTA,g_dtalist,sizeof(g_dtalist)/4,&g_gatt_prop_list[pos]);
	if(ret>=0)
		pos+=ret;
	ret=gatt_prop_list_add(LIST_DIA,g_dialist,sizeof(g_dialist)/4,&g_gatt_prop_list[pos]);
	if(ret>=0)
		pos+=ret;

	log_i("gatt created  :members %d  ram used:%d...!!!\n",pos,g_gatt_data_ram_mng.pos);

	//for(i=0;i<pos;i++)
	{
		//log_i("[%d] %s :%d...!!!\n",g_gatt_prop_list[i].list_name,g_gatt_prop_list[i].prop,g_gatt_prop_list[i].len);
		}
		

}

uint8_t gatt_set_prop_data(char* prop,uint8_t*data)
{
	gatt_prop_Typedef *p=NULL;
	uint8_t i;

	for(i=0;i<GATT_PROP_LIST_NUM;i++)
	{
		if(strstr(prop,g_gatt_prop_list[i].prop)!=NULL)
		{
			p=&g_gatt_prop_list[i];
			break;
			}
		}
	
	if(p==NULL)
		return FALSE;
	
	
	switch(p->data_type)
	{
		case TYPE_UINT16:
			memcpy(p->value,data,2);
			break;
		case TYPE_STRS:	
			memcpy((uint8_t*)p->value,data,strlen((char*)data));
			break;
		case TYPE_INT:	
			memcpy((uint8_t*)p->value,data,2);
			break;
		case TYPE_FLOAT_STR:	
			memcpy(p->value,data,strlen((char*)data));
			break;	
		case TYPE_UINT32:	
			memcpy(p->value,data,4);
			break;
		case TYPE_INT32:	
			memcpy(p->value,data,4);
			break;
		 default:
			return FALSE;
	}
	return TRUE;
}

gatt_prop_Typedef* gatt_get_prop_list(void)
{
	return g_gatt_prop_list;
}

uint8_t gatt_get_prop_data(char* prop,uint8_t*data)
{
	gatt_prop_Typedef *p=NULL;
	uint8_t i;

	for(i=0;i<GATT_PROP_LIST_NUM;i++)
	{
		if(strstr(prop,g_gatt_prop_list[i].prop)!=NULL)
		{
			p=&g_gatt_prop_list[i];
			break;
			}
		}


	if(p==NULL)
		return FALSE;
	
	switch(p->data_type)
	{
		case TYPE_UINT16:
			memcpy(data,(uint8_t*)p->value,2);
			break;
		case TYPE_STRS:	
			memcpy(data,(uint8_t*)p->value,strlen((char*)p->value));
			break;
		case TYPE_INT:	
			memcpy(data,(uint8_t*)p->value,2);
			break;
		case TYPE_FLOAT_STR:	
			memcpy(data,p->value,strlen((char*)p->value));
			break;
		case TYPE_UINT32:	
			memcpy(data,p->value,4);
			break;
		case TYPE_INT32:	
			memcpy(data,p->value,4);
			break;	
		default:
			return FALSE;
	}
	return TRUE;
}


void ov_gatt_init(void)
{
	gatt_list_create();
}



gatt_prop_Typedef *GattGetListInfor(uint8_t dt_list,uint8_t *count)
{
	gatt_prop_Typedef *p=NULL;
	uint16_t i,num=0;
	uint16_t first=0xffff;

	for(i=0;i<GATT_PROP_LIST_NUM;i++)
	{
		if(g_gatt_prop_list[i].list_name==dt_list)
		{	
			num++;
			if(first==0xffff)
				first=i;
			}
		}

	*count=num;

	p=(gatt_prop_Typedef*)&g_gatt_prop_list[first];
	if(first!=0xffff)
		return p;
	else
		return NULL;


}


uint8_t* GattAllFieldJsonMerge(void)
{
	uint8_t i,j,*json,count;
	gatt_prop_Typedef *p;

	memset(g_pub_json,0x00,JSON_LEN);

	json=g_pub_json;

	//GattSetGprsConnectFailCnt(g_UserSet.report_fail_cnt);
	//GattSetGprsConnectTime(g_UserSet.onlinetime);
   	//GattSetGprsSleepTime(g_UserSet.sleeptime);

	//if(g_UserSet.raml_num==0)
	//	GattSetData( LIST_CMD, CMD_RAML, "null");
	
	for(i=0;i<LIST_COUNT;i++)
	{
		p=GattGetListInfor(i,&count);

		if(p==NULL)
			continue;	
		
		switch(i)
		{
			case LIST_ATT:
				memcpy(json,"{\"att\":{",8);
				break;
			case LIST_CMD:	
				json-=1;
				//cmd
				memcpy(json,"},\"cmd\":{",9);
				break;
			case LIST_STS:	
			    json-=1;
				//cmd
				memcpy(json,"},\"sts\":{",9);
				break;
			case LIST_DTA:	
			    json-=1;
				//cmd
				#ifdef CHARGE_STATION
				memcpy(json,"},",2);
				json+=2;
				GattDtaArrayFieldMerge(json);
				json+=strlen((char*)json);
				continue;
				#else
				memcpy(json,"},\"dta\":{",9);
				#endif
				break;
			case LIST_DIA:
				#ifdef CHARGE_STATION
				memcpy(json,",",1);
				json+=1;
				GattDiaArrayFieldMerge(json);
				json+=strlen((char*)json);
				continue;
				#else
			    json-=1;
				//cmd
				memcpy(json,"},\"dia\":{",9);
				#endif
				break;
			}
		
		json+=strlen((char*)json);
	
		for(j=0;j<count;j++)
		{
				/*if(*g_gatt_list.att[i].meta_ctrl)
					sprintf(json,"\"%s\":\"%s\",",g_gatt_list.att[i].prop,g_gatt_list.att[i].meta);
				else*/
				{
					if(p[j].data_type==TYPE_INT)
						sprintf((char*)json,"\"%s\":%d,",p[j].prop,*((int16_t*)p[j].value));
					else if(p[j].data_type==TYPE_STRS)
						sprintf((char*)json,"\"%s\":\"%s\",",p[j].prop,p[j].value);
					else if(p[j].data_type==TYPE_SUB_STRS)
						sprintf((char*)json,"\"%s\":%s,",p[j].prop,p[j].value);
					else if(p[j].data_type==TYPE_FLOAT_STR)
						sprintf((char*)json,"\"%s\":%s,",p[j].prop,p[j].value);
					else if(p[j].data_type==TYPE_UINT32)
						sprintf((char*)json,"\"%s\":%ld,",p[j].prop,(*(uint32_t*)p[j].value));
					else if(p[j].data_type==TYPE_INT32)
						sprintf((char*)json,"\"%s\":%ld,",p[j].prop,(*(int32_t*)p[j].value));
					else
						sprintf((char*)json,"\"%s\":%d,",p[j].prop,(*(uint16_t*)p[j].value));
					}

				json+=strlen((char*)json);
			}

		}

	#ifdef CHARGE_STATION
	memcpy(json,"}",1);
	#else
	json-=1;
	memcpy(json,"}}",2);
	#endif

	//AtSetTopicId(NULL);

	log_i("json:%s",g_pub_json);

	return g_pub_json;
}



uint8_t* GattDtTypeFieldJsonMerge(uint8_t dt_type,uint8_t meta)
{
	uint8_t i,*json;

	uint8_t topic_ext[20];
	gatt_prop_Typedef *p=NULL;
	uint8_t count=0;

	memset(g_pub_json,0x00,JSON_LEN);
	memset(topic_ext,0x00,20);

	json=g_pub_json;

	//memcpy(json,"{",1);

	//GattSetGprsConnectFailCnt(g_UserSet.report_fail_cnt);
	//GattSetGprsConnectTime(g_UserSet.onlinetime);
   //	GattSetGprsSleepTime(g_UserSet.sleeptime);

	//json+=7;
	json+=strlen((char*)json);

	p=GattGetListInfor(dt_type,&count);

	if(p==NULL)
		return g_pub_json;	
	
	switch(dt_type)
	{
		case LIST_ATT:
		   // memcpy(topic_ext,"/att",4);
		   if(meta)
		   		memcpy(json,"{\"_att\":{",9);
		   	else
				memcpy(json,"{\"att\":{",8);
			//json+=strlen(json);
			break;
		case LIST_CMD:	
			//memcpy(topic_ext,"/cmd",4);
			 if(meta)
			 	memcpy(json,"{\"_cmd\":{",9);
			 else	
			      memcpy(json,"{\"cmd\":{",8);
			//json+=strlen(json);
			break;
		case LIST_STS:	
			//memcpy(topic_ext,"/sts",4);
			if(meta)
				memcpy(json,"{\"sts\":{",9);
			else
				memcpy(json,"{\"sts\":{",8);
			//json+=strlen(json);
			break;
		case LIST_DTA:	
			///memcpy(topic_ext,"/dta",4);
			if(meta)
				memcpy(json,"{\"_dta\":{",9);
			else
				memcpy(json,"{\"dta\":{",8);
			//json+=strlen(json);
			break;
		case LIST_DIA:
			//memcpy(topic_ext,"/dia",4);
			if(meta)
				memcpy(json,"{\"_dia\":{",9);
			else	
				memcpy(json,"{\"dia\":{",8);
			//json+=strlen(json);
			break;
		}

		json+=strlen((char*)json);
		
		for(i=0;i<count;i++)
		{
			if(meta)
			{	sprintf((char*)json,"\"_%s\":\"%s\",",p[i].prop,p[i].meta);
				//memcpy(topic_ext,"/meta",5);
				//AtSetTopicId(topic_ext);
				}
			else
			{
				if(p[i].data_type==TYPE_INT)
					sprintf((char*)json,"\"%s\":%d,",p[i].prop,*((int16_t*)p[i].value));
				else if(p[i].data_type==TYPE_STRS)
					sprintf((char*)json,"\"%s\":\"%s\",",p[i].prop,p[i].value);
				else if(p[i].data_type==TYPE_SUB_STRS)
					sprintf((char*)json,"\"%s\":%s,",p[i].prop,p[i].value);
				else if(p[i].data_type==TYPE_FLOAT_STR)
					sprintf((char*)json,"\"%s\":%s,",p[i].prop,p[i].value);
				else if(p[i].data_type==TYPE_UINT32)
					sprintf((char*)json,"\"%s\":%ld,",p[i].prop,(*(uint32_t*)p[i].value));
				else if(p[i].data_type==TYPE_INT32)
					sprintf((char*)json,"\"%s\":%ld,",p[i].prop,(*(int32_t*)p[i].value));
				else
					sprintf((char*)json,"\"%s\":%d,",p[i].prop,*((uint16_t*)p[i].value));

				//AtSetTopicId(NULL);
				}

			json+=strlen((char*)json);
			
			}

		json-=1;


	memcpy(json,"}}",2);

	return g_pub_json;
}




uint8_t* GattSingleFieldMerge(uint8_t *tag_str)
{
	uint8_t i,j,*json;
	uint8_t topic_ext[20];
	gatt_prop_Typedef *p=NULL;
	uint8_t count=0,meta=0;

	memset(g_pub_json,0x00,JSON_LEN);
	memset(topic_ext,0x00,20);

	//GattSetGprsConnectFailCnt(g_UserSet.report_fail_cnt);
	//GattSetGprsConnectTime(g_UserSet.onlinetime);
   //	GattSetGprsSleepTime(g_UserSet.sleeptime);

	json=g_pub_json;

	

	for(j=0;j<LIST_COUNT;j++)
	{

		p=GattGetListInfor(j,&count);

		if(p==NULL)
			continue;
		

		for(i=0;i<count;i++)
		{
			if(strstr((char*)tag_str,p[i].prop)!=NULL)
			{
					if(strstr((char*)tag_str,"meta")!=NULL)
						meta=TRUE;
					
					switch(j)
					{
						case LIST_ATT:
							if(meta)
								memcpy(json,"{\"_att\":{",9);
							else
								memcpy(json,"{\"att\":{",8);
							break;
						case LIST_CMD:	
							if(meta)
								memcpy(json,"{\"_cmd\":{",9);
							else
								memcpy(json,"{\"cmd\":{",8);
							break;
						case LIST_STS:	
							if(meta)
								memcpy(json,"{\"_sts\":{",9);
							else
								memcpy(json,"{\"sts\":{",8);
							break;
						case LIST_DTA:	
							if(meta)
								memcpy(json,"{\"_dta\":{",9);
							else
								memcpy(json,"{\"dta\":{",8);
							break;
						case LIST_DIA:
							if(meta)
								memcpy(json,"{\"_dia\":{",9);
							else
								memcpy(json,"{\"dia\":{",8);
							break;
						}
					
				json+=strlen((char*)json);
				
				if(meta)
				{	
					sprintf((char*)json,"\"_%s\":\"%s\"",p[i].prop,p[i].meta);
					//memcpy(topic_ext,"/meta",5);
					//AtSetTopicId(topic_ext);
					}
				else
				{
					if(p[i].data_type==TYPE_INT)
						sprintf((char*)json,"\"%s\":%d",p[i].prop,*((int16_t*)p[i].value));
					else if(p[i].data_type==TYPE_STRS)
						sprintf((char*)json,"\"%s\":\"%s\"",p[i].prop,p[i].value);
					else if(p[i].data_type==TYPE_SUB_STRS)
						sprintf((char*)json,"\"%s\":%s",p[i].prop,p[i].value);
					else if(p[i].data_type==TYPE_FLOAT_STR)
						sprintf((char*)json,"\"%s\":%s",p[i].prop,p[i].value);
					else if(p[i].data_type==TYPE_UINT32)
						sprintf((char*)json,"\"%s\":%ld",p[i].prop,(*(uint32_t*)p[i].value));
					else if(p[i].data_type==TYPE_INT32)
						sprintf((char*)json,"\"%s\":%ld",p[i].prop,(*(int32_t*)p[i].value));
					else
						sprintf((char*)json,"\"%s\":%d",p[i].prop,*((uint16_t*)p[i].value));
					
					//AtSetTopicId(NULL);
				}

				json+=strlen((char*)json);

				memcpy(json,"}}",2);

				//AtSetTopicId(NULL);

				return g_pub_json;
			}

		}

	}


	return g_pub_json;
}


uint8_t* GattMultiFieldMerge(void)
{
	uint8_t i,j,*json,m=0;
	uint8_t topic_ext[20];
	gatt_prop_Typedef *p=NULL;
	uint8_t count=0,meta=0,head=0;
	uint8_t tempjson[128];

	memset(g_pub_json,0x00,JSON_LEN);
	memset(topic_ext,0x00,20);

	//GattSetGprsConnectFailCnt(g_UserSet.report_fail_cnt);
	//GattSetGprsConnectTime(g_UserSet.onlinetime);
   	//GattSetGprsSleepTime(g_UserSet.sleeptime);

	json=g_pub_json;

	if(g_UserSet.raml_num==0)
		return g_pub_json;

	
	memcpy(json,"{",1);
	json+=strlen((char*)json);


	for(j=0;j<LIST_COUNT;j++)
	{
		head=0;
		p=GattGetListInfor(j,&count);

		if(p==NULL)
			continue;

		if(m>=g_UserSet.raml_num)
			break;

		for(i=0;i<count;i++)
		{
			if(strstr((char*)g_UserSet.raml[m],(char*)p[i].prop)!=NULL)
			{
					switch(j)
					{
						case LIST_ATT:
							if(head==0)
								memcpy(json,"\"att\":{",7);
							head=TRUE;
							break;
						case LIST_CMD:	
							if(head==0)
								memcpy(json,"\"cmd\":{",7);
							head=TRUE;
							break;
						case LIST_STS:	
							if(head==0)
								memcpy(json,"\"sts\":{",7);
							head=TRUE;
							break;
						case LIST_DTA:	
							if(head==0)
								memcpy(json,"\"dta\":{",7);
							head=TRUE;
							break;
						case LIST_DIA:
							if(head==0)
								memcpy(json,"\"dia\":{",7);
							head=TRUE;
							break;
						}

				
					
				json+=strlen((char*)json);
					
				if(p[i].data_type==TYPE_INT)
					sprintf((char*)json,"\"%s\":%d,",p[i].prop,*((int16_t*)p[i].value));
				else if(p[i].data_type==TYPE_STRS)
					sprintf((char*)json,"\"%s\":\"%s\",",p[i].prop,p[i].value);
				else if(p[i].data_type==TYPE_SUB_STRS)
					sprintf((char*)json,"\"%s\":%s,",p[i].prop,p[i].value);
				else if(p[i].data_type==TYPE_FLOAT_STR)
					sprintf((char*)json,"\"%s\":%s,",p[i].prop,p[i].value);
				else if(p[i].data_type==TYPE_UINT32)
					sprintf((char*)json,"\"%s\":%ld,",p[i].prop,(*(uint32_t*)p[i].value));
				else if(p[i].data_type==TYPE_INT32)
					sprintf((char*)json,"\"%s\":%ld,",p[i].prop,(*(int32_t*)p[i].value));
				else
					sprintf((char*)json,"\"%s\":%d,",p[i].prop,*((uint16_t*)p[i].value));


				m++;

				if(m>=g_UserSet.raml_num)
						break;	
					
				
			}


		}
		
		if(head)
		{	json+=strlen((char*)json)-1;
			memcpy(json,"},",2);
		    json+=strlen((char*)json);
			}

	}

	json+=strlen((char*)json)-1;
	memcpy(json,"}",1);

	//AtSetTopicId(NULL);

	return g_pub_json;
}


