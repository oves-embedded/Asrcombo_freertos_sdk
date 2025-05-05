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
#include <string.h>
#include "duet_cm4.h"
#include "duet.h"
#include "duet_i2c.h"
#include "duet_pinmux.h"
#include "duet_dma.h"

//duet_i2c_slv_callback_t g_duet_i2c_slv_callback_handler[DUET_I2C_NUM];

duet_i2c_priv_cfg_t duet_i2c_priv_cfg =
{
    .speed_mode = I2C_MODE_STANDARD,
    .fifo_mode = I2C_MST_FIFO_MODE_ENABLE,
    .dma_mode = I2C_DMA_DISABLE,
};
void I2C0_IRQHandler(void)
{
    duet_intrpt_enter();
    if( (I2C0->SR) & I2C_STATUS_TRANS_DONE)
    {
//        printf("I2C0 trans done interrupt\n");
        I2C0->CR &= (~I2C_CR_SEND_STOP);
        I2C0->SR = I2C_STATUS_TRANS_DONE;
    }
    if( (I2C0->SR) & I2C_STATUS_SLAVE_ADDR_DET)
    {
        printf("I2C0 slave addr det\n");
        I2C0->SR = I2C_STATUS_SLAVE_ADDR_DET;
    }
    if( (I2C0->SR) & I2C_STATUS_RX_FIFO_FULL)
    {
        printf("I2C0 I2C_STATUS_RX_FIFO_FULL\n");
        I2C0->SR = I2C_STATUS_RX_FIFO_FULL;
    }
}

void I2C1_IRQHandler(void)
{
    duet_intrpt_enter();

    if( (I2C1->SR) & I2C_STATUS_TRANS_DONE)
    {
        printf("I2C1 trans done interrupt\n");
        I2C1->SR = I2C_STATUS_TRANS_DONE;
    }
    if( (I2C1->SR) & I2C_STATUS_SLAVE_ADDR_DET)
    {
        printf("I2C1 slave addr det\n");
        I2C1->SR = I2C_STATUS_SLAVE_ADDR_DET;
    }
    if( (I2C1->SR) & I2C_STATUS_RX_FIFO_HALF_FULL)
    {
        printf("I2C1 RX_FIFO_HALF_FULL\n");
        I2C1->SR = I2C_STATUS_RX_FIFO_HALF_FULL;
    }
}

ITstatus i2c_get_flag_status(I2C_TypeDef * I2Cx, uint32_t I2C_flag)
{

    if(I2Cx->SR & I2C_flag)
        return SET;
    else
        return RESET;

}

ITstatus i2c_get_wfifo_status(I2C_TypeDef * I2Cx, uint32_t I2C_flag)
{

    if(I2Cx->WFIFO_STATUS & I2C_flag)
        return SET;
    else
        return RESET;

}

ITstatus i2c_get_rfifo_status(I2C_TypeDef * I2Cx, uint32_t I2C_flag)
{

    if(I2Cx->RFIFO_STATUS & I2C_flag)
        return SET;
    else
        return RESET;

}

static int8_t duet_i2c_wait_flag(I2C_TypeDef * I2Cx, uint32_t flag)
{
    uint16_t timeout = 0;

    while( !i2c_get_flag_status(I2Cx, flag) )
    {
        if( i2c_get_flag_status(I2Cx, I2C_STATUS_BUS_ERROR_DET) )
            return -EBUSERR; //bus error
        timeout++;
        if(timeout > TIME_OUT)
            return -ETIMEOUT; // timeout
    }
    return 0;
}

static int8_t duet_i2c_wait_wfifo_flag_unset(I2C_TypeDef * I2Cx, uint32_t flag)
{
    uint16_t timeout = 0;

    while(i2c_get_wfifo_status(I2Cx, flag))
    {
        if( i2c_get_flag_status(I2Cx, I2C_STATUS_BUS_ERROR_DET) )
            return -EBUSERR; //bus error
        timeout++;
        if(timeout > TIME_OUT)
            return -ETIMEOUT; // timeout
    }
    return 0;
}

static int8_t duet_i2c_wait_rfifo_flag_unset(I2C_TypeDef * I2Cx, uint32_t flag)
{
    uint16_t timeout = 0;

    while(i2c_get_rfifo_status(I2Cx, flag))
    {
        if( i2c_get_flag_status(I2Cx, I2C_STATUS_BUS_ERROR_DET) )
            return -EBUSERR; //bus error
        timeout++;
        if(timeout > TIME_OUT)
            return -ETIMEOUT; // timeout
    }
    return 0;
}

void duet_i2c_interrupt_config(I2C_TypeDef * I2Cx, uint32_t I2C_interrupt_en, int8_t new_state)
{
    if(new_state == ENABLE)
    {
        I2Cx->CR |= I2C_interrupt_en;
    }
    else
    {
        I2Cx->CR &= ~I2C_interrupt_en;
    }
}

I2C_TypeDef *getI2cxViaIdx(uint8_t i2c_idx)
{
    switch(i2c_idx)
    {
        case I2C_DEVICE0:
            return I2C0;
        case I2C_DEVICE1:
            return I2C1;
        default:
            return NULL;
    }
}

/**
 * I2c device reset
 *
 * @param[in]   I2Cx      i2c typedef
 *
 * @return  0 : on success, EIO : if an error occurred during reset
 *
 */
int32_t duet_i2c_reset( I2C_TypeDef * I2Cx)
{
    uint32_t temp;
    if(I2Cx == I2C0)
    {
        REG_WR(PERI_SOFT_RESET_REG_BASE1, (I2C0_BUS_CLOCK_ENABLE | I2C0_PERI_CLOCK_ENABLE));
        REG_WR(PERI_SOFT_RESET_CLEAR_REG_BASE1, (I2C0_BUS_CLOCK_ENABLE | I2C0_PERI_CLOCK_ENABLE));
    }
    else if(I2Cx == I2C1)
    {
        REG_WR(PERI_SOFT_RESET_REG_BASE1, (I2C1_BUS_CLOCK_ENABLE | I2C1_PERI_CLOCK_ENABLE));
        REG_WR(PERI_SOFT_RESET_CLEAR_REG_BASE1, (I2C1_BUS_CLOCK_ENABLE | I2C1_PERI_CLOCK_ENABLE));
    }
    else
    {
        return EIO;
    }

    /* check unit busy */
    temp = I2C_WAIT_FOREVER;
    while( i2c_get_flag_status(I2Cx, I2C_STATUS_UNIT_BUSY) )
    {
        temp --;
    }

    if( temp )
    {
        I2Cx->CR &= I2C_UNIT_RESET; //clear rest of CR
        I2Cx->CR |= I2C_UNIT_RESET;  //set RESET bit
        I2Cx->SR = 0;
        I2Cx->CR &= ~I2C_UNIT_RESET; // clear RESET bit
    }
    else
        return EIO;

    return 0;
}

/**
 * Initialises an I2C interface
 * Prepares an I2C hardware interface for communication as a master or slave
 *
 * @param[in]  i2c  the device for which the i2c port should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t duet_i2c_init(duet_i2c_dev_t *i2c)
{
    I2C_TypeDef * I2Cx = NULL;
    if(!i2c)
        return -EI2CNUMERR;

    I2Cx = getI2cxViaIdx(i2c->port);
    if(!I2Cx)
        return -EI2CNUMERR;

    // pinmux config
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0, set PAD2,3 for i2c0 func 4.
    {
#if I2C0_PAD_GROUP0_ENABLE
        duet_pinmux_config(PAD2, PF_I2C0);
        duet_pinmux_config(PAD3, PF_I2C0);
#endif
#if I2C0_PAD_GROUP1_ENABLE
        duet_pinmux_config(PAD20, PF_I2C0);
        duet_pinmux_config(PAD21, PF_I2C0);
#endif
    }
    else
    {
#if I2C1_PAD_GROUP0_ENABLE
        duet_pinmux_config(PAD8, PF_I2C1);
        duet_pinmux_config(PAD9, PF_I2C1);
#endif
#if I2C1_PAD_GROUP1_ENABLE
        duet_pinmux_config(PAD22, PF_I2C1);
        duet_pinmux_config(PAD23, PF_I2C1);
#endif
    }

    // I2C clock enable
    if(I2C_DEVICE0 == i2c->port)
    {
        REG_WR(PERI_CLK_EN_REG1, (I2C0_BUS_CLOCK_ENABLE | I2C0_PERI_CLOCK_ENABLE));
    }
    else
    {
        REG_WR(PERI_CLK_EN_REG1, (I2C1_BUS_CLOCK_ENABLE | I2C1_PERI_CLOCK_ENABLE));
    }

    /* reset unit */
    if(duet_i2c_reset(I2Cx))
        return EIO;

    switch(i2c->config.freq)
    {
    case I2C_STANDARD_SPEED:
        duet_i2c_priv_cfg.speed_mode = I2C_MODE_STANDARD;
        break;
    case I2C_FAST_SPEED:
        duet_i2c_priv_cfg.speed_mode = I2C_MODE_FAST;
        break;
    case I2C_HIGH_SPEED:
        duet_i2c_priv_cfg.speed_mode = I2C_MODE_HIGH_SPEED_0;
        break;
    default:
        break;
    }

    if (i2c->config.mode == I2C_MASTER)
    {
        I2Cx->SAR = i2c->config.dev_addr; // set unit address as slave

        I2Cx->CR &= ~I2C_MODE_SET_MASK; // reset speed mode to 0
        I2Cx->CR |= (duet_i2c_priv_cfg.speed_mode << I2C_MODE_SET_POS); // set speed mode
        I2Cx->LCR = 0;
        I2Cx->LCR = (((I2C_CLK / I2C_STANDARD_SPEED - 8) / 2) | (((I2C_CLK / I2C_FAST_SPEED - 8) / 2 - 1) << 9) | (((I2C_CLK / I2C_HIGH_SPEED - 9) / 2) << 18) | (((I2C_CLK / I2C_HIGH_SPEED - 9) / 2) << 27)); // set divider
        // set wait count value to adjust clock for standart and fast mode
        I2Cx->WCR = (((I2C_CLK / I2C_FAST_SPEED - 8) / 2 - 1) / 3);
        I2Cx->CR |= duet_i2c_priv_cfg.fifo_mode;  // set FIFO mode

        I2Cx->CR |= I2C_UNIT_ENABLE | I2C_SCL_ENABLE; // scl driving enable & unit enable

        /*        duet_i2c_interrupt_config(I2Cx, I2C_INTERRUPT_SLAVE_ADDR_DET_EN | I2C_INTERRUPT_TRANS_DONE_EN | I2C_INTERRUPT_RX_FIFO_FULL_EN | I2C_INTERRUPT_BUS_ERROR_DET_EN \
                                          | I2C_INTERRUPT_MASTER_STOP_DET_EN, ENABLE);//master
        */
    }
    else
    {
        /* i2c as slave */
        I2Cx->SAR = i2c->config.dev_addr; // set unit address as slave
        I2Cx->CR &= ~I2C_MODE_SET_MASK; // reset speed mode to 0
        I2Cx->CR |= (duet_i2c_priv_cfg.speed_mode << I2C_MODE_SET_POS); // set speed mode
//            I2Cx->CR |= pI2C_InitStrcut->i2c_mst_fifo_mode;  // FIFO mode is not for slave mode, so this has no effect
        I2Cx->CR |= I2C_INTERRUPT_SLAVE_ADDR_DET_EN | I2C_INTERRUPT_RX_FIFO_FULL_EN | I2C_INTERRUPT_RX_BUFER_FULL_EN | I2C_INTERRUPT_SLAVE_STOP_DET_EN | I2C_INTERRUPT_TRANS_DONE_EN | I2C_INTERRUPT_TX_BUFFER_EMPTY_EN; //master read
        I2Cx->CR |= I2C_UNIT_ENABLE; // unit enable

        duet_i2c_interrupt_config(I2Cx, I2C_INTERRUPT_SLAVE_ADDR_DET_EN | I2C_INTERRUPT_RX_FIFO_FULL_EN | I2C_INTERRUPT_RX_BUFER_FULL_EN | I2C_INTERRUPT_SLAVE_STOP_DET_EN \
                                  | I2C_INTERRUPT_TRANS_DONE_EN | I2C_INTERRUPT_TX_BUFFER_EMPTY_EN, ENABLE);//slave
    }
    if(duet_i2c_priv_cfg.dma_mode == I2C_DMA_ENABLE)
    {
        I2Cx->CR |= I2C_DMA_ENABLE;
    }

    //enable cm4 interrupt
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        REG_WR(DUTE_IRQ_EN_REG, I2C0_IRQ_ENABLE); //I2C0 IRQ enable
        NVIC_EnableIRQ(I2C0_IRQn); //enable I2C0 interrupt
    }
    else
    {
        REG_WR(DUTE_IRQ_EN_REG, I2C1_IRQ_ENABLE); //I2C1 IRQ enable
        NVIC_EnableIRQ(I2C1_IRQn); //enable I2C1 interrupt
    }

    /* check the bus busy after unit enable */
    if( i2c_get_flag_status(I2Cx, I2C_STATUS_BUS_BUSY) )
        return EIO;
    else
        return 0;
}

/**
 * I2c master send
 *
 * @param[in]  i2c       the i2c device
 * @param[in]  dev_addr  device address
 * @param[in]  pwdata    i2c send data
 * @param[in]  wlen      i2c send data size
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
SEG_I2C int32_t duet_i2c_master_send(duet_i2c_dev_t *i2c, uint8_t dev_addr, const uint8_t *pwdata, uint32_t wlen)
{
    int8_t ret = 0;
    int i = 0;
    I2C_TypeDef * I2Cx = NULL;

    if(i2c->port >= DUET_I2C_NUM)
        return -EI2CNUMERR;
    I2Cx = getI2cxViaIdx(i2c->port);

    if(!wlen)
        return -EDATAERR;

    I2Cx->CR &= (~I2C_CR_SEND_STOP);
    I2Cx->CR &= (~I2C_CR_SEND_NACK);
    // set TXBEGIN bit before starting another transfer
    I2Cx->CR |= I2C_TRANS_BEGIN;
    // send slave address first
    I2Cx->WFIFO = (dev_addr << 1) | I2C_WRITE | I2C_SEND_START | I2C_TB;
    I2Cx->SR = I2C_STATUS_TX_FIFO_EMPTY | I2C_STATUS_TRANS_DONE;
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TX_FIFO_EMPTY);
    if(ret)
        return ret;

    // send write cmd
    while(i < wlen)
    {
        ret = duet_i2c_wait_wfifo_flag_unset(I2Cx, I2C_WFIFO_STATUS_FULL);
        if(ret)
            return ret;
        if((wlen - 1) == i)
        {

            I2Cx->WFIFO = *pwdata | I2C_TB | I2C_SEND_STOP;
        }
        else
        {
            I2Cx->WFIFO = (*pwdata++) | I2C_TB;
        }
        i++;
    }
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TRANS_DONE);
    if(ret)
        return ret;
    return 0;
}

/**
 * I2c master receive
 *
 * @param[in]  i2c       the i2c device
 * @param[in]  dev_addr  device address
 * @param[out] rdata     i2c receive data
 * @param[in]  rlen      i2c receive data size
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
SEG_I2C int32_t duet_i2c_master_recv(duet_i2c_dev_t *i2c, uint8_t dev_addr, uint8_t *rdata, uint32_t rlen)
{
    int8_t ret = 0;
    int i = 0;
    I2C_TypeDef * I2Cx = NULL;

    if(i2c->port >= DUET_I2C_NUM)
        return -EI2CNUMERR;
    I2Cx = getI2cxViaIdx(i2c->port);

    if(!rlen)
        return -EDATAERR;

    I2Cx->CR &= (~I2C_CR_SEND_STOP);
    I2Cx->CR &= (~I2C_CR_SEND_NACK);
    // set TXBEGIN bit before starting another transfer
    I2Cx->CR |= I2C_TRANS_BEGIN;

    // send slave address first
    I2Cx->WFIFO = (dev_addr << 1) | I2C_READ | I2C_SEND_START | I2C_TB;
    I2Cx->SR = I2C_STATUS_TX_FIFO_EMPTY | I2C_STATUS_TRANS_DONE;
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TX_FIFO_EMPTY);
    if(ret)
        return ret;

    while(i < rlen)
    {
        ret = duet_i2c_wait_wfifo_flag_unset(I2Cx, I2C_WFIFO_STATUS_FULL);
        if(ret)
            return ret;
        if((rlen - 1) == i)
        {
            I2Cx->WFIFO = I2C_SEND_STOP | I2C_SEND_NACK | I2C_TB;
        }
        else
        {
            I2Cx->WFIFO = I2C_TB;
        }
        ret = duet_i2c_wait_rfifo_flag_unset(I2Cx, I2C_RFIFO_STATUS_EMPTY);
        if(ret)
            return ret;
        *(rdata + i) = I2Cx->RFIFO & 0xFF;
        i++;
    }
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TRANS_DONE);
    if(ret)
        return ret;
    return 0;
}

/**
 * I2c master repeated write read
 *
 * @param[in]  i2c       the i2c device
 * @param[in]  dev_addr  device address
 * @param[in]  pwdata    i2c send data
 * @param[out] rdata     i2c receive data
 * @param[in]  wlen      i2c send data size
 * @param[in]  rlen      i2c receive data size
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t duet_i2c_master_repeated_write_read(duet_i2c_dev_t *i2c, uint8_t dev_addr, const uint8_t * pwdata, uint8_t * rdata, uint32_t wlen, uint32_t rlen)
{
    /**** master write ****/
    int8_t ret = 0;
    int i = 0;
    I2C_TypeDef * I2Cx =NULL;

    if(i2c->port >= DUET_I2C_NUM)
        return -EI2CNUMERR;
    I2Cx = getI2cxViaIdx(i2c->port);

    if((!wlen) | (!rlen))
        return -EDATAERR;

    I2Cx->CR &= (~I2C_CR_SEND_STOP);
    I2Cx->CR &= (~I2C_CR_SEND_NACK);
    // set TXBEGIN bit before starting another transfer
    I2Cx->CR |= I2C_TRANS_BEGIN;

    // send slave address first
    I2Cx->WFIFO = (dev_addr << 1) | I2C_WRITE | I2C_SEND_START | I2C_TB;
    I2Cx->SR = I2C_STATUS_TX_FIFO_EMPTY | I2C_STATUS_TRANS_DONE;
    //wait till tx fifo is empty to avoid overflowing tx fifo
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TX_FIFO_EMPTY);
    if(ret)
        return ret;

    // send write cmd
    while(i < wlen)
    {
        ret = duet_i2c_wait_wfifo_flag_unset(I2Cx, I2C_WFIFO_STATUS_FULL);
        if(ret)
            return ret;
        if((wlen - 1) == i)
        {
            I2Cx->WFIFO = *pwdata | I2C_TB;
        }
        else
        {
            I2Cx->WFIFO = (*pwdata++) | I2C_TB;
        }
        i++;
    }
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TX_FIFO_EMPTY);
    if(ret)
        return ret;

    /**** master read ****/
    // send slave address first
    I2Cx->WFIFO = (dev_addr << 1) | I2C_READ | I2C_SEND_START | I2C_TB;
    I2Cx->SR = I2C_STATUS_TX_FIFO_EMPTY | I2C_STATUS_TRANS_DONE;
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TX_FIFO_EMPTY);
    if(ret)
        return ret;

    i = 0;
    while(i < rlen)
    {
        ret = duet_i2c_wait_wfifo_flag_unset(I2Cx, I2C_WFIFO_STATUS_FULL);
        if(ret)
            return ret;
        if((rlen - 1) == i)
        {
            I2Cx->WFIFO = I2C_SEND_STOP | I2C_SEND_NACK | I2C_TB;
        }
        else
        {
            I2Cx->WFIFO = I2C_TB;
        }
        ret = duet_i2c_wait_rfifo_flag_unset(I2Cx, I2C_RFIFO_STATUS_EMPTY);
        if(ret)
            return ret;
        *(rdata + i) = I2Cx->RFIFO & 0xFF;
        i++;
    }
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TRANS_DONE);
    if(ret)
        return ret;
    return 0;
}


/**
 * I2c mem write
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       mem address
 * @param[in]  pwdata         i2c master send data
 * @param[in]  wlen           i2c master send data size
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t duet_i2c_mem_write(duet_i2c_dev_t *i2c, uint8_t dev_addr, uint8_t mem_addr, const uint8_t *pwdata, uint16_t wlen)
{
    int8_t ret = 0;
    int i = 0;
    I2C_TypeDef * I2Cx = NULL;

    if(i2c->port >= DUET_I2C_NUM)
        return -EI2CNUMERR;
    I2Cx = getI2cxViaIdx(i2c->port);

    if(!wlen)
        return -EDATAERR;

    I2Cx->CR &= (~I2C_CR_SEND_STOP);
    I2Cx->CR &= (~I2C_CR_SEND_NACK);
    // set TXBEGIN bit before starting another transfer
    I2Cx->CR |= I2C_TRANS_BEGIN;

    // send slave address first
    I2Cx->WFIFO = (dev_addr << 1) | I2C_WRITE | I2C_SEND_START | I2C_TB;
    I2Cx->SR = I2C_STATUS_TX_FIFO_EMPTY | I2C_STATUS_TRANS_DONE;
    //wait till tx fifo is empty to avoid overflowing tx fifo
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TX_FIFO_EMPTY);
    if(ret)
        return ret;

    // send memory address
    I2Cx->WFIFO = mem_addr | I2C_TB;

    // send write cmd
    while(i < wlen)
    {
        ret = duet_i2c_wait_wfifo_flag_unset(I2Cx, I2C_WFIFO_STATUS_FULL);
        if(ret)
            return ret;
        if((wlen - 1) == i)
        {
            I2Cx->WFIFO = *pwdata | I2C_TB | I2C_SEND_STOP;
        }
        else
        {
            I2Cx->WFIFO = (*pwdata++) | I2C_TB;
        }
        i++;
    }
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TRANS_DONE);
    if(ret)
        return ret;
    return 0;
}

/**
 * I2c master mem read
 *
 * @param[in]   i2c            the i2c device
 * @param[in]   dev_addr       device address
 * @param[in]   mem_addr       mem address
 * @param[out]  rdata          i2c master receive data
 * @param[in]   rlen           i2c master receive data size
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t duet_i2c_mem_read(duet_i2c_dev_t *i2c, uint8_t dev_addr, uint8_t mem_addr, uint8_t *rdata, uint16_t rlen)
{
    int8_t ret = 0;
    int i = 0;
    I2C_TypeDef * I2Cx =NULL;

    if(i2c->port >= DUET_I2C_NUM)
        return -EI2CNUMERR;
    I2Cx = getI2cxViaIdx(i2c->port);

    if(!rlen)
        return -EDATAERR;

    I2Cx->CR &= (~I2C_CR_SEND_STOP);
    I2Cx->CR &= (~I2C_CR_SEND_NACK);
    // set TXBEGIN bit before starting another transfer
    I2Cx->CR |= I2C_TRANS_BEGIN;

    // send slave address first
    I2Cx->WFIFO = (dev_addr << 1) | I2C_WRITE | I2C_SEND_START | I2C_TB;
    I2Cx->SR = I2C_STATUS_TX_FIFO_EMPTY | I2C_STATUS_TRANS_DONE;
    //wait till tx fifo is empty to avoid overflowing tx fifo
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TX_FIFO_EMPTY);
    if(ret)
        return ret;

    // send memory address
    I2Cx->WFIFO = mem_addr | I2C_TB;

    /**** master read ****/
    // send slave address
    I2Cx->WFIFO = (dev_addr << 1) | I2C_READ | I2C_SEND_START | I2C_TB;
    I2Cx->SR = I2C_STATUS_TX_FIFO_EMPTY | I2C_STATUS_TRANS_DONE;
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TX_FIFO_EMPTY);
    if(ret)
        return ret;

    while(i < rlen)
    {
        ret = duet_i2c_wait_wfifo_flag_unset(I2Cx, I2C_WFIFO_STATUS_FULL);
        if(ret)
            return ret;
        if((rlen - 1) == i)
        {
            I2Cx->WFIFO = I2C_SEND_STOP | I2C_SEND_NACK | I2C_TB;
        }
        else
        {
            I2Cx->WFIFO = I2C_TB;
        }
        ret = duet_i2c_wait_rfifo_flag_unset(I2Cx, I2C_RFIFO_STATUS_EMPTY);
        if(ret)
            return ret;
        *(rdata + i) = I2Cx->RFIFO & 0xFF;
        i++;
    }
    ret = duet_i2c_wait_flag(I2Cx, I2C_STATUS_TRANS_DONE);
    if(ret)
        return ret;
    return 0;

}

/**
 * Deinitialises an I2C device
 *
 * @param[in]  i2c  the i2c device
 *
 * @return  0 : on success, EIO : if an error occurred during deinitialisation
 */
int32_t duet_i2c_finalize(duet_i2c_dev_t *i2c)
{
    I2C_TypeDef * I2Cx = NULL;
    if(!i2c)
        return -EI2CNUMERR;

    I2Cx = getI2cxViaIdx(i2c->port);
    if(!I2Cx)
        return -EI2CNUMERR;

    //reset i2c
    if(I2Cx == I2C0)
    {
        REG_WR(PERI_SOFT_RESET_REG_BASE1, (I2C0_BUS_CLOCK_ENABLE | I2C0_PERI_CLOCK_ENABLE));
        REG_WR(PERI_SOFT_RESET_CLEAR_REG_BASE1, (I2C0_BUS_CLOCK_ENABLE | I2C0_PERI_CLOCK_ENABLE));
    }
    else
    {
        REG_WR(PERI_SOFT_RESET_REG_BASE1, (I2C1_BUS_CLOCK_ENABLE | I2C1_PERI_CLOCK_ENABLE));
        REG_WR(PERI_SOFT_RESET_CLEAR_REG_BASE1, (I2C1_BUS_CLOCK_ENABLE | I2C1_PERI_CLOCK_ENABLE));
    }

    // disable i2c cm4 irq
    if(I2Cx == I2C0) // I2C_DEVICE0
    {
        REG_WR(DUTE_IRQ_DIS_REG, I2C0_IRQ_DISABLE);
        NVIC_DisableIRQ(I2C0_IRQn); //disable I2C0 interrupt
    }
    else
    {
        REG_WR(DUTE_IRQ_DIS_REG, I2C1_IRQ_DISABLE);
        NVIC_DisableIRQ(I2C1_IRQn); //disable I2C1 interrupt
    }

    //i2c clk disable
    if(I2Cx == I2C0) // I2C_DEVICE0
    {
        //I2C0 clock disable
        REG_WR(PERI_CLK_DIS_REG1, (I2C0_BUS_CLOCK_DISABLE | I2C0_PERI_CLOCK_DISABLE));
    }
    else // I2C_DEVICE1
    {
        //I2C1 clock disable
        REG_WR(PERI_CLK_DIS_REG1, (I2C1_BUS_CLOCK_DISABLE | I2C1_PERI_CLOCK_DISABLE));
    }
    //callback function pointer clear
    //g_duet_i2c_slv_callback_handler[i2c->port].tx_func = NULL;
    //g_duet_i2c_slv_callback_handler[i2c->port].rx_func = NULL;

    return 0;
}

/**
 * I2c master dma send
 *
 * @param[in]   iic_idx        i2c tx channel
 * @param[in]   data           i2c master dma send data
 * @param[in]   len            i2c master dma send data size
 *
 */
void duet_i2c_master_dma_send(uint8_t iic_idx, uint32_t *data, uint16_t len)
{
    uint8_t dma_chan = 0;
    I2C_TypeDef *I2Cx = 0;

    if( iic_idx == 0)
    {
        dma_chan = 12;//i2c0 tx channel
        I2Cx = I2C0;
    }
    else if(iic_idx == 1)
    {
        dma_chan = 14;//i2c1 tx channel
        I2Cx = I2C1;
    }
    else
    {
        return;
    }
    // malloc for channel descriptor
    Chan_Cfg_TypeDef * pChan_Cfg_Align = duet_dma_ctrl_block_init();
    Chan_Ctl_Data_TypeDef ch_ctl_data;
    Chan_Cfg_TypeDef ch_cfg;

    ch_ctl_data.cycle_ctl = DMA_OP_MODE_BASIC;//DMA_OP_MODE_AUTO_REQ;
    ch_ctl_data.n_minus_1 = len - 1;
    ch_ctl_data.R_pow = 0;
    ch_ctl_data.src_inc = DMA_SRC_ADDR_INC_WORD;
    ch_ctl_data.dst_inc = DMA_DST_ADDR_INC_FIX;
    ch_ctl_data.src_size = DMA_SRC_DATA_WIDTH_WORD;
    ch_ctl_data.dst_size = DMA_DST_DATA_WIDTH_WORD;

    ch_cfg.chan_ctr = ch_ctl_data;
    ch_cfg.chan_src_end_ptr = (uint32_t)(data + len - 1);
    ch_cfg.chan_dst_end_ptr = (uint32_t) & (I2Cx->WFIFO);

    (pChan_Cfg_Align + dma_chan)->chan_ctr = ch_cfg.chan_ctr;
    (pChan_Cfg_Align + dma_chan)->chan_src_end_ptr = ch_cfg.chan_src_end_ptr;
    (pChan_Cfg_Align + dma_chan)->chan_dst_end_ptr = ch_cfg.chan_dst_end_ptr;

    NVIC_EnableIRQ(DMA_IRQn);

    DMA->CFG |= 0x1; // dma enable
    DMA_INT_MASK |= (1 << dma_chan); // dma interrupt unmask, write 1
    // set channel to use primary data struct only for basic/auto-request type
    DMA->CHAN_PRI_ALT_CLR |= (1 << dma_chan);
    DMA->CTL_BASE_PTR = (uint32_t)pChan_Cfg_Align;
    //DMA->CHAN_EN_CLR |= ~(1 << dma_chan); // disable other channels
    DMA->CHAN_EN_SET |= (1 << dma_chan); // enable channel 6
}

/**
 * I2c master dma receive
 *
 * @param[in]   iic_idx        i2c rx channel
 * @param[out]  data           i2c master dma receive data
 * @param[in]   len            i2c master dma receive data size
 *
 */
void duet_i2c_master_dma_recv(uint8_t iic_idx, uint32_t *data, uint16_t len)
{
    uint8_t dma_chan = 0;
    I2C_TypeDef *I2Cx = 0;

    if( iic_idx == 0)
    {
        dma_chan = 13;//i2c0 rx channel
        I2Cx = I2C0;
    }
    else if(iic_idx == 1)
    {
        dma_chan = 15;//i2c1 rx channel
        I2Cx = I2C1;
    }
    else
    {
        return;
    }
    // malloc for channel descriptor
    Chan_Cfg_TypeDef * pChan_Cfg_Align = duet_dma_ctrl_block_init();

    Chan_Ctl_Data_TypeDef ch_ctl_data;
    Chan_Cfg_TypeDef ch_cfg;

    ch_ctl_data.cycle_ctl = DMA_OP_MODE_BASIC;//DMA_OP_MODE_AUTO_REQ;
    ch_ctl_data.n_minus_1 = len - 1;
    ch_ctl_data.R_pow = 0;
    ch_ctl_data.src_inc = DMA_SRC_ADDR_INC_FIX;
    ch_ctl_data.dst_inc = DMA_SRC_ADDR_INC_WORD;
    ch_ctl_data.src_size = DMA_SRC_DATA_WIDTH_WORD;
    ch_ctl_data.dst_size = DMA_DST_DATA_WIDTH_WORD;

    ch_cfg.chan_ctr = ch_ctl_data;
    ch_cfg.chan_src_end_ptr = (uint32_t) & (I2Cx->RFIFO);
    ch_cfg.chan_dst_end_ptr = (uint32_t)(data + len - 1);

    (pChan_Cfg_Align + dma_chan)->chan_ctr = ch_cfg.chan_ctr;
    (pChan_Cfg_Align + dma_chan)->chan_src_end_ptr = ch_cfg.chan_src_end_ptr;
    (pChan_Cfg_Align + dma_chan)->chan_dst_end_ptr = ch_cfg.chan_dst_end_ptr;

    NVIC_EnableIRQ(DMA_IRQn);

    DMA->CFG |= 0x1; // dma enable
    DMA_INT_MASK |= (1 << dma_chan); // dma interrupt unmask, write 1
    // set chan1 to use primary data struct only
    DMA->CHAN_PRI_ALT_CLR |= (1 << dma_chan);
    DMA->CTL_BASE_PTR = (uint32_t)pChan_Cfg_Align;
    //DMA->CHAN_EN_CLR |= ~(1 << dma_chan); // disable other channels
    DMA->CHAN_EN_SET |= (1 << dma_chan); // enable channel

}
