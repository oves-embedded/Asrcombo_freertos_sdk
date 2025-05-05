
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "lega_wlan_api.h"
#include "tcpip.h"
#include "lega_at_api.h"
#include "soc_init.h"
#include "lwip_app_iperf.h"
#include "duet_version.h"
#include "duet_uart.h"
#include "duet_pinmux.h"
#include "duet_flash_kv.h"
#include "duet_gpio.h"
#include "elog.h"

#include "i2c.h"


extern duet_gpio_dev_t g_duet_gpio0, g_duet_gpio1;

#define   i2c_scl_cfg(a)  {g_duet_gpio0.config = a;duet_gpio_init(&g_duet_gpio0);}
#define   i2c_scl(a)     duet_gpio_input_get(&g_duet_gpio0,&a)
#define   i2c_set_scl(a)  duet_gpio_output_high(&g_duet_gpio0);
#define   i2c_clr_scl(a)  duet_gpio_output_low(&g_duet_gpio0);

#define   i2c_sda_cfg(a)  {g_duet_gpio1.config = a;duet_gpio_init(&g_duet_gpio1);}
#define   i2c_sda(a)      duet_gpio_input_get(&g_duet_gpio1,&a)
#define   i2c_set_sda(a)   duet_gpio_output_high(&g_duet_gpio1);
#define   i2c_clr_sda(a)   duet_gpio_output_low(&g_duet_gpio1);



/*******************************************************************************
* Function Name  : I2c_Delay2us
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


void I2c_Delay2us(void)// 2us
{
        uint8_t i;
        for(i=0;i<250;i++);
}

void I2c_DelayMs(uint16_t ms)// Ms
{
    uint16_t i;
	ms*=500;
    for(i=0;i<ms;i++)
	   I2c_Delay2us();
}

 
uint8_t I2c_Send(uint8_t bValue) 
{
 
  uint8_t bBitMask = 0x80;
  uint16_t delay_i2c=1000;
  uint32_t io_state=0;
 
// step 1 : 8-bit data transmission


  while(bBitMask)
  {
    if(bBitMask & bValue)
    {
      i2c_set_sda();
    }
    else
    {
      i2c_clr_sda();
    }

    I2c_Delay2us();
    i2c_set_scl();                    // data clock in
    I2c_Delay2us();
    i2c_clr_scl();                    // ready for next clock in

    I2c_Delay2us();
    bBitMask = bBitMask >> 1;   // MSB first & timing delay
  }

  // step 2 : slave acknowledge check
  i2c_set_sda();                      // release SDA for ACK polling
  I2c_Delay2us();

  i2c_set_scl();                      // start ACK polling
  bBitMask = ACK_DELAY;         // time out protection

  I2c_Delay2us();
  
  i2c_sda_cfg(DUET_INPUT_HIGH_IMPEDANCE);
  I2c_Delay2us();
  while(io_state && --delay_i2c)
  {
    i2c_sda(io_state);  // wait for ACK, SDA=0 or bitMask=0->jump to this loop
  }
  i2c_sda_cfg(DUET_OUTPUT_OPEN_DRAIN_NO_PULL);
 
  I2c_Delay2us();

   // timing delay 2us
  i2c_clr_scl();                     // end ACK polling
  I2c_Delay2us();

  if(delay_i2c)
  {
    return(TRUE);              // return TRUE if ACK detected
  }
  else
  {
    return(FALSE);             // return FALSE if time out
  }
}

/************************************************************************
     Function : void vI2CRead(uint8_t *prValue, BOOL fgSeq_Read)
  Description : Read Routine
                timing : SCL ___|^|___|^|__~__|^|___|^|__
                         SDA __/D7 \_/D6 \_~_/D0 \_/ACK\_
    Parameter : *prValue(8-bit input pointer value)
    Return    : NONE
************************************************************************/
void I2c_Read(uint8_t *prValue, uint8_t fgSeq_Read) 
{
  uint8_t bBitMask = 0x80;
  uint32_t io_state=0;


  *prValue = 0;                 // reset data buffer
  i2c_set_sda();                      // make sure SDA released
  I2c_Delay2us();


   I2c_Delay2us();

   i2c_sda_cfg(DUET_INPUT_HIGH_IMPEDANCE);
   I2c_Delay2us();
   
// step 1 : 8-bit data reception
  while(bBitMask)
  {
    i2c_set_scl();                    // data clock out
    I2c_Delay2us();

	i2c_sda(io_state);
    if(io_state)
    {
      *prValue = *prValue | bBitMask;   // Get all data
    }                                   // non-zero bits to buffer


    i2c_clr_scl();                            // ready for next clock out
    I2c_Delay2us();

    bBitMask = bBitMask >> 1;           // shift bit mask & clock delay
  }

  i2c_sda_cfg(DUET_OUTPUT_OPEN_DRAIN_NO_PULL);
  I2c_Delay2us();
  

// step 2 : acknowledgement to slave
  if(fgSeq_Read)
  {
    i2c_clr_sda();                            // ACK here for Sequential Read
  }
  else
  {
    i2c_set_sda();                            // NACK here (for single uint8_t read)
  }

    I2c_Delay2us();
  i2c_set_scl();                    // NACK clock out
    I2c_Delay2us();
  i2c_clr_scl();                    // ready for next clock out
  I2c_Delay2us();
  i2c_set_sda();                    // release SDA
   I2c_Delay2us();

}

/************************************************************************
     Function : BOOL fgI2CStart(uint8_t bValue, BOOL fgRead)
  Description : Start Routine
                timing : SCL ^^^^|___|^|___|^|__~__|^|___|^|___|^|__~
                         SDA ^^|____/A6 \_/A5 \_~_/A0 \_/R/W\_/ACK\_~
                              (S)
                               |<--- start condition
    Parameter : bDevice(7-bit slave address) + fgRead(R/W bit)
    Return    : TRUE  : successful with ACK from slave
                FALSE  : bus (SCL = 0) or ACK failure
************************************************************************/
uint8_t I2c_Start(uint8_t bDevice) 
{
  uint16_t bBusDelayTemp = 1000;
  uint32_t io_state=0;


			  
  i2c_set_sda();            // make sure SDA released
  I2c_Delay2us();

  i2c_set_scl();            // make sure SCL released
  I2c_Delay2us();


  i2c_scl_cfg(DUET_INPUT_HIGH_IMPEDANCE);
  I2c_Delay2us();
  while((!io_state) && (--bBusDelayTemp))
  {
    i2c_scl(io_state);  // simple bus abritration
  }
  i2c_scl_cfg(DUET_OUTPUT_OPEN_DRAIN_NO_PULL);

 I2c_Delay2us();
 
  if(!bBusDelayTemp)
  {
    return(FALSE);           // time out protection & timing delay
  }


  i2c_clr_sda();          // start condition here
  I2c_Delay2us();
  i2c_clr_scl();          // ready for clocking
  I2c_Delay2us();

  return(I2c_Send(bDevice));// slave address & R/W transmission
}

/************************************************************************
     Function : void vI2CStop(void)
  Description : Stop Routine
                timing : SCL ___|^^^^^
                         SDA xx___|^^^
                                 (P)
                                  |<--- stop condition
    Parameter : NONE
    Return    : NONE
************************************************************************/
void I2c_Stop(void) 
{
  i2c_clr_sda();          // ready for stop condition
  I2c_Delay2us();
  i2c_set_scl();          // ready for stop condition
  I2c_Delay2us();
  i2c_set_sda();          // stop condition here
  I2c_Delay2us();

}


uint8_t I2c_PageWrite(uint8_t bDevice, uint16_t bData_Addr,uint16_t bDataCount, uint8_t *prData)            
{

  //bDevice = bDevice << 1;      // Shift the 7-bit address to 7+1 format

  if(!I2c_Start(bDevice))     // Write Command
  {
  

    I2c_Stop();
    return(FALSE);             // Device Address exceeds the range
  }

  if(!I2c_Send(bData_Addr>>8))   // Word Address
  {
    I2c_Stop();
    return(FALSE);             // Device Address exceeds the range
  }

  if(!I2c_Send(bData_Addr))   // Word Address
  {
    I2c_Stop();
    return(FALSE);             // Device Address exceeds the range
  }

  while(bDataCount)
  {
    if(!I2c_Send(*(prData++))) // Data Content Write
    { 
      I2c_Stop();
      return(FALSE);            // Device Address exceeds the range
    }
    bDataCount--;
  }

  I2c_Stop();

 // log_i("---Eeprom page write succ \r\n");
  return(TRUE);
}

/************************************************************************
     Function : BOOL fgI2CDataRead(uint8_t bDevice, uint8_t bData_Addr,
                                    uint8_t bDataCount, uint8_t *prData)
  Description : DataRead Routine
    Parameter : bDevice -> Device Address
                bData_Addr -> Data Address
                bDataCount -> Data Content Cont
                *prData -> Data Content Pointer
    Return    : TRUE  : successful with ACK from slave
                FALSE  : bus (SCL = 0) or ACK failure
************************************************************************/
uint8_t I2c_PageRead(uint8_t bDevice, uint16_t bData_Addr, uint16_t bDataCount,uint8_t *prData) 
{

  {
    //bDevice = bDevice << 1;   // Shift the 7-bit address to 7+1 format

    if(!I2c_Start(bDevice))  // Write Command
    {
    
      I2c_Stop();
      return(FALSE);          // Start fail
    }

    if(!I2c_Send(bData_Addr>>8))// Word Address
    {
      I2c_Stop();
      return(FALSE);          // Data Address Fail
    }

	if(!I2c_Send(bData_Addr))// Word Address
    {
      I2c_Stop();
      return(FALSE);          // Data Address Fail
    }

    // Step 2 : Real Read
    bDevice = bDevice + 1;    // Shift the 7-bit address to 7+1 format

    if(!I2c_Start(bDevice))  // Read Command
    {
      I2c_Stop();
      return(FALSE);          // Start fail
    }
  }

  while (bDataCount)
  {
    if (bDataCount == 1)
    {
      I2c_Read(prData++, FG_RANDREAD);  // Data Content Read
    }
    else
    {
      I2c_Read(prData++, FG_SEQREAD);  // Data Content Read
    }
    bDataCount--;
  }

  // Step 3 : Stop
  I2c_Stop();


  return (TRUE);
}








