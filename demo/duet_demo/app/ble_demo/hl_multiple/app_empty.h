/**
 ****************************************************************************************
 *
 * @file app.h
 *
 * @brief Application entry point
 *
 * Copyright (C) ASR 2020-2029
 *
 ****************************************************************************************
 */

#ifndef APP_H_
#define APP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "sonata_ble_hook.h"
#include "sonata_gap.h"



/*
 * DEFINES
 ****************************************************************************************
 */



/*
 * MACROS
 ****************************************************************************************
 */
/// debug trace
#define APP_DBG    1
#if APP_DBG
#define APP_TRC    printf
#else
#define APP_TRC(...)
#endif //APP_DBG


#define SYS_APP_VERSION_SEG __attribute__((section("app_version_sec")))


extern sonata_ble_hook_t app_hook;

void app_init(void);

#endif // APP_H_
