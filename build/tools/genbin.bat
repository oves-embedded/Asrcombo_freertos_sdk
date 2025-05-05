echo genbin...
..\..\..\tools\hex2bin\hex2bin.exe .\Objects\%1.hex
..\..\..\tools\genBurnImg\win\genBurnImg.exe -i .\Objects\%1.bin -o .\Objects\%1crypt.bin
..\..\..\tools\otaImage\image_gen_header.exe .\Objects\%1.bin flash_remapping