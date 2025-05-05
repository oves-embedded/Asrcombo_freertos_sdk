
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



/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "arch.h"
#include "app.h"                     // Application Definition
#include "sonata_ble_hook.h"
#include "app_platform.h"

#ifdef USE_LEGA_RTOS
#include "lega_rtos_api.h"
#endif
/*
 * MACRO DEFINES
 ****************************************************************************************
 */

extern int __wrap_printf(const char *format, ...);

/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
sonata_ble_hook_t app_hook = {
        .pf_assert_err       = assert_err,
        .pf_assert_param     = assert_param,
        .pf_assert_warn      = assert_warn,
        .pf_app_init         = app_init,
        .pf_platform_reset   = platform_reset,
        .pf_get_stack_usage  = get_stack_usage,
        .pf_printf           = __wrap_printf,
        .pf_app_prf_api_init = app_prf_api_init,
#if defined(CFG_PLF_SONATA) || defined(CFG_PLF_RV32)
        .pf_app_config       = app_config,
#endif
#ifdef USE_LEGA_RTOS
        .pf_init_semaphore = (void *) lega_rtos_init_semaphore,
        .pf_get_semaphore  = (void *) lega_rtos_get_semaphore,
        .pf_set_semaphore  = (void *) lega_rtos_set_semaphore,
#endif

};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void app_platform_reset_complete_handler()
{
    app_reset();
    #ifdef USE_LEGA_RTOS
    void user_peri_deinit(void);
    void ble_reset_cmp(void);
    user_peri_deinit();
    ble_reset_cmp();
    #endif

}




