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

#include <stdint.h>
#include "duet.h"

#define ALWAYS_ON_REGFILE           0x40000A00
#define REG_PMU_CTRL                *((volatile uint32_t *)(ALWAYS_ON_REGFILE + 0x20))

#define REG_PL_RD(addr)              (*(volatile uint32_t *)(addr))
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)
#define SYS_REG_BASE_WIFI_CLK          ((SYS_REG_BASE + 0x85C))
#define SYS_REG_BASE_REF_ROOT_CLK      ((SYS_REG_BASE + 0x80C))
#define COMBO_SPI_CTRL_ADDR     0x4000B000
#define TRANS_MODE_OFT          0x0800
#define PRESCALER_OFT           0x0804
#define ADDR_REG_OFT            0x0808
#define READNOTWRITE_OFT        0x080c
#define WDATA_REG_OFT           0x0810
#define RDATA_REG_OFT           0x0814
#define START_FLAG_OFT          0x0818
#define SPI_COMMAND             COMBO_SPI_CTRL_ADDR
#define SPI_RDATA               COMBO_SPI_CTRL_ADDR

extern void delay(unsigned int cycles);
void duet_soc_wifi_clk_enable(void)
{
    //Enable WiFi core clock
    //REG_PL_WR(SYS_REG_BASE_WIFI_CLK,0x1);
    //enable all wifi/ble block clock
    REG_WR(0x40000840, 0x1FF<<15);
    delay(50);
}

void duet_soc_wifi_clk_disable(void)
{
    //Disable WiFi core clock
    REG_PL_WR(SYS_REG_BASE_WIFI_CLK,0x0);
    delay(50);
}

void mdm_rcclkforce_setf(uint8_t rcclkforce)
{
    //ASSERT_ERR((((uint32_t)rcclkforce << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)rcclkforce << 27));
}

void spi_mst_write(uint16_t addr, uint16_t data)
{
 #if 1 //def COMBO_V7_FPGA
    uint32_t var=0;
     //udelay(300);

    *((volatile int *) (SPI_COMMAND + TRANS_MODE_OFT))   = 0;
    *((volatile int *) (SPI_COMMAND + PRESCALER_OFT))    = 8;//8:80M/16=5M, 2:80M/4=20M
    *((volatile int *) (SPI_COMMAND + ADDR_REG_OFT))     = addr;
    *((volatile int *) (SPI_COMMAND + READNOTWRITE_OFT)) = 0;
    *((volatile int *) (SPI_COMMAND + WDATA_REG_OFT))     = data;
    *((volatile int *) (SPI_COMMAND + START_FLAG_OFT))   = 1;

    do{
        var =(*((volatile int *)(SPI_COMMAND+ START_FLAG_OFT)));
    }while(var);

#else
    uint32_t pre = 8<<28;
    uint32_t start = 1 <<27;
    uint32_t readnotwrite = 0 <<26;
    uint32_t addr_t = addr <<16;
    uint32_t data_t = data << 0;
    uint32_t register_00 = pre | start  | readnotwrite | addr_t | data_t ;

    uint32_t var=0;
    *((volatile int *) SPI_COMMAND)= register_00;

    do{
        var =(*((volatile int *)SPI_COMMAND)) & (0x1 << 27);
    }while(var);
#endif
}

uint16_t spi_mst_read(uint16_t addr)
{
#if 1 //def COMBO_V7_FPGA
    //udelay(300);
    uint32_t var=0;

    *((volatile int *)(SPI_COMMAND + TRANS_MODE_OFT))   = 0;
    *((volatile int *)(SPI_COMMAND + PRESCALER_OFT))    = 8;
    *((volatile int *)(SPI_COMMAND + ADDR_REG_OFT))     = addr;
    *((volatile int *)(SPI_COMMAND + READNOTWRITE_OFT)) = 1;
    *((volatile int *)(SPI_COMMAND + START_FLAG_OFT))   = 1;

    do{
        var =(*((volatile int *)(SPI_COMMAND+ START_FLAG_OFT)));
    }while(var);

    return(  (uint16_t)*((volatile uint32_t *)(SPI_RDATA + RDATA_REG_OFT)));
#else

    uint32_t pre = 8 << 28;
    uint32_t start = 1 <<27;
    uint32_t readnotwrite = 1 <<26;
    uint32_t addr_t = addr <<16;
    uint32_t data_t = 0;
    uint32_t register_00 = pre | start  | readnotwrite | addr_t | data_t ;

    uint32_t var=0;
    *((volatile uint32_t *) SPI_COMMAND) = register_00;

    do{
        var =( *((volatile uint32_t *) SPI_COMMAND) ) & (0x1 << 27);
    }while(var);

    return(  (uint16_t)*((volatile uint32_t *) SPI_RDATA) );
#endif
}

void rf_set_reg_bit(uint16_t reg, uint8_t start_bit, uint8_t len, uint16_t src_val)
{
    uint16_t tmp,mask,val;

    if((reg < 0xFF) && (start_bit < 16) && (len <= 16)&&(src_val<(1<<len)))
    {
        tmp = spi_mst_read(reg);

        mask = (1<<len)-1;          //1. clear dst bit. eg: len=4, mask = 0xf, 1111
        mask = ~(mask<<start_bit);  //~(mask<<4):0xff0f: 1111 1111 0000 1111

        val = tmp & mask;           //2.val =spi_read() & 0xff0f, clear [7:4]

        src_val = (src_val << start_bit);
        val = val | src_val;        //3. val spi_read & 0xff0f | val << 4

        spi_mst_write(reg,val);
    }
    else
        printf("set_reg input params not support \r\n");
}

enum check_type
{
    D_RCM_EOC = 0x1,
    D_PVM_EOC,
    APLL_FCAL_DONE,
    DBL_CAL_DONE,
    SX_VCO_FULL_CAL_DONE,
    SX_LO_MIX_CAL_DONE,
    DCOC_BIT0,
    DCOC_BIT1,
    DCOC_BIT2,
    DCOC_BIT3,
    DCOC_BIT4,
    DCOC_BIT5,
    TXPWR_CAL_AUXADC_DOWN,
    RCO_CAL_EN,
    NULL_BIT
};

uint16_t rf_get_reg_bit(uint16_t reg, uint8_t start_bit, uint8_t len)
{
    uint16_t mask,val;

    if((reg < 0xFF) && (start_bit < 16) && (len <= 16))
    {
        val = spi_mst_read(reg);    // 1. read reg val

        mask = (1<<len)-1;          //eg: len =4, 0xf,1111
        mask = mask << start_bit;   // 0x0f00;
        val = val & mask;           // 2. get dst bit

        val = (val >> start_bit);   // 3. ror

        return val;
    }
    else
    {
        printf("get_reg input params not support \r\n");
        return -1;
    }
}

uint8_t check_val(uint8_t type)
{
    uint16_t val = 0;
    uint8_t ret = 1;
    switch(type){
        case NULL_BIT:
            ret = 1;
            break;
        case D_RCM_EOC:
            val = rf_get_reg_bit(0xA6,9,1);
            if(val == 1)
                ret = 0;
            break;
        case D_PVM_EOC:
            val = rf_get_reg_bit(0xA8,6,1);
            if(val == 1)
                ret = 0;
            break;
        case APLL_FCAL_DONE:
            val = rf_get_reg_bit(0x6E,15,1);
            if(val == 1)
                ret = 0;
            break;
        case DBL_CAL_DONE:
            val = rf_get_reg_bit(0x3F,2,1);
            if(val == 1)
                ret = 0;
            break;
        case SX_LO_MIX_CAL_DONE:
            val = rf_get_reg_bit(0x4D,0,1);
            if(val == 1)
                ret = 0;
            break;
        case DCOC_BIT0:
            val = rf_get_reg_bit(0xAC,0,1);
            if(val == 0)
                ret = 0;
            break;
        case DCOC_BIT1:
            val = rf_get_reg_bit(0xAC,1,1);
            if(val == 0)
                ret = 0;
            break;
        case DCOC_BIT2:
            val = rf_get_reg_bit(0xAC,2,1);
            if(val == 0)
                ret = 0;
            break;
        case DCOC_BIT3:
            val = rf_get_reg_bit(0xAC,3,1);
            if(val == 0)
                ret = 0;
            break;
        case DCOC_BIT4:
            val = rf_get_reg_bit(0xAC,4,1);
            if(val == 0)
                ret = 0;
            break;
        case DCOC_BIT5:
            val = rf_get_reg_bit(0xAC,5,1);
            if(val == 0)
                ret = 0;
            break;
        case SX_VCO_FULL_CAL_DONE:
            val = rf_get_reg_bit(0x43,6,1);
            if(val == 1)
                ret = 0;
            break;
        case RCO_CAL_EN:
            val = rf_get_reg_bit(0xC4,5,1);
            if(val == 0)
                ret = 0;
            break;
        case TXPWR_CAL_AUXADC_DOWN:
            val = rf_get_reg_bit(0xA4,3,1);
            if(val == 1)
                ret = 0;
            break;
        default:
            break;
    }
    return ret;
}

void check_timeout_nop(uint32_t nus,uint32_t type)
{
    uint16_t ret = 0;

    while(nus--)
    {
        __asm("nop");
        if(ret == check_val(type))
            return;
    }

#if RF_DEBUG
    if(nus == 0)
    {
        print_s("\r\n check bit failed, timeout.type:0x");
        print32(type);
    }
#endif
}

void duet_rfinit15_rco_cal()
{
#if RF_DEBUG
    uint16_t rc_cali;
#endif

    //Enable PA DA,because it's default val error
    //pmu_ctrl = REG_PL_RD(REG_PMU_CTRL);
    //bit10 clear
    //REG_PL_WR(REG_PMU_CTRL, (pmu_ctrl & 0xFFFFFBFF));
    REG_PMU_CTRL &= 0xFFFFFBFF;

    rf_set_reg_bit(0x03,0,2,0x3);
    spi_mst_write(0x06,0x57DF);
    spi_mst_write(0x07,0xBF12);
    spi_mst_write(0x08,0xEEFF);
    spi_mst_write(0x09,0xFE00);

    //rf_set_reg_bit(0xB9,7,1,0x1);
    spi_mst_write(0xC4,0x450A);
    spi_mst_write(0xC6,0x30);
    rf_set_reg_bit(0xC4,5,1,0x1);
    check_timeout_nop(20000,RCO_CAL_EN);
    rf_set_reg_bit(0xC4,15,1,0x1);
    rf_set_reg_bit(0xC4,5,1,0x1);
    check_timeout_nop(10000,RCO_CAL_EN);

#if RF_DEBUG
    print_s("\r\n========== case 15:rco_cal ==========\r\n");
    print_s("\r\n RCO_MEAS_NCNT_H (0xC6[1:0]):0x");
    print32(rf_get_reg_bit(0xC6,0,2));
    rc_cali = rf_get_reg_bit(0xC5,0,16);
    print_s("\r\n RCO_MEAS_NCNT (0xC5):0x");
    print32(rc_cali);
    if((rc_cali > RC_CALI_MAX) || (rc_cali < RC_CALI_MIN))
        print_s("RC Calibration failed.");
    print_s("\r\n=====================================\r\n");
#endif
}

void duet_drv_rco_cal(void)
{
    duet_soc_wifi_clk_enable();
    //wifi ref clk sel to 52MHz
    REG_PL_WR(SYS_REG_BASE_REF_ROOT_CLK, REG_PL_RD(SYS_REG_BASE_REF_ROOT_CLK) | 0x1);
    /* Enable RC clock */
    mdm_rcclkforce_setf(1);
    duet_rfinit15_rco_cal();
    /* Disable RC clock */
    mdm_rcclkforce_setf(0);
    duet_soc_wifi_clk_disable();
}
