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


/*************************   **************************************/
#ifndef __DUET_CM4_H__
#define __DUET_CM4_H__

#ifdef __cplusplus
 extern "C" {
#endif

#define __CM0_REV                 0 /*!< Core Revision r0p0                            */
#define __MPU_PRESENT             1 /*!< MPU present or not                                                        */
#define __ICACHE_PRESENT          1 /*!< instruction cache present                 */
#define __DCACHE_PRESENT          1 /*!< data cache present                        */
#define __NVIC_PRIO_BITS          3 /*!< duetwifi uses 3 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used  */
//hightest interrupt priority is configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY&((1<<__NVIC_PRIO_BITS)-1)=5
//lowest interrupt priority is configLIBRARY_LOWEST_INTERRUPT_PRIORITY&((1<<__NVIC_PRIO_BITS)-1)=7
//set normal interrupt priority 6
#define configLIBRARY_NORMAL_INTERRUPT_PRIORITY 6

typedef enum IRQn
{
/**************   Processor Exceptions Numbers ******************************************/
  NonMaskableInt_IRQn           = -14,      /*!< 2 Non Maskable Interrupt                         */
  MemoryManagement_IRQn         = -12,      /*!< 4 Cortex-M3 Memory Management Interrupt          */
  BusFault_IRQn                 = -11,      /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
  UsageFault_IRQn               = -10,      /*!< 6 Cortex-M3 Usage Fault Interrupt                */
  SVCall_IRQn                   = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                   */
  DebugMonitor_IRQn             = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
  PendSV_IRQn                   = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                   */
  SysTick_IRQn                  = -1,       /*!< 15 Cortex-M3 System Tick Interrupt               */
/******   Interrupt Numbers *******************************************************/
  ASRW_WI_IP_IRQn               = 0,        /*!< ASRW RW IP Interrupt                             */
  SLEEP_IRQn                    = 1,        /*!< Sleep Wake-Up Interrupt                          */
  WDG_IRQn                      = 2,        /*!< Window WatchDog                                  */
  FLASH_IRQn                    = 3,        /*!< FLASH Interrupt                                  */
  GPIO_IRQn                     = 4,        /*!< GPIO Interrupt                                   */
  TIMER_IRQn                    = 5,        /*!< Timer Interrupt                                  */
  CRYPTOCELL310_IRQn            = 6,        /*!< CryptoCell 310 Interrupt                         */
  DMA_IRQn                      = 7,        /*!< Generic DMA Ctrl Interrupt                       */
  UART0_IRQn                    = 8,        /*!< UART0 Interrupt                                  */
  UART1_IRQn                    = 9,        /*!< UART1 Interrupt                                  */
  UART2_IRQn                    = 10,       /*!< UART2 Interrupt                                  */
  SPI0_IRQn                     = 11,       /*!< SPI0 Interrupt                                   */
  SPI1_IRQn                     = 12,       /*!< SPI1 Interrupt                                   */
  SPI2_IRQn                     = 13,       /*!< SPI2                                             */
  I2C0_IRQn                     = 14,       /*!< I2C0 Interrupt                                   */
  I2C1_IRQn                     = 15,       /*!< I2C1 Interrupt                                   */
  SDIO_IRQn                     = 16,       /*!< SDIO Combined Interrupt                          */
  D_APLL_UNLOCK_IRQn            = 17,       /*!< RF added: D_APLL_UNLOCK Interrupt                */
  D_SX_UNLOCK_IRQn              = 18,       /*!< RF added: D_SX_UNLOCK Interrupt                  */
  AUX_ADC_IRQn                  = 20,       /*!< ADC Interrupt                                    */
  PLF_WAKEUP_IRQn               = 23,       /*!< WiFi Platform Wake-Up Interrupt                  */
  I2S_IRQn                      = 24,       /*!< I2S Interrupt                                    */
  RW_BLE_IRQn                   = 25,       /*!< BLE Interrupt                                    */
} IRQn_Type;


#ifdef DUET_CM4
#include "core_cm4.h"
#endif

#include <stdint.h>

// ---------------------------------------------------------------------------
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITstatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

typedef enum
{
    SYSTEM_CORE_FREQ_52M  = 0,
    SYSTEM_CORE_FREQ_120M = 1,
} Core_freq_type;

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
#define XTAL_26M               26000000

extern uint32_t system_bus_clk;
extern uint32_t system_core_clk;
extern uint32_t system_peri_clk;

#define SYSTEM_CORE_CLOCK      system_core_clk
#define SYSTEM_CLOCK           system_bus_clk

#define SYSTEM_CORE_CLOCK_INIT  (52000000)
#define SYSTEM_BUS_CLOCK_INIT   (52000000)

#define SYSTEM_CORE_CLOCK_160M  (160000000)
#define SYSTEM_BUS_CLOCK_80M    (80000000)

#define SYSTEM_CORE_CLOCK_120M  (120000000)
#define SYSTEM_BUS_CLOCK_60M    (60000000)

#define SYSTEM_CLOCK_NORMAL    (80000000)

#define HAPS_FPGA 0
#define V7_FPGA 1
#define FPGA_PLATFORM V7_FPGA

#define PINMUX_CTRL_REG0                0x40000004 //pad0-7
#define PINMUX_CTRL_REG1                0x40000008 //pad8-15
#define PINMUX_CTRL_REG2                0x4000000C //pad16-23
#define PINMUX_CTRL_REG3                0x40000010 //pad24-31

#define SYS_REG_BASE                    0x40000000
#define SYS_REG_BASE_FLASH_CLK          ((SYS_REG_BASE + 0x808))
#define PERI_CLK_CFG                    (SYS_REG_BASE + 0x850)

#define WIFI_BLE_FLASH_CLK_CTRL_REG     (0x40000804)
#define APB_PERI_CLK_CTRL_REG           (0x40000808)
#define MCU_CLK_HARD_MODE_REG           (0x40000814)
#define ADC_SDIO_BLE_DEBUG_CTRL_REG     (0x40000908)

#define SYS_REG_BASE_CLK1_ENABLE        (SYS_REG_BASE + 0x840)
#define CC310_CLOCK_ENABLE              (1 << 11)

#define SYS_REG_BASE_CLK1_DISABLE       (SYS_REG_BASE + 0x848)
#define CC310_CLOCK_DISABLE             (1 << 11)

#define REG_INTERRUPT_ENABLE            (SYS_REG_BASE + 0x944)
#define REG_INTERRUPT_DISABLE           (SYS_REG_BASE + 0x948)

#define SYS_CRM_SYS_CLK_CTRL1           *((volatile uint32_t *)(SYS_REG_BASE + 0x950))
#define SYS_CRM_REG_0x804               *((volatile uint32_t *)(SYS_REG_BASE + 0x804))
#define SYS_CRM_UART2_FRAC_DIV          *((volatile uint32_t *)(SYS_REG_BASE + 0x82C))

#define ALWAYS_ON_REGFILE           0x40000A00
#define REG_AHB_BUS_CTRL            *((volatile uint32_t *)(ALWAYS_ON_REGFILE + 0x90))

#define REG_PLF_WAKEUP_INT_EN       (0x1<<23)
#define SDIO_HCLK_EN                    (1 << 4)
#define PWM_CLK_EN                      (1 << 2)
#define WDG_CLK_EN                      (1 << 1)
#define TIMER_SCLK_EN                   0x1

#define SYS_CRM_WIFI_BLK_CLK        *((volatile uint32_t *)(SYS_REG_BASE + 0x85C))
#define MDM_CLKGATEFCTRL0_ADDR      0x60C00874
#define MDM_CLKGATEFCTRL0           *((volatile uint32_t *)(MDM_CLKGATEFCTRL0_ADDR))
#define RTC_REG_RCO32K_ADDR         0x40000A44
#define RTC_REG_RCO32K              *((volatile uint32_t *)(RTC_REG_RCO32K_ADDR))
#define SYS_CRM_CLR_HCLK_REC        *((volatile uint32_t *)(SYS_REG_BASE + 0x844))

#define TRX_PD_CTRL1_REG_ADDR       0x06
#define TRX_PD_CTRL2_REG_ADDR       0x07
#define APLL_PD_CTRL_REG_ADDR       0x0D
#define APLL_RST_CTRL_REG_ADDR      0x0E
#define XO_PD_CTRL_REG_ADDR         0x0F
#define APLL_CLK_PHY_REG_ADDR       0x6B
#define APLL_FCAL_FSM_CTRL_ADDR     0x6E

//efuse memory
typedef struct
{
    uint8_t tx_params[5];           //0x8B-0x8F
    uint8_t mac_addr0[6];           //0x90-0x95
    uint8_t freq_err;               //0x96
    uint8_t tmmt1;                  //0x97
    uint8_t tmmt2;                  //0x98
    uint8_t cus_tx_pwr[19];         //0x99-0xab
    uint8_t cal_tx_pwr0[6];         //0xac-0xb1
    uint8_t cus_tx_total_pwr[3];    //0xb2-0xb4
    uint8_t cal_tx_evm0[6];         //0xb5-0xba
    uint8_t ble_tx_pwr0[3];         //0xbb-0xbd
    uint8_t reserved1[2];           //0xbe-0xbf
    uint8_t mac_addr1[6];           //0xc0-0xc5
    uint8_t mac_addr2[6];           //0xc6-0xcb
    uint8_t cal_tx_pwr1[6];         //0xcc-0xd1
    uint8_t cal_tx_evm1[6];         //0xd2-0xd7
    uint8_t cal_tx_pwr2[6];         //0xd8-0xdd
    uint8_t cal_tx_evm2[6];         //0xde-0xe3
    uint8_t ble_tx_pwr1[3];         //0xe4-0xe6
    uint8_t ble_tx_pwr2[3];         //0xe7-0xe9
    uint8_t mac0_first;             //0xea
    uint8_t mac1_first;             //0xeb
    uint8_t mac2_first;             //0xec
    uint8_t reserved2[3];           //0xed-0xef
}efuse_info_t;
#define EFUSE_INFO_START_ADDR       0x8B
#define EFUSE_TMMT_PN               0x97
#define EFUSE_TMMT_PN_LEN           2
#define EFUSE_INFO_LEN              (sizeof(efuse_info_t))
#define EFUSE_INFO_CHIP_TYPE_ADDR   0x1F6

#ifdef CFG_DUET_5822T
#define FLASH_MAX_SIZE                      0x400000
#else
#define FLASH_MAX_SIZE                      0x200000
#endif

#if (0x200000 == FLASH_MAX_SIZE)

    #ifdef _OTA_CMP_VENDOR_

        #define BOOTLOADER_FLASH_START_ADDR 0x10000000
        #define INFO_FLASH_START_ADDR       0x10010000
        #define APP_FLASH_START_ADDR        0x10012000
        #define OTA_FLASH_START_ADDR        0x1013E000
        #define KV_FLASH_START_ADDR         0x101EE000
        #define NVDS_FLASH_START_ADDR       0x101FC000
        #define OLL_FLASH_START_ADDR        0x101FE000
        #define MATTER_FLASH_START_ADDR     0x101FF000

        #define BOOTLOADER_MAX_SIZE         0x10000
        #define INFO_MAX_SIZE               0x2000
        #define APP_MAX_SIZE                0x12C000
        #define OTA_MAX_SIZE                0xB0000
        #define KV_MAX_SIZE                 0xE000
        #define NVDS_MAX_SIZE               0x2000
        #define OLL_MAX_SIZE                0x1000
        #define MATTER_CONFIG_MAX_SIZE      0x1000

    #else

        #define BOOTLOADER_FLASH_START_ADDR 0x10000000
        #define INFO_FLASH_START_ADDR       0x10010000
        #define APP_FLASH_START_ADDR        0x10012000
        #define OTA_FLASH_START_ADDR        0x10100000
        #define KV_FLASH_START_ADDR         0x101EE000
        #define NVDS_FLASH_START_ADDR       0x101FC000
        #define OLL_FLASH_START_ADDR        0x101FE000
        #define MATTER_FLASH_START_ADDR     0x101FF000

        #define BOOTLOADER_MAX_SIZE         0x10000
        #define INFO_MAX_SIZE               0x2000
        #define APP_MAX_SIZE                0xEE000
        #define OTA_MAX_SIZE                0xEE000
        #define KV_MAX_SIZE                 0xE000
        #define NVDS_MAX_SIZE               0x2000
        #define OLL_MAX_SIZE                0x1000
        #define MATTER_CONFIG_MAX_SIZE      0x1000

    #endif

#elif (0x400000 == FLASH_MAX_SIZE)

    #define BOOTLOADER_FLASH_START_ADDR 0x10000000
    #define INFO_FLASH_START_ADDR       0x10010000
    #define APP_FLASH_START_ADDR        0x10012000
    #define OTA_FLASH_START_ADDR        0x10200000
    #define KV_FLASH_START_ADDR         0x101EE000
    #define NVDS_FLASH_START_ADDR       0x101FC000
    #define OLL_FLASH_START_ADDR        0x101FE000
    #define MATTER_FLASH_START_ADDR     0x101C1000

    #define BOOTLOADER_MAX_SIZE         0x10000
    #define INFO_MAX_SIZE               0x2000
    #define APP_MAX_SIZE                0x166000
    #define OTA_MAX_SIZE                0x166000
    #define KV_MAX_SIZE                 0xE000
    #define NVDS_MAX_SIZE               0x2000
    #define OLL_MAX_SIZE                0x1000
    #define MATTER_CONFIG_MAX_SIZE      0x2000
#endif

#ifdef ALIOS_SUPPORT
#include <k_api.h>
#define duet_intrpt_enter() krhino_intrpt_enter()
#define duet_intrpt_exit() krhino_intrpt_exit()
#else
#define duet_intrpt_enter()
#define duet_intrpt_exit()
#endif

#ifdef __cplusplus
}
#endif

#endif
