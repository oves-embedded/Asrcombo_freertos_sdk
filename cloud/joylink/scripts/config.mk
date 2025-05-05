# arm-none-eabi-gcc (GNU Tools for Arm Embedded Processors 7-2018-q2-update) 7.3.1 20180622 (release) [ARM/embedded-7-branch revision 261907]
CROSS_COMPILE := /work/disk1/share/toolchain/arm-none-eabi-7.3.1/bin/arm-none-eabi-

# x86 arch
CFLAGS := -DuECC_PLATFORM=uECC_arch_other -c -mcpu=cortex-m4 -mthumb -gdwarf-2 -MD -Wall -Os -mthumb-interwork  -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -fsingle-precision-constant -std=gnu99 -fno-builtin-printf -fno-builtin-sprintf -fno-builtin-snprintf -ffunction-sections -fdata-sections
# other arch
# CFLAGS := -DuECC_PLATFORM=uECC_arch_other -D__LINUX_PAL__ -DJOYLINK_SDK_EXAMPLE_TEST -D_SAVE_FILE_

LDFLAGS = -lpthread -lm

USE_JOYLINK_JSON=yes

#----------------------------------------------以下固定参数
CFLAGS += -D_IS_DEV_REQUEST_ACTIVE_SUPPORTED_ -D_GET_HOST_BY_NAME_

TARGET_DIR = ${TOP_DIR}/target
TARGET_LIB = ${TARGET_DIR}/lib
TARGET_BIN = ${TARGET_DIR}/bin

CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib
STRIP=$(CROSS_COMPILE)strip

RM = rm -rf
CP = cp -rf
MV = mv -f


