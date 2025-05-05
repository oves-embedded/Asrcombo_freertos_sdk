include $(TOOLCHAIN_MAKEFILE)
NAME := duet_demo
$(NAME)_GEN := $(NAME)

$(NAME)_SOURCE_GROUP := platform/duet/ at_cmd/ freertos/Source/ lwip/lwip_app/ lwip/port/ lwip/lwip_2.1.2/ peripheral/duet/ \
demo/duet_demo/app/ /lib/ /common/

$(NAME)_SOURCE_DIR := $(SOURE_ROOT)

$(NAME)_SOURCE := $($(NAME)_SOURCE_DIR)/platform/duet/lega_rtos.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/ethernetif_wifi.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_user.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_test.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_comm.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_wifi.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_cloud.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_net.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_ota.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_ble.c \
    $($(NAME)_SOURCE_DIR)/at_cmd/atcmd_ble_perf.c \
    $(sort $(wildcard $($(NAME)_SOURCE_DIR)/freertos/Source/*.c)) \
    $($(NAME)_SOURCE_DIR)/freertos/Source/portable/MemMang/heap_5.c \
    $($(NAME)_SOURCE_DIR)/freertos/Source/portable/GCC/ARM_CM4F/port.c \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/netif/ethernet.c \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/port/sys_arch.c \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/lwip_app_ping/lwip_app_ping.c \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/ota/http_ota.c \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/iperf/lwip_iperf.c \
    $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/api/*.c)) \
    $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/core/*.c)) \
    $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/core/ipv4/*.c)) \
    $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/core/ipv6/*.c)) \
    $(sort $(wildcard $($(NAME)_SOURCE_DIR)/common/duet/*.c)) \
    $(sort $(wildcard $($(NAME)_SOURCE_DIR)/common/duet/easylogger/*.c)) \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_boot.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_board.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_efuse.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_flash.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_flash_alg.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_wdg.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_timer.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_gpio.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_rf_spi.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_pwm.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_rtc.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_pinmux.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_uart.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_dma.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/sysCalls.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/soc_init.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/printf_uart.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/printf-stdarg.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/system_cm4.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/startup_cm4.S


ifneq ($(findstring $(ic_type),5822s 5822t),)
ifeq ($(version),mesh_generic_onoff_server)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff_server/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff_server/user_platform.c
else ifeq ($(version),mesh_generic_onoff_client)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff_client/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff_client/user_platform.c
else ifeq ($(version),mesh_generic_onoff)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff/user_platform.c
else ifeq ($(version),mesh_coexist_with_ble)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_coexist_with_ble/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_coexist_with_ble/user_platform.c
else ifeq ($(version),hl_perf_test_peripheral)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_perf_test_peripheral/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_perf_test_peripheral/data_transport_svc.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_perf_test_peripheral/user_platform.c
else ifeq ($(version),hl_perf_test_central)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_perf_test_central/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_perf_test_central/app_uart.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_perf_test_central/user_platform.c
else ifeq ($(version),hl_peripheral_longrange)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral_longrange/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral_longrange/data_transport_svc.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral_longrange/user_platform.c
else ifeq ($(version),hl_central_longrange)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_central_longrange/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_central_longrange/user_platform.c
else ifeq ($(version),ll_demo)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/ll_demo/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/ll_demo/user_platform.c
else ifeq ($(version),duet_2_4g_demo)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/duet_2_4g_demo/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/duet_2_4g_demo/user_platform.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/duet_2_4g_demo/app_custom_svc.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/duet_2_4g_demo/ft_2_4g_hal.c
else ifeq ($(version),duet_mult_demo)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app.c
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app_command.c
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app_nv.c
#    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app_ota.c
#    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app_patch.c
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app_platform.c
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app_sec.c
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app_uart.c
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/app_utils.c
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/data_transport_svc.c
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple/user_platform.c
else
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/app_custom_svc.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/user_platform.c
endif
else ifeq ($(ic_type),5822n)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/app_custom_svc.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/user_platform.c
else ifeq ($(ic_type),5822c)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/app.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/app_custom_svc.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral/user_platform.c
else
    $(error error:missing ic_type!)
endif
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/main.c \
    #$(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/oves/src/i2c.c \
    #$(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/oves/src/eeprom.c \
    #$(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/oves/src/payg.c \
    #$(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/oves/src/ds1302.c \
    #$(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/oves/src/LlzInverter.c \
   #$(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/oves/src/keyboard.c 
   $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/oves/src/*.c))


$(NAME)_INC_PATH := $($(NAME)_SOURCE_DIR)/freertos/Source/include\
    $($(NAME)_SOURCE_DIR)/freertos/Source/portable/MemMang\
    $($(NAME)_SOURCE_DIR)/freertos/Source/portable/GCC/ARM_CM4F
ifeq ($(version),mesh_generic_onoff_server)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff_server
else ifeq ($(version),mesh_generic_onoff_client)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff_client
else ifeq ($(version),mesh_generic_onoff)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_generic_onoff
else ifeq ($(version),mesh_coexist_with_ble)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/mesh_coexist_with_ble
else ifeq ($(version),hl_perf_test_peripheral)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_perf_test_peripheral
else ifeq ($(version),hl_perf_test_central)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_perf_test_central
else ifeq ($(version),hl_peripheral_longrange)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral_longrange
else ifeq ($(version),hl_central_longrange)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_central_longrange
else ifeq ($(version),ll_demo)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/ll_demo
else ifeq ($(version),duet_2_4g_demo)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/duet_2_4g_demo
else ifeq ($(version),duet_mult_demo)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_multiple
else
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ble_demo/hl_peripheral
endif
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/lib/wifi \
    $($(NAME)_SOURCE_DIR)/lib/ble \
    $($(NAME)_SOURCE_DIR)/lib/sys \
    $($(NAME)_SOURCE_DIR)/lib/security \
    $($(NAME)_SOURCE_DIR)/version \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/inc\
    $($(NAME)_SOURCE_DIR)/common/duet \
    $($(NAME)_SOURCE_DIR)/common/duet/easylogger \
    $($(NAME)_SOURCE_DIR)/platform/duet \
    $($(NAME)_SOURCE_DIR)/platform/duet/CMSIS/Include \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/include \
    $($(NAME)_SOURCE_DIR)/platform/duet/kv/include \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/port/include \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/lwip\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/lwip_if \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app\
    $($(NAME)_SOURCE_DIR)/lwip/port/dhcps \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/lwip_app_ping \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/lwip/prot \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/lwip_if/arch \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/netif \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/iperf \
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/ota 
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/oves/inc

ifneq ($(wpa3_crypto), ws)
    $(NAME)_MBEDTLS_SRC := $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library/bignum.c
    $(NAME)_MBEDTLS_SRC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library/ecp.c
    $(NAME)_MBEDTLS_SRC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library/ecp_curves.c
    $(NAME)_MBEDTLS_SRC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library/constant_time.c
    $(NAME)_MBEDTLS_SRC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library/platform_util.c

    $(NAME)_MBEDTLS_INC := $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/include
    $(NAME)_MBEDTLS_INC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/asr_mbedtls
endif

ifeq ($(ic_type),5822s)
    $(NAME)_PRE_LIB := $($(NAME)_SOURCE_DIR)/lib/5822s/libasr_combo.a
    $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/ble/lib_ble_mesh.a
    $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/security/lib_security.a
    $(NAME)_PRE_LIB +=  $($(NAME)_SOURCE_DIR)/lib/5822s/libasr_combo.a
else ifeq ($(ic_type),5822t)
    $(NAME)_PRE_LIB := $($(NAME)_SOURCE_DIR)/lib/5822s/libasr_combo.a
    $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/ble/lib_ble_mesh.a
    $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/security/lib_security.a
    $(NAME)_PRE_LIB +=  $($(NAME)_SOURCE_DIR)/lib/5822s/libasr_combo.a
else ifeq ($(ic_type),5822n)
    $(NAME)_PRE_LIB := $($(NAME)_SOURCE_DIR)/lib/libasr_combo.a
    $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/security/lib_security.a
else ifeq ($(ic_type),5822c)
    $(NAME)_PRE_LIB := $($(NAME)_SOURCE_DIR)/lib/libasr_combo.a
    $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/security/lib_security.a
else
    $(error error:missing ic_type!)
endif

$(NAME)_PRE_LIB_LOACATE :=

$(NAME)_CFLAGS := -D_SPI_FLASH_240MHz_ -DDCDC_PFMMODE_CLOSE -DLEGA_A0V2 -DDUET_CM4 -D_SPI_FLASH_ENABLE_ -DSYSTEM_SUPPORT_OS -DWIFI_DEVICE -DCFG_BATX=1 -DCFG_BARX=1 -DCFG_REORD_BUF=4 -DCFG_SPC=4 -DCFG_TXDESC0=4 -DCFG_TXDESC1=4 -DCFG_TXDESC2=4 -DCFG_TXDESC3=4 -DCFG_TXDESC4=4 -DCFG_CMON -DCFG_MDM_VER_V21 -DCFG_SOFTAP_SUPPORT -DCFG_SNIFFER_SUPPORT -DCFG_CUS_FRAME -DAT_USER_DEBUG -DCFG_WF_DBG=1 -D__FPU_PRESENT=1 -DDX_CC_TEE -DHASH_SHA_512_SUPPORTED -DCC_HW_VERSION=0xF0 -DDLLI_MAX_BUFF_SIZE=0x10000 -DSSI_CONFIG_TRNG_MODE=0 -DLWIP_APP_IPERF -DSYSTEM_RECOVERY -DSYSTEM_COREDUMP -DCFG_REC -DCFG_SNIFFER_UNICAST_SUPPORT -DCFG_OS_VERSION_CHECK -DPS_CLOSE_APLL -DCFG_STA_AP_COEX_N -DCFG_SOFTAP_SWITCH_CHAN_N -DCFG_RESET_OPT
$(NAME)_CFLAGS += -DCFG_PLF_DUET -DSONATA_RTOS_SUPPORT -DCFG_SEG -DCFG_DUET_FREERTOS -DPRINTF2_SUPPORT -DDUET_RF_SLEEP -DCFG_DUET_BLE -DCFG_RTC_INDEPENDENT -DCFG_EASY_LOG_ENABLE=0 -DCFG_EASY_LOG_MODULE_EN=0
$(NAME)_CFLAGS += -DCFG_SAE -DCFG_SAE_AUTH_OPT -DCFG_MFP -DCONFIG_IEEE80211W -DWFA_CERTIFICATE_N -DSECURITY_ENGINE_INIT -DHTTP_OTA_SUPPORT -DCFG_BSS_OPT -DCFG_UWIFI_TASK_STACK_OPT -DCFG_HOST_MSGBUF_OPT -DCFG_NEW_SRRC_V2

ifeq ($(ic_type),5822s)
$(NAME)_CFLAGS += -DCFG_DUET_5822S
else ifeq ($(ic_type),5822t)
$(NAME)_CFLAGS += -DCFG_DUET_5822S -DCFG_DUET_5822T
endif

ifeq ($(version),duet_2_4g_demo)
$(NAME)_CFLAGS += -DASR_2_4G_DEMO -DCONFIG_2_4G_RX_MODE_PASSIVESCAN
$(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/ble/lib_2_4g.a
endif

ifeq ($(freq),120)
$(NAME)_CFLAGS += -DCFG_CORE_FREQ_120M
endif


ifeq ($(cpu_usage),1)
$(NAME)_CFLAGS += -DCFG_CPU_USAGE
endif

#JOYLINK_CLOUD
JOYLINK_CLOUD_SWITCH := 0
ifeq ($(JOYLINK_CLOUD_SWITCH), 1)
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src/joylink_flash.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src/joylink_memory.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src/joylink_socket.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src/joylink_softap.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src/joylink_stdio.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src/joylink_string.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src/joylink_thread.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src/joylink_time.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/example/joylink_extern.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/example/joylink_extern_json.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/example/joylink_extern_ota.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/example/joylink_extern_sub_dev.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/example/joylink_extern_user.c \
                      $($(NAME)_SOURCE_DIR)/cloud/joylink/ble_adapter/adapter/joylink_adapter.c \

    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/cloud/joylink \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/joylink/inc \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/joylink/inc/json \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/joylink/inc/softap \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/joylink/lib \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/example \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/inc \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/pal/src \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/ble_adapter/adapter \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/ble_adapter/include \
                        $($(NAME)_SOURCE_DIR)/cloud/joylink/ble_adapter/lib \

    $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/cloud/joylink/joylink/lib/libjoylink.a \
                       $($(NAME)_SOURCE_DIR)/cloud/joylink/ble_adapter/lib/libjoylink_ble.a

    $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/apps/altcp_tls/*.c))
    $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/apps/mqtt/*.c))
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/apps/http/http_client.c

    $(NAME)_SOURCE_GROUP += lwip/mbedtls-2.28.5/
    $(NAME)_MBEDTLS_SRC := $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library/*.c))
    $(NAME)_MBEDTLS_SRC += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/asr_mbedtls/alt/*.c))
    $(NAME)_MBEDTLS_INC := $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/include
    $(NAME)_MBEDTLS_INC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library
    $(NAME)_MBEDTLS_INC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/asr_mbedtls
    $(NAME)_MBEDTLS_INC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/asr_mbedtls/alt

    # $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/libasr_tls_duet.a

    $(NAME)_CFLAGS += -D_SAVE_FILE_ -D__FREERTOS_PAL__ -DCLOUD_JOYLINK_SUPPORT
endif

CLOUD_SWITCH := 1
#$(info $(CLOUD_SWITCH))
ifeq ($(CLOUD_SWITCH), 1)
    $(NAME)_CFLAGS += -DCLOUD_HUAWEI_SUPPORT
    $(NAME)_CFLAGS += -DCLOUD_BAIDU_SUPPORT
    $(NAME)_CFLAGS += -DCLOUD_AWS_SUPPORT
    $(NAME)_CFLAGS += -DCLOUD_TENCENT_SUPPORT
    $(NAME)_CFLAGS += -DCLOUD_JD_SUPPORT
    $(NAME)_CFLAGS += -DCLOUD_PRIVATE_SUPPORT
    $(NAME)_CFLAGS += -DCLOUD_MQTT_BROKER_SUPPORT

    # $(NAME)_CFLAGS += -DCLOUD_TEST
    # $(NAME)_CFLAGS += -DCLOUD_HUAWEI_TEST
    # $(NAME)_CFLAGS += -DCLOUD_BAIDU_TEST
    # $(NAME)_CFLAGS += -DCLOUD_AWS_TEST
    # $(NAME)_CFLAGS += -DCLOUD_TENCENT_TEST

    # $(NAME)_CFLAGS += -DCLOUD_CONNECT_TEST
    # $(NAME)_CFLAGS += -DCLOUD_CONNECT_HUAWEI
    # $(NAME)_CFLAGS += -DCLOUD_CONNECT_AWS
    # $(NAME)_CFLAGS += -DCLOUD_CONNECT_TENCENT
    # $(NAME)_CFLAGS += -DCLOUD_CONNECT_BAIDU
    # $(NAME)_CFLAGS += -DCLOUD_CONNECT_PRIVATE

    $(NAME)_SOURCE_GROUP += cloud/src
    $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/cloud/src/*.c))
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/cloud/include
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/cloud/cjson/cJSON.c
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/cloud/cjson
    $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/apps/altcp_tls/*.c))
    $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/apps/mqtt/*.c))
    $(NAME)_SOURCE += $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/apps/http/http_client.c
    $(NAME)_CFLAGS += -DCLOUD_SUPPORT

    $(NAME)_SOURCE_GROUP += lwip/mbedtls-2.28.5/
    $(NAME)_MBEDTLS_SRC := $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library/*.c))
    $(NAME)_MBEDTLS_SRC += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/asr_mbedtls/alt/*.c))
    $(NAME)_MBEDTLS_INC := $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/include
    $(NAME)_MBEDTLS_INC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/library
    $(NAME)_MBEDTLS_INC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/asr_mbedtls
    $(NAME)_MBEDTLS_INC += $($(NAME)_SOURCE_DIR)/lwip/mbedtls-2.28.5/asr_mbedtls/alt

    # $(NAME)_PRE_LIB += $($(NAME)_SOURCE_DIR)/lib/libasr_tls_duet.a
endif

TEST_SWITCH := 0
ifeq ($(TEST_SWITCH), 1)
    $(NAME)_CFLAGS += -DTEST_UNITY
    $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/Unity/src/*.c))
    # $(NAME)_SOURCE += $(wildcard $($(NAME)_SOURCE_DIR)/Unity/extras/fixture/src/*.c)
    # $(NAME)_SOURCE += $(wildcard $($(NAME)_SOURCE_DIR)/Unity/extras/fixture/memory/*.c)
    # $(NAME)_SOURCE += $(wildcard $($(NAME)_SOURCE_DIR)/Unity/extras/fixture/memory/src/*.c)
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/Unity/src
    # $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/Unity/extras/fixture/src
    # $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/Unity/extras/memory/src
    $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/Unity/examples
    TEST_EXAMPLE_1 := 1
    ifeq ($(TEST_EXAMPLE_1), 1)
        $(NAME)_CFLAGS += -DTEST_UNITY_CLOUD
        # $(NAME)_CFLAGS += -DTEST_UNITY_EXAMPLE
        $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/Unity/examples/example_1/src/*.c))
        $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/Unity/examples/example_1/test/*.c))
        $(NAME)_SOURCE += $(sort $(wildcard $($(NAME)_SOURCE_DIR)/Unity/examples/example_1/test/test_runners/*.c))
        $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/Unity/examples/example_1/src
        $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/Unity/examples/example_1/test/test_runners
        # $(NAME)_SOURCE += $(wildcard $($(NAME)_SOURCE_DIR)/Unity/examples/example_2/src/*.c)
        # $(NAME)_SOURCE += $(wildcard $($(NAME)_SOURCE_DIR)/Unity/examples/example_2/test/*.c)
        # $(NAME)_SOURCE += $(wildcard $($(NAME)_SOURCE_DIR)/Unity/examples/example_2/test/test_runners/*.c)
        # $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/Unity/examples/example_2/src
        # $(NAME)_INC_PATH += $($(NAME)_SOURCE_DIR)/Unity/examples/example_2/test/test_runners
    endif

endif

ifdef $(NAME)_MBEDTLS_SRC
    $(NAME)_SOURCE += $($(NAME)_MBEDTLS_SRC)
    $(NAME)_INC_PATH += $($(NAME)_MBEDTLS_INC)

    $(NAME)_CFLAGS += -DMBEDTLS_HW
    $(NAME)_CFLAGS += -DMBEDTLS_HW_RSA
    $(NAME)_CFLAGS += -DMBEDTLS_HW_ECC
    $(NAME)_CFLAGS += -DMBEDTLS_HW_AES

    # $(NAME)_CFLAGS += -DMBEDTLS_RSA_ALT
    # $(NAME)_CFLAGS += -DMBEDTLS_AES_ALT
endif

ifeq ($(ota_mode), REMAPPING)
	OTA_IMAGE_MODE := "REMAPPING"
else ifeq ($(ota_mode), COMPRESS)
	OTA_IMAGE_MODE := "COMPRESS"
else ifeq ($(ota_mode), COPY)
	OTA_IMAGE_MODE := "COPY"
else
    OTA_IMAGE_MODE := "REMAPPING"
endif

ifeq ($(OTA_IMAGE_MODE), "COMPRESS")
    ifneq ($(ic_type),5822t)
        $(NAME)_CFLAGS += -D_OTA_CMP_VENDOR_
    endif
endif

$(NAME)_LINK_LD := $($(NAME)_SOURCE_DIR)/demo/duet_demo/app/ld/gcc.ld

ifeq ($(SECUREBOOT), 1)
    $(NAME)_CFLAGS += -D_CFG_SECUREBOOT_
endif

##################################################################################################
$(NAME)_INCLUDES := $(addprefix -I ,$($(NAME)_INC_PATH))

C_OPTS := $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS)

CMISC := -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -fsingle-precision-constant -std=gnu99 -fno-builtin-printf -fno-builtin-sprintf -fno-builtin-snprintf -ffunction-sections -fdata-sections -fno-common  -fmessage-length=0  -fno-strict-aliasing
AMISC :=

CFLAGS := $(C_OPTS)
AFLAGS :=

LINKER_MISC := -Wl,--wrap=printf -Wl,--wrap=sprintf -Wl,--wrap=snprintf  -Wl,-gc-sections -Wl,-R
LINKER_MISC_FILE := $($(NAME)_SOURCE_DIR)/lib/ble/asr_ble_rom_symbol_v20200507204845.txt
$(NAME)_PRO_OUT :=

LINKER_PLUS += $(LINKER_MISC) $(LINKER_MISC_FILE)

C_SOURCES := $(filter %.c,$($(NAME)_SOURCE))
S_SOURCES := $(filter %.s %.S,$($(NAME)_SOURCE))

$(NAME)_C_OBJ := $(patsubst %.c,%.o,$(C_SOURCES))
$(NAME)_S_OBJ := $(patsubst %.S,%.o,$(S_SOURCES:.s=.o))
$(NAME)_PACK_OBJ := $($(NAME)_C_OBJ) $($(NAME)_S_OBJ)

ifneq ($(IDE),keil)
include $(CONFIG_MAKEFILES_PATH)/$(TARGET)/gen_ota_bin.mk
endif