CC := gcc
ROOT := ../..
LIBPATH := $(ROOT)/lib

DLFLAGS := -Wl,-rpath $(LIBPATH) -L$(LIBPATH) -lmyapue -lpthread -lbsd -lrt
CFLAGS := -Wall -O# -I$(ROOT)/include

SERVS := server.c do_args.c do_client.c sloop.c ploop.c eloop.c
CLIS := client.c open.c
SERVOBJS := $(SERVS:c=o)
CLIOBJS := $(CLIS:c=o)
LIBAPUE := $(LIBPATH)/libmyapue.so

.PHONY:
all: $(LIBAPUE) $(SERVOBJS) $(CLIOBJS)
	$(CC) -o server $(SERVOBJS) $(CFLAGS) $(DLFLAGS)
	$(CC) -o client $(CLIOBJS) $(CFLAGS) $(DLFLAGS)

$(LIBAPUE):
	cd $(LIBPATH) && make

$(SERVOBJS):%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(CLIOBJS):%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean: rmobj
	-rm server client

rmobj:
	-rm $(SERVOBJS) $(CLIOBJS)
