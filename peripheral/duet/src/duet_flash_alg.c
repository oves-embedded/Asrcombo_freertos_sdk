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

/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2010 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for New Device 256kB Flash                            */
/*                                                                     */
/***********************************************************************/
#include <stdlib.h>
#include <string.h>
#include "duet_flash_alg.h"        // FlashOS Structures
#include "duet_cm4.h"
#include "duet.h"

volatile uint32_t g_flash_wip = 0;

static volatile uint32_t g_flash_icached = 0;
static volatile uint32_t g_flash_dcached = 0;
static volatile uint32_t g_flash_irq = 0;
static volatile uint32_t g_flash_sus = 0;
static volatile uint32_t g_flash_polling = 0;

FLASH_DRIVER_SEG static void duet_flash_alg_disable_cache(){
    g_flash_icached = SCB_IsEnabledICache();
    g_flash_dcached = SCB_IsEnabledDCache();
    if(g_flash_icached)
        SCB_DisableICache();
    if(g_flash_dcached)
        SCB_DisableDCache();
}

FLASH_DRIVER_SEG static void duet_flash_alg_enable_cache(){
    if(g_flash_icached)
        SCB_EnableICache();
    if(g_flash_dcached)
        SCB_EnableDCache();
}

FLASH_DRIVER_SEG static void duet_flash_alg_disable_interrupts(){
    g_flash_irq = __get_PRIMASK();
    __disable_irq();
}

FLASH_DRIVER_SEG static void duet_flash_alg_enable_interrupts(){
    g_flash_irq?__disable_irq():__enable_irq();
}

FLASH_DRIVER_SEG void duet_flash_alg_f_delay(unsigned char cycle)
{
    while(cycle--)
        __asm("nop");
}

FLASH_DRIVER_SEG int duet_flash_alg_check_busy (void) {

    int var_rdata = 0;
    int cnt = 0;
    /* Add your Code */
    var_rdata = FLASH->QSPI_SR; //read back

    while(((var_rdata>>5 & 0x01) ==1)){
        var_rdata = FLASH->QSPI_SR; //read back
        if(cnt < 80000000)//erase chip time(10s)/(160MHz*2cycle)
            cnt = cnt+1;
        else
            return (1);
    }
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int duet_flash_alg_check_abort_busy (void) {

    int var_rdata = 0;
    int cnt = 0;
    /* Add your Code */
    var_rdata = FLASH->QSPI_SR; //read back

    while(((var_rdata>>6 & 0x01) ==1)){
        var_rdata = FLASH->QSPI_SR; //read back
        if(cnt < 200)// /160MHz*10cycled
            cnt = cnt+1;
        else
            return (1);
    }
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int duet_flash_alg_clr_flg (void) {
    /* Add your Code */
    FLASH->QSPI_FCR = 0xF001B;//0x1F;
    FLASH->QSPI_FCR = 0x0;

    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int duet_flash_alg_abort_en (void) {
    /* Add your Code */
    int var_rdata;
    var_rdata=FLASH->QSPI_CR;
    FLASH->QSPI_CR = (var_rdata&0xFFFFFFFD)+0x2;
    FLASH->QSPI_CR = var_rdata&0xFFFFFFFD;

    FLASH->QSPI_CR &= 0xFFFFFFFE;   //disable qspi
    duet_flash_alg_f_delay(32);
    FLASH->QSPI_CR |= 0x1;          //enable qspi

    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int duet_flash_alg_polling_wip (unsigned char en_suspend){
    int var_rdata = 0;
    int cycle_count = 0;

    g_flash_polling = 1;
    //dumode=2'b00,fmode=2'b10,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h05;
    FLASH->QSPI_CCR = 0x9000105;
    var_rdata = FLASH->QSPI_CR;
    FLASH->QSPI_CR = (var_rdata & 0xFFBFFFFF) + 0x400000; //QSPI_CR[22],apms= 1'b1;
    FLASH->QSPI_DLR = 0x0;//one byte
    FLASH->QSPI_PSMKR = 0x1;//mask = 0x1;
    FLASH->QSPI_PSMAR = 0x0;//match = 0x0;
    FLASH->SBUS_START = 0x1;
    duet_flash_alg_f_delay(10);
    var_rdata = FLASH->QSPI_SR; //read back
    if(var_rdata>>5 & 0x01)
    {
        if(en_suspend)
            duet_flash_alg_enable_interrupts();

        while((var_rdata>>5 & 0x01) && cycle_count < 80000000){
            var_rdata = FLASH->QSPI_SR;
            cycle_count++;
        }

        if(en_suspend)
            duet_flash_alg_disable_interrupts();
    }
    duet_flash_alg_clr_flg();
    return g_flash_polling;
}

FLASH_DRIVER_SEG int duet_flash_alg_is_wip (void){
    return (int)g_flash_wip;
}

FLASH_DRIVER_SEG int duet_flash_alg_polling_wel (void){
    int var_rdata = 0;
    //dumode=2'b00,fmode=2'b10,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h05;
    FLASH->QSPI_CCR = 0x9000105;
    var_rdata = FLASH->QSPI_CR;
    FLASH->QSPI_CR = (var_rdata & 0xFFBFFFFF) + 0x400000; //QSPI_CR[22],apms= 1'b1;
    FLASH->QSPI_DLR = 0x0;//one byte
    FLASH->QSPI_PSMKR = 0x2;//mask = 0x1;
    FLASH->QSPI_PSMAR = 0x2;//match = 0x0;
    FLASH->SBUS_START = 0x1;
    duet_flash_alg_f_delay(10);
    duet_flash_alg_check_busy();
    duet_flash_alg_clr_flg();
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int duet_flash_alg_setqe (unsigned char quad) {
    //bypass enable 0x4000308C bit10 set 1
    //*((volatile unsigned int *)(0x4000308C)) |= (1 << 10);
    FLASH->QSPI_CFGR |= (1 << 10);
    duet_flash_alg_abort_en();
    duet_flash_alg_check_abort_busy();

    FLASH->QSPI_CCR = 0x106;//IMODE=2'b01,INSTRUCTION=WREN
    duet_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    duet_flash_alg_f_delay(10);
    duet_flash_alg_check_busy();
    duet_flash_alg_polling_wel();

    FLASH->QSPI_CCR = 0x01000101;//dumode=2'b00,fmode=2'b00,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h01;
    FLASH->QSPI_DLR = 0x1;//two byte
    if(quad == 0x1){
        FLASH->QSPI_DR = 0x200;
    }
    else{
        FLASH->QSPI_DR = 0x0; //0x200;
    }
    duet_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    duet_flash_alg_f_delay(10);
    duet_flash_alg_check_busy();
    duet_flash_alg_clr_flg();
    duet_flash_alg_polling_wip(0);
    //bypass disable 0x4000308C bit10 clr
    //*((volatile unsigned int *)(0x4000308C)) &= ~(1 << 10);
    FLASH->QSPI_CFGR &= ~(1 << 10);
    /* Add your Code */
    return (0);                                  // Finished without Errors
}

vu32 duet_flash_line_cfg = 0; //flash line number config
vu32 duet_flash_clk_cfg = 52; //flash clock config, default 52MHz
FLASH_DRIVER_SEG void duet_flash_alg_load_cfg (void) {
    duet_flash_alg_abort_en();
    duet_flash_alg_check_abort_busy();

#ifdef _SPI_FLASH_240MHz_
    if(duet_flash_clk_cfg == 240)
    {
        FLASH->QSPI_CR    = FLASH_QSPI_DIV3;       //div3, //offset 0x00
        FLASH->QSPI_CR      |= 0x20;               // set loop back en bit 5=1
        FLASH->QSPI_CR      &= ~(7<<13);
        FLASH->QSPI_CR      |= (5<<13);            //ds = 5

        FLASH->QSPI_DCR   = 0x00150000  ;          //offset 0x04
        FLASH->QSPI_FCR   = 0x0  ;                 //offset 0x0C
        FLASH->QSPI_DLR   = 0x0  ;                 //offset 0x10
        FLASH->QSPI_CCR   = duet_flash_line_cfg;   //dumode=2'b10,,sio=1'b0,fmode=2'b11,dmode=2'b10,reserved=1'b0,dcyc=5'h7,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h3B;
        FLASH->QSPI_AR    = 0x0  ;                 //offset 0x18
        FLASH->QSPI_ABR   = 0x0  ;                 //offset 0x1C
        FLASH->QSPI_DR    = 0x0  ;                 //offset 0x20
        FLASH->QSPI_PSMKR = 0x0  ;                 //offset 0x24
        FLASH->QSPI_PSMAR = 0x0 ;                  //offset 0x28
        FLASH->QSPI_PIR   = 0x0 ;                  //offset 0x2C
        FLASH->QSPI_TOR   = 0x12FFFF ;             //offset 0x30
        FLASH->QSPI_CFGR = 0x7202;                 //offset 0x8C [15:12] set loop back delay=7
    }
    else
#endif
    {
        FLASH->QSPI_CR    = FLASH_QSPI_DIV2;       //div2, //offset 0x00
        FLASH->QSPI_DCR   = 0x00150000  ;          //offset 0x04
        FLASH->QSPI_FCR   = 0x0  ;                 //offset 0x0C
        FLASH->QSPI_DLR   = 0x0  ;                 //offset 0x10
        FLASH->QSPI_CCR   = duet_flash_line_cfg;   //dumode=2'b10,,sio=1'b0,fmode=2'b11,dmode=2'b10,reserved=1'b0,dcyc=5'h7,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h3B;
        FLASH->QSPI_AR    = 0x0  ;                 //offset 0x18
        FLASH->QSPI_ABR   = 0x0  ;                 //offset 0x1C
        FLASH->QSPI_DR    = 0x0  ;                 //offset 0x20
        FLASH->QSPI_PSMKR = 0x0  ;                 //offset 0x24
        FLASH->QSPI_PSMAR = 0x0 ;                  //offset 0x28
        FLASH->QSPI_PIR   = 0x0 ;                  //offset 0x2C
        FLASH->QSPI_TOR   = 0x12FFFF ;             //offset 0x30
        FLASH->QSPI_CFGR  = 0x202 ;                //offset 0x8C bit1:direct access optimization enable
    }

}

FLASH_DRIVER_SEG int duet_flash_alg_init ()
{
    duet_flash_alg_disable_interrupts();
    duet_flash_alg_disable_cache();

    duet_flash_line_cfg = FLASH_QSPI_L4; //config flash line here

    duet_flash_alg_abort_en();
    duet_flash_alg_check_abort_busy();

    //attempt to resume
    FLASH->QSPI_CCR = 0x000017A;//dumode=2'b0,fmode=2'b00,dmode=2'b00,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h7A;
    duet_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    duet_flash_alg_check_busy();

    //if resume indeed, wait until wip cleared
    duet_flash_alg_f_delay(32);
    duet_flash_alg_polling_wip(0);

    if(duet_flash_line_cfg == FLASH_QSPI_L4)
    {
        duet_flash_alg_setqe(1);
    }
    else
    {
        duet_flash_alg_setqe(0);
    }

    duet_flash_alg_load_cfg();

    duet_flash_alg_enable_cache();
    duet_flash_alg_enable_interrupts();

    return (0);                                  // Finished without Errors
}

/*
 *  cmd: CHIP_ERASE_CMD or SECTOR_ERASE_CMD or BLOCK32_ERASE_CMD or BLOCK64_ERASE_CMD
 *  adr: not used for CHIP_ERASE_CMD
 */
FLASH_DRIVER_SEG int duet_flash_alg_erase(unsigned int cmd, unsigned long adr) {
    int ret = 0;
    duet_flash_alg_disable_interrupts();
    if(g_flash_wip)
    {
        printf("\n%s error...............g_flash_wip:%d \n\n", __func__, (unsigned int)g_flash_wip);
        ret = 1;
    }
    else
    {
        g_flash_wip = 1;
        g_flash_sus = 0;

        duet_flash_alg_disable_cache();
        //add delay to avoid confilict between flash cfg and instruction fetch by cache
        duet_flash_alg_f_delay(32);

        duet_flash_alg_abort_en();
        duet_flash_alg_check_abort_busy();

        FLASH->QSPI_CCR = 0x106;//IMODE=2'b01,INSTRUCTION=WREN
        duet_flash_alg_clr_flg();
        FLASH->SBUS_START = 0x1;
        duet_flash_alg_f_delay(10);
        duet_flash_alg_check_busy();
        duet_flash_alg_polling_wel();

        FLASH->QSPI_CCR = cmd;
        if(CHIP_ERASE_CMD != cmd)
        {
            FLASH->QSPI_AR = adr;
        }
        duet_flash_alg_clr_flg();
        FLASH->SBUS_START = 0x1;
        duet_flash_alg_f_delay(10);
        duet_flash_alg_check_busy();
        duet_flash_alg_clr_flg();
        while(!duet_flash_alg_polling_wip(1));
        duet_flash_alg_f_delay(10);
        duet_flash_alg_clr_flg();
        duet_flash_alg_load_cfg();

        duet_flash_alg_enable_cache();

        g_flash_wip = 0;
        g_flash_sus = 0;

        ret = 0;
    }
    duet_flash_alg_enable_interrupts();

    return ret;                                  // Finished without Errors
}

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

FLASH_DRIVER_SEG int duet_flash_alg_programpage(unsigned long adr, unsigned long sz, unsigned char *buf) {
    unsigned long sz_temp;
    //volatile int cycle_count = 0;
    int var_rdata = 0;
    int fthres = 8;
    int cnt = 0;
    int ret = 0;
    unsigned long ram_buf[SPI_FLASH_PAGE_SIZE/4] = {0};
    duet_flash_alg_disable_interrupts();
    if(g_flash_wip)
    {
        printf("\n%s error...............g_flash_wip:%d \n\n", __func__, (unsigned int)g_flash_wip);
        ret = 1;
    }
    else if((0 == sz) || (NULL == buf))
    {
        printf("\n%s error...............adr:0x%x, sz:%d \n\n", __func__, (unsigned int)adr, (unsigned int)sz);
        ret = 1;
    }
    else
    {
        g_flash_wip = 2;
        g_flash_sus = 0;

        duet_flash_alg_disable_cache();

        if(((uint32_t)buf >= SPI_FLASH_ADDR_START && (uint32_t)buf <=SPI_FLASH_ADDR_END) || ((uint32_t)buf & 0x3))
        {
            memcpy(ram_buf, buf, sz);
            buf = (unsigned char *)ram_buf;
        }

        sz_temp = sz - 1;
        sz = (sz + 3) & ~3;                           // Adjust size for Words

        //add delay to avoid confilict between flash cfg and instruction fetch by cache
        duet_flash_alg_f_delay(32);

        duet_flash_alg_abort_en();
        duet_flash_alg_check_abort_busy();

        FLASH->QSPI_CCR = 0x106;//IMODE=2'b01,INSTRUCTION=WREN
        duet_flash_alg_clr_flg();
        FLASH->SBUS_START = 0x1;
        duet_flash_alg_f_delay(10);
        duet_flash_alg_check_busy();
        duet_flash_alg_polling_wel();

        FLASH->QSPI_CCR = 0x1002502;//fmode=2'b00,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h02;
        FLASH->QSPI_DLR = sz_temp;
        FLASH->QSPI_AR = adr;
        while ((sz>0)&&(cnt<fthres)) {
            var_rdata = FLASH->QSPI_SR;
            var_rdata = var_rdata >>8 & 0x3F;
            if(var_rdata < fthres) {
                FLASH->QSPI_DR = *((unsigned long *)buf);        // Program Word
                cnt += 1;
                buf += 4;
                sz  -= 4;
            }
        }
        duet_flash_alg_f_delay(10);
        duet_flash_alg_check_busy();
        FLASH->SBUS_START = 0x01; //WRITE
        duet_flash_alg_f_delay(10);
        while (sz) {
            var_rdata = FLASH->QSPI_SR;
            var_rdata = var_rdata >>8 & 0x3F;
            if(var_rdata < fthres) {
                FLASH->QSPI_DR = *((unsigned long *)buf);        // Program Word
                buf += 4;
                sz  -= 4;
            }
        }
        duet_flash_alg_f_delay(10);
        duet_flash_alg_check_busy();
        duet_flash_alg_clr_flg();
        while(!duet_flash_alg_polling_wip(1));
        duet_flash_alg_load_cfg();

        duet_flash_alg_enable_cache();

        g_flash_wip = 0;
        g_flash_sus = 0;

        ret = 0;
    }
    duet_flash_alg_enable_interrupts();

    return ret;                                  // Finished without Errors
}

FLASH_DRIVER_SEG void duet_flash_alg_suspend (void)
{
    if(g_flash_wip)
    {
        duet_flash_alg_disable_interrupts();
        g_flash_sus++;
        if(g_flash_sus == 1)
        {
            duet_flash_alg_abort_en();
            duet_flash_alg_check_abort_busy();

            FLASH->QSPI_CCR = 0x0000175;//dumode=2'b0,fmode=2'b00,dmode=2'b00,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h75;
            FLASH->SBUS_START = 0x1;
            duet_flash_alg_check_busy();
            duet_flash_alg_clr_flg();

            duet_flash_alg_polling_wip(0);
            g_flash_polling = 0;;

            duet_flash_alg_load_cfg ();
            duet_flash_alg_enable_cache();
        }
        duet_flash_alg_enable_interrupts();
    }
}

FLASH_DRIVER_SEG void duet_flash_alg_resume (void)
{
    if(g_flash_wip)
    {
        duet_flash_alg_disable_interrupts();
        g_flash_sus--;
        if(g_flash_sus == 0)
        {
            duet_flash_alg_disable_cache();

            duet_flash_alg_abort_en();
            duet_flash_alg_check_busy();

            FLASH->QSPI_CCR = 0x000017A;//dumode=2'b0,fmode=2'b00,dmode=2'b00,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h7A;
            duet_flash_alg_clr_flg();
            FLASH->SBUS_START = 0x1;
            duet_flash_alg_check_busy();
        }
        duet_flash_alg_enable_interrupts();
    }
}


#if defined(_SPI_FLASH_120MHz_) || defined(CFG_OPTIMIZE_CHANNEL_SENSITIVITY)
FLASH_DRIVER_SEG void duet_flash_alg_set_clk_120(unsigned char en_120m)
{
    duet_flash_alg_disable_interrupts();
    duet_flash_alg_disable_cache();
    if(en_120m)
    {
        //switch flash clock source to XO 52M
        REG_WR(WIFI_BLE_FLASH_CLK_CTRL_REG, (REG_RD(WIFI_BLE_FLASH_CLK_CTRL_REG) & (~(1<<7))));

        duet_flash_clk_cfg = 120;
        duet_flash_alg_load_cfg();

        //flash clock is source/4
        REG_WR(APB_PERI_CLK_CTRL_REG, ((REG_RD(APB_PERI_CLK_CTRL_REG) & ~(0x3)) | 0x2));
        //switch flash clock source to RF 480M
        REG_WR(WIFI_BLE_FLASH_CLK_CTRL_REG, (REG_RD(WIFI_BLE_FLASH_CLK_CTRL_REG) | (1<<7)));
    }
    else
    {
        //switch flash clock source to XO 52M
        REG_WR(WIFI_BLE_FLASH_CLK_CTRL_REG, (REG_RD(WIFI_BLE_FLASH_CLK_CTRL_REG) & (~(1<<7))));

        duet_flash_clk_cfg = 52;
        duet_flash_alg_load_cfg();
    }
    duet_flash_alg_enable_cache();
    duet_flash_alg_enable_interrupts();
}
#endif

#ifdef _SPI_FLASH_240MHz_
FLASH_DRIVER_SEG void duet_flash_alg_set_clk_240(unsigned char en_240m)
{
    duet_flash_alg_disable_interrupts();
    duet_flash_alg_disable_cache();

    if(en_240m)
    {
        //switch flash clock source to XO 52M
        REG_WR(WIFI_BLE_FLASH_CLK_CTRL_REG, (REG_RD(WIFI_BLE_FLASH_CLK_CTRL_REG) & (~(1<<7))));

        duet_flash_clk_cfg = 240;
        duet_flash_alg_load_cfg();

        //0x0:flash clock = source APLL 480MHZ / 2
        REG_WR(APB_PERI_CLK_CTRL_REG, ((REG_RD(APB_PERI_CLK_CTRL_REG) & ~(0x3)) | 0x0));
        //switch flash clock source to RF 480M
        REG_WR(WIFI_BLE_FLASH_CLK_CTRL_REG, (REG_RD(WIFI_BLE_FLASH_CLK_CTRL_REG) | (1<<7)));
    }
    else
    {
        //switch flash clock source to XO 52M
        REG_WR(WIFI_BLE_FLASH_CLK_CTRL_REG, (REG_RD(WIFI_BLE_FLASH_CLK_CTRL_REG) & (~(1<<7))));

        duet_flash_clk_cfg = 52;
        duet_flash_alg_load_cfg();
    }

    duet_flash_alg_enable_cache();
    duet_flash_alg_enable_interrupts();
}
#endif

FLASH_DRIVER_SEG int duet_flash_alg_read_id(void){
    int var_rdata;
    int dshift;

    duet_flash_alg_disable_interrupts();
    duet_flash_alg_disable_cache();

    duet_flash_alg_abort_en();
    duet_flash_alg_check_abort_busy();

    FLASH->QSPI_CCR = 0x500019F;//fmode=2'b01,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h9F;
    FLASH->QSPI_DLR = 0x2;
    duet_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;

    duet_flash_alg_f_delay(10);
    duet_flash_alg_check_busy();
    var_rdata = FLASH->QSPI_CR;
    dshift = (var_rdata>>13)&0x7;
    duet_flash_alg_f_delay(dshift);

    var_rdata = FLASH->QSPI_DR;

    duet_flash_alg_load_cfg();

    duet_flash_alg_enable_cache();
    duet_flash_alg_enable_interrupts();

    return(var_rdata);
}

#if 0   //no used and with some fault!!!
FLASH_DRIVER_SEG void duet_flash_alg_calibrate_shift(int prescaler)
{
    volatile int delay_shift = 0;
    volatile int sshift = 0;
    unsigned int rdata;
    unsigned int wdata;
    unsigned int identi;
    volatile int id, mem_type, capacity;
    int i = 0;
    int sshift_max[8]={0};
    int sshift_min[8]={0};
    int match_flag[8]={0};
    int match_interval[8] = {0};

    //*(volatile int *)(0x40000808) = 0x2;
    REG_WR(CLK_FLASH_SEL,FLASH_240_CLK);

    for(delay_shift = 0; delay_shift < 8; delay_shift = delay_shift +1)
    {
        sshift_max[delay_shift]=0;
        sshift_min[delay_shift]=0;
        for(sshift = 0; sshift < prescaler; sshift = sshift +1)
        {
            rdata = FLASH->QSPI_CR;
            wdata = rdata&0x87FF1FFF;
            wdata |= (sshift&0xF) <<27;
            wdata |= (delay_shift&0x7) <<13;
            FLASH->QSPI_CR = wdata;
            //read_id
            identi = duet_flash_alg_read_id();

            id = identi&0xFF;
            mem_type = (identi&0xFF00)>>8;
            capacity = (identi&0xFF0000)>>16;

            if(id == 0x0B && mem_type == 0x40 && capacity == 0x15)
            {
                match_flag[delay_shift]=0x1;
                if(sshift_max[delay_shift] < sshift)
                    sshift_max[delay_shift] = sshift;
                if(sshift_min[delay_shift] > sshift)
                    sshift_min[delay_shift]=sshift;
            }
            else
            {
                sshift_min[delay_shift]=sshift + 1;
                if(sshift_min[delay_shift]==prescaler)
                {
                    sshift_min[delay_shift] = 0;
                }
            }
        }
    }

    for(i = 0;i< 8;i++)
    {
        if(match_flag[i] == 1)
            match_interval[i]= sshift_max[i]-sshift_min[i]+1;
        else{
            match_interval[i] = 0;
        }
    }
    //get the best delay_shift(means max mactch_interval
    delay_shift = 0;
    for(i = 1;i < 8;i = i+1)
    {
        if(match_interval[i] > match_interval[delay_shift])
        {
            delay_shift = i;
        }
    }
    sshift = (sshift_max[delay_shift] + sshift_min[delay_shift])/2 + (sshift_max[delay_shift] + sshift_min[delay_shift])%2;
    rdata = FLASH->QSPI_CR;
    wdata = rdata&0x87FF1FFF;
    wdata |= (sshift&0xF) <<27;
    wdata |= (delay_shift&0x7) <<13;
    FLASH->QSPI_CR = wdata;
}
#endif
