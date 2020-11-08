CC := gcc
ROOT := ../..
LIBPATH := $(ROOT)/lib
INCLUDE := $(ROOT)/include

DLFLAGS := -Wl,-rpath $(LIBPATH) -L$(LIBPATH) -lmyapue -lpthread -lbsd -lrt
CFLAGS := -Wall -O -I$(INCLUDE)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:c=o)
PRGS := $(SRCS:.c=)
LIBAPUE := $(LIBPATH)/libmyapue.so

.PHONY: all
all: $(LIBAPUE) $(PRGS)

$(PRGS):%:%.o
	$(CC) $(CFLAGS) $(DLFLAGS) $< -o $@

$(OBJS):%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(LIBAPUE):
	cd $(LIBPATH) && make

clean: rmobjs
	-rm $(PRGS)
	
rmobjs:
	-rm $(OBJS)
