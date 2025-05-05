
#ifndef __I2C_H__
#define __I2C_H__

#define   FG_SEQREAD    1
#define   FG_RANDREAD   0

#define 	ACK_DELAY 200
#define 	BUS_DELAY  255 

void I2c_Read(uint8_t *prValue, uint8_t fgSeq_Read) ;
void I2c_Stop(void) ;
void I2c_DelayMs(uint16_t ms);// Ms
void I2c_Delay2us(void);
uint8_t I2c_Send(uint8_t bValue) ;
uint8_t I2c_Start(uint8_t bDevice) ;
uint8_t I2c_PageWrite(uint8_t bDevice, uint16_t bData_Addr,uint16_t bDataCount, uint8_t *prData) ;                   
uint8_t I2c_PageRead(uint8_t bDevice, uint16_t bData_Addr, uint16_t bDataCount,uint8_t *prData) ;


#endif  /* I2C_H */

