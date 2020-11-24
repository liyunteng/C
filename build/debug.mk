MODE=debug

.PHONY: show help-global

all:

help-global:
	@echo "make <BUILD_ENV=[release|debug]> <CROSS_COMPILE=arm-linux-gnueabi-> <O=/opt/out> <V=[0|1]> <D=[0|1]> <show> <help>"
	@echo ""
	@echo "    BUILD_ENV       release or debug, default is release"
	@echo "    CROSS_COMPILE   cross compile toolchain"
	@echo "    O               output"
	@echo "    V               verbose"
	@echo "    D               debug or release"
	@echo "    show            show current configuration"
	@echo "    help            show this help"
	@echo ""

show:
	@echo "=============== $(CURDIR) ==============="
	@echo "BUILD_ENV          = " $(BUILD_ENV)
	@echo "BUILD_VERBOSE      = " $(BUILD_VERBOSE)
	@echo "BUILD_PWD          = " $(BUILD_PWD)
	@echo "BUILD_OUTPUT       = " $(BUILD_OUTPUT)
	@echo "D                  = " $(D)
	@echo "Q                  = " $(Q)
	@echo "O                  = " $(O)
	@echo ""

	@echo "OUT_ROOT           = " $(OUT_ROOT)
	@echo "OUT_INCLUDE        = " $(OUT_INCLUDE)
	@echo "OUT_BIN            = " $(OUT_BIN)
	@echo "OUT_LIB            = " $(OUT_LIB)
	@echo "OUT_OBJECT         = " $(OUT_OBJECT)
	@echo "OUT_DEPEND         = " $(OUT_DEPEND)
	@echo "OUT_CFG            = " $(OUT_CFG)
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

	@echo "MODE               = " $(MODE)
	@echo "MODULE_ROOT        = " $(MODULE_ROOT)
	@echo "MODULE_NAME        = " $(MODULE_NAME)
	@echo "SOURCE_ROOT        = " $(SOURCE_ROOT)
	@echo "SOURCE_DIRS        = " $(SOURCE_DIRS)
	@echo "SOURCE_OMIT        = " $(SOURCE_OMIT)
	@echo "SOURCE_C           = " $(SOURCE_C)
	@echo "OBJECT_C           = " $(OBJECT_C)
	@echo "DPEND_C            = " $(DEPEND_C)
	@echo "SOURCE_CXX         = " $(SOURCE_CXX)
	@echo "OBJECT_CXX         = " $(OBJECT_CXX)
	@echo "DEPEND_CXX         = " $(DEPEND_CXX)
	@echo "INCLUDE_DIRS       = " $(INCLUDE_DIRS)
	@echo "OUT_OBJECT_DIRS    = " $(OUT_OBJECT_DIRS)
	@echo "EXPORT_DIRS        = " $(EXPORT_DIRS)
	@echo "CreateResult       = " $(CreateResult)
	@echo ""

	@echo "BIN                = " $(BIN)
	@echo "LIB_TYPE           = " $(LIB_TYPE)
	@echo "LIB                = " $(LIB)
	@echo "SOLIB              = " $(SOLIB)
	@echo "SUBDIRS            = " $(SUBDIRS)
	@echo ""
	@echo ""
