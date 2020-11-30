PROJECT_ROOT ?= $(abspath ../..)
include $(PROJECT_ROOT)/build/def.mk

SOURCE_DIRS = .
LOADLIBES += -ljson-c

DUMMY := $(OUT_BIN)
OUT_BIN = $(DUMMY)/json

include $(PROJECT_ROOT)/build/multibin.mk
