
/**
 ****************************************************************************************
 *
 * @file (app_ota.h)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */
#ifndef _APP_OTA_H_
#define _APP_OTA_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app.h"

/*
 * MACRO DEFINES
 ****************************************************************************************
 */


/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
uint8_t app_ota_start_callback(uint8_t length, uint8_t *version);
void app_ota_before_start_callback(uint8_t conidx);
void app_ota_stop_callback();
void app_ota_fail_callback();
void app_ota_disable(bool disable);
bool app_ota_is_start();




#endif //_APP_OTA_H_
