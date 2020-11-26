ifeq ($(MAKELEVEL),0)

# Env
BUILD_ENV ?= release
ifeq ("$(origin D)", "command line")
ifeq ($(D),1)
	BUILD_ENV = debug
else ifeq ($(D),2)
	BUILD_ENV = debuginfo
else ifeq ($(D),3)
	BUILD_ENV = map
endif
endif

# Verbose
BUILD_VERBOSE ?= 0
ifeq ("$(origin V)", "command line")
    BUILD_VERBOSE = $(V)
endif
ifeq ($(BUILD_VERBOSE),0)
    Q1 = @
    Q2 = @
    Q3 = @
    MAKEFLAGS += --no-print-directory -s
else ifeq ($(BUILD_VERBOSE),1)
    Q1 =
    Q2 = @
    Q3 = @
    MAKEFLAGS += --no-print-directory
else ifeq ($(BUILD_VERBOSE),2)
    Q1 =
    Q2 =
    Q3 = @
    MAKEFLAGS += --no-print-directory
else ifeq ($(BUILD_VERBOSE),3)
    Q1 =
    Q2 =
    Q3 =
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

OUT_ROOT	  := $(abspath $(BUILD_OUTPUT))
OUT_INCLUDE   := $(OUT_ROOT)/include
OUT_BIN       := $(OUT_ROOT)/bin
OUT_LIB       := $(OUT_ROOT)/lib
OUT_OBJECT    := $(OUT_ROOT)/obj
OUT_DEPEND    := $(OUT_ROOT)/obj
OUT_CONFIG    := $(OUT_ROOT)/etc

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
CP          := cp -rf
endif
RM          := rm -rf
MKDIR       := mkdir -p


ifeq ($(BUILD_ENV), release)
	CFLAGS += -O2 -DNDEBUG
	CXXFLAGS += -O2 -DNDEBUG
else ifeq ($(BUILD_ENV), debug)
	CFLAGS += -g -ggdb
	CXXFLAGS += -g -ggdb
else ifeq ($(BUILD_ENV), debuginfo)
	CFLAGS += -g -ggdb
	CXXFLAGS += -g -ggdb
else ifeq ($(BUILD_ENV), map)
	CFLAGS += -g -ggdb
	CXXFLAGS += -g -ggdb
endif

COLOR_RED    := \033[1;31m
COLOR_GREEN  := \033[1;32m
COLOR_YELLOW := \033[1;33m
COLOR_BLUE   := \033[1;34m
COLOR_PURPLE := \033[1;35m
COLOR_CYAN   := \033[1;36m
COLOR_NORMAL := \033[0m

CCMSG     := "CC"
CXXMSG    := "CXX"
DEPENDMSG := "DEP"
LDMSG     := "LD"
ARMSG     := "AR"
STRIPMSG  := "STRIP"
CPMSG     := "COPY"
DBGMSG    := "DBG"
# PRINT4    := @printf "$(COLOR_GREEN)%-6.6s$(COLOR_NORMAL) [%s]  %s  =>  %s\n"
PRINT4    := @printf "$(COLOR_GREEN)%-6.6s$(COLOR_NORMAL) [%s]  %0.0s%s\n"
# PRINT4    := @printf "$(COLOR_GREEN)%-6.6s$(COLOR_NORMAL) [%s]  %s%0.0s\n"
PRINT3    := @printf "$(COLOR_GREEN)%-6.6s$(COLOR_NORMAL) [%s]  %s\n"

export
endif
