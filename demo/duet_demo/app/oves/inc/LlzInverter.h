
#ifndef __LL_INVERTER_H__
#define __LL_INVERTER_H__


enum
{

	LLZ_QUERY_CELL,//B0
	LLZ_QUERY_SOC,//9B
	LLZ_QUERY_SN,
	LLZ_QUERY_VOLTAGE,//A1
	LLZ_QUERY_TEMP,// A2
	LLZ_QUERY_FAULT,// A3
	LLZ_QUERY_VERSION,// A4
	
	
	LLZ_CTRL_WDGON,//4
	LLZ_CTRL_WDGOFF,//5
	LLZ_CTRL_ALARM_ON,
	LLZ_CTRL_ALARM_OFF,
	LLZ_CTRL_HEARTBEAT,
	LLZ_CTRL_WDGFEED,//6
	LLZ_CTRL_PAYMODE,//9
	//LLZ_CTRL_COUNTER,
	//LLZ_CTRL_CONNECT,
	LLZ_CTRL_CTRL_IDUPDATE,
	LLZ_CMD_COUNT
};

enum
{

	LLZ_CTRL_COUNT
};


typedef struct
{
	uint8_t bus_addr;
	uint8_t fun;
	uint8_t cmd;
	uint8_t size;
	
	uint8_t data[61];
}LLZ_READ_TypeDef;

typedef struct
{
	uint8_t bus_addr;
	uint8_t fun;
	uint8_t cmd;
	uint8_t size;
	
	
	//uint8_t dataH;
	uint8_t data[61];

}LLZ_WRITE_TypeDef;



void LLZTimeoutTimerStart(void);
void LLZGetEnable(void);
void LLZInit(void);
void LLZGetInfo(uint8_t cmd);
void LLZTask(void);
void LLZMosCtrl(uint8_t on);
void LLZSend(uint8_t *buffer,uint16_t size);
void LLZWrite(uint8_t addr,uint8_t fun,uint8_t cmd,uint16_t size,uint8_t *Data);
void LLZRead(uint8_t addr,uint8_t fun,uint8_t cmd,uint8_t size,uint8_t *dataN);


#endif

