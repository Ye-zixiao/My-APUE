CC:=gcc
cflags:=-Wall -lapue -Wl,-rpath,../lib -L../lib -lmyapue \
		-O #-Werror
#-WL,-rpath,../lib -L../lib -lmyapue 的意思是到相对路径../lib中找动态链接库

SRCS:=tellwait.c abort.c good_system.c whySystem.c Sleep.c \
		hrsleep.c jobctrl.c strsignal.c linux_system.c
OBJS:=${SRCS:c=o}
PROGS:=${SRCS:.c=}

.PHONY:
all:${PROGS}

${PROGS}:%:%.o Makefile
	${CC} $< -o $@ $(cflags)

clean:
	rm -f ${PROGS} ${OBJS}

rmprg:
	rm -f ${PROGS}

rmobj:
	rm -f $(OBJS)

%.o:%.c Makefile
	${CC} ${cflags} -c $<
