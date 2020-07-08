CC=gcc
OPT=-O -Wall -lapue

all:myls.o mycat.o mycat_stdio.o pro_id.o fork_exec.o error.o myid.o
	$(CC) -o myls myls.o $(OPT)
	$(CC) -o mycat mycat.o $(OPT)
	$(CC) -o mycat_stdio mycat_stdio.o $(OPT)
	$(CC) -o pro_id pro_id.o $(OPT)
	$(CC) -o fork_exec fork_exec.o $(OPT)
	$(CC) -o error error.o $(OPT)
	$(CC) -o myid myid.o $(OPT)

myls.o:myls.c
	$(CC) -c myls.c $(OPT)

mycat.o:mycat.c
	$(CC) -c mycat.c $(OPT)

mycat_stdio.o:mycat_stdio.c
	$(CC) -c mycat_stdio.c $(OPT)

pro_id.o:pro_id.c
	$(CC) -c pro_id.c $(OPT)

fork_exec.o:fork_exec.c
	$(CC) -c fork_exec.c $(OPT)

error.o:error.c
	$(CC) -c error.c $(OPT)

myid.o:myid.c
	$(CC) -c myid.c $(OPT)

clean:
	rm error.o myls.o mycat.o mycat_stdio.o pro_id.o fork_exec.o myid.o
	rm myls mycat mycat_stdio pro_id fork_exec error myid
