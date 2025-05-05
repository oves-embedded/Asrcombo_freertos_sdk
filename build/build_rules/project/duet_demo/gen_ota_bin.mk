EXTRA_POST_BUILD_TARGETS := gen_ota_bin

BINIMAGE_SUFFIX := .bin
CERT_DIR := build_rules/project/$(TARGET)/
SIGN_ALG_TYPE := rsa
IMAGE_SIGN_PARAMS := --cert=$(CERT_DIR) --base=0x10012000
BOOT_SIGN_PARAMS := --cert=$(CERT_DIR) --base=0x10000000
ROLL_BACK_EN := -r

ifeq ($(SECUREBOOT), 1)

ifneq ($(OTA_IMAGE_MODE), "REMAPPING")
$(error error: $(OTA_IMAGE_MODE) secureboot only support remapping ota!)
endif

ifeq ($(ic_type), 5822t)
BOOTLOADER_PATH = $(basename $(wildcard ../tools/factory_bin/ASR582X/ASRBOOTLOADER-*-4M-*-SECUREBOOT-*.bin))
else
BOOTLOADER_PATH = $(basename $(wildcard ../tools/factory_bin/ASR582X/ASRBOOTLOADER-*-2M-*-SECUREBOOT-*.bin))
endif

ifeq ($(BOOTLOADER_PATH), )
$(error error: secureboot bootloader not found!)
endif

endif

ifneq ($(shell uname), Linux)
IMAGE_GEN_HEADER_TOOL := "tools/otaImage/image_gen_header.exe"
GEN_SIGN_TOOL := "tools/gen_sign/gen_sign.exe"
else  # linux
IMAGE_GEN_HEADER_TOOL := "tools/otaImage/image_gen_header"
GEN_SIGN_TOOL := "tools/gen_sign/gen_sign"
endif #

gen_ota_bin:
	$(shell if [ ! -d "$(OUT_DIR)/$(TARGET)/bin" ];then mkdir -p $(OUT_DIR)/$(TARGET)/bin; fi)
	@cp -f $(OUT_DIR)/$(TARGET)/$(TARGET)* $(OUT_DIR)/$(TARGET)/bin/
	@rm $(OUT_DIR)/$(TARGET)/$(TARGET)*
ifeq ($(SECUREBOOT), 1)
	-@$(GEN_SIGN_TOOL) --sign_$(SIGN_ALG_TYPE)='$(BOOT_SIGN_PARAMS) --image=$(BOOTLOADER_PATH).bin'
	-@$(GEN_SIGN_TOOL) --sign_$(SIGN_ALG_TYPE)='$(IMAGE_SIGN_PARAMS) --image=$(OUT_DIR)/$(TARGET)/bin/$(TARGET)$(BINIMAGE_SUFFIX)'
	-@$(IMAGE_GEN_HEADER_TOOL) $(OUT_DIR)/$(TARGET)/bin/$(TARGET).$(SIGN_ALG_TYPE).signed$(BINIMAGE_SUFFIX) -d COMBO -b $(OTA_IMAGE_MODE) $(ROLL_BACK_EN)
	$(shell) $(MAKEFILES_PATH)/check_ota_bin.sh $(OUT_DIR)/$(TARGET)/bin/$(TARGET).$(SIGN_ALG_TYPE).signed
	@cp -f $(CERT_DIR)/$(SIGN_ALG_TYPE)/pub_key.hash.txt $(OUT_DIR)/$(TARGET)/bin/
	@mv $(BOOTLOADER_PATH).$(SIGN_ALG_TYPE).signed.bin $(OUT_DIR)/$(TARGET)/bin/
else #
	-@$(IMAGE_GEN_HEADER_TOOL) $(OUT_DIR)/$(TARGET)/bin/$(TARGET)$(BINIMAGE_SUFFIX) -d COMBO -b $(OTA_IMAGE_MODE) $(ROLL_BACK_EN)
	$(shell) $(MAKEFILES_PATH)/check_ota_bin.sh $(OUT_DIR)/$(TARGET)/bin/$(TARGET)
endif

