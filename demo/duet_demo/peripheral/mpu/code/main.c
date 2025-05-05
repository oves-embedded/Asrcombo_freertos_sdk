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
#include "duet_common.h"
#include "duet_uart.h"
#include "duet_pinmux.h"

#define APBCLK_DIV_0   (0)
#define APBCLK_DIV_2   (0x00000001<<2)
#define APBCLK_DIV_3   (0x00000002<<2)
#define APBCLK_DIV_4   (0x00000003<<2)

enum {
    TEST_CASE_NONE, //0
    TEST_CASE_DEFAULT_CONFIG, //1
    TEST_CASE_MPU_OPRERATION_UNPRIVILEGED, //2
    TEST_CASE_BACKGROUND_REGION_OFF, //3
    TEST_CASE_BACKGROUND_REGION_ON,  //4
    TEST_CASE_REGION_MAX,     //5
    TEST_CASE_REGION_OVERLAP, //6
    TEST_CASE_ROOT_RW, //7
    TEST_CASE_ROOT_RO, //8
    TEST_CASE_ROOT_RW_PROTECTED, //9
    TEST_CASE_ROOT_RO_PROTECTED, //10
    TEST_CASE_ROOT_X,  //11
    TEST_CASE_ROOT_NX, //12
    TEST_CASE_ROOT_X_PROTECTED,  //13
    TEST_CASE_ROOT_NX_PROTECTED, //14
    TEST_CASE_USER_RW, //15
    TEST_CASE_USER_RO, //16
    TEST_CASE_USER_RW_PROTECTED, //17
    TEST_CASE_USER_RO_PROTECTED, //18
    TEST_CASE_USER_X,  //19
    TEST_CASE_USER_NX, //20
    TEST_CASE_USER_X_PROTECTED,  //21
    TEST_CASE_USER_NX_PROTECTED, //22

    TEST_CASE_NUM
}TEST_CASE;

#define TEST_ADDR_START     0x21000000
#define TEST_ADDR_END       0x21000400
#define TEST_LOOPS          100000

volatile uint8_t *gTestCase = (uint8_t *)0x20FE0000;    //should confirm to not be used
volatile uint8_t gWaitingHardFault = 0;
volatile uint8_t gWaitingMMFault = 0;

typedef enum {
    MEM_ATTR_NON_CACHEABLE,
    MEM_ATTR_MEMORY_WT,
    MEM_ATTR_MEMORY_WBWA,
    MEM_ATTR_DEVICE_nGnRE,
    MEM_ATTR_DEVICE_nGnRnE
} MEM_ATTR;

#if defined( __GNUC__ ) || defined ( __clang__ )
static const int PATH_DELIMITER = '/';
#else
static const int PATH_DELIMITER = '\\';
#endif

#define __FILENAME__ ((strrchr(__FILE__, PATH_DELIMITER) != NULL) ? (strrchr(__FILE__, PATH_DELIMITER) + 1) : __FILE__)

void __assert_result (const char *fn, uint32_t ln, uint32_t cond, uint32_t reset) {
    if (cond) {
        if(reset)
        {
            printf("\r\nMPU test case %d PASS\r\n", *gTestCase);
            (*gTestCase)++;
            delay(1000);
            duet_system_reset();
        }
    }
    else
    {
        printf("\r\nMPU test case %d FAILED at %s %d\r\n", *gTestCase, fn, (int)ln);
        while(1);
    }
}

#define MPU_TEST_PASS   1
#define MPU_TEST_FAILED 0

#define ASSERT_RESULT(cond) (void)__assert_result(__FILENAME__, __LINE__, cond, 1)
#define ASSERT_RESULT_NO_RESET(cond) (void)__assert_result(__FILENAME__, __LINE__, cond, 0)

#define ASSERT_MPU_REGION(rnr, table) \
    MPU->RNR = rnr; \
    ASSERT_RESULT_NO_RESET(MPU->RBAR == table[rnr].RBAR); \
    ASSERT_RESULT_NO_RESET(MPU->RLAR == table[rnr].RLAR)

#if defined(__MPU_PRESENT) && __MPU_PRESENT
static void ClearMpu(void) {
  for(uint32_t i = 0U; i < 8U; ++i) {
    MPU->RNR = i;
    MPU->RBAR = 0U;
    MPU->RLAR = 0U;
  }
}
#endif

void HardFault_Handler()
{
//    printf("HardFault_Handler\r\n");
    ASSERT_RESULT(gWaitingHardFault);
}

void MemManage_Handler()
{
//    printf("MemManage_Handler for 0x%08x access\r\n", (unsigned int)SCB->MMFAR);
    ASSERT_RESULT(gWaitingMMFault);
}

void SVC_Handler()
{
    __asm volatile
    (
        "    mrs r1, control        \n" /* Obtain current control value. */
        "    bic r1, #1             \n" /* Set privilege bit. */
        "    msr control, r1        \n" /* Write back new control value. */
        :::"r1"
    );
}

void rw_test(uint32_t start, uint32_t end)
{
    uint32_t temp;
    uint32_t loop;
    printf("R:");
    loop = TEST_LOOPS;
    while(loop--) {
        for(uint32_t addr = start; addr < end; addr += 4)
        {
            temp = *(volatile uint32_t *)(addr);
        }
    }
    printf("W:");
    loop = TEST_LOOPS;
    while(loop--) {
        for(uint32_t addr = start, temp = 0; addr < end; addr += 4, temp++)
        {
            *(volatile uint32_t *)(addr) = temp;
        }
    }
    printf("R:");
    loop = TEST_LOOPS;
    while(loop--) {
        for(uint32_t addr = start, temp = 0; addr < end; addr += 4, temp++)
        {
            if(*(volatile uint32_t *)(addr) != temp)
                ASSERT_RESULT(MPU_TEST_FAILED);
        }
    }
    if(temp)
        printf("RW OK\r\n");
}

void raise_privilege( void )
{
    __asm volatile
    (
        "    mrs r0, control    \n"
        "    tst r0, #1         \n" /* Is the task running privileged? */
        "    itte ne            \n"
        "    movne r0, #0       \n" /* CONTROL[0]!=0, return false. */
        "    svcne %0           \n" /* Switch to privileged. */
        "    moveq r0, #1       \n" /* CONTROL[0]==0, return true. */
        "    bx lr              \n"
        :: "i" (0) : "r0"
    );
}

void down_privilege( void )
{
    __asm volatile
    (
        " mrs r0, control    \n"
        " orr r0, #1         \n"
        " msr control, r0    \n"
        :::"r0"
    );
}

extern duet_uart_callback_func g_duet_uart_callback_handler[UART_NUM];

#define UART_TEST_INDEX UART1_INDEX
#define UART_HW_FLOW_CONTROL_ENABLE 1
#define UART_HW_FLOW_CONTROL_DISABLE 0

void set_uart_config(uint8_t uart_idx,uint8_t hw_flow_control)
{
    duet_uart_dev_t uart_config_struct  = {0};
    duet_uart_struct_init(&uart_config_struct);
    uart_config_struct.port = uart_idx;
//    uart_config_struct.priv = uart_recv_test;
//set user define config
    uart_config_struct.config.baud_rate = UART_BAUDRATE_115200;
    if(hw_flow_control)
        uart_config_struct.config.flow_control = FLOW_CTRL_CTS_RTS;
    else
        uart_config_struct.config.flow_control = FLOW_CTRL_DISABLED;
//make config take effect
    duet_uart_init(&uart_config_struct);
//    duet_uart_set_callback(uart_idx,uart_recv_test);
}

void set_uart_pinmux(uint8_t uart_idx,uint8_t hw_flow_control)
{
    switch(uart_idx)
    {
        case UART0_INDEX:
            duet_pinmux_config(PAD0,PF_UART0);
            duet_pinmux_config(PAD1,PF_UART0);
            if(hw_flow_control)
            {
                duet_pinmux_config(PAD6,PF_UART0);
                duet_pinmux_config(PAD7,PF_UART0);
            }
            break;
        case UART1_INDEX:
            duet_pinmux_config(PAD2,PF_UART1);
            duet_pinmux_config(PAD3,PF_UART1);
            if(hw_flow_control)
            {
                duet_pinmux_config(PAD14,PF_UART1);
                duet_pinmux_config(PAD15,PF_UART1);
            }
            break;
        case UART2_INDEX:
            duet_pinmux_config(PAD12,PF_UART2);
            duet_pinmux_config(PAD13,PF_UART2);
            if(hw_flow_control)
            {
                duet_pinmux_config(PAD10,PF_UART2);
                duet_pinmux_config(PAD11,PF_UART2);
            }
            break;
        default:
            break;
    }
}

int main(void)
{
    uint32_t tmp_value;
    tmp_value = REG_RD(0X40000844);
    REG_WR(0X40000844, (tmp_value | (0x000fffff))); //UART CLK BUS&PERI
    tmp_value = REG_RD(0X40000844) & (~0x00800400);
    REG_WR(0X40000844, (tmp_value | (0x00800400)));
    REG_WR(0X40000944, (0x0000ff30));
    //APB CLK DIV
    tmp_value = REG_RD(0X40000808);
    tmp_value &= (~(0X03<<2));
    REG_WR(0X40000808, (tmp_value | (APBCLK_DIV_0)));

    set_uart_pinmux(UART_TEST_INDEX,UART_HW_FLOW_CONTROL_DISABLE);
    set_uart_config(UART_TEST_INDEX,UART_HW_FLOW_CONTROL_DISABLE);
    printf_uart_register(UART_TEST_INDEX);

    if(*gTestCase == 0 || *gTestCase > TEST_CASE_NUM)
        *gTestCase = (TEST_CASE_NONE + 1);

    printf("DUET mpu verify for case %d\r\n", *gTestCase);

    if(*gTestCase == TEST_CASE_DEFAULT_CONFIG)
    {
        ASSERT_RESULT(ARM_MPU_Is_Valid());
    }

    if(*gTestCase == TEST_CASE_MPU_OPRERATION_UNPRIVILEGED)
    {
        down_privilege();
        gWaitingHardFault = 1;
        ClearMpu();
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    ClearMpu();

    ARM_MPU_SetMemAttr(MEM_ATTR_NON_CACHEABLE, ARM_MPU_ATTR(        /* Normal memory - Non-Cacheable */
        ARM_MPU_ATTR_NON_CACHEABLE,
        ARM_MPU_ATTR_NON_CACHEABLE
        ));

    ARM_MPU_SetMemAttr(MEM_ATTR_MEMORY_WT, ARM_MPU_ATTR(            /* Normal memory - WT */
        /*                    NT,  WB,  RA,  WA     */
        ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL),   /* Outer Write-Back transient with read and write allocate */
        ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL)    /* Inner Write-Through transient with read and write allocate */
        ));

    ARM_MPU_SetMemAttr(MEM_ATTR_MEMORY_WBWA, ARM_MPU_ATTR(          /* Normal memory - WBWA*/
        /*                    NT,  WB,  RA,  WA     */
        ARM_MPU_ATTR_MEMORY_(1UL, 1UL, 1UL, 1UL),   /* Outer Write-Back transient with read and write allocate */
        ARM_MPU_ATTR_MEMORY_(1UL, 1UL, 1UL, 1UL)    /* Inner Write-Through transient with read and write allocate */
        ));

    ARM_MPU_SetMemAttr(MEM_ATTR_DEVICE_nGnRE, ARM_MPU_ATTR(         /* nGnRE Device */
        ARM_MPU_ATTR_DEVICE,
        ARM_MPU_ATTR_DEVICE_nGnRE
        ));

    ARM_MPU_SetMemAttr(MEM_ATTR_DEVICE_nGnRnE, ARM_MPU_ATTR(        /* nGnRnE Device */
        ARM_MPU_ATTR_DEVICE,
        ARM_MPU_ATTR_DEVICE_nGnRnE
        ));

    if(*gTestCase == TEST_CASE_BACKGROUND_REGION_OFF)
    {
        gWaitingHardFault = 1;
        ARM_MPU_Enable(0);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_BACKGROUND_REGION_ON)
    {
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        ASSERT_RESULT(MPU_TEST_PASS);
    }

    if(*gTestCase == TEST_CASE_REGION_MAX)
    {
        ARM_MPU_Region_t mpu_table[8] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
//          { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   1UL, 0UL, 0UL), .RLAR = ARM_MPU_RLAR(0x0000FFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM
            { .RBAR = ARM_MPU_RBAR(0x00080000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x00097FFFUL, MEM_ATTR_NON_CACHEABLE) }, //ITCM
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON,   1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH
            { .RBAR = ARM_MPU_RBAR(0x20FD0000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //DTCM/RAM
            { .RBAR = ARM_MPU_RBAR(0x2F000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x6001FFFFUL, MEM_ATTR_MEMORY_WT) }, //Wifi
            { .RBAR = ARM_MPU_RBAR(0x60030000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x62007FFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Wifi/Ble Reg/Function
            { .RBAR = ARM_MPU_RBAR(0x62008000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x6200FFFFUL, MEM_ATTR_MEMORY_WT) }, //Ble
//          { .RBAR = ARM_MPU_RBAR(0xE0000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_DEVICE_nGnRnE) }  //ARM REG
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));

        for(int i = 0; i < sizeof(mpu_table)/sizeof(ARM_MPU_Region_t); i++)
        {
            ASSERT_MPU_REGION(i, mpu_table);
        }
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        ASSERT_RESULT(MPU_TEST_PASS);
    }

    if(*gTestCase == TEST_CASE_REGION_OVERLAP)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x21007FFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        gWaitingMMFault = 1;
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
        ARM_MPU_Disable();
    }

    if(*gTestCase == TEST_CASE_ROOT_RW)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ARM_MPU_Disable();
        ASSERT_RESULT(MPU_TEST_PASS);
    }

    if(*gTestCase == TEST_CASE_ROOT_RO)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON, 1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        gWaitingMMFault = 1;
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_ROOT_RW_PROTECTED)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON, 0UL, 0UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ARM_MPU_Disable();
        ASSERT_RESULT(MPU_TEST_PASS);
    }

    if(*gTestCase == TEST_CASE_ROOT_RO_PROTECTED)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON, 1UL, 0UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        gWaitingMMFault = 1;
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_ROOT_X)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH              RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ARM_MPU_Disable();
        ASSERT_RESULT(MPU_TEST_PASS);
    }

    if(*gTestCase == TEST_CASE_ROOT_NX)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH              RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON, 1UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        gWaitingHardFault = 1;
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_ROOT_X_PROTECTED)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH              RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON, 1UL, 0UL, 0UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ARM_MPU_Disable();
        ASSERT_RESULT(MPU_TEST_PASS);
    }

    if(*gTestCase == TEST_CASE_ROOT_NX_PROTECTED)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH              RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON, 1UL, 0UL, 1UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        gWaitingHardFault = 1;
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_USER_RW)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x20FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM/ITCM/FLASH/DTCM
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
            { .RBAR = ARM_MPU_RBAR(0x2F000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_NON_CACHEABLE) }, //Wifi/Ble/ARM REG
        };
        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        down_privilege();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        raise_privilege();
        ARM_MPU_Disable();
        ASSERT_RESULT(MPU_TEST_PASS);
    }

    if(*gTestCase == TEST_CASE_USER_RO)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x20FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM/ITCM/FLASH/DTCM
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON,   1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
            { .RBAR = ARM_MPU_RBAR(0x2F000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_NON_CACHEABLE) }, //Wifi/Ble/ARM REG
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        gWaitingMMFault = 1;
        down_privilege();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_USER_RW_PROTECTED)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH              RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x20FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM/ITCM/FLASH/DTCM
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON,   0UL, 0UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
            { .RBAR = ARM_MPU_RBAR(0x2F000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_NON_CACHEABLE) }, //Wifi/Ble/ARM REG
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        gWaitingMMFault = 1;
        down_privilege();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_USER_RO_PROTECTED)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x20FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM/ITCM/FLASH/DTCM
            { .RBAR = ARM_MPU_RBAR(0x21000000UL, ARM_MPU_SH_NON,   1UL, 0UL, 0UL), .RLAR = ARM_MPU_RLAR(0x2101FFFFUL, MEM_ATTR_MEMORY_WBWA) }, //RAM
            { .RBAR = ARM_MPU_RBAR(0x2F000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_NON_CACHEABLE) }, //Wifi/Ble/ARM REG
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        gWaitingMMFault = 1;
        down_privilege();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_USER_X)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x0FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM/ITCM
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON,   1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH for test
            { .RBAR = ARM_MPU_RBAR(0x20FD0000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //DTCM/RAM/PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_NON_CACHEABLE) }, //Wifi/Ble/ARM REG
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        down_privilege();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        raise_privilege();
        ARM_MPU_Disable();
        ASSERT_RESULT(MPU_TEST_PASS);
    }

    if(*gTestCase == TEST_CASE_USER_NX)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x0FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM/ITCM
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON,   1UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH for test
            { .RBAR = ARM_MPU_RBAR(0x20FD0000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //DTCM/RAM/PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_NON_CACHEABLE) }, //Wifi/Ble/ARM REG
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        gWaitingHardFault = 1;
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        down_privilege();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_USER_X_PROTECTED)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x0FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM/ITCM
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON,   1UL, 0UL, 0UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH for test
            { .RBAR = ARM_MPU_RBAR(0x20FD0000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //DTCM/RAM/PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_NON_CACHEABLE) }, //Wifi/Ble/ARM REG
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        gWaitingMMFault = 1;
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        down_privilege();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    if(*gTestCase == TEST_CASE_USER_NX_PROTECTED)
    {
        ARM_MPU_Region_t mpu_table[] = {
            //                     BASE          SH                RO   NP   XN                         LIMIT         ATTR
            { .RBAR = ARM_MPU_RBAR(0x00000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x0FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //ROM/ITCM
            { .RBAR = ARM_MPU_RBAR(0x10000000UL, ARM_MPU_SH_NON,   1UL, 0UL, 1UL), .RLAR = ARM_MPU_RLAR(0x17FFFFFFUL, MEM_ATTR_MEMORY_WT) }, //FLASH for test
            { .RBAR = ARM_MPU_RBAR(0x20FD0000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0x3FFFFFFFUL, MEM_ATTR_MEMORY_WT) }, //DTCM/RAM/PSRAM BUFFER/Ext PSRAM
            { .RBAR = ARM_MPU_RBAR(0x40000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(0x400FFFFFUL, MEM_ATTR_DEVICE_nGnRE) }, //Peripheral
            { .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_NON,   0UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(0xFFFFFFFFUL, MEM_ATTR_NON_CACHEABLE) }, //Wifi/Ble/ARM REG
        };

        ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
        gWaitingHardFault = 1;
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
        SCB_EnableICache();
        SCB_EnableDCache();
        down_privilege();
        rw_test(TEST_ADDR_START, TEST_ADDR_END);
        ASSERT_RESULT(MPU_TEST_FAILED);
    }

    printf("\r\nDUET mpu verify all PASS!!!\r\n");
    *gTestCase = 0;

    while(1);
}
