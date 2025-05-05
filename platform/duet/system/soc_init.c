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

#include <stdio.h>
#include "duet_cm4.h"
#include "soc_init.h"
#include "duet_wdg.h"
#include "duet_boot.h"
#include "duet_flash.h"
#include "duet_common.h"
#include "systick_delay.h"
#include "lega_wlan_api.h"
#include "duet_flash_alg.h"
#include "FreeRTOSConfig.h"
#include "mpu.h"

#include "pmu.h"

OTA_MAX_SIZE_SEG const uint32_t ota_max_size = OTA_MAX_SIZE;

#ifdef _CFG_SECUREBOOT_
#define APP_SECURBOOT_CERT_AREA_TOKEN "SECURBOOT_CERT_TOKEN"
APP_CERT_SEG const char app_cert_area[] = APP_SECURBOOT_CERT_AREA_TOKEN;
#endif

extern pmu_state_t current_state;
extern void *_vtable_start;

#define REG_AHB_BUS_CTRL   *((volatile uint32_t *)(ALWAYS_ON_REGFILE + 0x90))

/***********************************************************
* redirect vtable to _vtable_start in ram
* wrap all the XXX_Handler for flash suspend/resume
**********************************************************/
extern void Default_Handler();
extern void Reset_Handler();
extern void NMI_Handler();
extern void HardFault_Handler();
extern void MemManage_Handler();
extern void BusFault_Handler();
extern void UsageFault_Handler();
extern void SVC_Handler();
extern void DebugMon_Handler();
extern void PendSV_Handler();
extern void SysTick_Handler();
extern void intc_irq();
extern void SLEEP_IRQHandler();
extern void WDG_IRQHandler();
extern void FLASH_IRQHandler();
extern void GPIO_IRQHandler();
extern void TIMER_IRQHandler();
extern void CRYPTOCELL310_IRQHandler();
extern void DMA_IRQHandler();
extern void UART0_IRQHandler();
extern void UART1_IRQHandler();
extern void UART2_IRQHandler();
extern void SPI0_IRQHandler();
extern void SPI1_IRQHandler();
extern void SPI2_IRQHandler();
extern void I2C0_IRQHandler();
extern void I2C1_IRQHandler();
extern void SDIO_IRQHandler();
extern void D_APLL_UNLOCK_IRQHandler();
extern void D_SX_UNLOCK_IRQHandler();
extern void AUX_ADC_IRQHandler();
extern void PLATFORM_WAKEUP_IRQHandler();
extern void I2S_IRQHandler();
extern void BLE_IRQHandler();

#define IRQ_HANDLER_WRAPPER(irq_handler) (uint32_t)irq_handler##_Wrapper

#define IRQ_HANDLER_WRAPPER_FUN(irq_handler) \
    FLASH_DRIVER_SEG void irq_handler##_Wrapper() \
    { \
        duet_flash_alg_suspend(); \
        irq_handler(); \
        duet_flash_alg_resume(); \
    }

IRQ_HANDLER_WRAPPER_FUN(Default_Handler)
IRQ_HANDLER_WRAPPER_FUN(Reset_Handler)
//IRQ_HANDLER_WRAPPER_FUN(NMI_Handler)
//IRQ_HANDLER_WRAPPER_FUN(HardFault_Handler)
//IRQ_HANDLER_WRAPPER_FUN(MemManage_Handler)
//IRQ_HANDLER_WRAPPER_FUN(BusFault_Handler)
//IRQ_HANDLER_WRAPPER_FUN(UsageFault_Handler)
IRQ_HANDLER_WRAPPER_FUN(SVC_Handler)
//IRQ_HANDLER_WRAPPER_FUN(DebugMon_Handler)
//IRQ_HANDLER_WRAPPER_FUN(PendSV_Handler)
IRQ_HANDLER_WRAPPER_FUN(SysTick_Handler)
IRQ_HANDLER_WRAPPER_FUN(intc_irq)
IRQ_HANDLER_WRAPPER_FUN(SLEEP_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(WDG_IRQHandler)
//IRQ_HANDLER_WRAPPER_FUN(FLASH_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(GPIO_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(TIMER_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(CRYPTOCELL310_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(DMA_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(UART0_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(UART1_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(UART2_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(SPI0_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(SPI1_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(SPI2_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(I2C0_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(I2C1_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(SDIO_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(D_APLL_UNLOCK_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(D_SX_UNLOCK_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(AUX_ADC_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(PLATFORM_WAKEUP_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(I2S_IRQHandler)
IRQ_HANDLER_WRAPPER_FUN(BLE_IRQHandler)

FLASH_DRIVER_SEG __attribute__ (( naked )) void HardFault_Handler_Wrapper()
{
    __asm volatile
    (
    "    tst     lr, #4                            \n"
    "    ite     eq                                \n"
    "    mrseq   r0,msp                            \n"
    "    mrsne   r0,psp                            \n"
    "    mov     r1,lr                             \n"
    "    push    {r4-r11}                          \n"
    "    mov     r2, sp                            \n"
    "    push    {r0, r1, r2}                      \n"
    "    bl      duet_flash_alg_suspend            \n"
    "    pop     {r0, r1, r2}                      \n"
    "    bl      HardFault_Handler_C               \n"
    "    pop     {r4-r11}                          \n"
    "    ldr     r2,=duet_flash_alg_resume         \n"
    "    bx      r2                                \n"
    );
}

FLASH_DRIVER_SEG __attribute__ (( naked )) void MemManage_Handler_Wrapper()
{
    __asm volatile
    (
    "    tst     lr, #4                            \n"
    "    ite     eq                                \n"
    "    mrseq   r0,msp                            \n"
    "    mrsne   r0,psp                            \n"
    "    mov     r1,lr                             \n"
    "    push    {r4-r11}                          \n"
    "    mov     r2, sp                            \n"
    "    push    {r0, r1, r2}                      \n"
    "    bl      duet_flash_alg_suspend            \n"
    "    pop     {r0, r1, r2}                      \n"
    "    bl      MemManage_Handler_C               \n"
    "    pop     {r4-r11}                          \n"
    "    ldr     r2,=duet_flash_alg_resume         \n"
    "    bx      r2                                \n"
    );
}

FLASH_DRIVER_SEG __attribute__ (( naked )) void PendSV_Handler_Wrapper()
{
    __asm volatile (
                    "    ldr r3, gFlashWipConst         \n"
                    "    ldr r3, [r3]                   \n"
                    "    cmp r3, #0x0                   \n"
                    "    beq PendSV_Handler             \n"
                    "    bx r14                         \n"
                    "gFlashWipConst: .word g_flash_wip  \n"
                );
}

void redirect_vtable()
{
    uint32_t *vtable = (uint32_t *)&_vtable_start;

    for(int i = 0; i < 48; i++)
        vtable[i] = IRQ_HANDLER_WRAPPER(Default_Handler);

    vtable[0] = *((uint32_t *)SCB->VTOR);
    vtable[1] = IRQ_HANDLER_WRAPPER(Reset_Handler);
//  vtable[2] = IRQ_HANDLER_WRAPPER(NMI_Handler);
    vtable[3] = IRQ_HANDLER_WRAPPER(HardFault_Handler);
    vtable[4] = IRQ_HANDLER_WRAPPER(MemManage_Handler);
//  vtable[5] = IRQ_HANDLER_WRAPPER(BusFault_Handler);
//  vtable[6] = IRQ_HANDLER_WRAPPER(UsageFault_Handler);
//  vtable[7] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[8] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[9] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[10] = IRQ_HANDLER_WRAPPER(Default_Handler);
    vtable[11] = IRQ_HANDLER_WRAPPER(SVC_Handler);
//  vtable[12] = IRQ_HANDLER_WRAPPER(DebugMon_Handler);
//  vtable[13] = IRQ_HANDLER_WRAPPER(Default_Handler);
    vtable[14] = IRQ_HANDLER_WRAPPER(PendSV_Handler);
    vtable[15] = IRQ_HANDLER_WRAPPER(SysTick_Handler);
    vtable[16] = IRQ_HANDLER_WRAPPER(intc_irq);
    vtable[17] = IRQ_HANDLER_WRAPPER(SLEEP_IRQHandler);
    vtable[18] = IRQ_HANDLER_WRAPPER(WDG_IRQHandler);
//  vtable[19] = IRQ_HANDLER_WRAPPER(FLASH_IRQHandler);
    vtable[20] = IRQ_HANDLER_WRAPPER(GPIO_IRQHandler);
    vtable[21] = IRQ_HANDLER_WRAPPER(TIMER_IRQHandler);
    vtable[22] = IRQ_HANDLER_WRAPPER(CRYPTOCELL310_IRQHandler);
    vtable[23] = IRQ_HANDLER_WRAPPER(DMA_IRQHandler);
    vtable[24] = IRQ_HANDLER_WRAPPER(UART0_IRQHandler);
    vtable[25] = IRQ_HANDLER_WRAPPER(UART1_IRQHandler);
    vtable[26] = IRQ_HANDLER_WRAPPER(UART2_IRQHandler);
    vtable[27] = IRQ_HANDLER_WRAPPER(SPI0_IRQHandler);
    vtable[28] = IRQ_HANDLER_WRAPPER(SPI1_IRQHandler);
    vtable[29] = IRQ_HANDLER_WRAPPER(SPI2_IRQHandler);
    vtable[30] = IRQ_HANDLER_WRAPPER(I2C0_IRQHandler);
    vtable[31] = IRQ_HANDLER_WRAPPER(I2C1_IRQHandler);
    vtable[32] = IRQ_HANDLER_WRAPPER(SDIO_IRQHandler);
    vtable[33] = IRQ_HANDLER_WRAPPER(D_APLL_UNLOCK_IRQHandler);
    vtable[34] = IRQ_HANDLER_WRAPPER(D_SX_UNLOCK_IRQHandler);
//  vtable[35] = IRQ_HANDLER_WRAPPER(Default_Handler);
    vtable[36] = IRQ_HANDLER_WRAPPER(AUX_ADC_IRQHandler);
//  vtable[37] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[38] = IRQ_HANDLER_WRAPPER(Default_Handler);
    vtable[39] = IRQ_HANDLER_WRAPPER(PLATFORM_WAKEUP_IRQHandler);
    vtable[40] = IRQ_HANDLER_WRAPPER(I2S_IRQHandler);
    vtable[41] = IRQ_HANDLER_WRAPPER(BLE_IRQHandler);
//  vtable[42] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[43] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[44] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[45] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[46] = IRQ_HANDLER_WRAPPER(Default_Handler);
//  vtable[47] = IRQ_HANDLER_WRAPPER(Default_Handler);

    __DMB();
    SCB->VTOR = (uint32_t)&_vtable_start;
    __DSB();

}

/***********************************************************
* set IRQ priority and enable IRQ it later
*
**********************************************************/
void NVIC_init()
{
    //set irq priority, default set configLIBRARY_NORMAL_INTERRUPT_PRIORITY
    NVIC_SetPriority(UART0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(UART1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(UART2_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(ASRW_WI_IP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(D_APLL_UNLOCK_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(D_SX_UNLOCK_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SLEEP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(WDG_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(FLASH_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(GPIO_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(TIMER_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CRYPTOCELL310_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(DMA_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI2_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(I2C0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(I2C1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SDIO_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(PLF_WAKEUP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(RW_BLE_IRQn, configLIBRARY_NORMAL_INTERRUPT_PRIORITY-1);
}

/***********************************************************
* watchdog init config
*
**********************************************************/
duet_wdg_dev_t duet_wdg;
void wdg_init(void)
{
    duet_wdg.port = 0;
    duet_wdg.config.timeout = WDG_TIMEOUT_MS;
    duet_wdg_init(&duet_wdg);
}


/***********************************************************
*  init device: flash config,irq priority,rco and systick
*
**********************************************************/
void lega_devInit()
{
    //default close watchdog for step debug on keil
    //wdg_init();

    RTC_REG_RCO32K |= (1<<(5+16)) | (1<<(7+16)) | (1<<(8+16));

    NVIC_init();

    lega_drv_rco_cal();

    //Config SysTick and init delay config with api of delay_us & delay_ms
    delay_init(SYSTEM_CORE_CLOCK/1000000);
}

/**********************************************************
*  use soc_init to init board
*
**********************************************************/
int soc_init(void)
{
    lega_devInit();
    return 0;
}

void HCLK_SW_IRQHandler(void)
{
    SYS_CRM_CLR_HCLK_REC = 0x1;
}

void delay_nop(unsigned int dly)
{
    volatile unsigned int i;
    for(i=dly; i>0; i--)
    {
    }
}
void soc_delay_us(uint32_t nus)
{
    delay_us(nus);
}
void ahb_sync_brid_open(void)
{
    unsigned int is_using_sync_down = (REG_AHB_BUS_CTRL & (0x1<<1));
    if(!is_using_sync_down || duet_get_boot_type() == DEEP_SLEEP_RST)
    {
        REG_AHB_BUS_CTRL |= (0x1<<1); //0x40000A90 bit1 sw_use_hsync

        __enable_irq();
        NVIC_EnableIRQ(24);
        __asm volatile("DSB");
        __asm volatile("WFI");
        __asm volatile("ISB");

        delay_nop(50);
    }
}

void enable_sleep_irq_after_deepsleep(void)
{
    if (duet_get_boot_type() == DEEP_SLEEP_RST)
    {
        // In this case means wakeup from deep sleep and need process RTC interrupt
        current_state = PMU_STATE_DEEPSLEEP;
        NVIC_ClearPendingIRQ(SLEEP_IRQn);
        NVIC_EnableIRQ(SLEEP_IRQn);
    }
}

extern void lega_reset_rw_rf(void);
extern void lega_soc_wifi_ble_clk_disable(void);
extern void lega_enable_all_soc_interrupt(void);
extern uint8_t lega_get_chip_type(void);

/*************************************************************
*  soc init config, don't run any code before it
*
**************************************************************/
int soc_pre_init(void)
{
    SCB_DisableICache();
    SCB_DisableDCache();

    redirect_vtable();

    lega_get_chip_type();
#ifdef _SPI_FLASH_ENABLE_
    duet_flash_init();
#endif

    mpu_deinit();

    mpu_init();

    SCB_EnableICache();
    SCB_EnableDCache();

    // enable sleep irq here to clear interrupt status after deepsleep reset
    enable_sleep_irq_after_deepsleep();

    //without bootload write LDO to 3.3V, otherwise TXPWR/EVM is bad
    //REG_WR(0x40000a58,0x5FFFF);

    lega_drv_open_dcdc_pfm();
    lega_reset_rw_rf();
    //turn off all wifi/ble clock
    lega_soc_wifi_ble_clk_disable();

    //enable all soc interrupt
    lega_enable_all_soc_interrupt();

    return 0;
}

/*************************************************************
*  whether bypass 5V to 3.3V LDO when deep sleep
*
**************************************************************/
#define REG_DCDC_RSV_L_CFG   *((volatile uint32_t *)(ALWAYS_ON_REGFILE + 0x58))

void lega_voltage_bypass_vcc33_ldo_config(void)
{
#ifdef VCC3_3_BYPASS_DCDC
    REG_DCDC_RSV_L_CFG |= (1 << 19);
#endif
}

