CC:= gcc
DPATH:= -lpthread -lbsd -lrt -lapue -Wl,-rpath,../../lib -L../../lib -lmyapue
CFLAGS:= -Wall -O
SERVER:= server.c do_args.c do_client.c sloop.c ploop.c eloop.c
CLIENT:= client.c open.c
SERVEROBJS:= $(SERVER:c=o)
CLIENTOBJS:= $(CLIENT:c=o)

.PHONY: all
all:$(SERVEROBJS) $(CLIENTOBJS)
	$(CC) -o server $(SERVEROBJS) $(CFLAGS) $(DPATH) 
	$(CC) -o client $(CLIENTOBJS) $(CFLAGS) $(DPATH)

$(SERVEROBJS):%.o:%.c
	$(CC) -c $< $(CFLAGS) $(DPATH)

$(CLIENTOBJS):%.o:%.c
	$(CC) -c $< $(CFLAGS) $(DPATH)

clean:rmobjs
	-rm server client

rmobjs:
	@-rm $(CLIENTOBJS) $(SERVEROBJS)


