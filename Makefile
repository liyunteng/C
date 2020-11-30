PROJECT_ROOT ?= $(abspath .)
include build/def.mk

SUBDIRS += apue c-test leecode

include build/subdir.mk
