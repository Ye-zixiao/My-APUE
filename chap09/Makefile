CC := gcc
ROOT := ..
LIBPATH := $(ROOT)/lib
DPATH := -lpthread -lbsd -lrt -lapue -Wl,-rpath $(LIBPATH) -L$(LIBPATH) -lmyapue
CFLAGS := -Wall -O
SRCS := $(wildcard *.c)
OBJS := $(SRCS:c=o)
PRGS := $(SRCS:.c=)
LIBAPUE := $(LIBAPUE)/libmyapue.so

.PHONY: all
all: $(LIBAPUE) $(PRGS)

$(PRGS):%:%.o
	$(CC) $(CFLAGS) $(DPATH) $< -o $@

$(OBJS):%.o:%.c

$(LIBAPUE):
	cd $(LIBPATH) && make

clean: rmobjs
	rm $(PRGS)
	
rmobjs:
	rm *.o
