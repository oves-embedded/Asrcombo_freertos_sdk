# Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors) 5.4.1 20160919 (release) [ARM/embedded-5-branch revision 240496]
CROSS_COMPILE := /work/disk1/share/toolchain/arm-none-eabi-5.4.1-20160919/bin/arm-none-eabi-

# x86 arch 
CFLAGS := -c -mcpu=cortex-m4 -mthumb -gdwarf-2 -MD -Wall -Os -mthumb-interwork
# other arch 
# CFLAGS := -DuECC_PLATFORM=uECC_arch_other -D__LINUX_PAL__ -DJOYLINK_SDK_EXAMPLE_TEST -D_SAVE_FILE_

LDFLAGS = -lpthread -lm

USE_JOYLINK_JSON=yes

#----------------------------------------------以下固定参数
CFLAGS += -D_IS_DEV_REQUEST_ACTIVE_SUPPORTED_ -D_GET_HOST_BY_NAME_ -DuECC_PLATFORM=uECC_arch_other

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


