
#ifndef __OVAPP_H__
#define __OVAPP_H__

#define UART0_RX_SIZE 256
#define UART2_RX_SIZE 256

#define DI_LLZINVERTER_PROJECT  1


void ovapp_uart_init(void);
void ovapp_gpio_init(void);
void ovapp_init(void);
void ovapp_task(void *arg);

#endif


