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
#include "llzinverter.h"

#include "device.h"




Device_TypeDef *g_DeviceTable[MAX_DEVICES];

const Device_TypeDef g_LlzInverterDevice={
	.name="llzinverter",
	.init=&LLZInit,
	.process=&LLZTask,
	.write=NULL,
	.ioctrl=NULL,
	.read=NULL,
	.write=NULL
};

uint8_t register_device(Device_TypeDef*dev)
{
	uint8_t i=0;

	for(i=0;i<MAX_DEVICES;i++)
	{
		if(!g_DeviceTable[i])
		{
			g_DeviceTable[i]=dev;	
			return TRUE;
			}
		}
	return FALSE;
}

uint8_t register_destroy(Device_TypeDef*dev)
{
	uint8_t i=0;

	for(i=0;i<MAX_DEVICES;i++)
	{
		if(g_DeviceTable[i]==dev)
		{
			g_DeviceTable[i]=NULL;	
			return TRUE;
			}
		}
	return FALSE;
}







