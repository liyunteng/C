# MODULE_ROOT:     The root directory of this module
# MODULE_NAME:     The name of this mudule
# SOURCE_ROOT:     Source Root Directory (default MODULE_ROOT)
# SOURCE_DIRS:     Source directories (default src)
# SOURCE_OMIT:     Ignored files
# INCLUDE_DIRS:    Include directories (default include)
# CFLAGS:          gcc -c Flags
# CPPFLAGS:        cpp Flags
# CXXFLAGS:        g++ -c Flags
# LDFLAGS:         ld Flags
# BUILD_VERBOSE:   Verbose output (MUST Before def.mk)
# BUILD_OUTPUT:    Output dir (MUST Before def.mk)

# Create Directory
CreateDirectory = $(shell [ -d $1 ] || $(MKDIR) $1 || echo "mkdir '$1' failed")
# Remove Directory
RemoveDirectory = $(shell [ -d $1 ] && $(RM) $1 || echo "rm dir '$1' failed")

MODE=bin
MODULE_ROOT ?= $(shell pwd)
MODULE_NAME ?= $(shell basename $(MODULE_ROOT))

# Source FileList
SOURCE_ROOT  ?= $(MODULE_ROOT)
SOURCE_DIRS  ?= src
SOURCE_OMIT  ?=

SOURCE_C   := $(foreach dir, $(SOURCE_DIRS), $(shell find $(SOURCE_ROOT)/$(dir) -name "*.c"))
SOURCE_CXX := $(foreach dir, $(SOURCE_DIRS), $(shell find $(SOURCE_ROOT)/$(dir) -name "*.cpp"))
ifneq ($(strip $(SOURCE_OMIT)),)
SOURCE_OMIT := $(addprefix $(SOURCE_ROOT)/, $(SOURCE_OMIT))
SOURCE_C   := $(filter-out $(SOURCE_ROOT)/$(SOURCE_OMIT), $(SOURCE_C))
SOURCE_CXX := $(filter-out $(SOURCE_ROOT)/$(SOURCE_OMIT), $(SOURCE_CXX))
endif

# Object FileList
OBJECT_C   := $(SOURCE_C:$(SOURCE_ROOT)/%.c=$(OUT_OBJECT)/$(MODULE_NAME)/%.o)
OBJECT_CXX := $(SOURCE_CXX:$(SOURCE_ROOT)/%.cpp=$(OUT_OBJECT)/$(MODULE_NAME)/%.o)
DEPEND_C   := $(SOURCE_C:$(SOURCE_ROOT)/%.c=$(OUT_DEPEND)/$(MODULE_NAME)/%.d)
DEPEND_CXX := $(SOURCE_CXX:$(SOURCE_ROOT)/%.cpp=$(OUT_DEPEND)/$(MODULE_NAME)/%.d)

# Include Configure
INCLUDE_DIRS ?= $(SOURCE_ROOT)/include
INCLUDE_PATH += $(foreach dir, $(INCLUDE_DIRS), -I$(dir))
CPPFLAGS += $(INCLUDE_PATH)

# BIN Name
BIN   := $(OUT_BIN)/$(MODULE_NAME)

# CreateDirectory
OUT_OBJECT_DIRS := $(sort $(dir $(OBJECT_C)))
OUT_OBJECT_DIRS += $(sort $(dir $(OBJECT_CXX)))
OUT_OBJECT_DIRS += $(sort $(dir $(DEPEND_C)))
OUT_OBJECT_DIRS += $(sort $(dir $(DEPEND_CXX)))
CreateResult :=
ifeq ($(MAKECMDGOALS),all)
CreateResult := $(call CreateDirectory, $(OUT_ROOT))
CreateResult += $(call CreateDirectory, $(OUT_OBJECT))
CreateResult += $(call CreateDirectory, $(OUT_BIN))
dummy := $(foreach dir, $(OUT_OBJECT_DIRS), CreateResult += $(call CreateDirectory, $(dir)))
else ifeq ($(MAKECMDGOALS),)
CreateResult := $(call CreateDirectory, $(OUT_ROOT))
CreateResult += $(call CreateDirectory, $(OUT_OBJECT))
CreateResult += $(call CreateDirectory, $(OUT_BIN))
dummy := $(foreach dir, $(OUT_OBJECT_DIRS), CreateResult += $(call CreateDirectory, $(dir)))
endif
ifneq ($(strip $(CreateResult)),)
	err = $(error create directory failed: $(CreateResult))
endif

# Compiler
default: all
all: bin

.PHONY: before after success
bin: before $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX) $(BIN) after success

before:

after:

success:

$(DEPEND_C): $(OUT_DEPEND)/$(MODULE_NAME)/%.d : $(SOURCE_ROOT)/%.c
	@printf $(FORMAT) $(DEPENDMSG) $(MODULE_NAME) $@
	$(Q)set -e; \
	$(CC) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
ifeq ($(MAKECMDGOALS),all)
sinclude $(DEPEND_C)
else ifeq ($(MAKECMDGOALS),)
sinclude $(DEPEND_C)
endif

$(OBJECT_C):  $(OUT_OBJECT)/$(MODULE_NAME)/%.o : $(SOURCE_ROOT)/%.c
	@printf $(FORMAT) $(CCMSG) $(MODULE_NAME) $@
	$(Q) $(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(DEPEND_CXX) : $(OUT_DEPEND)/$(MODULE_NAME)/%.d : $(SOURCE_ROOT)/%.cpp
	@printf $(FORMAT) $(DEPENDMSG) $(MODULE_NAME) $@
	$(Q)set -e; \
	$(CC) -MM $(CPPFLAGS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
ifeq ($(MAKECMDGOALS),all)
sinclude $(DEPEND_CXX)
else ifeq ($(MAKECMDGOALS),)
sinclude $(DEPEND_CXX)
endif

$(OBJECT_CXX):  $(OUT_OBJECT)/$(MODULE_NAME)/%.o : $(SOURCE_ROOT)/%.cpp
	@printf $(FORMAT) $(CXXMSG) $(MODULE_NAME) $@
	$(Q) $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(BIN): $(DEPEND_C) $(OBJECT_C) $(DEPEND_CXX) $(OBJECT_CXX)
	@printf $(FORMAT) $(LDMSG) $(MODULE_NAME) $@
	$(Q)$(CC) -o $@ $(OBJECT_C) $(OBJECT_CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS)
ifeq ($(BUILD_ENV),release)
	@printf $(FORMAT) $(STRIPMSG) $(MODULE_NAME) $@
	$(Q)$(STRIP) $@
endif

.PHONY: install
install:


.PHONY: uninstall
uninstall:

.PHONY: help
help:
	@echo "bin: Build executable"
	@echo ""
	@echo "    MODULE_ROOT         the root directory of this module"
	@echo "    MODULE_NAME         the name of this mudule"
	@echo "    SOURCE_ROOT         source root directory (default MODULE_ROOT)"
	@echo "    SOURCE_DIRS         source directories (default src)"
	@echo "    SOURCE_OMIT         ignored files"
	@echo "    INCLUDE_DIRS        include directories (default include)"
	@echo ""
	@echo "    BUILD_VERBOSE       verbose output (MUST before def.mk)"
	@echo "    BUILD_OUTPUT        output dir (MUST before def.mk)"
	@echo ""
	@echo "    CFLAGS              gcc -c Flags"
	@echo "    CPPFLAGS            cpp Flags"
	@echo "    CXXFLAGS            g++ -c Flags"
	@echo "    LDFLAGS             ld Flags"
	@echo ""

.PHONY: clean
clean:
# $(Q)$(RM) $(OBJECT_C) $(OBJECT_CXX)
# $(Q)$(RM) $(BIN)
# $(Q)$(RM) $(DEPEND_C) $(DEPEND_CXX)
# $(Q)[ "`ls $(OUT_OBJECT)`" ] || $(RM) $(OUT_OBJECT)
# $(Q)[ "`ls $(OUT_BIN)`" ] || $(RM) $(OUT_BIN)
# $(Q)[ "`ls $(OUT_DEPEND)`" ] || $(RM) $(OUT_DEPEND)
# $(Q)[ "`ls $(OUT_ROOT)`" ] || $(RM) $(OUT_ROOT)
	$(Q)$(RM) $(OUT_ROOT)
ifeq ($(MAKELEVEL),0)
	@echo "clean done"
endif

.PHONY: distclean
distclean: clean
