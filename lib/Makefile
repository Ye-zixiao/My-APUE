CC := gcc
ROOT := ..
INCLUDE := $(ROOT)/include

DLFLAGS := -shared
CFLAGS := -fPIC -O -Wall -I$(INCLUDE)
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
LIBAPUE := libmyapue.so

.PHONY:
$(LIBAPUE): $(OBJS)
	$(CC) -o $(LIBAPUE) $(DLFLAGS) $(OBJS)

$(OBJS):%.o:%.c
	$(CC)  $(CFLAGS) -c $< -o $@

clean: rmobj
	-rm $(LIBAPUE)

rmobj:
	-rm $(OBJS)
