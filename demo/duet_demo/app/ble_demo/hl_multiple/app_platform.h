
/**
 ****************************************************************************************
 *
 * @file (app_platform.h)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */


#ifndef _APP_PLATFORM_H_
#define _APP_PLATFORM_H_



/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include "arch.h"
#include "app.h"                     // Application Definition


/*
* For 5961 serial
****************************************************************************************
*/
#include "lega_at_api.h"


#ifdef CFG_PLF_RV32
#include "asr_uart.h"
#include "asr_efuse.h"
#include "lega_at_api.h"
#include "asr_common.h"
#include "asr_pinmux.h"
#include "asr_dma.h"

#define app_cmd_entry       cmd_entry
#define app_at_cmd_register lega_at_cmd_register

#define app_uart_dev_t       asr_uart_dev_t
#define app_uart_send        asr_uart_send
#define app_uart_init        asr_uart_init
#define app_uart_struct_init asr_uart_struct_init
#define app_pinmux_config    asr_pinmux_config
#define app_dma_init         asr_dma_init

#endif

/*
* For 5821 serial
****************************************************************************************
*/
#ifdef CFG_PLF_DUET
#include "duet_uart.h"
#include "duet_efuse.h"
#include "duet_common.h"
#include "duet_pinmux.h"
#include "duet_dma.h"

#define app_cmd_entry        cmd_entry
#define app_at_cmd_register  lega_at_cmd_register

#define app_uart_dev_t       duet_uart_dev_t
#define app_uart_send        duet_uart_send
#define app_uart_init        duet_uart_init
#define app_uart_struct_init duet_uart_struct_init
#define app_pinmux_config    duet_pinmux_config
#define app_dma_init         duet_dma_init
#endif
/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */




/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void app_platform_reset_complete_handler();




#endif //_APP_PLATFORM_H_
