CC:=gcc
cflags:=-Wall -Werror -lapue -O

SRCS:=getputc.c fgetputs.c fgetputc.c readwrite.c freopen.c \
		stream_buf.c buf.c tmpnam.c mkstemp.c fmemopen.c SetBuf.c \

OBJS:=${SRCS:c=o}
PROGS:=${SRCS:.c=}

.PHONY:
all:${PROGS}

${PROGS}:%:%.o Makefile
	${CC} $< -o $@ $(cflags)

clean:
	rm -f ${PROGS} ${OBJS}
	rm -f outfile foo bar

rmprg:
	rm -f ${PROGS}

rmobj:
	rm -f $(OBJS)

%.o:%.c Makefile
	${CC} ${cflags} -c $<
