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
	$(Q3)$(MAKE) -C $@ all OUT_OBJECT=$(OUT_OBJECT)/$@ OUT_DEPEND=$(OUT_DEPEND)/$@ || exit 1

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
ifeq ($(MAKELEVEL),0)
	@echo "clean done"
endif
$(SUBDIRS_CLEAN):
	$(Q3)$(MAKE) -C $(patsubst %_clean,%,$@) clean OUT_OBJECT=$(OUT_OBJECT)/$(@:%_clean=%) OUT_DEPEND=$(OUT_DEPEND)/$(@:%_clean=%) || exit 1


.PHONY: distclean $(SUBDIRS_DISTCLEAN)
distclean: $(SUBDIRS_DISTCLEAN) clean
$(SUBDIRS_DISTCLEAN):
	$(Q3)$(MAKE) -C $(patsubst %_distclean,%,$@) distclean || exit 1


.PHONY: show $(SUBDIRS_SHOW)
show: $(SUBDIRS_SHOW)
$(SUBDIRS_SHOW):
	$(Q3)$(MAKE) -C $(patsubst %_show,%,$@) show || exit 1

.PHONY: help
help:
	@echo "subdir: Build Sub Directories"
	@echo ""
	@echo "    MODULE_ROOT         the root directory of this module"
	@echo "    MODULE_NAME         the name of this mudule"
	@echo "    SUBDIRS             subdirs"
	@echo ""
