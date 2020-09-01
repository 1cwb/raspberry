$(info <---------------------------------------------------------->)
ifeq ($(OLED_SUPPORT), true) 
CFLAGS += -DOLED_DRIVER_ON
$(info <-------------------add oled mode device------------------->)
endif
ifeq ($(DAEMON), true) 
CFLAGS += -DRUN_AS_DAEMON
$(info <-------------------run as a daemon------------------------>)
endif

# 指定编译器
CC = g++

# CFLAG包括头文件目录
CFLAGS += -g -Wall
# 头文件查找路径
INC = -I./ -I./stdlib/ -I./oled/ -I./fonts/

# 静态链接库
LDFLAGS =
LDLIBS = -l pthread

# 目标
TARGET = runtest
# 源文件
LIBSRC += stdlib/mutexlock.cpp stdlib/condition.cpp main.cpp threadtask.cpp stdlib/thread.cpp stdlib/msg.cpp stdlib/usbEvent.cpp \
      stdlib/common.cpp stdlib/cfile.cpp stdlib/cfileSys.cpp stdlib/timemeasure.cpp stdlib/csignal.cpp \
	  stdlib/clog.cpp oled/oled.cpp
LIBFONTSRC += fonts/font8.cpp fonts/font12.cpp fonts/font16.cpp fonts/font20.cpp fonts/font24.cpp 

SRC += $(LIBSRC)
SRC += $(LIBFONTSRC)
# 源文件编译为目标文件
OBJS = $(SRC:.cpp=.o)

# 链接为可执行文件
$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# 清除可执行文件和目标文件
clean:
	rm -f *.o
	rm -f $(TARGET)
	rm -f stdlib/*.o
	rm -f oled/*.o
	rm -f fonts/*.o
# 编译规则 加入头文件 $@代表目标文件 $< 代表第一个依赖文件
%.o:%.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
