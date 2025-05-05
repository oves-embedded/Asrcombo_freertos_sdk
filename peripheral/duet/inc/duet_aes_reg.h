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

#ifndef _DUET_AES_REG_H_
#define _DUET_AES_REG_H_

#define REG_IP_RD(addr)              (*(volatile uint32_t *)(addr))

#define IP_INTSTAT1_ADDR   0x6200001C
#define IP_INTSTAT1_OFFSET 0x0000001C
#define IP_INTSTAT1_INDEX  0x00000007
#define IP_INTSTAT1_RESET  0x00000000

#define ASSERT_ERR(val)
#define __INLINE static __attribute__((__always_inline__)) inline
__INLINE uint32_t ip_intstat1_get(void)
{
    return REG_IP_RD(IP_INTSTAT1_ADDR);
}

// field definitions
#define IP_FIFOINTSTAT_BIT             ((uint32_t)0x00008000)
#define IP_FIFOINTSTAT_POS             15
#define IP_TIMESTAMPTGT2INTSTAT_BIT    ((uint32_t)0x00000040)
#define IP_TIMESTAMPTGT2INTSTAT_POS    6
#define IP_TIMESTAMPTGT1INTSTAT_BIT    ((uint32_t)0x00000020)
#define IP_TIMESTAMPTGT1INTSTAT_POS    5
#define IP_FINETGTINTSTAT_BIT          ((uint32_t)0x00000010)
#define IP_FINETGTINTSTAT_POS          4
#define IP_SWINTSTAT_BIT               ((uint32_t)0x00000008)
#define IP_SWINTSTAT_POS               3
#define IP_CRYPTINTSTAT_BIT            ((uint32_t)0x00000004)
#define IP_CRYPTINTSTAT_POS            2
#define IP_SLPINTSTAT_BIT              ((uint32_t)0x00000002)
#define IP_SLPINTSTAT_POS              1
#define IP_CLKNINTSTAT_BIT             ((uint32_t)0x00000001)
#define IP_CLKNINTSTAT_POS             0

#define IP_FIFOINTSTAT_RST             0x0
#define IP_TIMESTAMPTGT2INTSTAT_RST    0x0
#define IP_TIMESTAMPTGT1INTSTAT_RST    0x0
#define IP_FINETGTINTSTAT_RST          0x0
#define IP_SWINTSTAT_RST               0x0
#define IP_CRYPTINTSTAT_RST            0x0
#define IP_SLPINTSTAT_RST              0x0
#define IP_CLKNINTSTAT_RST             0x0
#define IP_INTACK1_ADDR   0x62000020
#define IP_INTACK1_OFFSET 0x00000020
#define IP_INTACK1_INDEX  0x00000008
#define IP_INTACK1_RESET  0x00000000

#define REG_IP_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)
__INLINE void ip_intack1_cryptintack_clearf(uint8_t cryptintack)
{
//    ASSERT_ERR((((uint32_t)cryptintack << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_IP_WR(IP_INTACK1_ADDR, (uint32_t)cryptintack << 2);
}
#define IP_INTCNTL1_ADDR   0x62000018
#define IP_INTCNTL1_OFFSET 0x00000018
#define IP_INTCNTL1_INDEX  0x00000006
#define IP_INTCNTL1_RESET  0x00008003
__INLINE void ip_intcntl1_cryptintmsk_setf(uint8_t cryptintmsk)
{
    //ASSERT_ERR((((uint32_t)cryptintmsk << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_IP_WR(IP_INTCNTL1_ADDR, (REG_IP_RD(IP_INTCNTL1_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)cryptintmsk << 2));
}
#define EM_BASE_ADDR 0x62008000
__INLINE void em_rd(void *sys_addr, uint16_t em_addr, uint16_t len)
{
    memcpy(sys_addr, (void *)(em_addr + EM_BASE_ADDR), len);
}
/// Write bytes to EM
__INLINE void em_wr(void const *sys_addr, uint16_t em_addr, uint16_t len)
{
    memcpy((void *)(em_addr + EM_BASE_ADDR), sys_addr, len);
}

#define EM_ENC_OFFSET          0x400
#define EM_ENC_IN_OFFSET       (EM_ENC_OFFSET)
#define EM_ENC_IN_SIZE         ((uint32_t)(16))
#define EM_ENC_OUT_OFFSET      (EM_ENC_IN_OFFSET + EM_ENC_IN_SIZE)
#define EM_ENC_OUT_SIZE        ((uint32_t)(16))
#define EM_ENC_END             (EM_ENC_OFFSET + EM_ENC_IN_SIZE + EM_ENC_OUT_SIZE)

#define REG_IP_RD(addr)              (*(volatile uint32_t *)(addr))
#define IP_AESCNTL_ADDR   0x620000B0
#define IP_AESCNTL_OFFSET 0x000000B0
#define IP_AESCNTL_INDEX  0x0000002C
#define IP_AESCNTL_RESET  0x00000000
__INLINE uint8_t ip_aescntl_aes_mode_getf(void)
{
    uint32_t localVal = REG_IP_RD(IP_AESCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}
#define REG_IP_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)

#define IP_AESKEY31_0_ADDR   0x620000B4
#define IP_AESKEY31_0_OFFSET 0x000000B4
#define IP_AESKEY31_0_INDEX  0x0000002D
#define IP_AESKEY31_0_RESET  0x00000000

__INLINE uint32_t ip_aeskey31_0_get(void)
{
    return REG_IP_RD(IP_AESKEY31_0_ADDR);
}

__INLINE void ip_aeskey31_0_set(uint32_t value)
{
    REG_IP_WR(IP_AESKEY31_0_ADDR, value);
}

// field definitions
#define IP_AESKEY31_0_MASK   ((uint32_t)0xFFFFFFFF)
#define IP_AESKEY31_0_LSB    0
#define IP_AESKEY31_0_WIDTH  ((uint32_t)0x00000020)

#define IP_AESKEY31_0_RST    0x0

__INLINE uint32_t ip_aeskey31_0_getf(void)
{
    uint32_t localVal = REG_IP_RD(IP_AESKEY31_0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aeskey31_0_setf(uint32_t aeskey310)
{
    ASSERT_ERR((((uint32_t)aeskey310 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(IP_AESKEY31_0_ADDR, (uint32_t)aeskey310 << 0);
}

/**
 * @brief AESKEY63_32 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          AESKEY63_32   0x0
 * </pre>
 */
#define IP_AESKEY63_32_ADDR   0x620000B8
#define IP_AESKEY63_32_OFFSET 0x000000B8
#define IP_AESKEY63_32_INDEX  0x0000002E
#define IP_AESKEY63_32_RESET  0x00000000

__INLINE uint32_t ip_aeskey63_32_get(void)
{
    return REG_IP_RD(IP_AESKEY63_32_ADDR);
}

__INLINE void ip_aeskey63_32_set(uint32_t value)
{
    REG_IP_WR(IP_AESKEY63_32_ADDR, value);
}

// field definitions
#define IP_AESKEY63_32_MASK   ((uint32_t)0xFFFFFFFF)
#define IP_AESKEY63_32_LSB    0
#define IP_AESKEY63_32_WIDTH  ((uint32_t)0x00000020)

#define IP_AESKEY63_32_RST    0x0

__INLINE uint32_t ip_aeskey63_32_getf(void)
{
    uint32_t localVal = REG_IP_RD(IP_AESKEY63_32_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aeskey63_32_setf(uint32_t aeskey6332)
{
    ASSERT_ERR((((uint32_t)aeskey6332 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(IP_AESKEY63_32_ADDR, (uint32_t)aeskey6332 << 0);
}

/**
 * @brief AESKEY95_64 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          AESKEY95_64   0x0
 * </pre>
 */
#define IP_AESKEY95_64_ADDR   0x620000BC
#define IP_AESKEY95_64_OFFSET 0x000000BC
#define IP_AESKEY95_64_INDEX  0x0000002F
#define IP_AESKEY95_64_RESET  0x00000000

__INLINE uint32_t ip_aeskey95_64_get(void)
{
    return REG_IP_RD(IP_AESKEY95_64_ADDR);
}

__INLINE void ip_aeskey95_64_set(uint32_t value)
{
    REG_IP_WR(IP_AESKEY95_64_ADDR, value);
}

// field definitions
#define IP_AESKEY95_64_MASK   ((uint32_t)0xFFFFFFFF)
#define IP_AESKEY95_64_LSB    0
#define IP_AESKEY95_64_WIDTH  ((uint32_t)0x00000020)

#define IP_AESKEY95_64_RST    0x0

__INLINE uint32_t ip_aeskey95_64_getf(void)
{
    uint32_t localVal = REG_IP_RD(IP_AESKEY95_64_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aeskey95_64_setf(uint32_t aeskey9564)
{
    ASSERT_ERR((((uint32_t)aeskey9564 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(IP_AESKEY95_64_ADDR, (uint32_t)aeskey9564 << 0);
}

/**
 * @brief AESKEY127_96 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         AESKEY127_96   0x0
 * </pre>
 */
#define IP_AESKEY127_96_ADDR   0x620000C0
#define IP_AESKEY127_96_OFFSET 0x000000C0
#define IP_AESKEY127_96_INDEX  0x00000030
#define IP_AESKEY127_96_RESET  0x00000000

__INLINE uint32_t ip_aeskey127_96_get(void)
{
    return REG_IP_RD(IP_AESKEY127_96_ADDR);
}

__INLINE void ip_aeskey127_96_set(uint32_t value)
{
    REG_IP_WR(IP_AESKEY127_96_ADDR, value);
}

// field definitions
#define IP_AESKEY127_96_MASK   ((uint32_t)0xFFFFFFFF)
#define IP_AESKEY127_96_LSB    0
#define IP_AESKEY127_96_WIDTH  ((uint32_t)0x00000020)

#define IP_AESKEY127_96_RST    0x0

__INLINE uint32_t ip_aeskey127_96_getf(void)
{
    uint32_t localVal = REG_IP_RD(IP_AESKEY127_96_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aeskey127_96_setf(uint32_t aeskey12796)
{
    ASSERT_ERR((((uint32_t)aeskey12796 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(IP_AESKEY127_96_ADDR, (uint32_t)aeskey12796 << 0);
}
__INLINE uint16_t co_read16p(void const *ptr16)
{
    uint16_t value = ((uint8_t *)ptr16)[0] | ((uint8_t *)ptr16)[1] << 8;
    return value;
}


__INLINE uint32_t co_read32p(void const *ptr32)
{
    uint16_t addr_l, addr_h;
    addr_l = co_read16p(ptr32);
    addr_h = co_read16p((uint8_t *)ptr32 + 2);
    return ((uint32_t)addr_l | (uint32_t)addr_h << 16);
}

#define IP_AESPTR_ADDR   0x620000C4
#define IP_AESPTR_OFFSET 0x000000C4
#define IP_AESPTR_INDEX  0x00000031
#define IP_AESPTR_RESET  0x00000000
__INLINE void ip_aesptr_setf(uint16_t aesptr)
{
    ASSERT_ERR((((uint32_t)aesptr << 0) & ~((uint32_t)0x00003FFF)) == 0);
    REG_IP_WR(IP_AESPTR_ADDR, (uint32_t)aesptr << 0);
}

#define IP_AESCNTL_ADDR   0x620000B0
#define IP_AESCNTL_OFFSET 0x000000B0
#define IP_AESCNTL_INDEX  0x0000002C
#define IP_AESCNTL_RESET  0x00000000
__INLINE void ip_aescntl_aes_mode_setf(uint8_t aesmode)
{
    ASSERT_ERR((((uint32_t)aesmode << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_IP_WR(IP_AESCNTL_ADDR, (REG_IP_RD(IP_AESCNTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)aesmode << 1));
}

__INLINE void ip_aescntl_aes_start_setf(uint8_t aesstart)
{
    ASSERT_ERR((((uint32_t)aesstart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(IP_AESCNTL_ADDR, (REG_IP_RD(IP_AESCNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)aesstart << 0));
}

#endif // _DUET_AES_REG_H_

