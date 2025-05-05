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

#!/bin/sh

export TOOLCHAIN_PATH=$(pwd)/../tools/toolchain/bin/
if [ $(uname -s) = "Linux" ];then
    if [ ! -e ../tools/toolchain/bin/arm-none-eabi-gcc ]; then
        cd ../tools/toolchain/
        tar -xvjf gcc-arm-none-eabi-5_4-2016q3-20160926-linux64.tar.bz2
        cd -
    fi

else
    if [ ! -e /bin/make.exe ]; then
        export PATH=$(PWD)/../tools/cmd/win32:$PATH
    fi

    if [ ! -e ../tools/toolchain/bin/arm-none-eabi-gcc.exe ]; then
        cd ../tools/toolchain/
        tar -xvjf gcc-arm-none-eabi-5_4-2016q3-20160926-win32.tar.bz2
        cd -
    fi
fi
