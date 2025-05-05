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
#include "duet_gpio.h"
#include "duet_gpio.h"

#include "ovapp.h"


#ifdef DI_JBD_PROJECT
#include "jbdbms.h"
#endif /* DI_JBD_PROJECT */
#ifdef SUPERPOWER_PROJECT
#include "superpower.h"
#endif /* SUPERPOWER_PROJECT */
#ifdef DI_INVERTER48V_PROJECT
#include "inverter48v.h"
#endif /* DI_INVERTER48V_PROJECT */
#ifdef DI_PEICHENGBMS_PROJECT
#include "peichengbms.h"
#endif /* DI_PEICHENGBMS_PROJECT */
#ifdef DI_HEROEE2BMS_PROJECT
#include "heroee2bms.h"
#endif /* DI_HEROEE2BMS_PROJECT */
#ifdef DI_LLZINVERTER_PROJECT
#include "llzinverter.h"
#endif /* DI_LLZINVERTER_PROJECT */
#ifdef DI_FREEZER_PROJECT
#include "freezer.h"
#endif /* DI_FREEZER_PROJECT */
#ifdef DI_FAN_PROJECT
#include "fan.h"
#endif /* DI_FAN_PROJECT */


#ifdef DI_EMCOOKE_PROJECT
#include "emcooke.h"
#endif /* DI_EMCOOKE_PROJECT */


#if defined(DI_JBD_PROJECT) || defined(SUPERPOWER_PROJECT)\
	|| defined(DI_INVERTER48V_PROJECT)|| defined(DI_PEICHENGBMS_PROJECT)\
	|| defined(DI_HEROEE2BMS_PROJECT)|| defined(DI_EMCOOKE_PROJECT)|| defined(DI_LLZINVERTER_PROJECT)\
	|| defined(DI_FREEZER_PROJECT)|| defined(DI_FAN_PROJECT)
#include "payg.h"

#include "keyboard.h"
#include "eeprom.h"
#include "ds1302.h"


char cmd_allhead[3] = {0xc5,0x6a,0x29};
char cmd_ppidhead[6]={0xc5,0x6a,0x29,0x1b,0x08,0x14};
char cmd_ppidhead_new[6]={0xc5,0x6a,0x29,0x15,0x08,0x0E};
char cmd_opidhead[6]={0xc5,0x6a,0x29,0x15,0x01,0x0e};
char cmd_roothead[5]={0xc5,0x6a,0x29,0x0e,0x12};
char cmd_hashtophead[5]={0xc5,0x6a,0x29,0x0e,0x11};
char cmd_handcmd[6]={0xc5,0x6a,0x29,0x06,0x04,0xaa};
char cmd_handAckcmd[7]={0xc5,0x6a,0x29,0x07,0x04,0x01,0x24};


unsigned char ppid_ok[7]   ={0xc5,0x6a,0x29,0x07,0xf1,0x0a,0x62};
unsigned char opid_ok[7]   ={0xc5,0x6a,0x29,0x07,0xef,0x0a,0x52};
unsigned char hashtop_ok[7]={0xc5,0x6a,0x29,0x07,0xfb,0x0a,0x85};
unsigned char root_ok[7]   ={0xc5,0x6a,0x29,0x07,0xf2,0x0a,0x37};
//unsigned char keyUesed[7]  ={0xC5,0x6A,0x29,0x07,0xFA,0x0B,0x1F};//密码已使用
//unsigned char errkey[7]    ={0xC5,0x6A,0x29,0x07,0xFA,0xA0,0x90};//错误密码
unsigned char keyOk[7]     ={0xC5,0x6A,0x29,0x07,0xFA,0x0A,0x41};

unsigned char Answer_RPD[5]={0xC5,0x6A,0x29,0x08,0x05};//Ramaining_Days
unsigned char Answer_DHB[5]={0xC5,0x6A,0x29,0x08,0x06};//Days_hve_been_run
unsigned char Answer_PD [5]={0xC5,0x6A,0x29,0x08,0x07};//Payg_Days
unsigned char Answer_PS [5]={0xC5,0x6A,0x29,0x08,0x09};//Payg_State
unsigned char Answer_OCS[5]={0xC5,0x6A,0x29,0x08,0x0A};//Output_Control_State
unsigned char Answer_RDB[5]={0xC5,0x6A,0x29,0x08,0x13};//Run Days Backup

//test Answer
//unsigned char Answer_Ramaining_PAYG_Days[8] =  {0xC5,0x6A,0x29,0x08,0x05,0x01,0x00,0x73};
////unsigned char Answer_Days_have_been_run[8]  =  {0xC5,0x6A,0x29,0x08,0x06,0x00,0x00,0x53};
//unsigned char Answer_PAYG_Days[8]           =  {0xC5,0x6A,0x29,0x08,0x07,0x01,0x00,0x3C};
//unsigned char Answer_PAYG_State[8]          =  {0xC5,0x6A,0x29,0x08,0x09,0x00,0x00,0x0C};
//unsigned char Answer_Output_Control_State[8]=  {0xC5,0x6A,0x29,0x08,0x0A,0x01,0x00,0x2C};
unsigned char Answer_System_Status_Code[8]  =  {0xC5,0x6A,0x29,0x08,0x0B,0x00,0x00,0x43};
unsigned char Answer_System_Status_CodeE[8] =  {0xC5,0x6A,0x29,0x08,0x0B,0x04,0x00,0x78};
unsigned char Answer_System_Status_CodeE2[8] =  {0xC5,0x6A,0x29,0x08,0x0B,0x02,0x00,0xD2};
//unsigned char Answer_Run_Days_Backup[8]     =  {0xC5,0x6A,0x29,0x08,0x13,0xCE,0x04,0x25};
unsigned char Answer_RREV[8]                =  {0xC5,0x6A,0x29,0x08,0x14,0x0A,0x00,0xB1};


unsigned char Answer_Upgrade[]                =  {0x53,0x01,0x04,0x00,0x02,0xf7,0x01,0x00,0x9b,0xb8};

extern  SYS_STATE_CODE_TypeDef  g_sysStateCode;

extern uint8_t g_tokenState;
//============================
unsigned char cmd_sendbuf[64];

char Uart_Buffer[64];
char Camp_cmd[64];
char gatt_buf[64];


char Camp_ComOK = 0;
//char SaveFlash_Flag  = 0;
uint32_t temphash;//交换HashTop
uint8_t g_keyboardPwrHoldTmrLock=FALSE;
uint8_t g_Programholdon=FALSE;

uint8_t * g_Uart5Buf;
uint16_t RxUart5Counter;
uint8_t g_ProgramholdCounter=0;


extern PAYG_TypeDef payg;
extern uint8_t Sys_Code;
#ifdef IAP_SUPPORT
//extern UART_HandleTypeDef huart1;
#endif

// BQ40Z50_TypeDef g_bq40z50_state;

 //uint8_t g_bleComEnable;

 uint16_t g_bleRptPause;

extern USER_SET_TypeDef g_UserSet;
extern uint8_t uart2_buf[UART2_RX_SIZE];
extern uint16_t uart2_index;

unsigned char crc_array[256] =
{
   0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
   0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41, 
   0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
   0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 
   0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
   0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 
   0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
   0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
   0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 
   0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
   0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
   0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
   0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 
   0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 
   0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
   0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9, 
   0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 
   0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
   0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
   0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, 
   0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 
   0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 
   0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 
   0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 
   0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
   0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
   0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
   0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, 
   0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 
   0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8, 
   0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
   0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

unsigned char CRC8(unsigned char *p, char counter)
{
    unsigned char crc8 = 0;

    for( ; counter > 0; counter--)
    {
          crc8 = crc_array[crc8^*p]; //????CRC?
           p++;
    }
     return crc8;

 }


void debug_printf(uint8_t*tstr,uint8_t*str,uint16_t value)
{
#ifdef DUBUG_LOG
	uint8_t printstr[128];
	memset(printstr,0x00,128);
	sprintf(printstr,"%s: %s%d \r\n",tstr,str,value);
	
	//HAL_UART_Transmit(&huart3,printstr,strlen(printstr),1000);
	Uart5Send(printstr,strlen(printstr));
#endif	
}
uint8_t Programholdstate(void)
{
	return g_Programholdon;
}
void KeyBoardInit(void)
{
	//  HAL_UART_Receive_IT(&huart1,g_Uart1Buf,UART3_RX_BUF_SIZE);
	  //HAL_UART_Receive_IT(&huart3,(uint8_t*)g_Uart3Buf,UART3_RX_BUF_SIZE);
    g_Uart5Buf=uart2_buf;
	//huart5.pRxBuffPtr=(uint8_t*)g_Uart5Buf;
	//huart5.RxXferCount=0;
	///huart5.RxXferSize=UART5_RX_BUF_SIZE;
	  
}

void Printf_Usart_num(unsigned char *str, int num)
{
	 //int8_t i;
      //HAL_UART_Transmit_IT(&huart3,str,num);
	  #ifdef SUPERPOWER_PROJECT
	  superPoweruartSend(str,num);
		#endif
        #ifdef DI_JBD_PROJECT
	  Jbd485Send(str,num);
		#endif
		#ifdef DI_INVERTER48V_PROJECT
		Inverter48VSend(str,num);
		#endif
		#ifdef DI_PEICHENGBMS_PROJECT
		PeichengBmsSend(str,num);
		#endif
		#ifdef DI_EMCOOKE_PROJECT
		EmCookeSend(str,num);
		#endif
		#ifdef DI_HEROEE2BMS_PROJECT
		HeroEE2Send(str,num);
		#endif
		#ifdef DI_LLZINVERTER_PROJECT
		LLZSend(str,num);
		#endif
		#ifdef DI_FAN_PROJECT
		FanSend(str,num);
		#endif
		#ifdef DI_FREEZER_PROJECT
		FreezerSend(str,num);
		#endif
}

void KeyBoardHand(void)
{
	// HAL_UART_Transmit_IT(&huart3,(uint8_t *)cmd_handcmd,6);
	#ifdef SUPERPOWER_PROJECT
	superPoweruartSend((uint8_t*)cmd_handcmd,6);
	#endif
	#ifdef DI_JBD_PROJECT
	Jbd485Send((uint8_t*)cmd_handcmd,6);
	#endif
	#ifdef DI_INVERTER48V_PROJECT
	Inverter48VSend((uint8_t*)cmd_handcmd,6);
	#endif
	#ifdef DI_PEICHENGBMS_PROJECT
	PeichengBmsSend((uint8_t*)cmd_handcmd,6);
	#endif
	#ifdef DI_EMCOOKE_PROJECT
	EmCookeSend((uint8_t*)cmd_handcmd,6);
	#endif
	#ifdef DI_HEROEE2BMS_PROJECT
	HeroEE2Send((uint8_t*)cmd_handcmd,6);
	#endif
	#ifdef DI_LLZINVERTER_PROJECT
	LLZSend((uint8_t*)cmd_handcmd,6);
	#endif
	#ifdef DI_FAN_PROJECT
	FanSend((uint8_t*)cmd_handcmd,6);
	#endif
	#ifdef DI_FREEZER_PROJECT
	FreezerSend((uint8_t*)cmd_handcmd,6);
	#endif
}


void PKeybordProc(void)  //
{
	uint16_t i,len=0;//,value_len=0;
	#if 1 //def UI1K_V13_PROJECT
	//uint8_t *p=NULL;
	#endif
 
	
	if(g_ProgramholdCounter)
	{
		g_ProgramholdCounter--;

		if(g_ProgramholdCounter==0)
			g_Programholdon=FALSE;
		}

	//if(huart3.RxState==HAL_UART_STATE_READY)
	//	KeyBoardInit();


	RxUart5Counter=uart2_index;

    for(i=0;i<RxUart5Counter;i++)
		log_i("%0X",g_Uart5Buf[i]);

	log_i("\r\n");


	for(i=0;i<RxUart5Counter;i++)
	{
		
		if((g_Uart5Buf[i]==0xc5&&g_Uart5Buf[i+1]==0x6a)&&g_Uart5Buf[i+2]==0x29)
		{

			len=g_Uart5Buf[i+3];

			if(RxUart5Counter-i>=len&&RxUart5Counter-i>=4)
			{

			  if(g_Uart5Buf[i+len - 1] == CRC8((unsigned char *)&g_Uart5Buf[i], len - 1))
			  {
			//          Camp_ComOK = 1;
			   g_Programholdon=TRUE;
			   g_ProgramholdCounter=5;
	           memcpy(Camp_cmd, (uint8_t*)&g_Uart5Buf[i], len);
	           memcpy(Uart_Buffer, (uint8_t*)&g_Uart5Buf[i], len);
	           Serial_Cmd(g_Uart5Buf[i+4]);
      	  #ifdef SUPERPOWER_PROJECT
			   superPower_timeout_timer_start();
            #endif
			#ifdef DI_INVERTER48V_PROJECT
			   Inverter48VTimeoutTimerStart();
            #endif
			#ifdef DI_PEICHENGBMS_PROJECT
			   PeichengBmsTimeoutTimerStart();
            #endif
			#ifdef DI_EMCOOKE_PROJECT
			   EmCookeTimeoutTimerStart();
            #endif
			#ifdef DI_HEROEE2BMS_PROJECT
			   HeroEE2TimeoutTimerStart();
            #endif
			#ifdef DI_LLZINVERTER_PROJECT
			 //  LLZTimeoutTimerStart();
            #endif
			#ifdef DI_FAN_PROJECT
			   FanTimeoutTimerStart();
            #endif
			#ifdef DI_FREEZER_PROJECT
			   FreezerTimeoutTimerStart();
            #endif
			   Process_cmd_all(Camp_ComOK);
			   }

			  //g_bleComEnable=FALSE;

			  //g_bleRptPause=2000u;// 5 min 30000u/150u;

			 // BUP_disconnect_handler();
			 memset(uart2_buf,0x00,UART2_RX_SIZE);
			 uart2_index=0;

			}

			 
			}


		
		}

	/*p=(uint8_t*)strstr((char*)g_Uart5Buf,"AT+ALLGATT?");	//"ip ,port,username,password"
	if(p!=NULL)
	{
		char tempstr[512]={0},*pgatt;
		pgatt=tempstr;
		BUP_disconnect_handler();
		//memset(Uart_Buffer,0x00,sizeof(Uart_Buffer));
		sprintf(pgatt,"{att {\"opid\":\"%s\",\0",payg.oem_id);
		
		//hal_uart_send_buff(UART1,(uint8_t*)Uart_Buffer,strlen(Uart_Buffer)); 
		pgatt+=strlen(pgatt);
		
		//memset(Uart_Buffer,0x00,sizeof(Uart_Buffer)); 
		sprintf(pgatt,"\"ppid\":\"%s\",\0",payg.payg_id);
		//hal_uart_send_buff(UART1,(uint8_t*)Uart_Buffer,strlen(Uart_Buffer)); 
		pgatt+=strlen(pgatt);
		//memset(Uart_Buffer,0x00,sizeof(Uart_Buffer));
		memset(gatt_buf,0x00,sizeof(gatt_buf));
		GattGetData(LIST_ATT,ATT_FRMV,gatt_buf);
		sprintf(pgatt,"\"frmv\":\"%s\",\0",gatt_buf);
		//hal_uart_send_buff(UART1,(uint8_t*)Uart_Buffer,sizeof(Uart_Buffer)); 

		pgatt+=strlen(pgatt);
		//memset(Uart_Buffer,0x00,sizeof(Uart_Buffer));
		memset(gatt_buf,0x00,sizeof(gatt_buf));
		GattGetData(LIST_CMD,CMD_PUBK,gatt_buf);
		
		sprintf(pgatt,"\"pubk\":\"%s\",\0",gatt_buf);
		//hal_uart_send_buff(UART1,(uint8_t*)Uart_Buffer,strlen(Uart_Buffer)); 

		pgatt+=strlen(pgatt);
		//memset(Uart_Buffer,0x00,sizeof(Uart_Buffer));
		memset(gatt_buf,0x00,sizeof(gatt_buf));
		
		sprintf(pgatt,"\"rcrd\":\"%d\",\0",payg.remaing_days);
		//hal_uart_send_buff(UART1,(uint8_t*)Uart_Buffer,strlen(Uart_Buffer)); 

		pgatt+=strlen(pgatt);
		//memset(Uart_Buffer,0x00,sizeof(Uart_Buffer));
		memset(gatt_buf,0x00,sizeof(gatt_buf));
		GattGetData(LIST_STS,STS_OCST,gatt_buf);
		sprintf(pgatt,"\"ocst\":\"%d\",\0",gatt_buf[0]);
		//hal_uart_send_buff(UART1,(uint8_t*)Uart_Buffer,strlen(Uart_Buffer)); 

		//memset(Uart_Buffer,0x00,sizeof(Uart_Buffer));
		pgatt+=strlen(pgatt);
		sprintf(pgatt,"\"tpgd\":\"%d\",",payg.recharge_days);
		//hal_uart_send_buff(UART1,(uint8_t*)Uart_Buffer,strlen(Uart_Buffer)); 

		pgatt+=strlen(pgatt);
		//memset(Uart_Buffer,0x00,sizeof(Uart_Buffer));
		memset(gatt_buf,0x00,sizeof(gatt_buf));
		GattGetData(LIST_STS,STS_PGST,gatt_buf);
		sprintf(pgatt,"\"pgst\":\"%d\"}}\0",gatt_buf[0]);
		hal_uart_send_buff(UART1,(uint8_t*)tempstr,strlen(tempstr));
		LOG("%s \r\n",tempstr);
		
		g_Programholdon=TRUE;
		g_ProgramholdCounter=5;

		//BUP_disconnect_handler();
		}*/

	
}


void Serial_Cmd(char cmd)
{
  switch (cmd)
  {
  case 0x00://单体电压 温度
    {Camp_ComOK = 0xff;};break;  
  case 0x01:
    {Camp_ComOK = 0x01;};break;  
  case 0x05:
    {Camp_ComOK = 0x05;};break;  
  case 0x06:
    {Camp_ComOK = 0x06;};break; 
  case 0x07:
    {Camp_ComOK = 0x07;};break;     
  case 0x08:
    {Camp_ComOK = 0x08;};break;
  case 0x09:
    {Camp_ComOK = 0x09;};break;   
  case 0x0a:
    {Camp_ComOK = 0x0a;};break;
  case 0x0b:
    {Camp_ComOK = 0x0b;};break;
  case 0x0c:
    {Camp_ComOK = 0x0c;};break; 
  case 0x0d:
    {Camp_ComOK = 0x0d;};break; 
  case 0x0E:
    {Camp_ComOK = 0x0E;};break;
  case 0x0F:
    {Camp_ComOK = 0x0F;};break; 
  case 0x10:
    {Camp_ComOK = 0x10;};break;
  case 0x11:
    {Camp_ComOK = 0x11;};break;
  case 0x12:
    {Camp_ComOK = 0x12;};break;
  case 0x13:
    {Camp_ComOK = 0x13;};break;     
  case 0x14:
    {Camp_ComOK = 0x14;};break;    
  case 0x17:
    {Camp_ComOK = 0x17;};break;  
  case 0x18:
    {Camp_ComOK = 0x18;};break;
  case 0x7F:
    {Camp_ComOK = 0x7F;};break;
  case 0xef:
    {Camp_ComOK = 0xef;};break;
  case 0xf1:
    {Camp_ComOK = 0xf1;};break; 
  case 0xf2:
    {Camp_ComOK = 0xf2;};break;
  case 0xf5:
    {Camp_ComOK = 0xf5;};break;  
  case 0xfa:
    {Camp_ComOK = 0xfa;};break;    
  case 0xfb:
    {Camp_ComOK = 0xfb;};break;    
  default:(Camp_ComOK = 0);break;
  }
}


void Send_RechargeOK(void)
{
   Printf_Usart_num(keyOk , 7);	
}

void Process_cmd(char cmd)
{

	switch (cmd)
	{
	case 0x01://Read_OEMID
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_opidhead, 6);
		memcpy(cmd_sendbuf + 6, payg.oem_id, 14);
		cmd_sendbuf[20] = CRC8(cmd_sendbuf, 20);
		Printf_Usart_num(cmd_sendbuf, 21);
	};break;
	//case 0x04:
	//	TimerKbInsertSet(5*1000);
	//	break;
	case 0x05://Read_Remaining_PAYG_Days
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_RPD, 5);
		cmd_sendbuf[5] = payg.remaing_days & 0xFF;
		cmd_sendbuf[6] = (payg.remaing_days >> 8) & 0xFF;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x06://Read_Days have been run
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_DHB, 5);
		cmd_sendbuf[5] = payg.days_has_been_runing & 0xFF;
		cmd_sendbuf[6] = (payg.days_has_been_runing >> 8) & 0xFF;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break; 
	case 0x07://Read_PAYG_Days 
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_PD, 5);
		cmd_sendbuf[5] = payg.recharge_days & 0xFF;
		cmd_sendbuf[6] = (payg.recharge_days >> 8) & 0xFF;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;     
	case 0x08://Read_PPID
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_ppidhead, 6);
		memcpy(cmd_sendbuf + 6, payg.payg_id, 20);
		cmd_sendbuf[26] = CRC8(cmd_sendbuf, 26);
		Printf_Usart_num(cmd_sendbuf, 27);

		
	};break;
	case 0x09://Read_PAYG_State
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_PS, 5);
		if(payg.free == YES)
			cmd_sendbuf[5] = 0x01;
		else        
			cmd_sendbuf[5] = 0x00;
		cmd_sendbuf[6] = 0x00;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x0a://Read_Output_Control_State
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_OCS, 5);
		if((payg.free == YES) || (payg.remaing_days > 0))
			cmd_sendbuf[5] = 0x01;
		else                
			cmd_sendbuf[5] = 0x00;

		cmd_sendbuf[6] = 0x00;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break; 
	case 0x0b://Read_System_Status_Code
	{
		if(/*Sys_Code == 0xE1*/  g_sysStateCode.error.rtc_error)//DS1302掉电
		{
			Printf_Usart_num(Answer_System_Status_CodeE, 8);
		}
		else if(Sys_Code == 0xE2)//晶体不工作
		{
			Printf_Usart_num(Answer_System_Status_CodeE2, 8);
		}
		else
			Printf_Usart_num(Answer_System_Status_Code, 8);
	};break;      
	case 0x11://Read_HashTop
	{
		memset(cmd_sendbuf, 0, 32);      
		memcpy(cmd_sendbuf, cmd_hashtophead, 5);

		memcpy(cmd_sendbuf + 5, payg.hast_otp + 1, 4);//这里指针+1 根据指针类型 uint32相当于4字节 不是1字节
		memcpy(cmd_sendbuf + 9, payg.hast_otp, 4);
		cmd_sendbuf[13] = CRC8(cmd_sendbuf, 13);
		Printf_Usart_num(cmd_sendbuf, 14);
	};break;
	case 0x12://Read_root
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_roothead, 5);
		memcpy(cmd_sendbuf + 5, payg.hast_root, 8);
		cmd_sendbuf[13] = CRC8(cmd_sendbuf, 13);
		Printf_Usart_num(cmd_sendbuf, 14);
	};break;
	case 0x13://Read_Run_Days_Backup
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_RDB, 5);
		cmd_sendbuf[5] = payg.run_days_backup & 0xFF;
		cmd_sendbuf[6] = (payg.run_days_backup >> 8) & 0xFF;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;    
	case 0x14://Read_REV
	{
		

		Answer_RREV[5]=KEYB_VERSION;

		Answer_RREV[7] = CRC8(Answer_RREV, 7);

		Printf_Usart_num(Answer_RREV, 8);

	};break;
	case 0xe1://RTC_Err
	{
		//Printf_Usart("Rtc_Err!!!!!!!!!!!!\r\n");
	};break;
	case 0xef://Write_OEMID
	{
		memset(payg.oem_id,0x00, 20);
		memcpy(payg.oem_id, Camp_cmd + 6, 14);
		//SaveFlash_Flag = 1;
		EEpUpdateEnable();
		Printf_Usart_num(opid_ok, 7);
	};break;
	case 0xf1://Write_PPID
	{
		memset(payg.payg_id,0x00, 20);
		memcpy(payg.payg_id, Camp_cmd + 6, 20); 
		//SaveFlash_Flag = 1;//保存到FLASH
		EEpUpdateEnable();
		Printf_Usart_num(ppid_ok, 7);      
	};break;
	case 0xf2://Write_HashRoot
	{   
		Printf_Usart_num(root_ok, 7);
	};break; 
	case 0xfa://充值
	{
		memset(cmd_sendbuf, 0, 32);
		//倒序 按照正确命令排序
		cmd_sendbuf[3] = Camp_cmd[12];
		cmd_sendbuf[2] = Camp_cmd[11];
		cmd_sendbuf[1] = Camp_cmd[10];
		cmd_sendbuf[0] = Camp_cmd[9];
		cmd_sendbuf[7] = Camp_cmd[8];
		cmd_sendbuf[6] = Camp_cmd[7];
		cmd_sendbuf[5] = Camp_cmd[6];
		cmd_sendbuf[4] = Camp_cmd[5];

		memcpy(payg.hast_input, cmd_sendbuf, 8);
		//      Oves_SaveConfig(Pay_Info.PPID, 0x08006000, 52);
		PaygOvesInput();

		#ifdef PUMP_PROJECT
		 if(payg.remaing_days>0)
		 {
		 	#ifdef DC_PUMP_SUPPORT
			PumpTxCmd(PUMP_SET_ON);
			#else
		 	PumpTxCmd(PUMP_FREE);
			HAL_Delay(50);
		 	PumpTxCmd(PUMP_RESET);
			HAL_Delay(50);
			PumpTxCmd(PUMP_ON);
			#endif
		 	}
		 #endif

		//SaveFlash_Flag = 1;//保存到FLASH
		EEpUpdateEnable();

	};break; 
	case 0xfb://write_HashTop
	{ 
		memset(payg.hast_otp,0x00, 8);
		memcpy(payg.hast_otp, Camp_cmd + 5, 8); 
		temphash = payg.hast_otp[0];
		payg.hast_otp[0] =  payg.hast_otp[1];
		payg.hast_otp[1] = temphash;
		//SaveFlash_Flag = 1;//保存到FLASH
		EEpUpdateEnable();
		Printf_Usart_num(hashtop_ok, 7);
	};break;    
	default:
		break;//(Printf_Usart("Command_err!\r\n"));break;
	}
	PaygOvesTask();
	Camp_ComOK = 0;
	}



void Process_cmd_all(char cmd)
{
 PAYG_TypeDef temppayg;
  switch (cmd)
{
	case 0x01://Read_OEMID
	{
	    EEpReadPage(0x0000,sizeof(PAYG_TypeDef),(uint8_t*)&temppayg);
		
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_opidhead, 6);
		memcpy(cmd_sendbuf + 6, temppayg.oem_id, 14);
		cmd_sendbuf[20] = CRC8(cmd_sendbuf, 20);
		Printf_Usart_num(cmd_sendbuf, 21);
	};break;
	case 0x05://Read_Remaining_PAYG_Days
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_RPD, 5);
		cmd_sendbuf[5] = payg.remaing_days & 0xFF;
		cmd_sendbuf[6] = (payg.remaing_days >> 8) & 0xFF;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x06://Read_Days have been run
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_DHB, 5);
		cmd_sendbuf[5] = payg.days_has_been_runing & 0xFF;
		cmd_sendbuf[6] = (payg.days_has_been_runing >> 8) & 0xFF;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break; 
	case 0x07://Read_PAYG_Days 
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_PD, 5);
		cmd_sendbuf[5] = payg.recharge_days & 0xFF;
		cmd_sendbuf[6] = (payg.recharge_days >> 8) & 0xFF;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;     
	case 0x08://Read_PPID
	{
		EEpReadPage(0x0000,sizeof(PAYG_TypeDef),(uint8_t*)&temppayg);
		
		if(Uart_Buffer[5] == 0x14)
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_ppidhead, 6);
			memcpy(cmd_sendbuf + 6, temppayg.payg_id, 20);
			cmd_sendbuf[26] = CRC8(cmd_sendbuf, 26);
			Printf_Usart_num(cmd_sendbuf, 27);
		}
		else
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_ppidhead_new, 6);
			memcpy(cmd_sendbuf + 6, temppayg.payg_id, 0x0e);
			cmd_sendbuf[20] = CRC8(cmd_sendbuf, 20);
			Printf_Usart_num(cmd_sendbuf, 21);
		}
	};break;
	case 0x09://Read_PAYG_State
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_PS, 5);
		if(payg.free == YES)
			cmd_sendbuf[5] = 0x01;
		else                
			cmd_sendbuf[5] = 0x00;
		cmd_sendbuf[6] = 0x00;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x0a://Read_Output_Control_State
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_OCS, 5);
		if((payg.free == YES) || (payg.remaing_days > 0))
			cmd_sendbuf[5] = 0x01;
		else                
			cmd_sendbuf[5] = 0x00;
		cmd_sendbuf[6] = 0x00;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break; 
	case 0x0b://Read_System_Status_Code
	{
		#if 1
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x08;
		cmd_sendbuf[4] = 0x0b;
		cmd_sendbuf[5] = g_sysStateCode.state[0];
		cmd_sendbuf[6] = g_sysStateCode.state[1];
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
		#else
		if(/*Sys_Code == 0xE1*/  g_sysStateCode.error.rtc_error)//DS1302掉电
		{
			Printf_Usart_num(Answer_System_Status_CodeE, 8);
		}
		else if(Sys_Code == 0xE2)//晶体不工作
		{
			Printf_Usart_num(Answer_System_Status_CodeE2, 8);
		}
		else
			Printf_Usart_num(Answer_System_Status_Code, 8);
		#endif
	};break;
	case 0x0c://Relative_SOC
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x08;
		cmd_sendbuf[4] = 0x0c;
		//cmd_sendbuf[5] = g_bq40z50_state.Relative_State_of_Charge[0];
		//cmd_sendbuf[6] = g_bq40z50_state.Relative_State_of_Charge[1];
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x0d://Remaning_Capacity
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x08;
		cmd_sendbuf[4] = 0x0d;
		//cmd_sendbuf[5] = g_bq40z50_state.Remaning_Capacity[0];
		//cmd_sendbuf[6] = g_bq40z50_state.Remaning_Capacity[1];
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x0e://Full_Charge_Capacity
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x08;
		cmd_sendbuf[4] = 0x0e;
		//cmd_sendbuf[5] = g_bq40z50_state.Full_charge_Capacity[0];
		//cmd_sendbuf[6] = g_bq40z50_state.Full_charge_Capacity[1];
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x0f://---Accu_Energy_Output老协议
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x08;
		cmd_sendbuf[4] = 0x0f;
		//cmd_sendbuf[5] = g_bq40z50_state.Average_Power[0];
		//cmd_sendbuf[6] = g_bq40z50_state.Average_Power[1];
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x10://Accu_Cycles
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x08;
		cmd_sendbuf[4] = 0x10;
		//cmd_sendbuf[5] = g_bq40z50_state.Cycle_Count[0];
		//cmd_sendbuf[6] = g_bq40z50_state.Cycle_Count[1];
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;
	case 0x11://Read_HashTop
	{
		EEpReadPage(0x0000,sizeof(PAYG_TypeDef),(uint8_t*)&temppayg);
		memset(cmd_sendbuf, 0, 32);      
		memcpy(cmd_sendbuf, cmd_hashtophead, 5);

		memcpy(cmd_sendbuf + 5, temppayg.hast_otp + 1, 4);//这里指针+1 根据指针类型 uint32相当于4字节 不是1字节
		memcpy(cmd_sendbuf + 9, temppayg.hast_otp, 4);
		cmd_sendbuf[13] = CRC8(cmd_sendbuf, 13);
		Printf_Usart_num(cmd_sendbuf, 14);
	};break;
	case 0x12://Read_root
	{
		EEpReadPage(0x0000,sizeof(PAYG_TypeDef),(uint8_t*)&temppayg);
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_roothead, 5);
		memcpy(cmd_sendbuf + 5, temppayg.hast_root, 8);
		cmd_sendbuf[13] = CRC8(cmd_sendbuf, 13);
		Printf_Usart_num(cmd_sendbuf, 14);
	};break;
	case 0x13://Read_Run_Days_Backup
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, Answer_RDB, 5);
		cmd_sendbuf[5] = payg.run_days_backup & 0xFF;
		cmd_sendbuf[6] = (payg.run_days_backup >> 8) & 0xFF;
		cmd_sendbuf[7] = CRC8(cmd_sendbuf, 7);
		Printf_Usart_num(cmd_sendbuf, 8);
	};break;    
	case 0x14://Read_REV
	{
		//Printf_Usart_num(Answer_RREV, 8);
		Answer_RREV[5]=KEYB_VERSION;

		Answer_RREV[7] = CRC8(Answer_RREV, 7);

		Printf_Usart_num(Answer_RREV, 8);
	};break;
	case 0x17://Load_Power  acc
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x0C;
		cmd_sendbuf[4] = 0x17;
		//cmd_sendbuf[5] = g_bq40z50_state.Acc_Power[0];
		//cmd_sendbuf[6] = g_bq40z50_state.Acc_Power[1];
		//cmd_sendbuf[7] = g_bq40z50_state.Acc_Power[2];
		//cmd_sendbuf[8] = g_bq40z50_state.Acc_Power[3];
		//      cmd_sendbuf[7] = g_bq40z50_state.Solar_Generation[0];
		//      cmd_sendbuf[8] = g_bq40z50_state.Solar_Generation[1];
		// cmd_sendbuf[9] = g_bq40z50_state.Average_Power[0];
		// cmd_sendbuf[10] = g_bq40z50_state.Average_Power[1];
		/*cmd_sendbuf[7] = 0;
		cmd_sendbuf[8] = 0;
		cmd_sendbuf[9] = 0;
		cmd_sendbuf[10] = 0;*/
		cmd_sendbuf[11] = CRC8(cmd_sendbuf, 11);
		Printf_Usart_num(cmd_sendbuf, 12);
	};break;
	case 0x18://Charge_Power  solar
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x0C;
		cmd_sendbuf[4] = 0x18;
		//cmd_sendbuf[5] = g_bq40z50_state.Solar_Power[0];
		//cmd_sendbuf[6] = g_bq40z50_state.Solar_Power[1];
		//cmd_sendbuf[7] = g_bq40z50_state.Solar_Power[2];
		//cmd_sendbuf[8] = g_bq40z50_state.Solar_Power[3];
		//      cmd_sendbuf[7] = g_bq40z50_state.Solar_Generation[0];
		//      cmd_sendbuf[8] = g_bq40z50_state.Solar_Generation[1];
		// cmd_sendbuf[9] = g_bq40z50_state.Charge_Power[0];
		// cmd_sendbuf[10] = g_bq40z50_state.Charge_Power[1];

		/*cmd_sendbuf[7] = 0;
		cmd_sendbuf[8] = 0;
		cmd_sendbuf[9] = 0;
		cmd_sendbuf[10] = 0;*/
		cmd_sendbuf[11] = CRC8(cmd_sendbuf, 11);
		Printf_Usart_num(cmd_sendbuf, 12);
	};break;
	case 0x22://flid id
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 5+24+1;
		cmd_sendbuf[4] = 0x22;
		#ifdef FLEED_ID_SUPPORT
		memcpy(&cmd_sendbuf[5],g_UserSet.fleed,24);
		#endif

		cmd_sendbuf[5+24] = CRC8(cmd_sendbuf, 5+24);
		Printf_Usart_num(cmd_sendbuf, 5+24+1);
	};break;
	case 0x23://banding state
	{
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 5+24+1;
		cmd_sendbuf[4] = 0x23;
		if(g_UserSet.ppid_cfg)
			cmd_sendbuf[5]=0;
		else
			cmd_sendbuf[5]=1;

		cmd_sendbuf[5+1] = CRC8(cmd_sendbuf, 5+1);
		Printf_Usart_num(cmd_sendbuf, 5+1+1);
	};break;
	case 0x7f://Charge_Power
	{
		//if(Uart_Buffer[5] == sizeof(BQ40Z50_TypeDef))//如果接受协议用户数据区域等于接受缓冲区定义
		{
		//	memcpy(&g_bq40z50_state, Uart_Buffer+6, sizeof(BQ40Z50_TypeDef));
		}
	};break;
	case 0xe1://RTC_Err
	{
		;//Printf_Usart("Rtc_Err!!!!!!!!!!!!\r\n");
	};break;
	case 0xef://Write_OEMID
	{
		memset(payg.oem_id,0x00, 20);
		memcpy(payg.oem_id, Camp_cmd + 6, 14);
		
		//SaveFlash_Flag = 1;
		EEpUpdateEnable();
		Printf_Usart_num(opid_ok, 7);
	};break;
	case 0xf1://Write_PPID
	{
		memset(payg.payg_id,0x00, 20);
		memcpy(payg.payg_id, Camp_cmd + 6, 20); 
		//SaveFlash_Flag = 1;//保存到FLASH
		//LOG("CMD -->>>  %s\n",Camp_cmd + 6);
		//LOG("Write_PPID -->>> %s\n",payg.payg_id);
		EEpUpdateEnable();
		Printf_Usart_num(ppid_ok, 7);      
	};break;
	case 0xf2://Write_HashRoot
	{   
		#ifdef FLEED_ID_SUPPORT
		memset(g_UserSet.fleed,0x00,MEM_SIZE_FLID);
		memcpy(g_UserSet.fleed,Camp_cmd + 6,24);
		#endif
		EEpUpdateEnable();
		memset(cmd_sendbuf, 0, 32);
		memcpy(cmd_sendbuf, cmd_allhead, 3);
		cmd_sendbuf[3] = 0x07;
		cmd_sendbuf[4] = 0xf2;
		cmd_sendbuf[5] = 0x0a;

		cmd_sendbuf[6] = CRC8(cmd_sendbuf, 6);
		Printf_Usart_num(cmd_sendbuf,7);
		//Printf_Usart_num(root_ok, 7);
	};break; 
	case 0xf5://Write_HashRoot
	{   
		if(/*(Sys_Code == 0xE1) || (Sys_Code == 0xE3)*/  g_sysStateCode.error.rtc_error)
		{
			#ifdef DI_INVERTER48V_PROJECT
			EEpSetPpidCfgState(FALSE);
			EEpUpdateEnable();
			#endif
			#ifdef DI_PEICHENGBMS_PROJECT
			EEpSetPpidCfgState(FALSE);
			EEpUpdateEnable();
			#endif
			#ifdef DI_EMCOOKE_PROJECT
			EEpSetPpidCfgState(FALSE);
			EEpUpdateEnable();
			#endif
			#ifdef DI_HEROEE2BMS_PROJECT
			EEpSetPpidCfgState(FALSE);
			EEpUpdateEnable();
			#endif
			#ifdef DI_LLZINVERTER_PROJECT
			EEpSetPpidCfgState(FALSE);
			EEpUpdateEnable();
			#endif
			#ifdef DI_FAN_PROJECT
			EEpSetPpidCfgState(FALSE);
			EEpUpdateEnable();
			#endif
			#ifdef DI_FREEZER_PROJECT
			EEpSetPpidCfgState(FALSE);
			EEpUpdateEnable();
			#endif

			payg.free = NO;
			payg.lock = NO;
			payg.remaing_days = 0;
			payg.days_has_been_runing = 0;
			InitDs1302();//已运行天数清0
			payg.time_cnt = 0;
			payg.recharge_days = 0;

			Sys_Code = 0;//clean Err
		        g_sysStateCode.error.rtc_error=0;

			payg.day_cem = 0;//运行天数备份清零
		}
		else
		{
			//Printf_Usart("No error!\r\n");
		}
	};break; 
	case 0xfa://充值
	{
		memset(cmd_sendbuf, 0, 32);
		//倒序 按照正确命令排序
		cmd_sendbuf[3] = Camp_cmd[12];
		cmd_sendbuf[2] = Camp_cmd[11];
		cmd_sendbuf[1] = Camp_cmd[10];
		cmd_sendbuf[0] = Camp_cmd[9];
		cmd_sendbuf[7] = Camp_cmd[8];
		cmd_sendbuf[6] = Camp_cmd[7];
		cmd_sendbuf[5] = Camp_cmd[6];
		cmd_sendbuf[4] = Camp_cmd[5];

		memcpy(payg.hast_input, cmd_sendbuf, 8);
		//      Oves_SaveConfig(Pay_Info.PPID, 0x08006000, 52);
		PaygOvesInput();

		//SaveFlash_Flag = 1;//保存到FLASH
		EEpUpdateEnable();

		#ifdef DC_PUMP_SUPPORT
		if(/*PaygGetPayState()&&*/g_tokenState==TOKEN_OK)
			PumpOn(TRUE);
	    #endif

	};break; 
	case 0xfb://write_HashTop
	{ 
		memset(payg.hast_otp,0x00, 8);
		memcpy(payg.hast_otp, Camp_cmd + 5, 8); 
		temphash = payg.hast_otp[0];
		payg.hast_otp[0] =  payg.hast_otp[1];
		payg.hast_otp[1] = temphash;
		//SaveFlash_Flag = 1;//保存到FLASH
		EEpUpdateEnable();
		Printf_Usart_num(hashtop_ok, 7);
	};break; 
	case 0xff://bat
	{
		if(Uart_Buffer[5] == 0x3F)//CV1
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_allhead, 3);
			cmd_sendbuf[3] = 0x0a;
			cmd_sendbuf[4] = 0x00;
			cmd_sendbuf[5] = 0x3F;
			cmd_sendbuf[6] = 0x02;
			//cmd_sendbuf[7] = g_bq40z50_state.CV1[0];
			//cmd_sendbuf[8] = g_bq40z50_state.CV1[1];
			//cmd_sendbuf[9] = CRC8(cmd_sendbuf, 9);
			Printf_Usart_num(cmd_sendbuf, 10);
		}
		if(Uart_Buffer[5] == 0x3E)//CV2
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_allhead, 3);
			cmd_sendbuf[3] = 0x0a;
			cmd_sendbuf[4] = 0x00;
			cmd_sendbuf[5] = 0x3e;
			cmd_sendbuf[6] = 0x02;
			//cmd_sendbuf[7] = g_bq40z50_state.CV2[0];
			//cmd_sendbuf[8] = g_bq40z50_state.CV2[1];
			cmd_sendbuf[9] = CRC8(cmd_sendbuf, 9);
			Printf_Usart_num(cmd_sendbuf, 10);
		}
		if(Uart_Buffer[5] == 0x3D)//CV3
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_allhead, 3);
			cmd_sendbuf[3] = 0x0a;
			cmd_sendbuf[4] = 0x00;
			cmd_sendbuf[5] = 0x3D;
			cmd_sendbuf[6] = 0x02;
			//cmd_sendbuf[7] = g_bq40z50_state.CV3[0];
			//cmd_sendbuf[8] = g_bq40z50_state.CV3[1];
			cmd_sendbuf[9] = CRC8(cmd_sendbuf, 9);
			Printf_Usart_num(cmd_sendbuf, 10);
		}
		if(Uart_Buffer[5] == 0x3C)//CV4
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_allhead, 3);
			cmd_sendbuf[3] = 0x0a;
			cmd_sendbuf[4] = 0x00;
			cmd_sendbuf[5] = 0x3C;
			cmd_sendbuf[6] = 0x02;
			//cmd_sendbuf[7] = g_bq40z50_state.CV4[0];
			//cmd_sendbuf[8] = g_bq40z50_state.CV4[1];
			cmd_sendbuf[9] = CRC8(cmd_sendbuf, 9);
			Printf_Usart_num(cmd_sendbuf, 10);
		}
		if(Uart_Buffer[5] == 0x0A)//BatteryCurrent
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_allhead, 3);
			cmd_sendbuf[3] = 0x0a;
			cmd_sendbuf[4] = 0x00;
			cmd_sendbuf[5] = 0x0A;
			cmd_sendbuf[6] = 0x02;
			//cmd_sendbuf[7] = g_bq40z50_state.Current[0];
			//cmd_sendbuf[8] = g_bq40z50_state.Current[1];
			cmd_sendbuf[9] = CRC8(cmd_sendbuf, 9);
			Printf_Usart_num(cmd_sendbuf, 10);
		}
		if(Uart_Buffer[5] == 0x09)//BatteryVoltage
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_allhead, 3);
			cmd_sendbuf[3] = 0x0a;
			cmd_sendbuf[4] = 0x00;
			cmd_sendbuf[5] = 0x09;
			cmd_sendbuf[6] = 0x02;
			//cmd_sendbuf[7] = g_bq40z50_state.Voltage[0];
			//cmd_sendbuf[8] = g_bq40z50_state.Voltage[1];
			cmd_sendbuf[9] = CRC8(cmd_sendbuf, 9);
			Printf_Usart_num(cmd_sendbuf, 10);
		}
		if(Uart_Buffer[5] == 0x08)//BatteryTemperature
		{
			memset(cmd_sendbuf, 0, 32);
			memcpy(cmd_sendbuf, cmd_allhead, 3);
			cmd_sendbuf[3] = 0x0a;
			cmd_sendbuf[4] = 0x00;
			cmd_sendbuf[5] = 0x08;
			cmd_sendbuf[6] = 0x02;
			//cmd_sendbuf[7] = g_bq40z50_state.Temperature[0];
			//cmd_sendbuf[8] = g_bq40z50_state.Temperature[1];
			cmd_sendbuf[9] = CRC8(cmd_sendbuf, 9);
			Printf_Usart_num(cmd_sendbuf, 10);
		}
	};break;
	default:
		//(Printf_Usart("Command_err!\r\n"));
	break;
	}
  
  PaygOvesTask();
  Camp_ComOK = 0;
}


#endif



