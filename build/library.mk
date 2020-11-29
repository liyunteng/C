# MODULE_ROOT:     The root directory of this module
# MODULE_NAME:     The name of this mudule
# LIB_TYPE:        Library type [static/dynamic/all]
# SOURCE_ROOT:     Source Root Directory (default MODULE_ROOT)
# SOURCE_DIRS:     Source directories (default src)
# SOURCE_OMIT:     Ignored files
# INCLUDE_DIRS:    Include directories (default include)
# EXPORT_DIR:     Export include directories (default include)
# CONFIG_FILES:    Files copy to OUT_CONFIG
# ADDED_FILES:     Files copy to OUT_BIN
# CFLAGS:          gcc -c Flags (added -fPIC)
# CPPFLAGS:        cpp Flags
# CXXFLAGS:        g++ -c Flags
# ARFLAGS:         ar Flags (Default rcs)
# LDFLAGS:         ld Flags (Added -shared for shared)
# BUILD_VERBOSE:   verbose output (MUST Before def.mk)
# BUILD_OUTPUT:    output dir (MUST Before def.mk)

# Create Directory
CreateDirectory = $(shell [ ! -d $1 ] && ($(MKDIR) $1 || echo "mkdir '$1' failed"))
# Remove Directory
RemoveDirectory = $(shell [ -d $1 ] && ($(RM) $1 || echo "rm dir '$1' failed"))

define cmd_cp
	$(PRINT4) $(CPMSG) $(MODULE_NAME) $< $@
	$(Q2)$(CP) $< $@
endef

define cmd_mkdir
	$(PRINT3) $(MKDIRMSG) $(MODULE_NAME) $1
	$(Q2)$(MKDIR) $1
endef

define cmd_rm
	$(PRINT3) $(RMMSG) $(MODULE_NAME) $1
	$(Q2)$(RM) $1
endef

define cmd_dep_c
	$(PRINT4) $(DEPENDMSG) $(MODULE_NAME) $< $@
	$(Q3)set -e; \
	$(CC) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
endef

define cmd_dep_cxx
	$(PRINT4) $(DEPENDMSG) $(MODULE_NAME) $< $@
	$(Q3)set -e; \
	$(CC) -MM $(CPPFLAGS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
endef

define cmd_c
	$(PRINT4) $(CCMSG) $(MODULE_NAME) $< $@
	$(Q1)$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
endef

define cmd_cxx
	$(PRINT4) $(CXXMSG) $(MODULE_NAME) $< $@
	$(Q1)$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
endef

ifeq ($(BUILD_ENV),debuginfo)
define cmd_debuginfo
	$(PRINT4) $(DBGMSG) $(MODULE_NAME) $@ $@.debuginfo
	$(Q1)$(OBJCOPY) --only-keep-debug $@ $@.debuginfo
	$(Q1)$(OBJCOPY) --strip-debug $@
	$(Q1)$(OBJCOPY) --add-gnu-debuglink=$@.debuginfo $@
endef
endif

ifeq ($(BUILD_ENV),debug)
define cmd_debug
	$(PRINT4) $(STRIPMSG) $(MODULE_NAME) $@ $@
	$(Q2)$(STRIP) $@
endef
endif
define cmd_ar
	$(PRINT3) $(ARMSG) $(MODULE_NAME) $@
	$(Q1)$(AR) $(ARFLAGS) $@ $(OBJECT_C) $(OBJECT_CXX)
	$(call cmd_debug)
endef

define cmd_solib
	$(PRINT3) $(LDMSG) $(MODULE_NAME) $@
	$(Q1)$(CC) -o $@ $(OBJECT_C) $(OBJECT_CXX) -shared $(LDFLAGS)
	$(call cmd_debuginfo)
	$(call cmd_debug)
endef

MODE=library
MODULE_ROOT ?= $(shell pwd)
MODULE_NAME ?= $(shell basename $(MODULE_ROOT))

# static/dynamic/all
LIB_TYPE    ?= all

# Source FileList
SOURCE_ROOT  ?= $(MODULE_ROOT)
SOURCE_DIRS  ?= src
SOURCE_OMIT  ?=

SOURCE_C   := $(foreach dir, $(SOURCE_DIRS), $(shell find $(abspath $(dir)) -name "*.c"))
SOURCE_CXX := $(foreach dir, $(SOURCE_DIRS), $(shell find $(abspath $(dir)) -name "*.cpp"))
ifneq ($(strip $(SOURCE_OMIT)),)
SOURCE_OMIT := $(addprefix $(SOURC_ROOT)/, $(SOURCE_OMIT))
SOURCE_C   := $(filter-out $(SOURCE_OMIT), $(SOURCE_C))
SOURCE_CXX := $(filter-out $(SOURCE_OMIT), $(SOURCE_CXX))
endif

# Object FileList
OBJECT_C   := $(SOURCE_C:$(SOURCE_ROOT)/%.c=$(OUT_OBJECT)/%.o)
OBJECT_CXX := $(SOURCE_CXX:$(SOURCE_ROOT)/%.cpp=$(OUT_OBJECT)/%.o)
DEPEND_C   := $(SOURCE_C:$(SOURCE_ROOT)/%.c=$(OUT_DEPEND)/%.d)
DEPEND_CXX := $(SOURCE_CXX:$(SOURCE_ROOT)/%.cpp=$(OUT_DEPEND)/%.d)

# Include FileList
INCLUDE_DIR ?= include
CPPFLAGS += $(foreach dir, $(SOURCE_ROOT)/$(INCLUDE_DIRS), -I$(dir))
CFLAGS += -fPIC

# Config FileList
CONFIG_FILES  ?=
OUT_CONFIG_FILES := $(addprefix $(OUT_CONFIG)/, $(CONFIG_FILES))
CONFIG_FILES     := $(addprefix $(SOURCE_ROOT)/, $(CONFIG_FILES))

# Added FileList
ADDED_FILES  ?=
OUT_ADDED_FILES := $(addprefix $(OUT_BIN)/, $(ADDED_FILES))
ADDED_FILES     := $(addprefix $(SOURCE_ROOT)/, $(ADDED_FILES))

# Export dirs
EXPORT_DIR := $(SOURCE_ROOT)/include
EXPORT_FILES := $(foreach dir, $(EXPORT_DIR), $(shell find $(dir) -type f))
OUT_EXPORT_FILES := $(EXPORT_FILES:$(EXPORT_DIR)/%=$(OUT_INCLUDE)/%)
CPPFLAGS += -I$(EXPORT_DIR)

# Lib Name
LIB   := $(OUT_LIB)/lib$(MODULE_NAME).a
SOLIB := $(OUT_LIB)/lib$(MODULE_NAME).so

ifeq ($(BUILD_ENV),map)
    LDFLAGS += -Wl,-Map,$@.map
endif

# CreateDirectory
OUT_DIRS := $(sort $(OUT_ROOT) $(OUT_LIB) $(OUT_OBJECT) $(OUT_DEPEND))
OUT_DIRS += $(sort $(dir $(OBJECT_C) $(OBJECT_CXX) $(DEPEND_C) $(DEPEND_CXX) $(OUT_EXPORT_FILES) $(OUT_CONFIG_FILES) $(OUT_ADDED_FILES)))
CreateResult :=
ifeq ($(MAKECMDGOALS),all)
CreateResult += $(foreach dir, $(OUT_DIRS), $(call CreateDirectory, $(dir)))
else ifeq ($(MAKECMDGOALS),)
CreateResult += $(foreach dir, $(OUT_DIRS), $(call CreateDirectory, $(dir)))
endif
ifneq ($(strip $(CreateResult)),)
	$(error create directory failed: $(CreateResult))
endif

##############################
default:all

all: library

.PHONY: before success
ifeq ($(strip $(LIB_TYPE)),static)
library: before header $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX) $(LIB)  after success
else ifeq ($(strip $(LIB_TYPE)),dynamic)
library: before header  $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX) $(SOLIB) after success
else ifeq ($(strip $(LIB_TYPE)),all)
library: before header $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX) $(LIB) $(SOLIB) after success
endif

before:

after: $(OUT_CONFIG_FILES) $(OUT_ADDED_FILES)

success:

header: $(OUT_EXPORT_FILES)

$(DEPEND_C): $(OUT_DEPEND)/%.d : $(SOURCE_ROOT)/%.c
	$(call cmd_dep_c)
ifeq ($(MAKECMDGOALS),all)
-include $(DEPEND_C)
else ifeq ($(MAKECMDGOALS),)
-include $(DEPEND_C)
endif

$(OBJECT_C):  $(OUT_OBJECT)/%.o : $(SOURCE_ROOT)/%.c
	$(call cmd_c)

$(DEPEND_CXX) : $(OUT_DEPEND)/%.d : $(SOURCE_ROOT)/%.cpp
	$(call cmd_dep_cxx)
ifeq ($(MAKECMDGOALS),all)
-include $(DEPEND_CXX)
else ifeq ($(MAKECMDGOALS),)
-include $(DEPEND_CXX)
endif

$(OBJECT_CXX): $(OUT_OBJECT)/%.o : $(SOURCE_ROOT)/%.cpp
	$(call cmd_cxx)

$(LIB): $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX)
	$(call cmd_ar)

$(SOLIB): $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX)
	$(call cmd_solib)

$(OUT_EXPORT_FILES) : $(OUT_INCLUDE)/% : $(EXPORT_DIR)/%
	$(call cmd_cp)

$(OUT_CONFIG_FILES) : $(OUT_CONFIG)/% : $(SOURCE_ROOT)/%
	$(call cmd_cp)

$(OUT_ADDED_FILES) : $(OUT_BIN)/% : %(SOURCE_ROOT)/%
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
	@echo "OUT_DEPEND         = " $(OUT_DEPEND)
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
	@echo "LIB_TYPE           = " $(LIB_TYPE)
	@echo "LIB                = " $(LIB)
	@echo "SOLIB              = " $(SOLIB)
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

	@echo "librarymk : Build Library"
	@echo ""
	@echo "    MODULE_ROOT         the root directory of this module"
	@echo "    MODULE_NAME         the name of this mudule"
	@echo "    LIB_TYPE            library type [static/dynamic/all]"
	@echo "    SOURCE_ROOT         source Root Directory (default MODULE_ROOT)"
	@echo "    SOURCE_DIRS         source directories (default src)"
	@echo "    SOURCE_OMIT         ignored files"
	@echo "    INCLUDE_DIRS        include directories (default include)"
	@echo "    EXPORT_DIR          export include directory (default include)"
	@echo "    CONFIG_FILES        files copy to OUT_CONFIG"
	@echo "    ADDED_FILES         files copy to OUT_BIN "
	@echo ""

	@echo "    BUILD_VERBOSE       verbose output (MUST Before def.mk)"
	@echo "    BUILD_OUTPUT        output dir (MUST Before def.mk)"
	@echo ""
	@echo "    CFLAGS              gcc -c Flags (add -fPIC)"
	@echo "    CPPFLAGS            cpp Flags"
	@echo "    CXXFLAGS            g++ -c Flags"
	@echo "    ARFLAGS             ar Flags (Default rcs)"
	@echo "    LDFLAGS             ld Flags (Added -shared for shared)"
	@echo ""

.PHONY: clean
clean:
# $(Q2)$(RM) $(OBJECT_C) $(OBJECT_CXX)
# $(Q2)$(RM) $(LIB) $(SOLIB)
# $(Q2)$(RM) $(DEPEND_C) $(DEPEND_CXX)
# $(Q2)$(RM) $(HEADER_FILES)
# $(Q2)[ "`ls $(OUT_OBJECT)`"  ] || $(RM) $(OUT_OBJECT)
# $(Q2)[ "`ls $(OUT_INCLUDE)`" ] || $(RM) $(OUT_INCLUDE)
# $(Q2)[ "`ls $(OUT_LIB)`" ] || $(RM) $(OUT_LIB)
# $(Q2)[ "`ls $(OUT_DEPEND)`" ] || $(RM) $(OUT_DEPEND)
# $(Q2)[ "`ls $(OUT_ROOT)`" ] || $(RM) $(OUT_ROOT)
	$(call cmd_rm,$(OUT_ROOT))

.PHONY: distclean
pdistclean: clean
