# MODULE_ROOT:     The root directory of this module
# MODULE_NAME:     The name of this mudule
# SOURCE_ROOT:     Source Root Directory (default MODULE_ROOT)
# SOURCE_DIRS:     Source directories (default src)
# SOURCE_OMIT:     Ignored files
# INCLUDE_DIRS:    Include directories (default include)
# CONFIG_FILES:    Files copy to OUT_CONFIG
# ADDED_FILES:     Files copy to OUT_BIN
# CFLAGS:          gcc -c Flags
# CPPFLAGS:        cpp Flags
# CXXFLAGS:        g++ -c Flags
# LDFLAGS:         ld Flags
# LDLIBS:          ld libs
# LOADLIBES:       ld libs
# BUILD_VERBOSE:   Verbose output (MUST Before def.mk)
# BUILD_OUTPUT:    Output dir (MUST Before def.mk)

define cmd_cp
	$(Q)$(PRINT4) $(CPMSG) $(MODULE_NAME) $< $@
	$(Q2)$(CP) $< $@
endef

define cmd_mkdir
	$(Q)$(PRINT3) $(MKDIRMSG) $(MODULE_NAME) $1
	$(Q2)$(MKDIR) $1
endef

define cmd_rm
	$(Q2)[ -d $1 ] && $(RM) $1 || exit 0; \
	$(PRINT3) $(RMMSG) $(MODULE_NAME) $1
endef

define cmd_c
	$(Q)$(PRINT4) $(CCMSG) $(MODULE_NAME) $< $@
	$(Q1)$(CC) -MMD -c $(CPPFLAGS) $(CFLAGS) $< -o $@
endef

define cmd_cxx
	$(Q)$(PRINT4) $(CXXMSG) $(MODULE_NAME) $< $@
	$(Q1)$(CXX) -MMD -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
endef

ifeq ($(BUILD_ENV),debuginfo)
define cmd_debuginfo
	$(Q)$(PRINT4) $(DBGMSG) $(MODULE_NAME) $@ $@.debuginfo
	$(Q1)$(OBJCOPY) --only-keep-debug $@ $@.debuginfo
	$(Q1)$(OBJCOPY) --strip-debug $@
	$(Q1)$(OBJCOPY) --add-gnu-debuglink=$@.debuginfo $@
endef
endif

ifeq ($(BUILD_ENV),debug)
define cmd_debug
	$(Q)$(PRINT4) $(STRIPMSG) $(MODULE_NAME) $@ $@
	$(Q2)$(STRIP) $@
endef
endif

define cmd_bin
	$(Q)$(PRINT3) $(LDMSG) $(MODULE_NAME) $@
	$(Q1)$(CC) -o $@ $(OBJECT_C) $(OBJECT_CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS)
	$(call cmd_debuginfo)
	$(call cmd_debug)
endef

##############################
MODE=bin
MODULE_ROOT ?= $(shell pwd)
MODULE_NAME ?= $(shell basename $(MODULE_ROOT))

# Source FileList
SOURCE_ROOT  ?= $(MODULE_ROOT)
SOURCE_DIRS  ?= src
SOURCE_OMIT  ?=

SOURCE_C   := $(foreach dir, $(SOURCE_DIRS), $(shell find $(abspath $(dir)) -name "*.c"))
SOURCE_CXX := $(foreach dir, $(SOURCE_DIRS), $(shell find $(abspath $(dir)) -name "*.cpp"))
ifneq ($(strip $(SOURCE_OMIT)),)
SOURCE_OMIT := $(addprefix $(SOURCE_ROOT)/, $(SOURCE_OMIT))
SOURCE_C   := $(filter-out $(SOURCE_ROOT)/$(SOURCE_OMIT), $(SOURCE_C))
SOURCE_CXX := $(filter-out $(SOURCE_ROOT)/$(SOURCE_OMIT), $(SOURCE_CXX))
endif

# Object FileList
OBJECT_C   := $(SOURCE_C:$(SOURCE_ROOT)/%.c=$(OUT_OBJECT)/%.o)
OBJECT_CXX := $(SOURCE_CXX:$(SOURCE_ROOT)/%.cpp=$(OUT_OBJECT)/%.o)
DEPEND_C   := $(SOURCE_C:$(SOURCE_ROOT)/%.c=$(OUT_OBJECT)/%.d)
DEPEND_CXX := $(SOURCE_CXX:$(SOURCE_ROOT)/%.cpp=$(OUT_OBJECT)/%.d)

# Include FileList
INCLUDE_DIRS ?= $(SOURCE_ROOT)/include
INCLUDE_PATH += $(foreach dir, $(INCLUDE_DIRS), -I$(dir))
CPPFLAGS += $(INCLUDE_PATH)

# Config FileList
CONFIG_FILES   ?=
OUT_CONFIG_FILES := $(addprefix $(OUT_CONFIG)/, $(CONFIG_FILES))
CONFIG_FILES     := $(addprefix $(SOURCE_ROOT)/, $(CONFIG_FILES))

# Added FileList
ADDED_FILES    ?=
OUT_ADDED_FILES := $(addprefix $(OUT_BIN)/, $(ADDED_FILES))
ADDED_FILES     := $(addprefix $(SOURCE_ROOT)/, $(ADDED_FILES))

# BIN Name
BIN   := $(OUT_BIN)/$(MODULE_NAME)

ifeq ($(BUILD_ENV),map)
    LDFLAGS += -Wl,-Map,$@.map
endif

# CreateDirectory
OUT_DIRS += $(sort $(patsubst %/,%, $(OUT_ROOT) $(OUT_BIN) $(OUT_OBJECT) \
	$(dir $(OBJECT_C) $(OBJECT_CXX) $(OUT_CONFIG_FILES) $(OUT_ADDED_FILES))))

##############################
default: all
all: bin

.PHONY: before success
bin: before $(OBJECT_C) $(OBJECT_CXX) $(BIN) after success

before: $(OUT_DIRS)

after: $(OUT_CONFIG_FILES) $(OUT_ADDED_FILES)

success:


$(OBJECT_C):  $(OUT_OBJECT)/%.o : $(SOURCE_ROOT)/%.c
	$(call cmd_c)
-include $(DEPEND_C)

$(OBJECT_CXX):  $(OUT_OBJECT)/%.o : $(SOURCE_ROOT)/%.cpp
	$(call cmd_cxx)
-include $(DEPEND_CXX)

$(BIN): $(OBJECT_C) $(OBJECT_CXX)
	$(call cmd_bin)

$(OUT_DIRS):
	$(call cmd_mkdir,$@)

$(OUT_CONFIG_FILES): $(OUT_CONFIG)/% : $(SOURCE_ROOT)/%
	$(call cmd_cp)

$(OUT_ADDED_FILES): $(OUT_BIN)/% : $(SOURCE_ROOT)/%
	$(call cmd_cp)

.PHONY: install
install:


.PHONY: uninstall
uninstall:

.PHONY: showall show
showall: show

show:
	@echo "=============== $(CURDIR) ==============="
	@echo "BUILD_ENV          = " $(BUILD_ENV)
	@echo "BUILD_VERBOSE      = " $(BUILD_VERBOSE)
	@echo "BUILD_PWD          = " $(BUILD_PWD)
	@echo "BUILD_OUTPUT       = " $(BUILD_OUTPUT)
	@echo "D                  = " $(D)
	@echo "Q1                 = " $(Q1)
	@echo "Q2                 = " $(Q2)
	@echo "Q3                 = " $(Q3)
	@echo "O                  = " $(O)
	@echo ""

	@echo "SHELL              = " $(SHELL)
	@echo "OS_TYPE            = " $(OS_TYPE)
	@echo "CP                 = " $(CP)
	@echo "RM                 = " $(RM)
	@echo "MKDIR              = " $(MKDIR)
	@echo ""

	@echo "CURDIR             = " $(CURDIR)
	@echo "MAKEFLAGS          = " $(MAKEFLAGS)
	@echo "MAKEFILE_LIST      = " $(MAKEFILE_LIST)
	@echo "MAKECMDGOALS       = " $(MAKECMDGOALS)
	@echo "MAKEOVERRIDES      = " $(MAKEOVERRIDES)
	@echo "MAKELEVEL          = " $(MAKELEVEL)
	@echo "VPATH              = " $(VPATH)
	@echo ""

	@echo "OUT_ROOT           = " $(OUT_ROOT)
	@echo "OUT_INCLUDE        = " $(OUT_INCLUDE)
	@echo "OUT_BIN            = " $(OUT_BIN)
	@echo "OUT_LIB            = " $(OUT_LIB)
	@echo "OUT_OBJECT         = " $(OUT_OBJECT)
	@echo "OUT_CONFIG         = " $(OUT_CONFIG)
	@echo ""

	@echo "CROSS_COMPILE      = " $(CROSS_COMPILE)
	@echo "CC                 = " $(CC)
	@echo "CXX                = " $(CXX)
	@echo "CPP                = " $(CPP)
	@echo "AS                 = " $(AS)
	@echo "LD                 = " $(LD)
	@echo "AR                 = " $(AR)
	@echo "NM                 = " $(NM)
	@echo "STRIP              = " $(STRIP)
	@echo "OBJCOPY            = " $(OBJCOPY)
	@echo "OBJDUMP            = " $(OBJDUMP)
	@echo "OBJSIZE            = " $(OBJSIZE)
	@echo ""

	@echo "CPPFLAGS           = " $(CPPFLAGS)
	@echo "CFLAGS             = " $(CFLAGS)
	@echo "CXXFLAGS           = " $(CXXFLAGS)
	@echo "ASFLAGS            = " $(ASFLAGS)
	@echo "LDFLAGS            = " $(LDFLAGS)
	@echo "LOADLIBES          = " $(LOADLIBES)
	@echo "LDLIBS             = " $(LDLIBS)
	@echo "ARFLAGS            = " $(ARFLAGS)
	@echo ""

	@echo "MODE               = " $(MODE)
	@echo "MODULE_ROOT        = " $(MODULE_ROOT)
	@echo "MODULE_NAME        = " $(MODULE_NAME)
	@echo "BIN                = " $(BIN)
	@echo "SOURCE_ROOT        = " $(SOURCE_ROOT)
	@echo "SOURCE_DIRS        = " $(SOURCE_DIRS)
	@echo "SOURCE_OMIT        = " $(SOURCE_OMIT)
	@echo "SOURCE_C           = " $(SOURCE_C)
	@echo "OBJECT_C           = " $(OBJECT_C)
	@echo "DPEND_C            = " $(DEPEND_C)
	@echo "SOURCE_CXX         = " $(SOURCE_CXX)
	@echo "OBJECT_CXX         = " $(OBJECT_CXX)
	@echo "DEPEND_CXX         = " $(DEPEND_CXX)
	@echo "INCLUDE_DIRS       = " $(INCLUDE_DIRS)
	@echo "CONFIG_FILES       = " $(CONFIG_FILES)
	@echo "ADDED_FILES        = " $(ADDED_FILES)
	@echo "OUT_DIRS           = " $(OUT_DIRS)
	@echo "OUT_EXPORT_FILES   = " $(OUT_EXPORT_FILES)
	@echo "OUT_CONFIG_FILES   = " $(OUT_CONFIG_FILES)
	@echo "OUT_ADDED_FILES    = " $(OUT_ADDED_FILES)
	@echo "CreateResult       = " $(CreateResult)
	@echo ""

.PHONY: help
help:
	@echo "make <BUILD_ENV=[release|debug|debuginfo|map]> <CROSS_COMPILE=arm-linux-gnueabi-> <O=/opt/out> <V=[0|1|2|3]> <D=[0|1|2|3]> <show> <help>"
	@echo ""
	@echo "    BUILD_ENV           [release|debug|debuginfo|map] default is release"
	@echo "    CROSS_COMPILE       cross compile toolchain"
	@echo "    O                   output"
	@echo "    V                   [0|1|2|3] verbose"
	@echo "    D                   0 release | 1 debug | 2 gen debuginfo | 3 gen map"
	@echo "    show                show current configuration"
	@echo "    help                show this help"
	@echo ""
	@echo ""

	@echo "bin.mk : Build executable"
	@echo ""
	@echo "    MODULE_ROOT         the root directory of this module"
	@echo "    MODULE_NAME         the name of this mudule"
	@echo "    SOURCE_ROOT         source root directory (default MODULE_ROOT)"
	@echo "    SOURCE_DIRS         source directories (default src)"
	@echo "    SOURCE_OMIT         ignored files"
	@echo "    INCLUDE_DIRS        include directories (default include)"
	@echo "    CONFIG_FILES        files copy to OUT_CONFIG"
	@echo "    ADDED_FILES         files copy to OUT_BIN"
	@echo ""
	@echo "    BUILD_VERBOSE       verbose output (MUST before def.mk)"
	@echo "    BUILD_OUTPUT        output dir (MUST before def.mk)"
	@echo ""
	@echo "    CFLAGS              gcc -c Flags"
	@echo "    CPPFLAGS            cpp Flags"
	@echo "    CXXFLAGS            g++ -c Flags"
	@echo "    LDFLAGS             ld Flags"
	@echo "    LDLIBS              ld libs"
	@echo "    LOADLIBES           ld libs"
	@echo ""

.PHONY: clean
clean:
	$(call cmd_rm,$(OUT_ROOT))

.PHONY: distclean
distclean: clean
