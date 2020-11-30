PROJECT_ROOT ?= $(abspath ..)
include $(PROJECT_ROOT)/build/def.mk

SUBDIRS += lib chapter_1 chapter_2 chapter_3 chapter_5 chapter_7
SUBDIRS += chapter_8 chapter_9 chapter_10 chapter_12 chapter_14
LOADLIBES += -L$(OUT_LIB) -Wl,-rpath $(OUT_LIB) -llib
OUT_BIN = $(OUT_ROOT)/bin/apue

include $(PROJECT_ROOT)/build/subdir.mk
