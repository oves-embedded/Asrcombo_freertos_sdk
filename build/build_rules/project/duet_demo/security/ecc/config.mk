include $(TOOLCHAIN_MAKEFILE)
NAME := ecc
$(NAME)_GEN := $(NAME)

$(NAME)_SOURCE_GROUP := lib ecc security/duet
$(NAME)_SOURCE_DIR := $(SOURE_ROOT)

$(NAME)_SOURCE := \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/security/ecc/code/main.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/duet_demo/startup_cm4.S \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/system_cm4.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/sysCalls.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_pinmux.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_uart.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/printf_uart.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/printf-stdarg.c \

$(NAME)_INC_PATH := \
    $($(NAME)_SOURCE_DIR)/lib/security \
    $($(NAME)_SOURCE_DIR)/platform/duet/CMSIS/Include \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/inc \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/include \

$(NAME)_PRE_LIB := $($(NAME)_SOURCE_DIR)/lib/security/lib_security.a
$(NAME)_PRE_LIB_LOACATE :=

$(NAME)_CFLAGS := -DDCDC_PFMMODE_CLOSE -DPS_CLOSE_APLL -DDUET_A0V2 -DDUET_CM4 -D_SPI_FLASH_ENABLE_ -DWIFI_DEVICE -DCFG_STA_MAX=5 -DCFG_BATX=1 -DCFG_BARX=0 -DCFG_REORD_BUF=4 -DCFG_SPC=4 -DCFG_TXDESC0=4 -DCFG_TXDESC1=4 -DCFG_TXDESC2=4 -DCFG_TXDESC3=4 -DCFG_TXDESC4=4 -DCFG_CMON -DCFG_MDM_VER_V21 -DCFG_SOFTAP_SUPPORT -DCFG_SNIFFER_SUPPORT -DCFG_CUS_FRAME -DCFG_DBG=1 -D__FPU_PRESENT=1 -DDX_CC_TEE -DHASH_SHA_512_SUPPORTED -DCC_HW_VERSION=0xF0 -DDLLI_MAX_BUFF_SIZE=0x10000 -DSSI_CONFIG_TRNG_MODE=0 -DAT_USER_DEBUG -DLWIP_APP_IPERF -DSYSTEM_RECOVERY -DSYSTEM_COREDUMP
$(NAME)_CFLAGS += -DOS_NOT_SUPPORT

$(NAME)_LINK_LD := $($(NAME)_SOURCE_DIR)/demo/duet_demo/security/ecc/cfg/gcc.ld
$(NAME)_LINK_INI := $($(NAME)_SOURCE_DIR)/demo/duet_demo/security/ecc/cfg/ram.ini

##################################################################################################
$(NAME)_INCLUDES := $(addprefix -I ,$($(NAME)_INC_PATH))

C_OPTS := $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS)

CMISC := -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -fsingle-precision-constant -std=gnu99 -fno-builtin-printf -fno-builtin-sprintf -fno-builtin-snprintf
AMISC :=

CFLAGS := $(C_OPTS)
AFLAGS :=

LINKER_MISC := -Wl,--wrap=printf -Wl,--wrap=sprintf -Wl,--wrap=snprintf  -Wl,-z,max-page-size=4096
$(NAME)_PRO_OUT :=

C_SOURCES := $(filter %.c,$($(NAME)_SOURCE))
S_SOURCES := $(filter %.s %.S,$($(NAME)_SOURCE))

$(NAME)_C_OBJ := $(patsubst %.c,%.o,$(C_SOURCES))
$(NAME)_S_OBJ := $(patsubst %.S,%.o,$(S_SOURCES:.s=.o))
$(NAME)_PACK_OBJ := $($(NAME)_C_OBJ) $($(NAME)_S_OBJ)

