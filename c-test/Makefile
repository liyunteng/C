PROJECT_ROOT ?= $(abspath ..)
include $(PROJECT_ROOT)/build/def.mk

SUBDIRS += futex http inotify json libcurl libev libevent libparted
SUBDIRS += libudev mem multicast regex shm socket speed_test sqlite xml
# SUBDIRS += aio
# SUBDIRS += libnice netlink socket zmq
OUT_BIN = $(OUT_ROOT)/bin/c-test
include $(PROJECT_ROOT)/build/subdir.mk
