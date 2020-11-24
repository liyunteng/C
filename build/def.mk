ifeq ($(MAKELEVEL),0)

# Env
BUILD_ENV ?= release
ifeq ("$(origin D)", "command line")
ifeq ($(D),1)
	BUILD_ENV = debug
endif
endif

# Verbose
BUILD_VERBOSE ?= 0
ifeq ("$(origin V)", "command line")
	BUILD_VERBOSE = $(V)
endif
ifeq ($(BUILD_VERBOSE),1)
	Q =
else
	Q = @
    MAKEFLAGS += --no-print-directory -s
endif

# Output
BUILD_PWD    := $(abspath $(CURDIR))
BUILD_OUTPUT ?= $(abspath $(CURDIR))/out
ifeq ("$(origin O)", "command line")
    BUILD_OUTPUT = $(abspath $(O))
endif
ifneq ($(BUILD_OUTPUT),$(BUILD_PWD))
# MAKEFLAGS += --include-dir=$(BUILD_PWD)
endif

OUT_ROOT	:= $(abspath $(BUILD_OUTPUT))
OUT_INCLUDE := $(OUT_ROOT)/include
OUT_BIN     := $(OUT_ROOT)/bin
OUT_LIB     := $(OUT_ROOT)/lib
OUT_OBJECT  := $(OUT_ROOT)/obj
OUT_DEPEND  := $(OUT_ROOT)/obj
OUT_CONFIG  := $(OUT_ROOT)/etc

# Compiler
# ******************************
CC		    := $(CROSS_COMPILE)gcc
CXX         := $(CROSS_COMPILE)g++
CPP		    := $(CC) -E
AS		    := $(CROSS_COMPILE)as
LD		    := $(CROSS_COMPILE)ld
AR		    := $(CROSS_COMPILE)ar
NM		    := $(CROSS_COMPILE)nm
STRIP	    := $(CROSS_COMPILE)strip
OBJCOPY	    := $(CROSS_COMPILE)objcopy
OBJDUMP	    := $(CROSS_COMPILE)objdump
OBJSIZE		:= $(CROSS_COMPILE)size

CPPFLAGS    := -I$(OUT_INCLUDE)
CFLAGS      := -Wall -fstack-protector -Wmissing-prototypes -Wstrict-prototypes
CXXFLAGS    := -Wall -fstack-protector
ASFLAGS     := -D__ASSEMBLY__ -fno-PIE
LDFLAGS     :=
LOADLIBES   :=
LDLIBS      :=
ARFLAGS     := rcs

# Tools
SHELL       := /bin/sh
OS_TYPE     := $(shell uname)
ifeq ($(OS_TYPE),Darwin)
CP          := rsync -a
else
CP          := cp -ru
endif
RM          := rm -rf
MKDIR       := mkdir -p


ifeq ($(BUILD_ENV), release)
	CFLAGS += -O2 -DNDEBUG
	CXXFLAGS += -O2 -DNDEBUG
else
	CFLAGS += -g -ggdb
	CXXFLAGS += -g -ggdb
endif

CCMSG="CC"
CXXMSG="CXX"
DEPENDMSG="DEP"
LDMSG="LD"
ARMSG="AR"
STRIPMSG="STRIP"
FORMAT="%-6.6s [%s]  %s\n"

export
endif
