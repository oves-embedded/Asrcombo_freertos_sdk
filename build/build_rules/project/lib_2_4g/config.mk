NAME := lib_2_4g
$(NAME)_GEN := $(NAME)
$(NAME)_LIB := lib_2_4g.a
$(NAME)_SOURCE_DIR := $(SOURE_ROOT)

include $(TOOLCHAIN_MAKEFILE)
include $(CONFIG_MAKEFILES_PATH)/$(TARGET)/files_process.mk

$(NAME)_SOURCE_GROUP := g_2_4 api

$(NAME)_SOURCE := $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/ble_api/src/sonata_private_protocol_api.c \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/private_2_4g/src/sonata_2_4g_co.c \
    $(DIR_PREFIX)/$($(NAME)_SOURCE_DIR)/ble_soc/rw-ble-sw-v10_0_8_asr/src/modules/private_2_4g/src/sonata_2_4g_driver.c\



$(NAME)_INC_PATH := $(HEADER_PATH)
$(NAME)_HEADER_FILES := $(HEADER_FILES)

$(NAME)_PRE_LIB :=
$(NAME)_PRE_LIB_LOACATE := $($(NAME)_SOURCE_DIR)/lib/ble

$(NAME)_CFLAGS := -D__VTOR_PRESENT=1 -D_IC_CODE_ -DCFG_SEG -DCFG_RF_SONATA_A0V2  -DCFG_PLF_DUET -DCFG_RF_2_4G  -DCFG_BLE_DYNAMIC_MEM -DCONFIG_2_4G_RX_MODE_PASSIVESCAN
$(NAME)_CFLAGS +=

$(NAME)_AFLAGS := -DSTACK_INIT_PATTERN=0xF3F3F3F3

##################################################################################################
$(NAME)_INCLUDES := $(addprefix -I ,$($(NAME)_INC_PATH))

C_OPTS := $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS)

CMISC :=  -std=gnu99 -fno-builtin-printf -fno-builtin-sprintf -fno-builtin-snprintf -fdata-sections -ffunction-sections -fno-common -fmessage-length=0
AMISC :=

CFLAGS := $(C_OPTS)
AFLAGS :=

LINKER_MISC := -Wl,--wrap=printf -Wl,--wrap=sprintf -Wl,--wrap=snprintf
$(NAME)_PRO_OUT := LIB

LINKER_PLUS += $(LINKER_MISC)

C_SOURCES := $(filter %.c,$($(NAME)_SOURCE))
S_SOURCES := $(filter %.s %.S,$($(NAME)_SOURCE))

$(NAME)_C_OBJ := $(patsubst %.c,%.o,$(C_SOURCES))
$(NAME)_S_OBJ := $(patsubst %.S,%.o,$(S_SOURCES:.s=.o))
$(NAME)_PACK_OBJ := $($(NAME)_C_OBJ) $($(NAME)_S_OBJ)

