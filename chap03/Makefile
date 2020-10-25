CC:=gcc
cflags:=-Wall -Werror -lapue -O

SRCS:=lseek.c file_hole.c cp.c pread.c mytouch.c getfile_status.c\
		cpsyn.c cp_fdatasync.c cp_fsync.c cpsync_fsync.c cp_realsync.c \
		cp_nonsync.c mydup3.c check_append_lseek.c
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
