CUR_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CUR_PATH_SRC_FILE := $(CUR_PATH)*.cpp
CUR_PATH_SRC_FILE += $(CUR_PATH)*.c
#$(info $(CUR_PATH_SRC_FILE))

# 头文件查找路径
GLOBAL_INC += -I $(CUR_PATH)


# 目标
#TARGET = runtest
# 源文件
LIBSRC = $(wildcard $(CUR_PATH_SRC_FILE))
#$(info $(LIBSRC))
GLOBAL_SRC_FILE += $(LIBSRC)

# 源文件编译为目标文件
GLOBAL_OBJS += $(LIBSRC:.cpp=.o)
