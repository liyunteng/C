# MODULE_ROOT:     The root directory of this module
# MODULE_NAME:     The name of this mudule
# LIB_TYPE:        Library type [static/dynamic/all]
# SOURCE_ROOT:     Source Root Directory (default MODULE_ROOT)
# SOURCE_DIRS:     Source directories (default src)
# SOURCE_OMIT:     Ignored files
# INCLUDE_DIRS:    Include directories (default include)
# EXPORT_DIRS:     Export include directories (default include)
# CFLAGS:          gcc -c Flags (added -fPIC)
# CPPFLAGS:        cpp Flags
# CXXFLAGS:        g++ -c Flags
# ARFLAGS:         ar Flags (Default rcs)
# LDFLAGS:         ld Flags (Added -shared for shared)
# BUILD_VERBOSE:   verbose output (MUST Before def.mk)
# BUILD_OUTPUT:    output dir (MUST Before def.mk)

# Create Directory
CreateDirectory = $(shell [ -d $1 ] || mkdir -p $1 || echo "mkdir '$1' failed")
# Remove Directory
RemoveDirectory = $(shell [ -d $1 ] && rm -rf $1 || echo "rm dir '$1' failed")


MODE=library
MODULE_ROOT ?= $(shell pwd)
MODULE_NAME ?= $(shell basename $(MODULE_ROOT))

# static/dynamic/all
LIB_TYPE    ?= all

# Source FileList
SOURCE_ROOT  ?= $(MODULE_ROOT)
SOURCE_DIRS  ?= src
SOURCE_OMIT  ?=

SOURCE_C   := $(foreach dir, $(SOURCE_DIRS), $(shell find $(SOURCE_ROOT)/$(dir) -name "*.c"))
SOURCE_CXX := $(foreach dir, $(SOURCE_DIRS), $(shell find $(SOURCE_ROOT)/$(dir) -name "*.cpp"))
ifneq ($(strip $(SOURCE_OMIT)),)
SOURCE_OMIT := $(addprefix $(SOURC_ROOT)/, $(SOURCE_OMIT))
SOURCE_C   := $(filter-out $(SOURCE_OMIT), $(SOURCE_C))
SOURCE_CXX := $(filter-out $(SOURCE_OMIT), $(SOURCE_CXX))
endif
# Object FileList
OBJECT_C   := $(SOURCE_C:$(SOURCE_ROOT)/%.c=$(OUT_OBJECT)/%.o)
OBJECT_CXX := $(SOURCE_CXX:$(SOURCE_ROOT)/%.cpp=$(OUT_OBJECT)/%.o)
DEPEND_C   := $(OBJECT_C:$(OUT_OBJECT)/%.o=$(OUT_DEPEND)/%.d)
DEPEND_CXX := $(OBJECT_CXX:$(OUT_OBJECT)/%.o=$(OUT_DEPEND)/%.d)

# Include Configure
INCLUDE_DIRS ?= include
INCLUDE_PATH += $(foreach dir, $(SOURCE_ROOT)/$(INCLUDE_DIRS), -I$(dir))
CPPFLAGS += $(INCLUDE_PATH)
CFLAGS += -fPIC

# Export include dirs
EXPORT_DIRS ?= include

# Lib Name
LIB   := $(OUT_LIB)/lib$(MODULE_NAME).a
SOLIB := $(OUT_LIB)/lib$(MODULE_NAME).so

# CreateDirectory
OUT_OBJECT_DIRS := $(sort $(dir $(OBJECT_C)))
OUT_OBJECT_DIRS += $(sort $(dir $(OBJECT_CXX)))
CreateResult :=
ifeq ($(MAKECMDGOALS),all)
CreateResult += $(call CreateDirectory, $(OUT_ROOT))
CreateResult += $(call CreateDirectory, $(OUT_INCLUDE))
CreateResult += $(call CreateDirectory, $(OUT_DEPEND))
CreateResult += $(call CreateDirectory, $(OUT_OBJECT))
CreateResult += $(call CreateDirectory, $(OUT_LIB))
dummy := $(foreach dir, $(OUT_OBJECT_DIRS), CreateResult += $(call CreateDirectory, $(dir)))
ifneq ($(strip $(CreateResult)),)
	err = $(error create directory failed: $(CreateResult))
endif
else ifeq ($(MAKECMDGOALS),)
CreateResult += $(call CreateDirectory, $(OUT_ROOT))
CreateResult += $(call CreateDirectory, $(OUT_INCLUDE))
CreateResult += $(call CreateDirectory, $(OUT_DEPEND))
CreateResult += $(call CreateDirectory, $(OUT_OBJECT))
CreateResult += $(call CreateDirectory, $(OUT_LIB))
dummy := $(foreach dir, $(OUT_OBJECT_DIRS), CreateResult += $(call CreateDirectory, $(dir)))
ifneq ($(strip $(CreateResult)),)
	err = $(error create directory failed: $(CreateResult))
endif
endif

# Compiler
default:all

all: library

ifeq ($(strip $(LIB_TYPE)),static)
library: before header $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX) $(LIB) after success
else ifeq ($(strip $(LIB_TYPE)),dynamic)
library: before header  $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX) $(SOLIB) after success
else ifeq ($(strip $(LIB_TYPE)),all)
library: before header $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX) $(LIB) $(SOLIB) after success
endif


before:

after:

success:

header:
	$(Q)for dir in $(EXPORT_DIRS); do \
		if [ -d $$dir ]; then \
			cp -ru $(SOURCE_ROOT)/$$dir/* $(OUT_INCLUDE) ;\
		fi \
	done

$(DEPEND_C): $(OUT_DEPEND)/%.d : $(SOURCE_ROOT)/%.c
	@printf $(FORMAT) $(DEPENDMSG) $(MODULE_NAME) $@
	$(Q)set -e;$(CC) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
ifeq ($(MAKECMDGOALS),all)
sinclude $(DEPEND_C)
endif

$(OBJECT_C):  $(OUT_OBJECT)/%.o : $(SOURCE_ROOT)/%.c
	@printf $(FORMAT) $(CCMSG) $(MODULE_NAME) $@
	$(Q)$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(DEPEND_CXX) : $(OUT_DEPEND)/%.d : $(SOURCE_ROOT)/%.cpp
	@printf $(FORMAT) $(DEPENDMSG) $(MODULE_NAME) $@
	$(Q)set -e;$(CC) -MM $(CPPFLAGS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
ifeq ($(MAKECMDGOALS),all)
sinclude $(DEPEND_CXX)
endif

$(OBJECT_CXX): $(OUT_OBJECT)/%.o : $(SOURCE_ROOT)/%.cpp
	@printf $(FORMAT) $(CXXMSG) $(MODULE_NAME) $@
	$(Q) $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(LIB): $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX)
	@printf $(FORMAT) $(ARMSG) $(MODULE_NAME) $@
	$(Q)$(AR) $(ARFLAGS) $@ $(OBJECT_C) $(OBJECT_CXX)
ifeq ($(BUILD_ENV),release)
	@printf $(FORMAT) $(STRIPMSG) $(MODULE_NAME) $@
	$(Q)$(STRIP) $@
endif

$(SOLIB): $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX)
	@printf $(FORMAT) $(LDMSG) $(MODULE_NAME) $@
	$(Q)$(CC) -shared $(LDFLAGS) $(OBJECT_C) $(OBJECT_CXX) -o $@
ifeq ($(BUILD_ENV),release)
	@printf $(FORMAT) $(STRIPMSG) $(MODULE_NAME) $@
	$(Q)$(STRIP) $@
endif

.PHONY: help
help:
	@echo "library: Build Library"
	@echo ""
	@echo "    MODULE_ROOT         the root directory of this module"
	@echo "    MODULE_NAME         the name of this mudule"
	@echo "    LIB_TYPE            library type [static/dynamic/all]"
	@echo "    SOURCE_ROOT         source Root Directory (default MODULE_ROOT)"
	@echo "    SOURCE_DIRS         source directories (default src)"
	@echo "    SOURCE_OMIT         ignored files"
	@echo "    INCLUDE_DIRS        include directories (default include)"
	@echo "    EXPORT_DIRS         Export include directories (default include)"

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
# $(Q)$(RM) -rf $(OBJECT_C) $(OBJECT_CXX)
# $(Q)$(RM) -rf $(LIB) $(SOLIB)
# $(Q)$(RM) -rf $(DEPEND_C) $(DEPEND_CXX)
# $(Q)$(RM) -rf $(HEADER_FILES)
# $(Q)[ "`ls $(OUT_OBJECT)`"  ] || rm -rf $(OUT_OBJECT)
# $(Q)[ "`ls $(OUT_INCLUDE)`" ] || rm -rf $(OUT_INCLUDE)
# $(Q)[ "`ls $(OUT_LIB)`" ] || rm -rf $(OUT_LIB)
# $(Q)[ "`ls $(OUT_DEPEND)`" ] || rm -rf $(OUT_DEPEND)
# $(Q)[ "`ls $(OUT_ROOT)`" ] || rm -rf $(OUT_ROOT)
	$(Q)$(RM) -rf $(OUT_ROOT)''
ifeq ($(MAKELEVEL),0)
	@echo "clean done"
endif
