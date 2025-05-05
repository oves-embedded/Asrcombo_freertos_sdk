ifeq ($(shell uname), Linux)
export EXECUTABLE_SUFFIX :=
else
export EXECUTABLE_SUFFIX := .exe
endif
include $(CONFIG_MAKEFILE)

out_dir := $(OUT_DIR)/$(TARGET)

LD_INPUT = $($(NAME)_LINK_LD)
LD_OUTPUT = $(out_dir)/$(notdir $(LD_INPUT)).generated

define PROCESS_LINKER_LDS
	sed -n '/#include/!p' $(LD_INPUT) > $(out_dir)/lds.ld
	sed -n '/#include/p' $(LD_INPUT) > $(out_dir)/lds.inc
	$(CC) -E -P -x c -dM $(out_dir)/lds.inc $(CFLAGS) $($(NAME)_CFLAGS) $($(NAME)_INCLUDES) -o $(out_dir)/lds_macros.h
	sed -i '/#define __STDC_/d' $(out_dir)/lds_macros.h
	$(CC) -E -P -x c -undef -imacros $(out_dir)/lds_macros.h $(out_dir)/lds.ld -o $(LD_OUTPUT)
#	$(VIEW)echo Preprocess linker file $(LD_INPUT) $(LD_OUTPUT)
endef

ifeq ($(findstring lib, $(TARGET)),lib)
all: $(out_dir)/$($(NAME)_LIB)
	$(VIEW)echo build lib complete

$(out_dir)/$($(NAME)_LIB): $(addprefix $(out_dir)/,$(notdir $($(NAME)_PACK_OBJ)))
	$(VIEW)echo $@
	$(VIEW)$(AR) $(COMPILER_SPECIFIC_ARFLAGS_CREATE) $@ $(addprefix $(out_dir)/,$(notdir $($(NAME)_PACK_OBJ))) $($(NAME)_PRE_LIB)
ifneq ($($(NAME)_PRE_LIB_LOACATE),)
	$(shell if [ ! -d "$($(NAME)_PRE_LIB_LOACATE)" ];then mkdir -p $($(NAME)_PRE_LIB_LOACATE); fi)
	$(VIEW)cp -r $@ $($(NAME)_PRE_LIB_LOACATE)
endif
else
all: $(if $(EXTRA_POST_BUILD_TARGETS),$(EXTRA_POST_BUILD_TARGETS),build_done)
$(EXTRA_POST_BUILD_TARGETS):build_done
build_done: $(out_dir)/$($(NAME)_GEN)$(BIN_OUTPUT_SUFFIX)
	$(VIEW)echo build complete
	$(VIEW)$(SIZE) $(out_dir)/$($(NAME)_GEN).elf

$(out_dir)/$($(NAME)_GEN)$(BIN_OUTPUT_SUFFIX): $(out_dir)/$($(NAME)_GEN)$(LINK_OUTPUT_SUFFIX)
	$(VIEW)echo $(out_dir)/$($(NAME)_GEN)$(HEX_OUTPUT_SUFFIX)
	$(VIEW)echo $@
	$(VIEW)$(OBJCOPY) $(OBJCOPY_BIN_FLAGS) $< $@
	$(VIEW)$(OBJCOPY) $(OBJCOPY_HEX_FLAGS) $< $(out_dir)/$($(NAME)_GEN)$(HEX_OUTPUT_SUFFIX)

$(out_dir)/$($(NAME)_GEN)$(LINK_OUTPUT_SUFFIX): $(addprefix $(out_dir)/,$(notdir $($(NAME)_PACK_OBJ)))
	$(VIEW)echo $@
ifneq ($(CXX_SOURCES),)
	$(call PROCESS_LINKER_LDS)
	$(VIEW)$(LINKER_CXX) $(LINKER_PLUS) $(CLIB_LDFLAGS_NANO) --specs=nosys.specs $(COMPILER_SPECIFIC_LINK_MAP)$(out_dir)/$($(NAME)_GEN)$(MAP_OUTPUT_SUFFIX) -T$(LD_OUTPUT) -o $@ $(addprefix $(out_dir)/,$(notdir $($(NAME)_PACK_OBJ))) -Wl,--start-group -Wl,--whole-archive $($(NAME)_PRE_LIB) -Wl,--no-whole-archive -lstdc++ -Wl,--end-group
else
	$(call PROCESS_LINKER_LDS)
	$(VIEW)$(LINKER) $(LINKER_PLUS) $(COMPILER_SPECIFIC_LINK_MAP)$(out_dir)/$($(NAME)_GEN)$(MAP_OUTPUT_SUFFIX) -T$(LD_OUTPUT) -o $@ $(addprefix $(out_dir)/,$(notdir $($(NAME)_PACK_OBJ))) $($(NAME)_PRE_LIB)
endif
endif


define BUILD_C_PROCESS
$(out_dir)/$(notdir $(2:.c=.o)): $(2)
	$(VIEW)echo Compiling $(notdir $(2))...
	$(VIEW)$(CC) $$(CFLAGS) $$($(1)_INCLUDES) $(CMISC) $$($(1)_CFLAGS) $$< -o $$@
endef

define BUILD_ASM_PROCESS
$(out_dir)/$(notdir $(patsubst %.S,%.o,$(2:.s=.o))): $(2)
	$(VIEW)$(CC) $$(CFLAGS) $$($(1)_CFLAGS) $$< -o $$@
endef

define BUILD_CXX_PROCESS
$(out_dir)/$(notdir $(2:.cpp=.o)): $(2)
	$(VIEW)echo Compiling $(notdir $(2))...
	$(VIEW)$(CXX) $$(CXXFLAGS) $$($(1)_INCLUDES) $(CXXMISC) $$($(1)_CFLAGS) $$< -o $$@
endef

$(foreach src,$(S_SOURCES),$(eval $(call BUILD_ASM_PROCESS,$(NAME),$(src))))
$(foreach src,$(C_SOURCES),$(eval $(call BUILD_C_PROCESS,$(NAME),$(src))))
$(foreach src,$(CXX_SOURCES),$(eval $(call BUILD_CXX_PROCESS,$(NAME),$(src))))
