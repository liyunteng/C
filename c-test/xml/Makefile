PROJECT_ROOT ?= $(abspath ../..)
include $(PROJECT_ROOT)/build/def.mk

ADDED_FILES += ata2slot.xml test.xml
INCLUDE_DIRS += /usr/include/libxml2
LOADLIBES += -lxml2

DUMMY := $(OUT_BIN)
OUT_BIN = $(DUMMY)/xml

include $(PROJECT_ROOT)/build/multibin.mk
