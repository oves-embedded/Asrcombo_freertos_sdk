include $(TOOLCHAIN_MAKEFILE)
NAME := i2s
$(NAME)_GEN := $(NAME)

$(NAME)_SOURCE_DIR := $(SOURE_ROOT)

$(NAME)_SOURCE_GROUP := duet

$(NAME)_SOURCE := \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/duet_demo/startup_cm4.S \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/peripheral/i2s/code/main.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/system_cm4.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_uart.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/sysCalls.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/printf-stdarg.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_pinmux.c \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/printf_uart.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_i2s.c \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/src/duet_rf_spi.c \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/peripheral/i2s/code/apll_rf.c \

$(NAME)_INC_PATH := \
    $($(NAME)_SOURCE_DIR)/demo/duet_demo/peripheral/i2s/code \
    $($(NAME)_SOURCE_DIR)/common/duet \
    $($(NAME)_SOURCE_DIR)/platform/duet/CMSIS/Include \
    $($(NAME)_SOURCE_DIR)/platform/duet/common \
    $($(NAME)_SOURCE_DIR)/platform/duet/system/include \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/inc \
    $($(NAME)_SOURCE_DIR)/peripheral/duet/inc \


$(NAME)_CFLAGS := -DI2S_DEMO

$(NAME)_LINK_LD := $($(NAME)_SOURCE_DIR)/demo/duet_demo/peripheral/i2s/cfg/gcc.ld
$(NAME)_LINK_INI := $($(NAME)_SOURCE_DIR)/demo/duet_demo/peripheral/i2s/cfg/ram.ini

##################################################################################################
$(NAME)_INCLUDES := $(addprefix -I ,$($(NAME)_INC_PATH))

C_OPTS := $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS)

CMISC :=  -mcpu=cortex-m4 -std=gnu99 -fno-builtin-printf -fno-builtin-sprintf -fno-builtin-snprintf -fdata-sections -ffunction-sections
AMISC :=

CFLAGS := $(C_OPTS)
AFLAGS :=

LINKER_MISC := -Wl,--wrap=printf -Wl,--wrap=sprintf -Wl,--wrap=snprintf  -Wl,-z,max-page-size=4096
$(NAME)_PRO_OUT :=

LINKER_PLUS += $(LINKER_MISC)

C_SOURCES := $(filter %.c,$($(NAME)_SOURCE))
S_SOURCES := $(filter %.s %.S,$($(NAME)_SOURCE))

$(NAME)_C_OBJ := $(patsubst %.c,%.o,$(C_SOURCES))
$(NAME)_S_OBJ := $(patsubst %.S,%.o,$(S_SOURCES:.s=.o))
$(NAME)_PACK_OBJ := $($(NAME)_S_OBJ) $($(NAME)_C_OBJ)

