#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__


#define KEYB_VERSION  33

#define UART5_RX_BUF_SIZE  512


#ifdef IAP_SUPPORT

typedef struct
{
	uint8_t header;
	uint8_t ctrl;  // PANEL:0X00  BASE:0X01
	uint16_t lenght;
	uint8_t cmd;  //CTRL 01,CTRL ACK 02  ,QUERY 03,QUERY ACK 04
	uint8_t sub;
	uint16_t opt;
	uint16_t chksum;
}IAP_PACKET_Def;


void IapAckUart1(uint8_t sub ,uint16_t data);
void IapDetect(uint8_t* buffer,uint16_t* size);

#endif

#define UART3_RX_BUF_SIZE 1024

typedef struct  
{
  unsigned char Temperature[2];
  unsigned char Voltage[2];
  unsigned char Current[2];
  unsigned char Relative_State_of_Charge[2];
  unsigned char Remaning_Capacity[2];
  unsigned char Full_charge_Capacity[2];
  unsigned char Cycle_Count[2];
  unsigned char State_of_Health[2];
  unsigned char CV1[2];
  unsigned char CV2[2];
  unsigned char CV3[2];
  unsigned char CV4[2];
  unsigned char Run_Time_To_Empty[2];
  unsigned char Run_Time_To_Full[2];
  unsigned char Average_Power[2];
   unsigned char Charge_Power[2];
  unsigned char Solar_Power[4];
  unsigned char Acc_Power[4];
  unsigned char OperationStatus[4];
  unsigned char Error;
  unsigned char OutputState;
} BQ40Z50_TypeDef;


void KeyBoardInit(void);
void PKeybordProc(void) ;
void Serial_Cmd(char cmd);
void Process_cmd_all(char cmd);
void Printf_Usart_num(unsigned char *str, int num);
void debug_printf(uint8_t*tstr,uint8_t*str,uint16_t value);
void Send_RechargeOK(void);
uint16_t hi_crc16(uint8_t *buffer, uint16_t length);
uint8_t Programholdstate(void);


#endif

