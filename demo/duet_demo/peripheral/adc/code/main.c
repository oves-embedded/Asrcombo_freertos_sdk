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

#include <string.h>
#include <stdio.h>
#include "duet_cm4.h"
#include "duet_uart.h"
#include "duet_pinmux.h"
#include "duet_rf_spi.h"
#include "duet_adc.h"
#include "printf_uart.h"

#define UART_TX_PIN PAD2
#define UART_RX_PIN PAD3
#define UART_TEST_INDEX UART1_INDEX
#define UART_TEST UART1

extern duet_uart_callback_func g_duet_uart_callback_handler[UART_NUM];

#define UART_TEST_INDEX UART1_INDEX
#define UART_HW_FLOW_CONTROL_ENABLE 1
#define UART_HW_FLOW_CONTROL_DISABLE 0

void HardFault_Handler(void)
{
  printf("hard fault\r\n");
  while (1);
}

uint8_t hw_spi_pta;

void uart_recv_test(char data)
{
  //UART_SendData(data);
  UART_SendData(getUartxViaIdx(UART_TEST_INDEX), data);
}

void set_uart_config(uint8_t uart_idx, uint8_t hw_flow_control)
{
  duet_uart_dev_t uart_config_struct = {0};
  duet_uart_struct_init(&uart_config_struct);
  uart_config_struct.port = uart_idx;
  uart_config_struct.priv = uart_recv_test;
  // set user define config
  uart_config_struct.config.baud_rate = UART_BAUDRATE_115200;
  if (hw_flow_control)
    uart_config_struct.config.flow_control = FLOW_CTRL_CTS_RTS;
  else
    uart_config_struct.config.flow_control = FLOW_CTRL_DISABLED;
  // make config take effect
  duet_uart_init(&uart_config_struct);
  //    duet_uart_set_callback(uart_idx,uart_recv_test);
}

void set_uart_pinmux(uint8_t uart_idx, uint8_t hw_flow_control)
{
  switch (uart_idx)
  {
    case UART0_INDEX:
      duet_pinmux_config(PAD0, PF_UART0);
      duet_pinmux_config(PAD1, PF_UART0);
      if (hw_flow_control)
      {
        duet_pinmux_config(PAD6, PF_UART0);
        duet_pinmux_config(PAD7, PF_UART0);
      }
      break;
    case UART1_INDEX:
      duet_pinmux_config(PAD2, PF_UART1);
      duet_pinmux_config(PAD3, PF_UART1);
      if (hw_flow_control)
      {
        duet_pinmux_config(PAD14, PF_UART1);
        duet_pinmux_config(PAD15, PF_UART1);
      }
      break;
    case UART2_INDEX:
      duet_pinmux_config(PAD12, PF_UART2);
      duet_pinmux_config(PAD13, PF_UART2);
      if (hw_flow_control)
      {
        duet_pinmux_config(PAD10, PF_UART2);
        duet_pinmux_config(PAD11, PF_UART2);
      }
      break;
    default:
      break;
  }
}

uint32_t auxdat[10];
bool Is_smpdon;
duet_adc_dev_t adc_config_struct;

void duet_auxadc_handler_dummy(void *arg)
{
  uint8_t cnt;
  duet_adc_dev_t duet_adc_struct;
  for (cnt = 0; cnt < 10; cnt++)
  {
    auxdat[cnt] = *(uint32_t *)(arg+4*cnt); //get register dat from handler buf
  }
  Is_smpdon = true;
  duet_adc_finalize(&duet_adc_struct);
}

int32_t duet_get_tmmt()
{
  duet_adc_dev_t adc_config_struct;
  adc_config_struct.config.sampling_cycle = 1;
  adc_config_struct.port                  = ADC_CHANNEL_TEMN;
  adc_config_struct.priv                  = (duet_adc_callback_func)NULL;
  duet_adc_init(&adc_config_struct);
  duet_tempr_get(&adc_config_struct);//get tmmt_n dat firt called   return 0
  adc_config_struct.port = ADC_CHANNEL_TEMP;
  duet_adc_init(&adc_config_struct);
  delay(1000);
  return duet_tempr_get(&adc_config_struct);    //second called return the temperature
}

int32_t duet_get_vol(duet_adc_dev_t *adc_config)
{
  int32_t datbuf[10];
  int32_t sum    = 0;
  uint8_t cnt, i = 0;
  if (adc_config->port >= 8)
    return 0;
  adc_config->config.sampling_cycle = 10;
  cnt = adc_config->config.sampling_cycle;
  adc_config->priv = (duet_adc_callback_func)NULL;
  duet_adc_init(adc_config);
  delay(100);
  while (cnt--)
  {
    datbuf[i] = duet_adc_get(adc_config);
    sum += datbuf[i];
    i++;
    delay(20);

  }
  return (sum / 10);
}

static void adc_preinit()
{
    rf_set_reg_bit(0x0f, 13, 2, 0x0);
    rf_set_reg_bit(0x06, 13, 1, 0x0);
    rf_set_reg_bit(0x06, 11, 1, 0x0);
    rf_set_reg_bit(0x07, 14, 1, 0x0);
    rf_set_reg_bit(0x07, 2, 2, 0x0);
}
int main(void)
{
  int32_t vol_val;
  uint32_t tmmt_p;
  uint32_t tmmt_n;
  int32_t tempra;
  uint8_t uart_idx        = UART_TEST_INDEX;
  uint8_t hw_flow_control = UART_HW_FLOW_CONTROL_DISABLE;
  // set pin mux
  set_uart_pinmux(uart_idx, hw_flow_control);
  set_uart_config(uart_idx, hw_flow_control);
  //    duet_uart_start(getUartxViaIdx(uart_idx));
  printf_uart_register(uart_idx);
  printf("duet ADC TEST!!\r\n");
    /*
    * before called duet_adc_init(duet_adc_dev_t *adc_config) if system called adc_preinit() block  adc_preinit()
    * if did not call  soc_pre_init() ,must call  adc_preinit()
    * */
  adc_preinit() ;
  adc_config_struct.port = ADC_CHANNEL_NUM4;
  vol_val = duet_get_vol(&adc_config_struct);
  printf("channel = %d input volage =%d mV\r\n", adc_config_struct.port, (int) vol_val);
  tempra = duet_get_tmmt();
  printf("temprature= %d C \r\n ", (int) tempra); // TMMT
  Is_smpdon = false;
  adc_config_struct.config.sampling_cycle = 10;
  adc_config_struct.port                  = ADC_CHANNEL_NUM4;
  adc_config_struct.priv                  = (duet_adc_callback_func) duet_auxadc_handler_dummy;
  duet_adc_init(&adc_config_struct);
  while (!Is_smpdon);
//  vol_val = 0.4243 * auxdat[5] + 6.9805;
  vol_val = 0.3907 * auxdat[5];
  printf("channel = %d input volage =%d mV\r\n", adc_config_struct.port, (int) vol_val);
  delay(200);
  Is_smpdon = false;
  adc_config_struct.port = ADC_CHANNEL_TEMP;
  duet_adc_init(&adc_config_struct);
  while (!Is_smpdon);
  tmmt_p= auxdat[5];
  printf(" tmmt_n data =0x%x \r\n", (int) tmmt_p);
  Is_smpdon = false;
  delay(200);
  adc_config_struct.port = ADC_CHANNEL_TEMN;
  duet_adc_init(&adc_config_struct);
  while (!Is_smpdon);
  tmmt_n= auxdat[5];
  printf(" tmmt_p data =0x%x \r\n", (int) tmmt_n);
    tempra=(tmmt_n - tmmt_p);
  tempra=(int32_t)((0.29 * (tempra))/ 5.25 + 41.5);
  printf("temprature= %d C\r\n ", (int) tempra); // TMMT
  Is_smpdon = false;
    printf("deut adc run finish!\r\n "); // TMMT
  while (1)
  {
    delay(250);
  }

}
