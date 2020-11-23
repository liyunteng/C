# SUBDIRS:          subdirs
MODE=subdir
SUBDIRS ?=

SUBDIRS_BUILD=$(addsuffix _build, $(SUBDIRS))
SUBDIRS_CLEAN=$(addsuffix _clean, $(SUBDIRS))
SUBDIRS_SHOW=$(addsuffix _show, $(SUBDIRS))
unexport SUBDIRS SUBDIRS_BUILD SUBDIRS_CLEAN SUBDIRS_SHOW

default: all
all: subdir

.PHONY: subdir
subdir: $(SUBDIRS_BUILD)

.PHONY: $(SUBDIRS_BUILD)
$(SUBDIRS_BUILD):
	@$(MAKE) -C $(patsubst %_build,%,$@) all || exit 1

.PHONY: clean
clean: $(SUBDIRS_CLEAN)
ifeq ($(MAKELEVEL),0)
	@echo "clean done"
endif

.PHONY: $(SUBDIRS_CLEAN)
$(SUBDIRS_CLEAN):
	@$(MAKE) -C $(patsubst %_clean,%,$@) clean  || exit 1

.PHONY: show
show: $(SUBDIRS_SHOW)

.PHONY: $(SUBDIRS_SHOW)
$(SUBDIRS_SHOW):
	@$(MAKE) -C $(patsubst %_show,%,$@) show || exit 1

.PHONY: help
help:
	@echo "subdir: Build Sub Directories"
	@echo ""
	@echo "    SUBDIRS:          subdirs"
	@echo ""
