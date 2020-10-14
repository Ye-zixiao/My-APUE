CC= gcc
CFLAGS= -fPIC -lpthread
SRCS:= $(wildcard *.c)
OBJS:= $(SRCS:c=o)

all:$(OBJS)
	$(CC) -shared $(CFLAGS) $(OBJS) -o libmyapue.so
	-rm $(OBJS)

%.o:%.c Makefile
	$(CC) $(CFLAGS) -c $<

.PHONY:
clean: rmobj
	-rm libmyapue.so

rmobj:
	-rm *.o 
