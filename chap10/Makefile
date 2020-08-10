CC:=gcc
cflags:=-Wall -lapue -O #-Werror

SRCS:=signal_handler.c unreentrant.c sig_cld.c mySleep.c kill.c \
		mySleep2.c Sigset_t.c pr_mask.c sigpending.c Signal.c \
		Signal_intr.c sigjmp.c
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
