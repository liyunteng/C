# MODULE_ROOT:     The root directory of this module
# MODULE_NAME:     The name of this mudule
# SUBDIRS:          subdirs
MODE := subdir
MODULE_ROOT ?= $(shell pwd)
MODULE_NAME ?= $(shell basename $(MODULE_ROOT))

SUBDIRS ?=

SUBDIRS_BUILD     := $(SUBDIRS)
SUBDIRS_INSTALL   := $(addsuffix _install, $(SUBDIRS))
SUBDIRS_UNINSTALL := $(addsuffix _uninstall, $(SUBDIRS))
SUBDIRS_CLEAN     := $(addsuffix _clean, $(SUBDIRS))
SUBDIRS_DISTCLEAN := $(addsuffix _distclean, $(SUBDIRS))
SUBDIRS_SHOW      := $(addsuffix _show, $(SUBDIRS))

unexport MODE MODULE_ROOT MODULE_NAME
unexport SUBDIRS SUBDIRS_BUILD SUBDIRS_INSTALL SUBDIRS_UNINSTALL
unexport SUBDIRS_CLEAN SUBDIRS_DISTCLEAN SUBDIRS_SHOW

######################################################################
default: all
all: subdir

.PHONY: subdir $(SUBDIRS_BUILD)
subdir: $(SUBDIRS_BUILD)
$(SUBDIRS_BUILD):
	$(Q3)$(MAKE) -C $@ all OUT_OBJECT=$(OUT_OBJECT)/$@ || exit 1

.PHONY: install $(SUBDIRS_INSTALL)
install: $(SUBDIRS_INSTALL)
$(SUBDIRS_INSTALL):
	$(Q3)$(MAKE) -C $(patsubst %_install,%,$@) install || exit 1

.PHONY: uninstall $(SUBDIRS_UNINSTALL)
uninstall: $(SUBDIRS_UNINSTALL)
$(SUBDIRS_UNINSTALL):
	$(Q3)$(MAKE) -C $(patsubst %_uninstall,%,$@) uninstall || exit 1

.PHONY: clean $(SUBDIRS_CLEAN)
clean: $(SUBDIRS_CLEAN)
$(SUBDIRS_CLEAN):
	$(Q3)$(MAKE) -C $(patsubst %_clean,%,$@) clean OUT_OBJECT=$(OUT_OBJECT)/$(@:%_clean=%) || exit 1


.PHONY: distclean $(SUBDIRS_DISTCLEAN)
distclean: $(SUBDIRS_DISTCLEAN) clean
$(SUBDIRS_DISTCLEAN):
	$(Q3)$(MAKE) -C $(patsubst %_distclean,%,$@) distclean || exit 1


.PHONY: showall $(SUBDIRS_SHOW) show
showall: $(SUBDIRS_SHOW) show
$(SUBDIRS_SHOW):
	$(Q3)$(MAKE) -C $(patsubst %_show,%,$@) showall OUT_OBJECT=$(OUT_OBJECT)/$(@:%_show=%) || exit 1

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
	@echo "SUBDIRS            = " $(SUBDIRS)
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

	@echo "subdir.mk : Build Sub Directories"
	@echo ""
	@echo "    MODULE_ROOT         the root directory of this module"
	@echo "    MODULE_NAME         the name of this mudule"
	@echo "    SUBDIRS             subdirs"
	@echo ""
