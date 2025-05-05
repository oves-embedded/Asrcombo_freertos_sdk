/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _UART_LITE_H_
#define _UART_LITE_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "duet.h"

#define MAX_UART_BUF                (256)

/*UART PINMUX REGISTER ADDRESS*/
#define UART0_PIN_MUX_REG           (PINMUX_CTRL_REG0)
#define UART1_PIN_MUX_REG           (PINMUX_CTRL_REG0)
#define UART2_PIN_MUX_REG           (PINMUX_CTRL_REG1)
#define UART3_PIN_MUX_REG           (PINMUX_CTRL_REG1)

/* UART PINMUX setting, UART0/1/2/3 fixed at GP2/3 & GP6/7 & GP8/8 & GP13/14 */
#define UART_FUNC                   (0x1)

#define UART0_GPIO_MASK             (0xFF << 8)
#define UART1_GPIO_MASK             (0xFF << 16)
#define UART2_GPIO_MASK             (0xFF)
#define UART3_GPIO_MASK             (0xFF << 20)

#define UART0_TX_PIN_GP2            (UART_FUNC << 8)
#define UART0_RX_PIN_GP3            (UART_FUNC << 12)
#define UART1_TX_PIN_GP4            (UART_FUNC << 16)
#define UART1_RX_PIN_GP5            (UART_FUNC << 20)
#define UART2_TX_PIN_GP8            (UART_FUNC)
#define UART2_RX_PIN_GP9            (UART_FUNC << 4)
#define UART3_TX_PIN_GP13           (UART_FUNC << 20)
#define UART3_RX_PIN_GP14           (UART_FUNC << 24)
#define UART1_TX_PIN_GP11           (UART_FUNC << 12)
#define UART1_RX_PIN_GP12           (UART_FUNC << 16)


/* UART BAUDRATE*/
#define UART_BAUDRATE_110                       (110)
#define UART_BAUDRATE_300                       (300)
#define UART_BAUDRATE_600                       (600)
#define UART_BAUDRATE_1200                      (1200)
#define UART_BAUDRATE_2400                      (2400)
#define UART_BAUDRATE_4800                      (4800)
#define UART_BAUDRATE_9600                      (9600)
#define UART_BAUDRATE_14400                     (14400)
#define UART_BAUDRATE_19200                     (19200)
#define UART_BAUDRATE_38400                     (38400)
#define UART_BAUDRATE_57600                     (57600)
#define UART_BAUDRATE_115200                    (115200)
#define UART_BAUDRATE_230400                    (230400)
#define UART_BAUDRATE_460800                    (460800)
#define UART_BAUDRATE_921600                    (921600)


/* UART interrutps */
#define UART_RX_INTERRUPT                (1<<4)
#define UART_TX_INTERRUPT                (1<<5)
#define UART_RX_TIMEOUT_INTERRUPT        (1<<6)


/* UART flags */
#define UART_FLAG_TX_FIFO_EMPTY          (1<<7)
#define UART_FLAG_RX_FIFO_FULL           (1<<6)
#define UART_FLAG_TX_FIFO_FULL           (1<<5)
#define UART_FLAG_RX_FIFO_EMPTY          (1<<4)
#define UART_FLAG_BUSY                   (1<<3)

#define UART_DATA_WIDTH_5_BITS           (0 << 5)
#define UART_DATA_WIDTH_6_BITS           (1 << 5)
#define UART_DATA_WIDTH_7_BITS           (2 << 5)
#define UART_DATA_WIDTH_8_BITS           (3 << 5)

#define UART_FIFO_MODE_EN_POS            (1 << 4)
#define UART_ENABLE_POS                  (1 << 0)


#define UART_PARITY_CHECK_EN             (1 << 1)

#define UART_PARITY_SET_POS              (1 << 2)
#define UART_STOP_BIT_POS                (1 << 3)


#define UART_TX_RX_MODE_EN               (3 << 8)

#define UART1_PAD4_5        0
#define UART1_PAD11_12      1
#define UART1_PAD_SEL       UART1_PAD4_5

#define PLF_UART1           1
#define PLF_UART2           0

#if (PLF_UART1)
#define UART_INDEX          UART1
#endif

#if (PLF_UART2)
#define UART_INDEX          UART2
#endif

void uart_init(UART_TypeDef* UARTx, int baudrate);
void uart_senddata(UART_TypeDef* UARTx, uint8_t Data);
void printstr(char *str);
void print32(uint32_t data);

#ifdef __cplusplus
}
#endif
#endif // _UART_LITE_H_

