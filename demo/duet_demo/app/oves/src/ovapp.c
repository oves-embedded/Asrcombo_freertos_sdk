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
#include "duet_gpio.h"
#include "duet_pinmux.h"
#include "duet_flash_kv.h"
#include "cloud.h"
#include "ovapp.h"

#include "payg.h"
#include "eeprom.h"
#include "i2c.h"
#include "ds1302.h"
#include "llzinverter.h"
#include "keyboard.h"


#define     OV_TASK_NAME              "ovapp_task"
#define     OV_TASK_PRIORITY          2
#define     OV_STACK_SIZE             2048
TaskHandle_t ovapp_task_Handler;



duet_uart_dev_t lega_di_uart;
duet_uart_dev_t lega_di_uart485;


#define UART0_TX_PIN PAD4
#define UART0_RX_PIN PAD5

#define UART2_TX_PIN PAD12
#define UART2_RX_PIN PAD13


duet_gpio_dev_t g_duet_gpio0, g_duet_gpio1;
duet_gpio_dev_t g_duet_gpio6_cs, g_duet_gpio7_clk,g_duet_gpio8_mosi,g_duet_gpio9_miso;
duet_gpio_dev_t g_duet_gpio15;

uint8_t uart0_buf[UART0_RX_SIZE]={0};
uint16_t uart0_index=0;
uint8_t uart2_buf[UART2_RX_SIZE]={0};
uint16_t uart2_index=0;

lega_wlan_init_type_t g_wifi_conf;
lega_semaphore_t g_ovapp_Semaphore = NULL;


extern _at_user_info at_user_info;
extern USER_SET_TypeDef g_UserSet;

void ovapp_gpio_init(void)
{

	    //gpio0 input
    g_duet_gpio0.port = GPIO0_INDEX;
    g_duet_gpio0.config = DUET_OUTPUT_OPEN_DRAIN_NO_PULL;
    g_duet_gpio0.priv = NULL;
    duet_gpio_init(&g_duet_gpio0);
   // duet_gpio_enable_irq(&g_duet_gpio0, DUET_IRQ_TRIGGER_FALLING_EDGE, duet_gpio0_irq_handler, NULL);

    //gpio1 output
    g_duet_gpio1.port = GPIO1_INDEX;
    g_duet_gpio1.config = DUET_OUTPUT_OPEN_DRAIN_NO_PULL;
    g_duet_gpio1.priv = NULL;
    duet_gpio_init(&g_duet_gpio1);

    //gpio6 output
    g_duet_gpio6_cs.port = GPIO6_INDEX;
    g_duet_gpio6_cs.config = DUET_OUTPUT_PUSH_PULL;
    g_duet_gpio6_cs.priv = NULL;
    duet_gpio_init(&g_duet_gpio6_cs);

	    //gpio7 output
    g_duet_gpio7_clk.port = GPIO7_INDEX;
    g_duet_gpio7_clk.config = DUET_OUTPUT_PUSH_PULL;
    g_duet_gpio7_clk.priv = NULL;
    duet_gpio_init(&g_duet_gpio7_clk);

	    //gpio8 output
    g_duet_gpio8_mosi.port = GPIO8_INDEX;
    g_duet_gpio8_mosi.config = DUET_OUTPUT_OPEN_DRAIN_NO_PULL;
    g_duet_gpio8_mosi.priv = NULL;
    duet_gpio_init(&g_duet_gpio8_mosi);

	    //gpio9 output
    g_duet_gpio9_miso.port = GPIO9_INDEX;
    g_duet_gpio9_miso.config = DUET_INPUT_HIGH_IMPEDANCE;
    g_duet_gpio9_miso.priv = NULL;
    duet_gpio_init(&g_duet_gpio9_miso);

	//gpio7 output
    g_duet_gpio15.port = GPIO15_INDEX;
    g_duet_gpio15.config = DUET_OUTPUT_PUSH_PULL;
    g_duet_gpio15.priv = NULL;
    duet_gpio_init(&g_duet_gpio15);
}

/*
 ************************************************************
 *           DI task and cmd process start
 *
 ************************************************************
 */
void di_handle_uartirq(char ch)
{
    uart0_buf[uart0_index++] = ch;
    if(uart0_index >=UART0_RX_SIZE)
    {
        uart0_index=0;
    }
}

void di_handle_uart485irq(char ch)
{
    uart2_buf[uart2_index++] = ch;
    if(uart2_index >=UART2_RX_SIZE)
    {
        uart2_index=0;
    }
	//log_i(" uart irq :%x \n",ch);
}


void ovapp_uart_init(void)
{
   // int32_t len = 0;
	
    memset(&lega_di_uart,0,sizeof(lega_di_uart));
	memset(&lega_di_uart485,0,sizeof(lega_di_uart485));
    //len = sizeof(lega_di_uart);

    lega_di_uart.config.baud_rate=UART_BAUDRATE_115200;
    lega_di_uart.config.data_width = DATA_8BIT;
    lega_di_uart.config.flow_control = FLOW_CTRL_DISABLED;
    lega_di_uart.config.parity = PARITY_NO;
    lega_di_uart.config.stop_bits = STOP_1BIT;
    lega_di_uart.config.mode = TX_RX_MODE;
	

    lega_di_uart.port=DUET_UART0_INDEX;
    duet_pinmux_config(UART0_TX_PIN,PF_UART0);
    duet_pinmux_config(UART0_RX_PIN,PF_UART0);
    //register uart callback func for receiving at command
    lega_di_uart.config.baud_rate=UART_BAUDRATE_38400;
    lega_di_uart.priv = (void *)(di_handle_uartirq);
    duet_uart_init(&lega_di_uart);

	memcpy((uint8_t*)&lega_di_uart485,(uint8_t*)&lega_di_uart,sizeof(lega_di_uart));
	lega_di_uart485.port=DUET_UART2_INDEX;
	lega_di_uart485.config.baud_rate=UART_BAUDRATE_9600;
    duet_pinmux_config(UART2_TX_PIN,PF_UART2);
    duet_pinmux_config(UART2_RX_PIN,PF_UART2);
	
	lega_di_uart485.priv = (void *)(di_handle_uart485irq);
    duet_uart_init(&lega_di_uart485);
    //printf_uart_init(&lega_at_uart);

	printf("di_uart(0,2)_init...\n");
}

void ovapp_init(void)
{
	memset(&g_wifi_conf,0,sizeof(lega_wlan_init_type_t));
    g_wifi_conf.wifi_mode = STA;
	g_wifi_conf.dhcp_mode = WLAN_DHCP_CLIENT;
	memcpy(g_wifi_conf.wifi_ssid,g_UserSet.wifi_ssid,strlen(g_UserSet.wifi_ssid));
	memcpy(g_wifi_conf.wifi_key,g_UserSet.wifi_password,strlen(g_UserSet.wifi_password));

	printf("EEP ssid:%s  key:%s \r\n",g_UserSet.wifi_ssid,g_UserSet.wifi_password);
	printf("ssid:%s  key:%s \r\n",g_wifi_conf.wifi_ssid,g_wifi_conf.wifi_key);

	lega_wlan_open(&g_wifi_conf);

	xTaskCreate(ovapp_task,OV_TASK_NAME,OV_STACK_SIZE>>2,0,OV_TASK_PRIORITY,&ovapp_task_Handler);

}

void ovapp_task(void *arg)
{
    int timestamp = 1000;
	//int32_t len=sizeof(g_UserSet);

	 lega_rtos_init_semaphore(&g_ovapp_Semaphore, 0);
	 lega_rtos_set_semaphore(&g_ovapp_Semaphore);

	//ovapp_uart_init();
	//ovapp_gpio_init();
	KeyBoardInit();
	PaygInit();

	//InitDs1302();
	//EEpInit();
	LLZInit();

    while(1)
    {
        printf("ovapp task... %d \n" ,at_user_info.sta_connected);
		lega_rtos_get_semaphore(&g_ovapp_Semaphore, 20000);

		//Show_Time();
		PaygUpdateEnable();
		PaygProcess();
		PKeybordProc();
		LLZTask();
		EEpProcess();

		if(Mqtt_broker_Cloud_Task_Handler==NULL&&at_user_info.sta_connected ==1)
		{	
			lega_rtos_create_thread(&Mqtt_broker_Cloud_Task_Handler, MQTT_BROKER_CLOUD_TASK_PRIORITY, MQTT_BROKER_CLOUD_TASK_NAME, mqtt_broker_cloud_task, MQTT_BROKER_CLOUD_STACK_SIZE, 0);
			}

		if(mqtt_broker_Semaphore!=NULL)
		{	
			lega_rtos_set_semaphore(&mqtt_broker_Semaphore);
			}

		lega_rtos_set_semaphore(&g_ovapp_Semaphore);

        lega_rtos_delay_milliseconds(timestamp);
#ifdef CLOUD_TEST
        cloud_test();
#endif
    }
}



