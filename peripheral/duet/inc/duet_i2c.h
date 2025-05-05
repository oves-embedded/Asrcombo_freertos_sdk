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

#ifndef _DUET_I2C_H_
#define _DUET_I2C_H_
#include <stdint.h>
#include <errno.h>
#include "duet.h"

#ifdef __cplusplus
extern "C"{
#endif

#define I2C_MASTER                  (0)
#define I2C_SLAVE                   (1)
#define I2C_ROLE_MODE               I2C_MASTER
#define I2C_ADDRESS                 0x66

#define I2C_SCL_STATUS_BIT  (1)
#define I2C_SDA_STATUS_BIT  (0)

// TWSI_NEW
#define I2C_INTERRUPT_RX_FIFO_OVERRUN_EN   (1 << 31)
#define I2C_INTERRUPT_RX_FIFO_FULL_EN      (1 << 30)
#define I2C_INTERRUPT_TX_FIFO_EMPTY_EN     (1 << 28)
#define I2C_INTERRUPT_RX_FIFO_HALF_FULL_EN (1 << 29)
#define I2C_INTERRUPT_TRANS_DONE_EN        (1 << 27)
#define I2C_INTERRUPT_SLAVE_ADDR_DET_EN    (1 << 23)
#define I2C_INTERRUPT_ARBT_LOSS_DET_EN     (1 << 18)
#define I2C_INTERRUPT_MASTER_STOP_DET_EN   (1 << 25)
#define I2C_INTERRUPT_SLAVE_STOP_DET_EN    (1 << 24)
#define I2C_INTERRUPT_BUS_ERROR_DET_EN     (1 << 22)
#define I2C_INTERRUPT_RX_BUFER_FULL_EN     (1 << 20)
#define I2C_INTERRUPT_TX_BUFFER_EMPTY_EN   (1 << 19)

#define I2C_STATUS_RX_FIFO_OVERRUN         (1 << 31)
#define I2C_STATUS_RX_FIFO_FULL            (1 << 30)
#define I2C_STATUS_TX_FIFO_EMPTY           (1 << 28)
#define I2C_STATUS_TX_FIFO_FULL            (0xffff)
#define I2C_STATUS_RX_FIFO_HALF_FULL       (1 << 29)
#define I2C_STATUS_TRANS_DONE              (1 << 27)
#define I2C_STATUS_MASTER_STOP_DET         (1 << 26)
#define I2C_STATUS_SLAVE_STOP_DET          (1 << 24)
#define I2C_STATUS_SLAVE_ADDR_DET          (1 << 23)
#define I2C_STATUS_BUS_ERROR_DET           (1 << 22)
#define I2C_STATUS_RX_BUFER_FULL           (1 << 20)
#define I2C_STATUS_TX_BUFFER_EMPTY         (1 << 19)
#define I2C_STATUS_UNIT_BUSY               (1 << 15)
#define I2C_STATUS_BUS_BUSY                (1 << 16)
#define I2C_STATUS_ACK_NACK                (1 << 14)
#define I2C_STATUS_RW_MODE                 (1 << 13)

#define I2C_WFIFO_STATUS_FULL              (1 << 0)
#define I2C_WFIFO_STATUS_EMPTY             (1 << 1)
#define I2C_WFIFO_STATUS_SIZE              (0xffff << 2)

#define I2C_RFIFO_STATUS_OVERRUN           (1 << 0)
#define I2C_RFIFO_STATUS_HALFFUL           (1 << 1)
#define I2C_RFIFO_STATUS_FULL              (1 << 2)
#define I2C_RFIFO_STATUS_EMPTY             (1 << 3)
#define I2C_RFIFO_STATUS_SIZE              (0xffff << 4)


#define I2C_TRANS_BEGIN                 (1 << 4)
#define I2C_MST_FIFO_MODE_ENABLE        (1 << 5)
#define I2C_MST_FIFO_MODE_DISABLE       (0)

#define I2C_MODE_STANDARD           (0)
#define I2C_MODE_FAST               (1)
#define I2C_MODE_HIGH_SPEED_0       (2)    // not supported in duet
#define I2C_MODE_HIGH_SPEED_1       (3)    // not supported in duet
#define I2C_MODE_SET_POS            (8)
#define I2C_MODE_SET_MASK           (3 << I2C_MODE_SET_POS)
#define I2C_HS_MASTER_CODE          (0x0A)

#define I2C_UNIT_RESET              (1 << 10)
#define I2C_DMA_ENABLE              (1 << 7)
#define I2C_UNIT_ENABLE             (1 << 14)
#define I2C_SCL_ENABLE              (1 << 13)
#define I2C_DMA_DISABLE                0

/* fifo mode control */
#define I2C_TB                      (1 << 11)
#define I2C_SEND_NACK               (1 << 10)
#define I2C_SEND_STOP               (1 << 9)
#define I2C_SEND_START              (1 << 8)
#define I2C_MASTER_ABORT            (1 << 12)

/* non fifo mode control */
#define I2C_CR_TB                   (I2C_TB >> 8)
#define I2C_CR_SEND_NACK            (I2C_SEND_NACK >> 8)
#define I2C_CR_SEND_STOP            (I2C_SEND_STOP >> 8)
#define I2C_CR_SEND_START           (I2C_SEND_START >> 8)

#define I2C_DEVICE0     0
#define I2C_DEVICE1     1
#define DUET_I2C_NUM    2

#define I2C_READ                    (1)
#define I2C_WRITE                   (0)

#define I2C_TX_FIFO_DEPTH           (8)
#define I2C_RX_FIFO_DEPTH           (16)

#define I2C_MEM_ADDR_SIZE_8         (0)
#define I2C_MEM_ADDR_SIZE_16        (1)
#define I2C_MEM_ADDR_SIZE_32        (3)

#define TIME_OUT                    (5000)
#define I2C_WAIT_FOREVER            (0xffffffff)

#define I2C_CLK               system_peri_clk
#define I2C_STANDARD_SPEED    (100000)
#define I2C_FAST_SPEED        (400000)
#define I2C_HIGH_SPEED        (1700000) // not supported in duet

// error number
#define I2C_SUCCESS     0 // success
#define EI2CNUMERR      1 // I2C port number error
#define ETIMEOUT        2 // I2C timeout
#define EBUSERR         3 // I2C bus error
#define EDATAERR        4 // I2C data length error

// I2C0 pad group 0: pad 2,3
// I2C0 pad group 1: pad 20, 21
// I2C1 pad group 0: pad 8, 9
// I2C1 pad group 1: pad 22, 23
#define I2C0_PAD_GROUP0_ENABLE          1
#define I2C0_PAD_GROUP1_ENABLE          0
#define I2C1_PAD_GROUP0_ENABLE          1
#define I2C1_PAD_GROUP1_ENABLE          0

#define SEG_I2C //__attribute__((section("seg_i2c_driver")))

typedef void (*duet_i2c_slv_tx_callback_func)(void);
typedef void (*duet_i2c_slv_rx_callback_func)(uint8_t);
typedef struct
{
    duet_i2c_slv_tx_callback_func tx_func;
    duet_i2c_slv_rx_callback_func rx_func;
} duet_i2c_slv_callback_t;

typedef struct
{
    uint32_t address_width;
    uint32_t freq;
    uint8_t  mode;
    uint16_t dev_addr;
} duet_i2c_config_t;

typedef struct
{
    uint8_t     speed_mode;
    uint8_t     fifo_mode;
    uint8_t     dma_mode;
    uint8_t     reserved;
} duet_i2c_priv_cfg_t;

typedef struct
{
    uint8_t      port;    /* i2c port */
    duet_i2c_config_t config;  /* i2c config */
    void        *priv;    /* priv data */
} duet_i2c_dev_t;


/**
 * I2c device reset
 *
 * @param[in]   I2Cx      i2c typedef
 *
 * @return  0 : on success, EIO : if an error occurred during reset
 *
 */
int32_t duet_i2c_reset( I2C_TypeDef * I2Cx);

/**
 * Initialises an I2C interface
 * Prepares an I2C hardware interface for communication as a master or slave
 *
 * @param[in]  i2c  the device for which the i2c port should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t duet_i2c_init(duet_i2c_dev_t *i2c);

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
int32_t duet_i2c_master_send(duet_i2c_dev_t *i2c, uint8_t dev_addr, const uint8_t *pwdata, uint32_t wlen);

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
int32_t duet_i2c_master_recv(duet_i2c_dev_t *i2c, uint8_t dev_addr, uint8_t *rdata, uint32_t rlen);

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
int32_t duet_i2c_master_repeated_write_read(duet_i2c_dev_t *i2c, uint8_t dev_addr, const uint8_t * pwdata, uint8_t * rdata, uint32_t wlen, uint32_t rlen);

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
int32_t duet_i2c_mem_write(duet_i2c_dev_t *i2c, uint8_t dev_addr, uint8_t mem_addr, const uint8_t *pwdata, uint16_t wlen);

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
int32_t duet_i2c_mem_read(duet_i2c_dev_t *i2c, uint8_t dev_addr, uint8_t mem_addr, uint8_t *rdata, uint16_t rlen);

/**
 * Deinitialises an I2C device
 *
 * @param[in]  i2c  the i2c device
 *
 * @return  0 : on success, EIO : if an error occurred during deinitialisation
 */
int32_t duet_i2c_finalize(duet_i2c_dev_t *i2c);

/**
 * I2c master dma send
 *
 * @param[in]   iic_idx        i2c tx channel
 * @param[in]   data           i2c master dma send data
 * @param[in]   len            i2c master dma send data size
 *
 */
void duet_i2c_master_dma_send(uint8_t iic_idx, uint32_t *data, uint16_t len);

/**
 * I2c master dma receive
 *
 * @param[in]   iic_idx        i2c rx channel
 * @param[out]  data           i2c master dma receive data
 * @param[in]   len            i2c master dma receive data size
 *
 */
void duet_i2c_master_dma_recv(uint8_t iic_idx, uint32_t *data, uint16_t len);


/* read one byte from fifo or buffer register */
__STATIC_INLINE uint8_t i2c_receive_byte(I2C_TypeDef * I2Cx)
{
    if(I2Cx->CR & I2C_MST_FIFO_MODE_ENABLE)
        return I2Cx->RFIFO;
    else
        return I2Cx->DBR;
}

/* write one byte to fifo or buffer reigster */
__STATIC_INLINE void i2c_write_byte(I2C_TypeDef * I2Cx, uint16_t data)
{
    // data = data_to_write | any_conrol_bit
    if(I2Cx->CR & I2C_MST_FIFO_MODE_ENABLE)
        I2Cx->WFIFO = data;
    else
        I2Cx->DBR = data;
}

/* clear i2c interrupt */
__STATIC_INLINE void i2c_clear_interrupt(I2C_TypeDef * I2Cx, uint32_t I2C_INTR)
{
    I2Cx->SR |= I2C_INTR;
}

/* I2C needs to set TB for transmitting and receiving a byte
   this function is mainly for when I2C is used as a slave
*/
__STATIC_INLINE void i2c_set_tb(I2C_TypeDef * I2Cx)
{
    I2Cx->CR |= I2C_CR_TB;
}

#ifdef __cplusplus
}
#endif
#endif //_DUET_I2C_H_