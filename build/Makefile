default: all

export SOURE_ROOT ?=..
export DIR_PREFIX = ..
export MAKEFILES_PATH ?=$(SOURE_ROOT)/build/build_rules
export CONFIG_MAKEFILES_PATH ?=$(SOURE_ROOT)/build/build_rules/project
export SCRIPTS_PATH ?=$(SOURE_ROOT)/build/scripts
export OUT_DIR ?= out
export TARGET
export VIEW
export IDE
export DY
export PYTHON = python
export HOST_ARCH := Cortex-M4F
PARA_JOBS := 5

# RSA_SUPPORT flag set to 1 means support RSA and 0 means not support RSA
# default not support RSA.
export RSA_SUPPORT ?= 0

ifeq ($(COMPILER),armcc)
export TOOLCHAIN_MAKEFILE := $(MAKEFILES_PATH)/toolchain_armcc.mk
else ifeq ($(COMPILER),iar)
export TOOLCHAIN_MAKEFILE := $(MAKEFILES_PATH)/toolchain_iar.mk
else
export TOOLCHAIN_MAKEFILE := $(MAKEFILES_PATH)/toolchain_gcc.mk
endif

export CONFIG_MAKEFILE := $(CONFIG_MAKEFILES_PATH)/$(TARGET)/config.mk

ifneq ($(OUT_DIR)/$(TARGET),$(wildcard $(OUT_DIR)/$(TARGET)))
$(shell mkdir -p $(OUT_DIR)/$(TARGET))
endif

ifneq ($(VERBOSE),1)
VIEW:=@
SILENT:=-s
else
VIEW:=
SILENT:=
endif

ifeq ($(JOBS),)
    JOBS = $(shell grep -c ^processor /proc/cpuinfo 2>/dev/null)
        ifeq ($(JOBS),)
            JOBS = 1
        endif
endif

JOBS_D = $$(($(JOBS) * $(PARA_JOBS)))

RELEASE_FILES_EXCEPT := $(filter-out scripts/keil.py scripts/asr_template.uvoptx scripts/asr_template.uvprojx scripts/gen_3rd_project.mk \
scripts/xml_format.py scripts/keil_cfg,$(wildcard scripts/*))

release:
ifeq ($(DUET_DY),1)
	sh duet_release.sh
else ifeq ($(DUET_LIB),1)
	sh duet_lib_copy.sh
endif
	$(VIEW)rm -rf $(OUT_DIR)
	$(VIEW)rm -rf ide
	$(VIEW)rm -rf $(RELEASE_FILES_EXCEPT)
	$(VIEW)echo release done


all: app

ifeq ($(IDE),keil)
app : keil_project
endif

app:
	$(VIEW)echo asr iot making..
	$(VIEW)echo build $(TARGET)
	$(VIEW)make $(SILENT) -f $(MAKEFILES_PATH)/target.mk -j$(JOBS_D)

keil_project:
	$(VIEW)make $(SILENT) -f $(SCRIPTS_PATH)/gen_3rd_project.mk
	$(VIEW)echo generate keil project done
clean:
	$(VIEW)echo cleaning
	echo $(OUT_DIR)/$(TARGET)
	echo $(OUT_DIR)/ide
	$(VIEW)rm -rf $(OUT_DIR)/$(TARGET)
	$(VIEW)rm -rf ide
	$(VIEW)echo clean done

