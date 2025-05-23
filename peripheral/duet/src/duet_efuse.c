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
#include <stdlib.h>
#include "duet.h"
#include "duet_cm4.h"
#include "duet_efuse.h"
#include "duet_rf_spi.h"

void efuse_ldo25_open(void)
{
    uint16_t tmp_16;
    uint32_t tmp_32;
    // ----- Change APLL clock to 80MHz -----
//    SYS_CRM_WIFI_BLK_CLK = 0x1; // Enable WiFi core clock
//    delay(5); // wait for a few cycles for WiFi core clock settle
//    MDM_CLKGATEFCTRL0 = (0x1<<27);  // Force RC clock open

    //open 10uA current
    tmp_16 = spi_mst_read(TRX_PD_CTRL1_REG_ADDR);
    tmp_16 &= (~(0x0001 << 13)); //clear bit13 (D_PD_BG)
    spi_mst_write(TRX_PD_CTRL1_REG_ADDR, tmp_16);

    tmp_16 = spi_mst_read(TRX_PD_CTRL2_REG_ADDR);
    tmp_16 &= (~(0x0003 << 2)); //clear bit<3:2> (D_PD_TRXTOP_BIAS, D_PD_TRXTOP_LDO)
    spi_mst_write(TRX_PD_CTRL2_REG_ADDR, tmp_16);

    //open PU LDO25: set D_AON_RCO32K_REG1<13> to 1
    tmp_32 = RTC_REG_RCO32K;
    RTC_REG_RCO32K = tmp_32 | (0x00000001 << (13 + 16));

    //adjust PU LDO25 voltage: set D_AON_RCO32K_REG1<12:9> to 4'b0011
    tmp_32 = RTC_REG_RCO32K;
    tmp_32 &= ~(0x0000000F << (9 + 16));
    tmp_32 |= (0x00000003 << (9 + 16));
    RTC_REG_RCO32K = tmp_32;
}

/*
    efuse init, must be called before read/write operation
    if efuse write is needed, ldo25_open must be set to 1
    if only efuse read is needed, then ldo25_open should be set to 0
*/
void duet_efuse_init(uint8_t ldo25_open)
{
    EFUSE->RD_CNT = EFUSE_READ_OP_WAIT_CYCLE;
    EFUSE->WR_CNT = EFUSE_WRITE_OP_WAIT_CYCLE;
    EFUSE->DIV_CNT = EFUSE_DIV_OP_WAIT_CYCLE;
    if(ldo25_open)
    {
        efuse_ldo25_open();
    }
}

/*
    read one efuse byte
    param-addr: efuse addr, from 0x000 to 0x1FF
*/
uint8_t duet_efuse_byte_read(uint16_t addr)
{
    EFUSE->B_ADDR = addr;
    EFUSE->CFG_TYPE = 0x0;//read type

    EFUSE->START = 0x1;
    while(EFUSE->START & 0x1);
    return EFUSE->RDBK_DATA;
}

/*
    read one efuse word
    param-addr: efuse addr, from 0x000 to 0x1FC
*/
uint32_t duet_efuse_word_read(uint16_t addr)
{
    uint32_t rd_word_data = 0;
    uint8_t rd_byte_data = 0;
    for(int i = 0; i < 4; i++)
    {
        rd_byte_data = duet_efuse_byte_read(addr+i);
        rd_word_data |= rd_byte_data << (i<<3);
    }
    return rd_word_data;
}


/*
    read multiple efuse bytes
    param-start_addr: efuse addr, from 0x000 to 0x1FF
    param-size_in_bytes: how many bytes to be read
    param-pData: where efuse data is stored
*/
void duet_efuse_multi_read(uint16_t start_addr, uint16_t size_in_bytes, uint8_t *pData)
{
    uint16_t i;
    //efuse init
    duet_efuse_init(EFUSE_LDO25_CLOSE);

    //efuse byte read
    for(i = 0; i < size_in_bytes; i++)
    {
        *(pData+i) = duet_efuse_byte_read(start_addr+i);
    }
}
