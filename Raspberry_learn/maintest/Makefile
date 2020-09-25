include ../common.mak

$(info <---------------------------------------------------------->)
ifeq ($(OLED_SUPPORT), true) 
GLOBAL_CFLAGS += -DOLED_DRIVER_ON
$(info <-------------------add oled mode device------------------->)
endif
ifeq ($(DAEMON), true) 
GLOBAL_CFLAGS += -DRUN_AS_DAEMON
$(info <-------------------run as a daemon------------------------>)
endif

CUR_PATH := $(dir $(abspath $(firstword $(MAKEFILE_LIST))))
CUR_PATH_SRC_FILE := $(CUR_PATH)*.cpp
CUR_PATH_SRC_FILE += $(CUR_PATH)*.c

# 目标
TARGET += runtest
# 源文件
LIBSRC := $(wildcard $(CUR_PATH_SRC_FILE))
GLOBAL_SRC_FILE += $(LIBSRC)

# 源文件编译为目标文件
GLOBAL_OBJS += $(LIBSRC:.cpp=.o)

include ../build.mak