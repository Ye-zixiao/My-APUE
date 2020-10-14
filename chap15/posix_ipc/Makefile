CC:= gcc
DPATH:= -lpthread -lbsd -lrt -lapue -Wl,-rpath,../../lib -L../../lib -lmyapue
CFLAGS:= -Wall -O
SRCS:= $(wildcard *.c)
OBJS:= $(SRCS:c=o)
PRGS:= $(SRCS:.c=)

.PHONY: all
all: $(PRGS)

#每一个可执行文件%的生成都依赖于其同名绝对目标文件%.o
$(PRGS):%:%.o
	$(CC) $(CFLAGS) $(DPATH) $< -o $@
	-rm $<

#每一个绝对目标文件%.o的生成都依赖于其同名源文件%.c
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $(DPATH) -c $<

clean: rmobjs
	-rm $(PRGS)
	
rmobjs:
	-rm *.o
