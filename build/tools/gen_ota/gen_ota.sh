#!/bin/bash
cd ../../../build
source setBuildEnv.sh
make TARGET=duet_demo ic_type=5822t
cd ..
cd build/tools/gen_ota
cp -rf ../../out/duet_demo/bin/duet_demo.bin 1.bin
python gen_ota.py 1.bin 1466368
mkdir -p ota_dir
mv *.bin ota_dir
