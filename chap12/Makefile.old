CC:= gcc
DPATH:= -lpthread -lbsd -lrt -Wl,-rpath,../lib -L../lib -lmyapue
CFLAGS:= -Wall -O
SRCS:= $(wildcard *.c)
OBJS:= $(SRCS:c=o)
PRGS:= $(SRCS:.c=)

.PHONY: all
all: $(PRGS)

$(PRGS):%:%.o
	$(CC) $(CFLAGS) $(DPATH) $< -o $@

$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $(DPATH) -c $<

clean: rmobjs
	-rm $(PRGS)
	
rmobjs:
	-rm *.o
