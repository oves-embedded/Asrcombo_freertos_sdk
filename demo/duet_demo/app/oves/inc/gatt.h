#ifndef __GATT_H__
#define __GATT_H__

#define PROP_LENGTH 16
#define META_LENGTH 63
#define TEXT_LENGTH 64

#define E_MOB48V_PROJECT


#ifdef P10KW_PROJECT
#define JSON_LEN  1024*4
#elif defined(CHARGE_STATION)
#define JSON_LEN  1024*6
#else
#define JSON_LEN  1024*4
#endif


#define GATT_RAM_SIZE 2048u
#define GATT_PROP_LIST_NUM 100
#define NEW_GATTVERSION

#define MEM_POS(name) MEM_ADDR_##name+MEM_SIZE_##name
//#define MEM_SIZE(name) MEM_SIZE_##name


//124
#define MEM_SIZE_OPID    20
#define MEM_SIZE_PPID    32
#define MEM_SIZE_FLID    64
#define MEM_SIZE_CCID    32
#define MEM_SIZE_FRMV    8


//CMD 154
#define MEM_SIZE_PUBK    32
#define MEM_SIZE_GSTW    2	
#define MEM_SIZE_GCTW    2
#define MEM_SIZE_NAPN    32	
#define MEM_SIZE_SWCH    2
#define MEM_SIZE_READ    32
#define MEM_SIZE_RPTM    2
#define MEM_SIZE_RAML    32
#define MEM_SIZE_RTMD 16
#define MEM_SIZE_HBFQ    2
#define MEM_SIZE_WIFI    128


#define MEM_SIZE_SFID        32
//#define MEM_SIZE_AOVS        2
//#define MEM_SIZE_AOFS        2
#define MEM_SIZE_CBTE        2
//#define MEM_SIZE_AOSS        2
#define MEM_SIZE_BCVC        2
#define MEM_SIZE_BCVF        2
//#define MEM_SIZE_MXCC        2
#define MEM_SIZE_SACC        2

#define MEM_SIZE_ACOS    2
#define MEM_SIZE_MPCS    2

#define MEM_SIZE_AOCT        2
#define MEM_SIZE_AOSS        2
#define MEM_SIZE_BCSS        2
#define MEM_SIZE_UOST        2
#define MEM_SIZE_UOET        2
#define MEM_SIZE_UCST        2
#define MEM_SIZE_UCET        2
#define MEM_SIZE_PIMD        2
#define MEM_SIZE_AIMD        2
#define MEM_SIZE_LNGS        2
#define MEM_SIZE_AOVS        2
#define MEM_SIZE_AOFS        2
#define MEM_SIZE_OLRS        2
#define MEM_SIZE_OTRS        2
#define MEM_SIZE_BZON        2
#define MEM_SIZE_CASS        2
#define MEM_SIZE_MXCC        2
#define MEM_SIZE_BBCV        2
#define MEM_SIZE_BFCV        2
#define MEM_SIZE_LBUS        2
#define MEM_SIZE_ACCS        2
#define MEM_SIZE_AGMD        2
#define MEM_SIZE_STYS        2
#define MEM_SIZE_STMS        2
#define MEM_SIZE_STDS        2
#define MEM_SIZE_STHS        2
#define MEM_SIZE_STMM        2
#define MEM_SIZE_STSS        2
//STS 98
#define MEM_SIZE_SSTC    2
#define MEM_SIZE_CRTM    16
#define MEM_SIZE_UDTM    16
#define MEM_SIZE_TIME    16
#define MEM_SIZE_PLAT    16
#define MEM_SIZE_PLON    16
#define MEM_SIZE_GPSS    2
#define MEM_SIZE_GPFC    2
#define MEM_SIZE_RCRD    2	
#define MEM_SIZE_TRHD    2	
#define MEM_SIZE_TPGD    2	
#define MEM_SIZE_PGST    2		
#define MEM_SIZE_TKRE    2
#define MEM_SIZE_OCST    2



////DTA
#define MEM_SIZE_BATP    2
#define MEM_SIZE_INPP    2
#define MEM_SIZE_OUTP    2
#define MEM_SIZE_AENG    2
#define MEM_SIZE_PCKV    4
#define MEM_SIZE_PCKC    2
#define MEM_SIZE_RSOC    2
#define MEM_SIZE_RCAP    2
#define MEM_SIZE_FCCP    2
#define MEM_SIZE_PCKT    2
#define MEM_SIZE_ACYC    2


////DTA 94
#define MEM_SIZE_BATP    2
#define MEM_SIZE_INPP    2
#define MEM_SIZE_OUTP    2
#define MEM_SIZE_AENG    2
#define MEM_SIZE_PCKV    4
#define MEM_SIZE_PCKC    2

#define MEM_SIZE_RSOC    2
#define MEM_SIZE_RCAP    2
#define MEM_SIZE_FCCP    2
#define MEM_SIZE_PCKT    2
#define MEM_SIZE_ACYC    2

#define MEM_SIZE_SLON    16
#define MEM_SIZE_SLAT    16
#define MEM_SIZE_SALT    2
#define MEM_SIZE_SSTM    24
#define MEM_SIZE_SGPS    2
#define MEM_SIZE_SSPE    2


#define MEM_SIZE_PPST    2
#define MEM_SIZE_PMCS    2
#define MEM_SIZE_CMOS    2
#define MEM_SIZE_DMOS    2
#define MEM_SIZE_CTMP    2
#define MEM_SIZE_MTPM    2
#define MEM_SIZE_MTRD    2
#define MEM_SIZE_TSPD    2
#define MEM_SIZE_RVLT    2
#define MEM_SIZE_RCUR    2

#define MEM_SIZE_RMAX    2
#define MEM_SIZE_CMXS    2
#define MEM_SIZE_CMXC    2
#define MEM_SIZE_TOMD 16
#define MEM_SIZE_CUMD 16

#define MEM_SIZE_PVIV        2
#define MEM_SIZE_PVIC        2
#define MEM_SIZE_PVIP        2
#define MEM_SIZE_PVIE        4
#define MEM_SIZE_ACIV        2
#define MEM_SIZE_ACOV        2
#define MEM_SIZE_ACFR        2
#define MEM_SIZE_ACCU        2
#define MEM_SIZE_ACIP        2
#define MEM_SIZE_ACOP        2
#define MEM_SIZE_ACIE        4
#define MEM_SIZE_ACOE        4
#define MEM_SIZE_IITE        2
#define MEM_SIZE_RSOH        2
#define MEM_SIZE_RDNT        2
#define MEM_SIZE_RCHT        2



//DIA
#define MEM_SIZE_CV01    2
#define MEM_SIZE_CV02    2
#define MEM_SIZE_CV03    2
#define MEM_SIZE_CV04    2
#define MEM_SIZE_CV05    2
#define MEM_SIZE_CV06    2
#define MEM_SIZE_CV07    2
#define MEM_SIZE_CV08    2
#define MEM_SIZE_CV09    2
#define MEM_SIZE_CV10    2
#define MEM_SIZE_CV11    2
#define MEM_SIZE_CV12    2
#define MEM_SIZE_CV13    2
#define MEM_SIZE_CV14    2
#define MEM_SIZE_CV15    2
#define MEM_SIZE_CV16    2

#define MEM_SIZE_CV17    2
#define MEM_SIZE_CV18    2
#define MEM_SIZE_CV19    2
#define MEM_SIZE_CV20    2
#define MEM_SIZE_CV21    2
#define MEM_SIZE_CV22    2
#define MEM_SIZE_CV23    2
#define MEM_SIZE_TEMP1    2
#define MEM_SIZE_TEMP2    2
#define MEM_SIZE_TEMP3    2
#define MEM_SIZE_TEMP4    2
#define MEM_SIZE_TEMP5    2
#define MEM_SIZE_TEMP6    2




enum
{
	TYPE_UINT16,
	TYPE_STRS,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_FLOAT_STR,
	TYPE_UINT32,
	TYPE_INT32,
	TYPE_SUB_STRS,
};

typedef struct Prop
{
	char *prop;
	uint8_t* value;
	char *meta;
	uint8_t data_type;
	uint8_t len;
	uint8_t list_name;
	uint8_t meta_ctrl;	
}gatt_prop_Typedef;


typedef struct 
{
	char *prop;//[PROP_LENGTH];
	uint16_t type;//  0 :uint   1:str  2:int
	uint16_t len;//  0 :uint   1:str  2:int
	char *meta;//[META_LENGTH];
}gatt_attri_ovitem_TypeDef;

typedef struct 
{
	uint16_t pos;
	uint16_t reserved;
	uint8_t *ram;
}gatt_data_ram_TypeDef;




enum
{
	LIST_ATT,
	LIST_CMD,
	LIST_STS,
	LIST_DTA,
	LIST_DIA,
	LIST_COUNT
};





void ov_gatt_init(void);
uint8_t gatt_set_prop_data(char* prop,uint8_t*data);
uint8_t gatt_get_prop_data(char* prop,uint8_t*data);
gatt_prop_Typedef* gatt_get_prop_list(void);
uint8_t* GattAllFieldJsonMerge(void);
uint8_t* GattDtTypeFieldJsonMerge(uint8_t dt_type,uint8_t meta);
uint8_t* GattSingleFieldMerge(uint8_t *tag_str);

uint8_t*  AtGetSubscribeId(void);
uint8_t* AtGetPubTopicId(uint8_t * ext_topic);

#endif


