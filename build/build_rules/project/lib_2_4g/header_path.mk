HEADER_PATH := \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/platform/duet/CMSIS/Include \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/platform/duet/common \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/platform/duet/system/include \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/rwip/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/rwip/src \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/ke/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/dbg/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/common/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/aes/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/nvds/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/rf/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/ecc_p256/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/h4tl/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/ip/hci/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/ip/ahi/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/ip/sch/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/ip/em/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/ip/ble/ll/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/ip/ble/ll/src \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/peripheral/duet/inc \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/ip/ble/hl/inc \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/ip/ble/hl/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/app/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/app/src \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/ble_api/inc \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/platform/duet/system \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/at \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/arch/common \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/arch/duet/api \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/arch/duet/rom \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/arch/duet/api/ble-splitemb/reg/fw \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/private_2_4g/src \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/projects/combo_a0v0/duet_private_protocol_dbg/src\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/peripheral/duet/inc\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/platform/duet/system/include\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/platform/duet/CMSIS/Include\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/common\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/platform/system/include \
    $($(NAME)_SOURCE_DIR)/freertos/Source/include\
    $($(NAME)_SOURCE_DIR)/freertos/Source/portable/MemMang\
    $($(NAME)_SOURCE_DIR)/freertos/Source/portable/GCC/ARM_CM4F\
    $($(NAME)_SOURCE_DIR)/legartos\
    $($(NAME)_SOURCE_DIR)/lib/security\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/compiler/armgcc_4_8\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/arch/cortex/boot\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/chan\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/hal\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/mm\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/p2p\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/ps\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/rwnx\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/rx/rxl\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/rx\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/scan\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/sta\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/td\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/tdls\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/tpc\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/tx/txl\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/tx\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/vif\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/lmac/src/uf\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/apm\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/bam\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/hsu\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/llc\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/me\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/mfp\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/rc\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/rxu\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/scanu\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/sm\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/umac/src/txu\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/utils/src/common\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/utils/src/emsg\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/utils/src/ke/template\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwifi/utils/src/ke\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/build/reg/virtex7/cortex/karst\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/arch/cortex\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/driver/dma\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/build/reg/virtex7/cortex\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/driver/intc/cortex\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/driver/ipc\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/driver/la\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/driver/phy\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/driver/reg\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/plf/refip/src/driver/sysctrl\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/sc_test\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/port/include\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/lwip_if\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/arch/duet/api\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/lwip\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/lwip/apps\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/include/compat/posix\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/apps/coap\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_2.1.2/netif\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/lwip_app_ping\
    $($(NAME)_SOURCE_DIR)/at\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/uwifi/edrv/include\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/uwifi/wpa/include\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/uwifi/wpa3/include\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/uwifi/wpa3/crypto/include\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/uwifi/wpa3/crypto/wolfssl/wolfssl\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/uwifi/wpa3/crypto/wolfssl/wolfssl/wolfcrypt\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/iperf\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/ota\
    $($(NAME)_SOURCE_DIR)/lwip/port/arch\
    $($(NAME)_SOURCE_DIR)/lwip/lwip_app/iperf\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/common/dbg/src\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/common/inc\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/common/mac/src\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/wifi/lwip_if\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/wifi_soc/os_alithings/dhcps\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/platform/duet/kv/include\
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/driver_plat/common/duet\

