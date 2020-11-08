CC := gcc
ROOT := ../..
LIBPATH := $(ROOT)/lib

DLFLAGS := -Wl,-rpath $(LIBPATH) -L$(LIBPATH) -lmyapue -lpthread -lbsd -lrt
CFLAGS := -Wall -O

SERVS := server.c opend.c
CLIS := client.c open.c
SERVOBJS := $(SERVS:c=o)
CLISOBJS := $(CLIS:c=o)
LIBAPUE := $(LIBPATH)/libmyapue.so

.PHONY:
all: $(LIBAPUE) $(CLISOBJS) $(SERVOBJS) 
	$(CC) -o server $(SERVOBJS) $(CFLAGS) $(DLFLAGS)
	$(CC) -o client $(CLISOBJS) $(CFLAGS) $(DLFLAGS)

$(SERVOBJS):%.o:%.c

$(CLISOBJS):%.o:%.c

$(LIBAPUE):
	cd $(LIBPATH) && make

clean: rmobj
	-rm server client

rmobj:
	-rm $(SERVOBJS) $(CLISOBJS)
