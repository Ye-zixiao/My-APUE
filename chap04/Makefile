CC:=gcc
cflags:=-Wall -Werror -lapue -O

SRCS:=myfile.c access_test.c umask.c chmod.c mychgrp.c blk.c \
		myrename.c mkrmdir.c myln.c myreadlink.c myslnk.c \
		truc.c mydir.c myftw.c cd-cwd.c devrdev.c myhandle_cp.c \
		mystat.c chftw.c testPathMax.c
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
