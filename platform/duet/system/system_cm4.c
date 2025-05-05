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

#include "duet_cm4.h"
#include "core_cm4.h"
#include "systick_delay.h"
#ifdef CFG_DUET_FREERTOS
#include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "portable.h"
#endif

uint32_t system_bus_clk  = SYSTEM_BUS_CLOCK_INIT;
uint32_t system_core_clk = SYSTEM_CORE_CLOCK_INIT;
uint32_t system_peri_clk = SYSTEM_BUS_CLOCK_INIT;

/* note:core high freq mode modify Core_freq_config to SYSTEM_CORE_FREQ_120M */
#ifdef CFG_CORE_FREQ_120M
Core_freq_type Core_freq_config = SYSTEM_CORE_FREQ_120M;
#else
Core_freq_type Core_freq_config = SYSTEM_CORE_FREQ_52M;
#endif

#ifdef CFG_RESET_OPT
extern void lega_drv_open_dcdc_pfm(void);
extern void lega_reset_rw_rf(void);
extern void lega_clk_sel_xtal_or_apll(uint8_t is_xtal);
extern void lega_drv_move_rf_to_lo(void);
#endif
void duet_system_reset(void)
{
    //disable irq when reboot
    __disable_irq();

#ifdef HIGHFREQ_MCU160_SUPPORT
    if(system_core_clk == SYSTEM_CORE_CLOCK_HIGH)
        duet_clk_sel_low();
#endif

#ifdef CFG_RESET_OPT
    lega_clk_sel_xtal_or_apll(1);

    lega_drv_move_rf_to_lo();
    lega_drv_open_dcdc_pfm();
    lega_reset_rw_rf();
#endif

    NVIC_SystemReset();
}

#ifdef CFG_DUET_FREERTOS
extern void *itcm_heap_start;
extern void *itcm_heap_len;
extern void *dtcm_heap_start;
extern void *dtcm_heap_len;
extern void *sram_heap_start;
extern void *sram_heap_len;

HeapRegion_t xHeapRegions[] =
{
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 }                     // Terminates the array.
};

void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif

  xHeapRegions[ 0 ].pucStartAddress = (uint8_t*)&itcm_heap_start;
  xHeapRegions[ 0 ].xSizeInBytes = (size_t)&itcm_heap_len;
  xHeapRegions[ 1 ].pucStartAddress = (uint8_t*)&dtcm_heap_start;
  xHeapRegions[ 1 ].xSizeInBytes = (size_t)&dtcm_heap_len;
  xHeapRegions[ 2 ].pucStartAddress = (uint8_t*)&sram_heap_start;
  xHeapRegions[ 2 ].xSizeInBytes = (size_t)&sram_heap_len;
  vPortDefineHeapRegions(xHeapRegions);
}

#else //CFG_DUET_FREERTOS

void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif
}
#endif

void SystemCoreClockUpdate(void)
{

}

#define FLASH_COMMON2_SEG __attribute__((section("seg_wf_flash_driver")))
#pragma GCC push_options
#pragma GCC optimize("O0")
FLASH_COMMON2_SEG void udelay(unsigned int us)
{
    unsigned int us2cycles;
    if(Core_freq_config == SYSTEM_CORE_FREQ_120M)
    {
        us2cycles = us * (SYSTEM_CORE_CLOCK/1000000) / 9;
        us2cycles = (us2cycles >= 6) ? (us2cycles - 6) : 0;
        /*9 clock cycles*/
    }
    else
        us2cycles = (us*52)/4;

    while(us2cycles > 0)
    {
        us2cycles--;
        asm("nop");
    }
}

FLASH_COMMON2_SEG void delay(unsigned int cycles)
{
    udelay(cycles);
}
#pragma GCC pop_options

#define REG_PL_RD(addr)              (*(volatile uint32_t *)(addr))
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)
void SystemCoreClkUpdate(uint8_t is_xtal)
{
    if(Core_freq_config == SYSTEM_CORE_FREQ_120M)
    {
        if(is_xtal)
        {
            //set mcu, bus divider to  /2
            REG_PL_WR(WIFI_BLE_FLASH_CLK_CTRL_REG, (REG_PL_RD(WIFI_BLE_FLASH_CLK_CTRL_REG) & (~(0x1F<<2))));
            system_core_clk = system_bus_clk = SYSTEM_CORE_CLOCK_INIT/2;
        }
        else
        {
            REG_PL_WR(MCU_CLK_HARD_MODE_REG, 0x1);
            udelay(20);
            system_core_clk = SYSTEM_CORE_CLOCK_120M;
            system_bus_clk  = SYSTEM_BUS_CLOCK_60M;
            //0x11:cpu120M/bus60M
            REG_PL_WR(WIFI_BLE_FLASH_CLK_CTRL_REG, ((REG_PL_RD(WIFI_BLE_FLASH_CLK_CTRL_REG) & ~(0x1F<<2)) | 0x11<<2));
            udelay(50);
        }
#ifdef SYSTEM_SUPPORT_OS
        delay_init(system_core_clk/1000000);
#endif
    }
}

void BleClkUpdate(void)
{
    uint32_t reg_val = REG_PL_RD(APB_PERI_CLK_CTRL_REG);
    if(Core_freq_config == SYSTEM_CORE_FREQ_120M)
        REG_PL_WR(APB_PERI_CLK_CTRL_REG, ((reg_val & (~(0xFF<<4))) | (20<<4))); //BLE_MAC_CLK_NUM
    else
        REG_PL_WR(APB_PERI_CLK_CTRL_REG, ((reg_val & (~(0xFF<<4))) | (26<<4))); //BLE_MAC_CLK_NUM
}

/********END OF FILE ***********/
