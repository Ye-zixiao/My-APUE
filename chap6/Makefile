CC:=gcc
cflags:=-Wall -Werror -lapue -O

SRCS:=userinfo.c groupinfo.c sgroup.c mydate.c myhostname.c \
		strftime.c
#	shadowinfo.c
OBJS:=${SRCS:c=o}
PROGS:=${SRCS:.c=}

.PHONY:
all:${PROGS}

${PROGS}:%:%.o Makefile
	${CC} $< -o $@ $(cflags)

clean:
	rm -f ${PROGS} ${OBJS}
	rm -f outfile

rmprg:
	rm -f ${PROGS}

rmobj:
	rm -f $(OBJS)

%.o:%.c Makefile
	${CC} ${cflags} -c $<
