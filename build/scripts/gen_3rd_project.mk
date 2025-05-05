include $(CONFIG_MAKEFILE)

comma:=,
semi:=;
py_script := $(SCRIPTS_PATH)/$(NAME)_config.py
keil_gen_script := $(SCRIPTS_PATH)/keil.py
FILE_CREATE = $(file >$(1),$(2))
FILE_APPEND = $(file >>$(1),$(2))

PATH_PREFIX := ../../../
GROUPS := $(foreach group, $($(NAME)_SOURCE_GROUP),'$(group)'$(comma))
SRC_PATH := $(foreach src, $($(NAME)_SOURCE),'$(addprefix $(PATH_PREFIX),$(src))'$(comma))
SRC_PATH += $(foreach lib, $($(NAME)_PRE_LIB),'$(addprefix $(PATH_PREFIX),$(lib))'$(comma))
INC_PATH := $(foreach inc, $($(NAME)_INC_PATH),$(addprefix $(PATH_PREFIX),$(inc))$(semi))
DEFINES := $(foreach dflags, $($(NAME)_CFLAGS), $(dflags)$(comma))
ASMDEFINES := $(foreach adflags,$($(NAME)_AFLAGS),$(adflags)$(comma))
C_MiscControls := $(CMISC)
A_MissControls := $(AMISC)

ifeq ($($(NAME)_PRO_OUT),LIB)
OUTPUT_TARGET := $(subst lib,,$($(NAME)_LIB))
else
OUTPUT_TARGET := $(NAME)
endif

ifeq ($(IDE),keil)
CPU_TYPE := Cortex-M4

ifneq ($($(NAME)_LINK_LD),)
LD_FILES := $(PATH_PREFIX)$($(NAME)_LINK_LD)
endif

ifneq ($($(NAME)_LINK_INI),)
INI_FILES := $(PATH_PREFIX)$($(NAME)_LINK_INI)
endif

LD_MISC := $(LINKER_MISC)

ifneq ($(LINKER_MISC_FILE),)
LD_MISC += $(PATH_PREFIX)$(LINKER_MISC_FILE)
endif

RUNUSER1 :=1
RUNUSER2 :=0
RUNUSER1_PRO :=$(PATH_PREFIX)tools/genbin.bat $(NAME)
RUNUSER2_PRO :=

ifeq ($(findstring LIB,$($(NAME)_PRO_OUT)),LIB)
CREATELIB := 1
CREATEEXE := 0
else
CREATEEXE := 1
CREATELIB := 0
endif
endif


all:
ifeq ($(IDE),keil)
	$(call FILE_CREATE,$(py_script),#!/usr/bin/env python)
	$(call FILE_APPEND,$(py_script),#-*- coding: UTF-8 -*-)
	$(call FILE_APPEND,$(py_script),group=[$(GROUPS)])
	$(call FILE_APPEND,$(py_script),src=[$(SRC_PATH)])
	$(call FILE_APPEND,$(py_script),include_path='$(INC_PATH)')
	$(call FILE_APPEND,$(py_script),defines='$(DEFINES)')
	$(call FILE_APPEND,$(py_script),adefines='$(ASMDEFINES)')
	$(call FILE_APPEND,$(py_script),cMisc='$(C_MiscControls)')
	$(call FILE_APPEND,$(py_script),aMisc='$(A_MissControls)')
	$(call FILE_APPEND,$(py_script),host_arch='$(CPU_TYPE)')
	$(call FILE_APPEND,$(py_script),ld_files='$(LD_FILES)')
	$(call FILE_APPEND,$(py_script),ini_files='$(INI_FILES)')
	$(call FILE_APPEND,$(py_script),ld_misc='$(LD_MISC)')
	$(call FILE_APPEND,$(py_script),runuser1='$(RUNUSER1)')
	$(call FILE_APPEND,$(py_script),runuser2='$(RUNUSER2)')
	$(call FILE_APPEND,$(py_script),runuser1_pro='$(RUNUSER1_PRO)')
	$(call FILE_APPEND,$(py_script),runuser2_pro='$(RUNUSER2_PRO)')
	$(call FILE_APPEND,$(py_script),createLib='$(CREATELIB)')
	$(call FILE_APPEND,$(py_script),createexe='$(CREATEEXE)')
	echo $(OUTPUT_TARGET)
	$(VIEW)$(PYTHON) $(keil_gen_script) $(NAME) $(OUTPUT_TARGET)
else
	$(VIEW)echo not support IDE currently
endif
