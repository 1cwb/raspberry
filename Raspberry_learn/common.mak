CUR_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

export GLOBAL_TOP_PATH := $(CUR_PATH)
export GLOBAL_INC :=

# 指定编译器
export GLOBAL_CC := g++

# CFLAG包括头文件目录
export GLOBAL_CFLAGS := -g -Wall

# 静态链接库
export GLOBAL_LDFLAGS :=
export GLOBAL_LDLIBS := -l pthread

export GLOBAL_OUTPUT_OBJS := $(GLOBAL_TOP_PATH)objs

export GLOBAL_OBJS :=
export GLOBAL_SRC_FILE :=

# 目标
export TARGET :=

export FIRST_MAKE := $(abspath $(firstword $(MAKEFILE_LIST)))
