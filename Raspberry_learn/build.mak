$(info common.mak $(GLOBAL_TOP_PATH))
include $(GLOBAL_TOP_PATH)stdlib/stdlibbuild.mak
include $(GLOBAL_TOP_PATH)oled/oledbuild.mak
include $(GLOBAL_TOP_PATH)network/networkbuild.mak
include $(GLOBAL_TOP_PATH)fonts/fontsbuild.mak
include $(GLOBAL_TOP_PATH)network/networkbuild.mak

# 链接为可执行文件
$(TARGET): $(GLOBAL_OBJS)
	$(GLOBAL_CC) $^ -o $@ $(GLOBAL_LDFLAGS) $(GLOBAL_LDLIBS)

# 清除可执行文件和目标文件
clean:
	rm -f $(GLOBAL_OBJS)
	rm -f $(TARGET)

# 编译规则 加入头文件 $@代表目标文件 $< 代表第一个依赖文件
%.o:%.cpp
	$(GLOBAL_CC) $(GLOBAL_CFLAGS) $(GLOBAL_INC) -o $@ -c $<