
#ifndef __EEPROM_H__
#define __EEPROM_H__

#define RAML_SIZE   20
#define MEM_SIZE_FLID 64
#define I2CX_SLAVE_ADDRESS7 0xa0


#ifdef ABACUSLEDER_SUPPORT
#define MAX_ABACUS_NUMBER  8

#define ABACUSLEDER_START_ADDR  0x400
#define ABACUSLEDED_SIZE   3*1024u	
#endif


typedef struct
{

	PAYG_TypeDef  Payg;  //104
	NET_INFOR_TypeDef NetInfor;  //152
	//CAMP_FREQ_TypeDef CampFreq;//16
	//NET_INFOR_TypeDef NetInforFactory;//152
	uint8_t reportt_auto;// 0,manu,1 auto
	uint8_t ble_state;
	uint8_t lowbat;
	uint8_t demo_Cnt;

	double tomd; 

	uint32_t sleeptime;   //min
	uint32_t onlinetime;   //min
	uint32_t report_fail_cnt;
	uint32_t heartbeat;
	uint32_t wakeup_cnt;
	
	uint32_t accuCap;
	uint32_t accuPower;
	uint32_t accuSolarGen;
	uint32_t accuRuntime;

	uint32_t ramt;
	uint32_t raml_num;
	uint8_t raml[RAML_SIZE][6];

	

	
#ifdef ABACUSLEDER_SUPPORT
	//uint32_t abacus;
	uint32_t abacus_num;
	uint8_t abacuslist[MAX_ABACUS_NUMBER][6];
#endif

	uint8_t jtag:4;
	uint8_t log:4;
	uint8_t ppid_cfg;
	uint8_t reserved[2];

	uint8_t DevId[32];


	uint8_t fleed[MEM_SIZE_FLID];
	char wifi_ssid[64];
	char wifi_password[64];

	#ifdef OPEN_PAYGO
	uint16_t UsedTokens;
	uint16_t TokenCount ;//(& UsedTokens if needed)
	
    uint8_t PAYGEnabled;
	uint8_t reserved1[3];
	
    uint32_t ActiveUntil;
    uint32_t TokenEntryLockedUntil;
	#endif
	
	uint32_t endmark;  // 4
	
}USER_SET_TypeDef;

typedef struct
{
	uint32_t sec[60]; 
	uint32_t min[60]; 
	uint32_t hour[24]; 
	uint32_t day[365];
	uint32_t year[10];
}ABACUS_IMAGE_TypeDef;

typedef struct
{
	uint32_t sec_counter; 
	uint32_t min_counter; 
	uint32_t hour_counter; 
	uint16_t day_counter;
	uint16_t year_counter;
}ABACUS_COUNTER_TypeDef;


#define EEPROM_START_ADDR  0x801f800//FLASH_EEPROM_BASE
#define eeprom_addr(a) ((uint32_t)(a+EEPROM_START_ADDR))

#define FLASH_START_ADDR   (uint32_t)0x8000000

#define PAGE_SIZE (uint32_t)2048
#define FMC_PAGE_SIZE (uint32_t)2048

#define EEP_PAGE_SIZE 64

#define ApplicationAddress    (uint32_t)0x8002000
#define UpgradeflagAddress FLASH_START_ADDR+0xfcc0+3*64*1024
#define BootModeflag           0x1a1a1a1a

#define UpgradeOtaflagAddress FLASH_START_ADDR+0xfcc0+3*64*1024+4
#define BootOtaModeflag           0x012345

#ifdef GD32F10X_MD
#define ApplicationAddress    (uint32_t)0x8002000
#define UpgradeflagAddress FLASH_START_ADDR+0xfcc0
#define BootModeflag           0x1a1a1a1a

#define UpgradeOtaflagAddress FLASH_START_ADDR+0xfcc0  //not use
#define BootOtaModeflag           0x012345

#define EEP_START_ADDR  (uint32_t)1024* (uint32_t)(61)
#else
#define ApplicationAddress    (uint32_t)0x8002000
#define UpgradeflagAddress FLASH_START_ADDR+0xfcc0+3*64*1024
#define BootModeflag           0x1a1a1a1a

#define UpgradeOtaflagAddress FLASH_START_ADDR+0xfcc0+3*64*1024+4
#define BootOtaModeflag           0x012345


#define EEP_START_ADDR  (uint32_t)1024* (uint32_t)(63)
#endif
void EEpInit(void);
void EEpWritePage(uint32_t PageAddress,uint32_t size,uint8_t* buffer);
void EEpReadPage(uint32_t PageAddress,uint32_t size,uint8_t* buffer);

void EEpWrite(uint32_t Address,uint16_t len,uint8_t  *buf);
void EEpRead(uint32_t Address,uint16_t len,uint8_t  *buf);
void FlashPageErase(uint32_t PageAddress);
 uint8_t FlashPageProgram(uint32_t PageAddress,uint32_t size,uint32_t* buffer);
 void FlashRead(uint32_t PageAddress,uint32_t size,uint32_t* buffer);

//void EEpUpdatePayg(PAYG_TypeDef* payg);
uint32_t EEpGetAccuCap(void);
uint32_t EEpGetAccuPower(void);
uint32_t EEpGetAccuSolarGen(void);
uint32_t EEpGetAccuRuntime(void);

void EEpSetAccuCap(uint32_t cap);
void EEpSetAccuPower(uint32_t power);
void EEpSetAccuSolarGen(uint32_t solar);
void EEpSetAccuRuntime(uint32_t solar);

void EEpUpdateEnable(void);
void EEpProcess(void);
uint32_t EEpGetTransFreq(void);
uint8_t EEpGetAttState(void);
uint8_t EEpGetStsState(void);
uint8_t EEpGetStaState(void);
uint32_t EEpGetOnlineTime(void);
uint32_t EEpGetSleepTime(void);
uint8_t EEpGetAutoReportState(void);
uint8_t EEpGetLowBatState(void);
void EEpSetLowBatFlag(uint8_t flag);
void EEpSetHeartbeat(uint32_t heartbeat);
void EEpSetWakeupCnt(uint32_t wkcnt);
uint32_t EEpGetHeartbeat(void );
uint32_t EEpGetWakeupCnt(void);

uint32_t EEpGetDemoCnt(void );
void EEpSetDemoCnt(uint8_t cnt);
uint32_t EEpGetRamLRptTime(void);
uint32_t EEpGetRamLRptNum(void);
uint8_t EEpGetJtagState(void);
void EEpSetJtag(uint8_t jtag);
void EEpSetTomd(double tomd);
double EEpGetTomd(void);

uint8_t* EEpGetPpid(void);
uint8_t* EEpGetOpid(void);
void EEpSetDevId(uint8_t *devid);
uint8_t* EEpGetDevId(void);
uint8_t EEpGetPpidCfgState(void);
void EEpSetPpidCfgState(uint8_t state);



#ifdef ABACUSLEDER_SUPPORT
void RamShift(uint32_t *buf,uint16_t size);
void AbacusLederInit(void);
void AbacusLederProc(void);
#endif
#endif

