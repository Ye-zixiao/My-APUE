CC := gcc
ROOT := ..
INCLUDE := $(ROOT)/include
LIBPATH := $(ROOT)/lib

DLFLAGS := -Wl,-rpath $(LIBPATH) -L$(LIBPATH) -lmyapue -lpthread -lbsd -lrt
CFLAGS := -O -Wall -I$(INCLUDE)

SRCS := $(wildcard *.c)
OBJS := $(SRCS:c=o)
PRGS := $(SRCS:.c=)
LIBAPUE := $(LIBPATH)/libmyapue.so

.PHONY:
all: $(LIBAPUE) $(PRGS)

$(LIBAPUE):
	cd $(LIBPATH) && make

$(PRGS):%:%.o
	$(CC) -o $@ $< $(DLFLAGS) $(CFLAGS)

$(OBJS):%.o:%.c

clean: rmobj
	-rm $(PRGS)

rmobj:
	-rm $(OBJS)
