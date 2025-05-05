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




#include "llzinverter.h"
#include "string.h"
/*#include "bleuart.h"
#include "OvGattDiaProfile_ble.h"
#include "OvGattDtaProfile_ble.h"
#include "OvGattStsProfile_ble.h"*/
#include "payg.h"
#include "eeprom.h"
#include "gatt.h"


uint8_t g_LLZCmd=0;
uint16_t g_LLZCounter=0;


uint8_t g_charge_counter=0;
uint16_t g_jbd_watchdog_timer=0;
uint16_t g_jbd_epromWrite_timer=0;

uint16_t g_LLZ_ComErr_Cnt=0;

__IO uint8_t g_LLZGetInfor_Enable;
uint8_t *g_Uart485Buf;
uint8_t g_freeState=TRUE;
uint8_t g_LLZBleState=false;
uint8_t g_LlzDisconnectCount=0;
uint8_t g_LlzWakeCount=0;
uint8_t g_LlzPackDevId[32];
uint16_t g_LlzPvPower=0;
uint8_t g_LlzPvPowerTick=0;

uint16_t g_LlzPvStoreCnt=0;



//extern uint8_t g_GattMem[MEM_GATT_SIZE];

extern duet_uart_dev_t lega_di_uart485;
extern uint8_t uart2_buf[256];
extern uint16_t uart2_index;
extern duet_gpio_dev_t g_duet_gpio15;

//查表法计算crc

static uint16_t const CRC16Table[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

uint16_t LLZCRC16(uint8_t *dataIn, uint8_t length)

{
    uint16_t result = 0;

    uint16_t tableNo = 0;

    int16_t i = 0;

    for (i = 0; i < length; i++)

    {
        tableNo = ((result & 0xff) ^ (dataIn[i] & 0xff));

        result = ((result >> 8) & 0xff) ^ CRC16Table[tableNo];
    }

    return result;
}



uint16_t  GetCharLenght(uint8_t *str,uint8_t end_tag)
{
	uint16_t ret=0,i=0,j=0;//,len=0;
	uint16_t len=strlen((char*)str);

	for(i=0;str[i]!=end_tag;i++)
	{	
		ret++;
		j++;

		if(i>=len)
			return 0;
		}
	return ret;
}

void LLZWrite(uint8_t addr,uint8_t fun,uint8_t cmd,uint16_t size,uint8_t *Data)
{
	LLZ_WRITE_TypeDef temp;
	uint16_t crc=0;

	temp.bus_addr=addr;
	temp.fun=fun;
	temp.cmd=cmd;
	temp.size=size;
	
    memcpy(temp.data,Data,size);

	crc=LLZCRC16(&temp.bus_addr, 4+size);

	temp.data[size]=crc>>8;
	temp.data[size+1]=crc;

	LLZSend(&temp.bus_addr, 4+size+2);
	//hal_pwrmgr_lock(MOD_UART1);
	//osal_start_timerEx(bleuart_TaskID, BUP_OSAL_EVT_AT, 2000);
}

void LLZRead(uint8_t addr,uint8_t fun,uint8_t cmd,uint8_t size,uint8_t *dataN)
{
	LLZ_READ_TypeDef temp;
	uint16_t crc;

	temp.bus_addr=addr;
	temp.fun=fun;
	temp.cmd=cmd;

    temp.size=size;
	memcpy(temp.data,dataN,size);

	crc=LLZCRC16(&temp.bus_addr, 4+size);

	temp.data[size]=crc>>8;
	temp.data[size+1]=crc;

	LLZSend(&temp.bus_addr, 4+size+2);

	//hal_pwrmgr_lock(MOD_UART1);

	//osal_start_timerEx(bleuart_TaskID, BUP_OSAL_EVT_AT, 2000);
	
}


void LLZTimeoutTimerStart(void)
{
  uint32_t ppie=EEpGetAccuSolarGen();

//  log_i"BLE start SBP_SAMPLE_TIMER Timer  %d\n",g_LlzDisconnectCount);	
 // osal_start_timerEx(bleuart_TaskID, SBP_SAMPLE_TIMER, (BUP_connect_state()==FALSE&&g_LlzDisconnectCount>=60&&g_LlzWakeCount<4)?1500U:1500);

 if(g_LlzDisconnectCount<60)
  {   
	  g_LlzDisconnectCount++;
  }	  

 if(g_LlzPvPowerTick)
	 g_LlzPvPowerTick=0;
 else
 	g_LlzPvPowerTick++;


 
  if(g_LlzPvStoreCnt++>=3600)//1.5hour
  {
  	     g_LlzPvStoreCnt=0;

		 if(ppie!=EEpGetAccuSolarGen())
		 		EEpUpdateEnable();
  	}
	
	//if(BUP_connect_state())
	{
		g_LlzDisconnectCount=0;
		}

	 if(g_LlzPvPowerTick)
	 {	ppie+=g_LlzPvPower; 
    
		//EEpSetAccuSolarGen(ppie);

		ppie=ppie/1600; //60*60/1s*2
		//GattSetData( LIST_DTA, DTA_PVIE,(uint8_t*)&ppie);
		gatt_set_prop_data("pvie\0",(uint8_t*)&ppie);
	 	}
	
	
}

void LLZSend(uint8_t *buffer,uint16_t size)
{
		
   uint8_t i=0;

   log_i("Uar send size %d \r\n",size);	
   for(i=0;i<size;i++)
   	log_i(" %02X ",buffer[i]);

   	log_i(" \r\n ");
   //Uart3Send(buffer,size);
   
   //hal_uart_send_buff(UART1,(uint8_t*)buffer,size); 
   duet_gpio_output_high(&g_duet_gpio15);
   //log_i(" g_duet_gpio15 high \r\n ");
   duet_uart_send(&lega_di_uart485,buffer,size,100);
   //log_i(" g_duet_gpio15 low \r\n ");
   lega_rtos_delay_milliseconds(size);
   duet_gpio_output_low(&g_duet_gpio15);
}


void LLZGetEnable(void)
{
	g_LLZGetInfor_Enable=TRUE;
}

void LLZInit(void)
{
	g_Uart485Buf=uart2_buf;
	g_LLZCmd=LLZ_QUERY_CELL;
	g_LLZCounter=0;

	duet_gpio_output_low(&g_duet_gpio15);

		
	if(PaygGetFreeState())
		g_freeState=TRUE;
	else
		g_freeState=FALSE;
	//DemoTimerStart();
	//DemoModeStart();
}


uint16_t LLZChecksum(uint8_t *cmd,uint8_t size)
{
	uint8_t i=0;
	uint16_t checksum=0;

	for(i=0;i<size;i++)
		checksum+=cmd[i];

	checksum=(~checksum)+1;

	return checksum ;
}

void LLZMosCtrl(uint8_t on)
{
	/*uint16_t chksum=0;

	memset(g_LLZ_tx_buf,0X00,16);
	
	g_LLZ_tx_buf[0]=0xdd;
	g_LLZ_tx_buf[1]=0x5a;
    g_LLZ_tx_buf[2]=0xe1;
	g_LLZ_tx_buf[3]=2;
	g_LLZ_tx_buf[4]=00;

	if(on)
		g_LLZ_tx_buf[5]=0x00;  //out
	else	
		g_LLZ_tx_buf[5]=0x02;  //out

	chksum=LLZChecksum(g_LLZ_tx_buf,2);


	g_LLZ_tx_buf[6]=chksum>>8;  //checksun high byte
	g_LLZ_tx_buf[7]=chksum;  //checksun low byte


	g_LLZ_tx_buf[8]=0x77; 

    LLZSend(g_LLZ_tx_buf,9);*/
}

void LLZGetInfo(uint8_t cmd)
{
    uint8_t  temp=0,tempbuf[8];
	//uint16_t chksum=0;//,temp16=0;
	uint8_t infor[32]={0};
	
	
	if(cmd<LLZ_CMD_COUNT)
	{

		switch(cmd)
		{	
			case LLZ_QUERY_CELL:
				temp=0x01;
				LLZRead(0x61,0x03,0xb0,0x00,&temp);
				break;
			case LLZ_QUERY_SOC:
				tempbuf[0]=0x01;
				tempbuf[1]=0x06;
				LLZRead(0x61,0x03,0x9B,0x02,tempbuf);
				break;	
			case LLZ_QUERY_SN:
				temp=0x01;
				LLZWrite(0x61,0x03,0x9d,0x01,&temp);    
				break;
			case LLZ_QUERY_VOLTAGE:
				temp=0x01;
				LLZRead(0x61,0x03,0xA1,0x00,&temp);    
				break;
			case LLZ_QUERY_TEMP:
				temp=0x01;
				LLZRead(0x61,0x03,0xA2,0x00,&temp);    
				break;
			case LLZ_QUERY_FAULT:
				temp=0x01;
				LLZRead(0x61,0x03,0xA3,0x00,&temp);    
				break;	
			case LLZ_QUERY_VERSION:
				temp=0x01;
				LLZRead(0x61,0x03,0xA4,0X01,&temp);  
				break;
			case LLZ_CTRL_WDGON:
				if(PaygGetFreeState()==FALSE&&g_freeState)
				{	
					g_freeState=FALSE;
					tempbuf[0]=0x03;
					tempbuf[1]=0x01;
					LLZWrite(0x61,0x10,0x9c,2,tempbuf);  
					}
				else
				{	g_LLZCmd++;
					}
				break;
			case LLZ_CTRL_WDGOFF:
				if(PaygGetFreeState()&&g_freeState==FALSE)
				{	g_freeState=TRUE;
					tempbuf[0]=0x03;
					tempbuf[1]=0x00;
					LLZWrite(0x61,0x10,0x9c,2,tempbuf);   
					}
				else
					g_LLZCmd++;
				break;
			case LLZ_CTRL_ALARM_ON:
				//if(PaygGetRemainDays()<=3&&PaygGetFreeState()==FALSE)
				{
					tempbuf[0]=0x02;
					tempbuf[1]=0x01;
					LLZWrite(0x61,0x10,0x9c,0x02,tempbuf);  
					}
			//	else
					g_LLZCmd++;
				break;
			case LLZ_CTRL_ALARM_OFF:
				//if(PaygGetRemainDays()>=3&&PaygGetFreeState()==FALSE)
				{
					tempbuf[0]=0x02;
					tempbuf[1]=0x00;
					LLZWrite(0x61,0x10,0x9c,0x02,tempbuf);  
					}
				//else
					g_LLZCmd++;
				break;
			case LLZ_CTRL_HEARTBEAT:
				if(/*(PaygGetPayState()||GetDemoState())*/1)
				{
					tempbuf[0]=0x01;
					tempbuf[1]=0x01;
					LLZWrite(0x61,0x10,0x9c,0x02,tempbuf);  
					}
				else
					g_LLZCmd++;
				break;
			case LLZ_CTRL_WDGFEED:

				//if((PaygGetPayState()||GetDemoState()))
				{	
					tempbuf[0]=0x04;
					tempbuf[1]=0x01;
					LLZWrite(0x61,0x10,0x9c,0x02,tempbuf);   
					}
			//	else
				{	
					g_LLZCmd=LLZ_QUERY_CELL;
					}
				break;
			case LLZ_CTRL_PAYMODE:
				g_LLZCmd=LLZ_QUERY_CELL;
				break;
			case LLZ_CTRL_CTRL_IDUPDATE:
				memset(infor,0x00,32);
				//memcpy(infor+1,EEpGetOpid(),14);

				infor[0]=0x01;
				LLZWrite(0x61,0x10,0x9d,0x11,infor);   
			   /// EEpSetDevId(infor+1);
				break;
			
			}
		//log_i"CMD:%d \r\n",cmd);
		
		}
	else
	{
		g_LLZCmd=LLZ_QUERY_CELL;
		}

	//BUP_disconnect_handler();
	uart2_index=0;

	g_jbd_watchdog_timer++;

	 
}


void LLZTask(void)
{

	uint16_t i=0,len=0,j=0,rx_counter=0,voltage=0,read_valid=0,write_valid=0;
	uint8_t freecmd[]={0x01,0x06,0x03,0x76,0x56,0xBC,0x56,0x45};
	uint8_t heartbeatcmd[]={0x01,0x06,0x03,0x70,0xAA,0xBB,0x56,0x87};
	uint8_t watchdogcmd[]={0x01,0x06,0x03,0x77,0xAA,0xBB,0x56,0x87};
	uint8_t rentalmodecmd[]={0x01,0x06,0x03,0x78,0x00,0x01,0x56,0x87};

	
	uint8_t temp=0,uint;
	uint8_t infor[32]={0};

	LLZTimeoutTimerStart(); //2sec 
	LLZGetEnable();

	
	log_i("LLZ PayState %02lX  Free state %ld  Demo %d cnt %ld remain %d\r\n",PaygGetPayState(),PaygGetFreeState(),1,EEpGetDemoCnt(),1);

	#ifdef CONFIG_PAY_CODE
	return ;
	#endif

	//rx_counter=BUP_data_uart_rx_size();

	//if(rx_counter)
	{	
		//for(j=0;j<rx_counter;j++)
		//	log_i"%02X ",g_Uart485Buf[j]);
		}

	//log_i"\r\n");

	#if 0
	if(BUP_connect_state()==FALSE&&g_LlzDisconnectCount>=60)
	{
		for(j=0;j<rx_counter;j++)
		{
			if(g_Uart485Buf[j]==0x61&&g_Uart485Buf[j+1]==0x03&&rx_counter>=g_Uart485Buf[j+3]+4)
			{	read_valid=TRUE;
				log_i"sleep read_valid \r\n");
			    osal_stop_timerEx(bleuart_TaskID, BUP_OSAL_EVT_AT);
				BUP_disconnect_handler();
				hal_pwrmgr_unlock(MOD_UART1);

				return ;
				
				}
			if(g_Uart485Buf[j]==0x61&&g_Uart485Buf[j+1]==0x10&&rx_counter>=7)
			{	write_valid=TRUE;
				
				log_i"sleep write_valid \r\n");
			    osal_stop_timerEx(bleuart_TaskID, BUP_OSAL_EVT_AT);
				BUP_disconnect_handler();
				hal_pwrmgr_unlock(MOD_UART1);
				return ;
				
				}
		}

		if(g_LLZGetInfor_Enable)
		{
			/*if(g_LlzWakeCount>=4)
			{	
				LLZGetInfo(LLZ_CTRL_WDGFEED);
				g_LlzWakeCount=0;
				}
			else
			{	LLZGetInfo(LLZ_CTRL_COUNTER);	
				g_LlzWakeCount++;
				}*/
			
			log_i"wake count %d \r\n",g_LlzWakeCount);
			g_LLZGetInfor_Enable=FALSE;
			}
		
		return ;
		}
    #endif

	
	if(g_LLZGetInfor_Enable)
	{	
		log_i("Command: %02X \r\n",g_LLZCmd);

		/*if(PaygGetFreeState())
			LLZSend(freecmd,8);
		else if(PaygGetPayState())
			LLZSend(heartbeatcmd,8);*/

		LLZGetInfo(g_LLZCmd);

		g_LLZGetInfor_Enable=FALSE;

		if(g_LLZ_ComErr_Cnt<100)
			g_LLZ_ComErr_Cnt++;

		if(g_LLZ_ComErr_Cnt>20)
		{
			g_LLZCmd++;

			if(g_LLZCmd>=LLZ_CMD_COUNT-1)
			    g_LLZCmd=LLZ_QUERY_CELL;
			
			g_LLZ_ComErr_Cnt=0;
			//g_sysStateCode.error.coulom_com=1;
			}
		}




  
	//for(i=0;i<rx_counter;i++)
	{
		//if((g_Uart485Buf[i+0]=='~'))
		{
			uint8_t *p,num=0,value=0,ntc=0,cmd=0,len=0,m=0;
			uint16_t temp16=0,tempratrue=0,crc,rsoc;
			int16_t *ptemp16,tmpint16=0,current=0;
			uint32_t charge_power=0,temp32;
			double tempdouble=0;
			int8_t tempint8=0;


			for(j=0;j<rx_counter;j++)
			{
				if(g_Uart485Buf[j]==0x61&&g_Uart485Buf[j+1]==0x03&&rx_counter>=g_Uart485Buf[j+3]+4)
				{	read_valid=TRUE;
					log_i("read_valid \r\n");
				    g_LLZ_ComErr_Cnt=0;
					
					}
				if(g_Uart485Buf[j]==0x61&&g_Uart485Buf[j+1]==0x10&&rx_counter>=7)
				{	write_valid=TRUE;
					
					log_i("write_valid \r\n");
				    g_LLZ_ComErr_Cnt=0;
					
					}
				if(read_valid)
				{
					cmd=g_LLZCmd;
					
					crc=(g_Uart485Buf[g_Uart485Buf[j+3]+4+1+j]|(g_Uart485Buf[g_Uart485Buf[j+3]+4+j]<<8));
					//log_i"crc: %04X crc16:%04X\r\n",crc,LLZCRC16((uint8_t*)&g_Uart485Buf[j], g_Uart485Buf[j+3]+4));

			        if(crc==LLZCRC16((uint8_t*)&g_Uart485Buf[j], g_Uart485Buf[j+3]+4)&&g_Uart485Buf[j+1]==0x03)
					{
						
						p=&g_Uart485Buf[j];

						log_i("read_valid CMD: %02X \r\n",g_LLZCmd);
						
						switch(cmd)
						{	
							case LLZ_QUERY_CELL: 
								g_LLZCmd++;
								temp32=0;

							    temp16=((p[4]<<8)|p[5]);
							    temp16*=10;

								temp32+=temp16;
								
								//GattSetData( LIST_DIA, DIA_CV01,(uint8_t*)&temp16);
								gatt_set_prop_data("cv01\0",(uint8_t*)&temp16);

								#ifdef UPP2_SET
								temp16=((p[6]<<8)|p[7]);
							    temp16*=10;
								temp32+=temp16;
								//GattSetData( LIST_DIA, DIA_CV02,(uint8_t*)&temp16);
								gatt_set_prop_data("cv02\0",(uint8_t*)&temp16);
								

								temp16=((p[8]<<8)|p[9]);
							    temp16*=10;
								temp32+=temp16;
								//GattSetData( LIST_DIA, DIA_CV03,(uint8_t*)&temp16);
								gatt_set_prop_data("cv03\0",(uint8_t*)&temp16);
								#endif

								//temp32=((p[10]<<8)|p[11]);
							   // temp32*=10;
								//GattSetData( LIST_DTA, DTA_PCKV,(uint8_t*)&temp32);
								gatt_set_prop_data("pckv\0",(uint8_t*)&temp32);

								p+=6;
							    //GattSetData( LIST_DTA, DTA_PCKV,(uint8_t*)&temp32);
								//GattSetData( LIST_DIA, DIA_CV01,(uint8_t*)&temp16);
								
								p+=6;

								temp16=((p[0]<<8)|p[1]);
								ptemp16=(int16_t*)&temp16;
								current=*ptemp16/100;
								//LOG("current : %04X %d\r\n",temp16,*ptemp16);
								//GattSetData( LIST_DTA, DTA_PCKC,(uint8_t*)&current);
								gatt_set_prop_data("pckc\0",(uint8_t*)&current);
								p+=2;

								if(current>0)
								{
									temp16=temp32/100*current/10;
									//GattSetData( LIST_DTA, DTA_BATP,(uint8_t*)&temp16);
									gatt_set_prop_data("batp\0",(uint8_t*)&temp16);
									}

								//temp16=p[0];
								//GattSetData( LIST_DTA, DTA_PCKT,(uint8_t*)&temp16);
								break;
								/*
								*ptemp16=*ptemp16/10;
								current=*ptemp16;

								//if(current>0)
								//	*ptemp16=*ptemp16*-1;

								
								LOG("current : %04X %d\r\n",temp16,*ptemp16);

							    //GattSetData( LIST_DTA, DTA_PCKC,(uint8_t*)&temp16);

								temp16=((p[4]<<8)|p[5]);
							    //temp16*=100;
							    rsoc=temp16;

							    GattSetData( LIST_DTA, DTA_RSOC,(uint8_t*)&temp16);

								temp16=((p[6]<<8)|p[7]);
							    temp16=temp16/100;

							    GattSetData( LIST_DTA, DTA_RSOH,(uint8_t*)&temp16);

								
								p+=8;
								//cell num
							
								//temp

								p+=40;

								for(m=0;m<6;m++)
								{
									temp16=((p[2*m]<<8)|p[2*m+1]);
									GattSetData( LIST_DIA, DIA_TEMP1+m,(uint8_t*)&temp16);
									}

								p+=40;
								
								temp16=((p[0]<<8)|p[1]);
								ptemp16=&temp16;

								//LOG("OUTP : %04X %d\r\n",temp16,*ptemp16);
								current*=-1;

								if(current>0)
								{	//temp16*=-1;
									
									GattSetData( LIST_DTA, DTA_INPP,(uint8_t*)&temp16);	
									GattSetData( LIST_DTA, DTA_BATP,(uint8_t*)&temp16);

									temp16=0;
									GattSetData( LIST_DTA, DTA_OUTP,(uint8_t*)&temp16);
								    
									GattSetData( LIST_DTA, DTA_PCKC,(uint8_t*)&current);	
									}
								else  //discharge
								{	//temp16*=-1;
									GattSetData( LIST_DTA, DTA_OUTP,(uint8_t*)&temp16);
									temp16=0;
									GattSetData( LIST_DTA, DTA_INPP,(uint8_t*)&temp16);
									GattSetData( LIST_DTA, DTA_BATP,(uint8_t*)&temp16);
								    
									//current*=-1;
									GattSetData( LIST_DTA, DTA_PCKC,(uint8_t*)&current);	
								    
									}

								p+=4;

								temp16=((p[0]<<8)|p[1]); //0.1ah
								temp16=temp16/10;
								GattSetData( LIST_DTA, DTA_FCCP,(uint8_t*)&temp16);
								temp16=rsoc*temp16/100;
								GattSetData( LIST_DTA, DTA_RCAP,(uint8_t*)&temp16);*/

								//break;
							case LLZ_QUERY_SOC:
								g_LLZCmd++;
								p+=4;

							    temp16=((p[0]<<8)|p[1]);
							    temp16/=10;
							   // GattSetData( LIST_DTA, DTA_RSOC,(uint8_t*)&temp16);
							    gatt_set_prop_data("rsoc\0",(uint8_t*)&temp16);
								p+=6;
								temp16=((p[0]<<8)|p[1]);
							    temp16/=10;
							    //GattSetData( LIST_DTA, DTA_RCAP,(uint8_t*)&temp16);
							    gatt_set_prop_data("rsoc\0",(uint8_t*)&temp16);
								p+=2;
								temp16=((p[0]<<8)|p[1]);
							    temp16/=10;
							    //temp16=314;
								//GattSetData( LIST_DTA, DTA_FCCP,(uint8_t*)&temp16);
								 gatt_set_prop_data("fccp\0",(uint8_t*)&temp16);
								
								break;
							case LLZ_QUERY_SN: 
								memset(g_LlzPackDevId,0x00,32);
																
								
								if(p[3]<30)
									memcpy(g_LlzPackDevId,p+4,p[3]);
								else
									memcpy(g_LlzPackDevId,p+4,30);
								
								log_i("\r\n g_PackDevId %s   %s\r\n",g_LlzPackDevId,EEpGetDevId());

								if(EEpGetPpidCfgState()!=TRUE)
								{ 
									uint8_t dev_sn[32]={0};
									
									if(PaygGetFreeState())
										g_freeState=FALSE;  
									else
										g_freeState=TRUE;

								    log_i("Update PPid\r\n");
									g_LLZCmd=LLZ_CTRL_CTRL_IDUPDATE;
									break;
									}
								
								p=(uint8_t*)strstr((char*)EEpGetDevId(),(char*)g_LlzPackDevId); 
								if((p!=NULL&&p==EEpGetDevId()&&strlen((char*)g_LlzPackDevId)>5&&strlen((char*)EEpGetDevId())>5)||GetDemoState())
								{
									log_i("PPid check valid \r\n");
									g_LLZCmd++;
									}
								else
								{
									log_i("PPid check Invalid \r\n");
									g_LLZCmd=LLZ_QUERY_CELL;
									}

								break;

							    
								
								//inverter internal temp
								/*temp16=((p[0]<<8)|p[1]);
							    temp16/=10;
								p+=2;

							    GattSetData( LIST_DTA, DTA_IITE,(uint8_t*)&temp16);*/
								
								break;
							case LLZ_QUERY_VOLTAGE:
								p+=3;

								//ac outp power
								temp16=((p[7]<<8)|p[8]);
							    temp16/=10;
							    //GattSetData( LIST_DTA, DTA_ACOP,(uint8_t*)&temp16);
							    gatt_set_prop_data("acop\0",(uint8_t*)&temp16);

								//GattSetData( LIST_DTA, DTA_OUTP,(uint8_t*)&temp16);
								gatt_set_prop_data("outp\0",(uint8_t*)&temp16);
								//ac input voltage
								temp16=((p[9]<<8)|p[10]);
							    temp16/=10;

							    //GattSetData( LIST_DTA, DTA_ACIV,(uint8_t*)&temp16);
							    gatt_set_prop_data("aciv\0",(uint8_t*)&temp16);

								//ac input current
							/*	temp16=((p[0]<<8)|p[1]);
							    temp16/=10;
								//p+=2;
								if(temp16)
							         GattSetData( LIST_DTA, DTA_ACCU,(uint8_t*)&temp16);*/
							         
							    //ac outp voltage
								temp16=((p[11]<<8)|p[12]);
							    temp16/=10;

							   // GattSetData( LIST_DTA, DTA_ACOV,(uint8_t*)&temp16);
							   gatt_set_prop_data("acov\0",(uint8_t*)&temp16);

								//ac input freq
								temp16=((p[13]<<8)|p[14]);
							    temp16/=100;
								
							    //GattSetData( LIST_DTA, DTA_ACFR,(uint8_t*)&temp16);
							    gatt_set_prop_data("acfr\0",(uint8_t*)&temp16);

								//ac input power
								temp16=((p[16]<<8)|p[17])/10;
								charge_power=temp16;
							    
							    //GattSetData( LIST_DTA, DTA_ACIP,(uint8_t*)&temp16);
							    gatt_set_prop_data("acip\0",(uint8_t*)&temp16);

								//ac output current
								temp16=((p[9]<<8)|p[10]);
							    temp16/=10;

								temp16=charge_power/temp16;
								ptemp16=&temp16;
								
								if(temp16)
							     {  
							     	//GattSetData( LIST_DTA, DTA_ACCU,(uint8_t*)&temp16);
							     	gatt_set_prop_data("accu\0",(uint8_t*)&temp16);
									}
								else
								{
									//ac power /ac voltage
									temp16=((p[7]<<8)|p[8])/((p[11]<<8)|p[12]);

									ptemp16=&temp16;

									*ptemp16=-temp16;
									//GattSetData( LIST_DTA, DTA_ACCU,(uint8_t*)&temp16);
									gatt_set_prop_data("accu\0",(uint8_t*)&temp16);
									}

								temp16=((p[18]<<8)|p[19]);
							    temp16/=100;

							    //GattSetData( LIST_DTA, DTA_PVIV,(uint8_t*)&temp16);
							    gatt_set_prop_data("pviv\0",(uint8_t*)&temp16);

								temp16=((p[20]<<8)|p[21]); //????
								ptemp16=&temp16;
							    *ptemp16/=100;

								if(*ptemp16<0)
									*ptemp16*=-1;

							    //GattSetData( LIST_DTA, DTA_PVIC,(uint8_t*)&temp16);
							    gatt_set_prop_data("pvic\0",(uint8_t*)&temp16);

								temp16=((p[22]<<8)|p[23]);
								temp16/=10;
						
							    //GattSetData( LIST_DTA, DTA_PVIP,(uint8_t*)&temp16);
							    gatt_set_prop_data("pvip\0",(uint8_t*)&temp16);

								g_LlzPvPower=temp16;
								g_LLZCmd++;
								break;
							case LLZ_QUERY_TEMP:
							    tempint8=0;
								p+=3;

								tempint8=p[13];
								ptemp16=&temp16;
								*ptemp16=tempint8;

								log_i("temp: %02x %02x %02x %02x \r\n",p[13],p[14],p[15],p[16]);
							    //GattSetData( LIST_DIA, DIA_TEMP1,(uint8_t*)&temp16);
							    gatt_set_prop_data("pkt1\0",(uint8_t*)&temp16);
								//GattSetData( LIST_DTA, DTA_PCKT,(uint8_t*)&temp16);
								gatt_set_prop_data("pckt\0",(uint8_t*)&temp16);

								tempint8=p[14];
								*ptemp16=tempint8;
								//GattSetData( LIST_DIA, DIA_TEMP2,(uint8_t*)&temp16);
								gatt_set_prop_data("pkt2\0",(uint8_t*)&temp16);

								tempint8=p[15];
								*ptemp16=tempint8;	
								//GattSetData( LIST_DIA, DIA_TEMP3,(uint8_t*)&temp16);
								gatt_set_prop_data("pkt3\0",(uint8_t*)&temp16);

								tempint8=p[16];
								*ptemp16=tempint8;
								//GattSetData( LIST_DIA, DIA_TEMP4,(uint8_t*)&temp16);
								gatt_set_prop_data("pkt4\0",(uint8_t*)&temp16);

								tempint8=p[17];
								*ptemp16=tempint8;
								//GattSetData( LIST_DIA, DIA_TEMP5,(uint8_t*)&temp16);
								gatt_set_prop_data("pkt5\0",(uint8_t*)&temp16);
								
								g_LLZCmd++;
								break;	
							case LLZ_QUERY_FAULT:
								g_LLZCmd++;
								break;
							case LLZ_QUERY_VERSION:	
								g_LLZCmd++;
								break;
								
							
							}

						    //osal_stop_timerEx(bleuart_TaskID, BUP_OSAL_EVT_AT);

						    //BUP_disconnect_handler();
						    //hal_pwrmgr_unlock(MOD_UART1);
							break;
					
						}

		
					}

				if(write_valid)
				{	
				    //log_i"write_valid CMD: %02X \r\n",g_LLZCmd);
				    //osal_stop_timerEx(bleuart_TaskID, BUP_OSAL_EVT_AT);
					
					if(g_LLZCmd==LLZ_CTRL_CTRL_IDUPDATE)
					{	//EEpSetPpidCfgState(TRUE);
						g_LLZCmd=LLZ_QUERY_CELL;
						}
					else
					    g_LLZCmd++;
					//BUP_disconnect_handler();
					//hal_pwrmgr_unlock(MOD_UART1);
					break;
					}
				}

		}
	}
	

}







